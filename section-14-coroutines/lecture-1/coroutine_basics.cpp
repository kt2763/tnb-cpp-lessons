// コルーチンの基礎（C++20）
// 協調的マルチタスクと非同期プログラミングの新しいパラダイム

#include <iostream>
#include <coroutine>
#include <vector>
#include <chrono>
#include <thread>

// 基本的なジェネレーター
template<typename T>
class Generator
{
public:
    struct promise_type
    {
        T current_value;
        
        Generator get_return_object()
        {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        
        std::suspend_always yield_value(T value)
        {
            current_value = value;
            return {};
        }
        
        void return_void() {}
        void unhandled_exception() {}
    };
    
    std::coroutine_handle<promise_type> handle;
    
    explicit Generator(std::coroutine_handle<promise_type> h) : handle(h) {}
    
    ~Generator()
    {
        if (handle)
            handle.destroy();
    }
    
    // ムーブのみ可能
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    
    Generator(Generator&& other) noexcept : handle(other.handle)
    {
        other.handle = {};
    }
    
    Generator& operator=(Generator&& other) noexcept
    {
        if (this != &other)
        {
            if (handle)
                handle.destroy();
            handle = other.handle;
            other.handle = {};
        }
        return *this;
    }
    
    bool next()
    {
        handle.resume();
        return !handle.done();
    }
    
    T value() const
    {
        return handle.promise().current_value;
    }
};

// 基本的なジェネレーター関数
Generator<int> simpleCounter(int start, int end)
{
    std::cout << "カウンター開始: " << start << " から " << end << " まで" << std::endl;
    
    for (int i = start; i <= end; ++i)
    {
        std::cout << "yield: " << i << std::endl;
        co_yield i;
    }
    
    std::cout << "カウンター終了" << std::endl;
}

// フィボナッチ数列のジェネレーター
Generator<int> fibonacciSequence(int count)
{
    std::cout << "フィボナッチ数列生成開始 (" << count << " 個)" << std::endl;
    
    int a = 0, b = 1;
    
    for (int i = 0; i < count; ++i)
    {
        if (i == 0)
        {
            co_yield a;
        }
        else if (i == 1)
        {
            co_yield b;
        }
        else
        {
            int next = a + b;
            a = b;
            b = next;
            co_yield next;
        }
    }
    
    std::cout << "フィボナッチ数列生成終了" << std::endl;
}

// ゲームでの使用例：敵の巡回パターン
struct Position
{
    float x, y;
    Position(float px, float py) : x(px), y(py) {}
};

Generator<Position> enemyPatrolPath()
{
    std::cout << "敵の巡回パターン開始" << std::endl;
    
    // 四角形の巡回パターン
    std::vector<Position> waypoints = {
        {0.0f, 0.0f},   // スタート地点
        {10.0f, 0.0f},  // 右へ
        {10.0f, 10.0f}, // 上へ
        {0.0f, 10.0f},  // 左へ
        {0.0f, 0.0f}    // 戻る
    };
    
    while (true)  // 無限ループで巡回
    {
        for (const auto& pos : waypoints)
        {
            std::cout << "敵が位置 (" << pos.x << ", " << pos.y << ") に移動" << std::endl;
            co_yield pos;
        }
    }
}

// より高度な例：アニメーションフレーム生成
struct AnimationFrame
{
    int frameNumber;
    float timeStamp;
    std::string spriteName;
    
    AnimationFrame(int frame, float time, const std::string& sprite)
        : frameNumber(frame), timeStamp(time), spriteName(sprite) {}
};

Generator<AnimationFrame> createWalkAnimation(float frameDuration)
{
    std::cout << "歩行アニメーション開始（フレーム間隔: " << frameDuration << "秒）" << std::endl;
    
    std::vector<std::string> walkSprites = {
        "walk_01.png",
        "walk_02.png", 
        "walk_03.png",
        "walk_04.png",
        "walk_05.png",
        "walk_06.png"
    };
    
    int frameCount = 0;
    float currentTime = 0.0f;
    
    while (true)  // ループアニメーション
    {
        for (const auto& sprite : walkSprites)
        {
            co_yield AnimationFrame(frameCount++, currentTime, sprite);
            currentTime += frameDuration;
        }
    }
}

// Task型の基本実装（非同期処理用）
template<typename T>
class Task
{
public:
    struct promise_type
    {
        T value;
        
        Task get_return_object()
        {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        
        void return_value(T val)
        {
            value = val;
        }
        
        void unhandled_exception() {}
    };
    
    std::coroutine_handle<promise_type> handle;
    
    explicit Task(std::coroutine_handle<promise_type> h) : handle(h) {}
    
    ~Task()
    {
        if (handle)
            handle.destroy();
    }
    
    // ムーブのみ可能
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    
    Task(Task&& other) noexcept : handle(other.handle)
    {
        other.handle = {};
    }
    
    Task& operator=(Task&& other) noexcept
    {
        if (this != &other)
        {
            if (handle)
                handle.destroy();
            handle = other.handle;
            other.handle = {};
        }
        return *this;
    }
    
    bool done() const
    {
        return handle.done();
    }
    
    T get_result()
    {
        while (!handle.done())
        {
            handle.resume();
        }
        return handle.promise().value;
    }
};

// 非同期計算の例
Task<int> asyncCalculation(int input)
{
    std::cout << "非同期計算開始: " << input << std::endl;
    
    // 重い計算をシミュレート
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    int result = input * input + 42;
    std::cout << "非同期計算完了: " << result << std::endl;
    
    co_return result;
}

// ゲームでの非同期処理例：リソース読み込み
Task<std::string> loadGameResource(const std::string& filename)
{
    std::cout << "リソース読み込み開始: " << filename << std::endl;
    
    // ファイル読み込みをシミュレート
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    std::string data = "リソースデータ: " + filename;
    std::cout << "リソース読み込み完了: " << filename << std::endl;
    
    co_return data;
}

// コルーチンの状態監視
void demonstrateCoroutineStates()
{
    std::cout << "\n=== コルーチンの状態監視 ===" << std::endl;
    
    auto gen = simpleCounter(1, 5);
    
    std::cout << "初期状態 - done: " << gen.handle.done() << std::endl;
    
    while (gen.next())
    {
        std::cout << "値: " << gen.value() << ", done: " << gen.handle.done() << std::endl;
    }
    
    std::cout << "最終状態 - done: " << gen.handle.done() << std::endl;
}

// 複数のコルーチンの協調
void demonstrateMultipleCoroutines()
{
    std::cout << "\n=== 複数のコルーチンの協調 ===" << std::endl;
    
    auto fibGen = fibonacciSequence(8);
    auto countGen = simpleCounter(10, 15);
    
    std::cout << "フィボナッチ数列:" << std::endl;
    for (int i = 0; i < 5 && fibGen.next(); ++i)
    {
        std::cout << "  " << fibGen.value() << std::endl;
    }
    
    std::cout << "カウンター:" << std::endl;
    for (int i = 0; i < 3 && countGen.next(); ++i)
    {
        std::cout << "  " << countGen.value() << std::endl;
    }
    
    std::cout << "残りのフィボナッチ数列:" << std::endl;
    while (fibGen.next())
    {
        std::cout << "  " << fibGen.value() << std::endl;
    }
}

// ゲームでの実用例
void demonstrateGameUsage()
{
    std::cout << "\n=== ゲームでの使用例 ===" << std::endl;
    
    // 敵の巡回パターン
    auto patrolGen = enemyPatrolPath();
    std::cout << "敵の巡回パターン（最初の8ステップ）:" << std::endl;
    for (int i = 0; i < 8 && patrolGen.next(); ++i)
    {
        auto pos = patrolGen.value();
        std::cout << "  ステップ " << (i + 1) << ": (" << pos.x << ", " << pos.y << ")" << std::endl;
    }
    
    // アニメーションフレーム生成
    auto animGen = createWalkAnimation(0.1f);
    std::cout << "\n歩行アニメーション（最初の10フレーム）:" << std::endl;
    for (int i = 0; i < 10 && animGen.next(); ++i)
    {
        auto frame = animGen.value();
        std::cout << "  フレーム " << frame.frameNumber 
                  << " (時刻: " << frame.timeStamp << "): " << frame.spriteName << std::endl;
    }
}

// 非同期処理のデモ
void demonstrateAsyncTasks()
{
    std::cout << "\n=== 非同期処理 ===" << std::endl;
    
    // 非同期計算
    auto task1 = asyncCalculation(10);
    auto task2 = asyncCalculation(20);
    
    std::cout << "他の処理を実行中..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    std::cout << "結果1: " << task1.get_result() << std::endl;
    std::cout << "結果2: " << task2.get_result() << std::endl;
    
    // リソース読み込み
    auto resourceTask = loadGameResource("texture.png");
    
    std::cout << "他の初期化処理中..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::string resourceData = resourceTask.get_result();
    std::cout << "読み込み結果: " << resourceData << std::endl;
}

int main()
{
    std::cout << "=== コルーチンの基礎（C++20）===" << std::endl;
    
    // 1. コルーチンの状態監視
    demonstrateCoroutineStates();
    
    // 2. 複数のコルーチンの協調
    demonstrateMultipleCoroutines();
    
    // 3. ゲームでの使用例
    demonstrateGameUsage();
    
    // 4. 非同期処理
    demonstrateAsyncTasks();
    
    std::cout << "\n=== コルーチンの特徴 ===" << std::endl;
    std::cout << "- co_yield: 値を返して一時停止" << std::endl;
    std::cout << "- co_return: 最終値を返して終了" << std::endl;
    std::cout << "- co_await: 非同期操作の待機" << std::endl;
    std::cout << "- 状態を保持したまま実行を中断・再開" << std::endl;
    std::cout << "- メモリ効率的な無限シーケンス生成" << std::endl;
    std::cout << "- 非同期プログラミングの簡素化" << std::endl;
    
    return 0;
}

// まとめ：
// - コルーチンは協調的マルチタスクを実現
// - co_yield で値を生成して一時停止
// - co_return で最終結果を返す
// - co_await で非同期操作を待機
// - ゲーム開発では巡回パターンやアニメーションに有効
// - 非同期処理をより直感的に記述可能