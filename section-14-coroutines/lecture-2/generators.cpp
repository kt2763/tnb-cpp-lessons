// ジェネレーター（Generator）
// 効率的な無限シーケンスとデータ生成

#include <iostream>
#include <coroutine>
#include <vector>
#include <string>
#include <random>
#include <cmath>
#include <algorithm>

// 改良されたジェネレーター
template<typename T>
class Generator
{
public:
    struct promise_type
    {
        T current_value;
        std::exception_ptr exception_ptr;
        
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
        
        void unhandled_exception()
        {
            exception_ptr = std::current_exception();
        }
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
    
    // イテレーター風のインターフェース
    class iterator
    {
    private:
        std::coroutine_handle<promise_type> handle_;
        
    public:
        explicit iterator(std::coroutine_handle<promise_type> h) : handle_(h) {}
        
        iterator& operator++()
        {
            handle_.resume();
            if (handle_.promise().exception_ptr)
            {
                std::rethrow_exception(handle_.promise().exception_ptr);
            }
            return *this;
        }
        
        T operator*() const
        {
            return handle_.promise().current_value;
        }
        
        bool operator!=(const iterator& other) const
        {
            return handle_ != other.handle_;
        }
    };
    
    iterator begin()
    {
        if (handle)
        {
            handle.resume();
            if (handle.promise().exception_ptr)
            {
                std::rethrow_exception(handle.promise().exception_ptr);
            }
        }
        return iterator{handle};
    }
    
    iterator end()
    {
        return iterator{nullptr};
    }
    
    bool done() const
    {
        return !handle || handle.done();
    }
};

// 基本的な数値シーケンス
Generator<int> range(int start, int end, int step = 1)
{
    std::cout << "範囲ジェネレーター: " << start << " から " << end << " (ステップ: " << step << ")" << std::endl;
    
    for (int i = start; step > 0 ? i < end : i > end; i += step)
    {
        co_yield i;
    }
}

// 無限数列
Generator<int> infiniteSequence(int start = 0)
{
    std::cout << "無限数列開始: " << start << " から" << std::endl;
    
    int current = start;
    while (true)
    {
        co_yield current++;
    }
}

// 素数ジェネレーター
Generator<int> primeNumbers()
{
    std::cout << "素数ジェネレーター開始" << std::endl;
    
    auto isPrime = [](int n) {
        if (n < 2) return false;
        if (n == 2) return true;
        if (n % 2 == 0) return false;
        
        for (int i = 3; i * i <= n; i += 2)
        {
            if (n % i == 0) return false;
        }
        return true;
    };
    
    for (int n = 2; ; ++n)
    {
        if (isPrime(n))
        {
            co_yield n;
        }
    }
}

// ゲームのランダムイベント生成器
struct GameEvent
{
    std::string type;
    int severity;  // 1-10の重要度
    std::string description;
    
    GameEvent(const std::string& t, int s, const std::string& desc)
        : type(t), severity(s), description(desc) {}
};

Generator<GameEvent> randomEventGenerator(unsigned int seed = 42)
{
    std::cout << "ランダムイベントジェネレーター開始（シード: " << seed << "）" << std::endl;
    
    std::mt19937 rng(seed);
    std::uniform_int_distribution<> severityDist(1, 10);
    std::uniform_int_distribution<> typeDist(0, 4);
    
    std::vector<std::string> eventTypes = {
        "敵の襲撃",
        "宝箱発見",
        "商人との遭遇",
        "天候変化",
        "謎の現象"
    };
    
    std::vector<std::vector<std::string>> descriptions = {
        {"弱い敵が現れた", "強力な敵が襲来", "ボス級の敵が出現"},
        {"小さな宝箱を発見", "貴重なアイテムを発見", "伝説の宝物を発見"},
        {"行商人に出会った", "武器商人を発見", "魔法商人との遭遇"},
        {"小雨が降り始めた", "激しい嵐が到来", "霧が立ち込めた"},
        {"奇妙な光を目撃", "古代の遺跡を発見", "神秘的な力を感じる"}
    };
    
    while (true)
    {
        int typeIndex = typeDist(rng);
        int severity = severityDist(rng);
        
        // 重要度に応じて説明を選択
        int descIndex = std::min((severity - 1) / 4, 2);
        
        co_yield GameEvent(
            eventTypes[typeIndex],
            severity,
            descriptions[typeIndex][descIndex]
        );
    }
}

// 波形ジェネレーター（ゲームの音楽やエフェクト用）
Generator<float> sineWave(float frequency, float amplitude, float duration, float sampleRate = 44100.0f)
{
    std::cout << "正弦波生成: 周波数 " << frequency << "Hz, 振幅 " << amplitude 
              << ", 長さ " << duration << "秒" << std::endl;
    
    int totalSamples = static_cast<int>(duration * sampleRate);
    
    for (int i = 0; i < totalSamples; ++i)
    {
        float time = i / sampleRate;
        float value = amplitude * std::sin(2.0f * M_PI * frequency * time);
        co_yield value;
    }
}

// フラクタルポイント生成器
struct Point2D
{
    float x, y;
    Point2D(float px, float py) : x(px), y(py) {}
};

Generator<Point2D> mandelbrotPoints(int width, int height, float zoom = 1.0f)
{
    std::cout << "マンデルブロ集合のポイント生成: " << width << "x" << height 
              << " (ズーム: " << zoom << ")" << std::endl;
    
    auto mandelbrot = [](float cx, float cy, int maxIter = 100) {
        float x = 0, y = 0;
        for (int i = 0; i < maxIter; ++i)
        {
            float x2 = x * x, y2 = y * y;
            if (x2 + y2 > 4) return i;
            y = 2 * x * y + cy;
            x = x2 - y2 + cx;
        }
        return maxIter;
    };
    
    for (int py = 0; py < height; ++py)
    {
        for (int px = 0; px < width; ++px)
        {
            float cx = (px - width / 2.0f) * 4.0f / (width * zoom);
            float cy = (py - height / 2.0f) * 4.0f / (height * zoom);
            
            int iterations = mandelbrot(cx, cy);
            if (iterations < 100)  // 集合外のポイントのみ生成
            {
                co_yield Point2D(cx, cy);
            }
        }
    }
}

// ゲームのレベル生成器
struct Room
{
    int id;
    std::string type;
    int difficulty;
    std::vector<std::string> enemies;
    std::vector<std::string> treasures;
    
    Room(int i, const std::string& t, int d) : id(i), type(t), difficulty(d) {}
};

Generator<Room> dungeonGenerator(int maxRooms, unsigned int seed = 123)
{
    std::cout << "ダンジョン生成器開始: 最大 " << maxRooms << " 部屋" << std::endl;
    
    std::mt19937 rng(seed);
    std::uniform_int_distribution<> typeDist(0, 3);
    std::uniform_int_distribution<> difficultyDist(1, 10);
    
    std::vector<std::string> roomTypes = {"通常部屋", "宝物庫", "ボス部屋", "休憩室"};
    
    for (int i = 0; i < maxRooms; ++i)
    {
        Room room(i, roomTypes[typeDist(rng)], difficultyDist(rng));
        
        // 部屋タイプに応じて内容を設定
        if (room.type == "通常部屋")
        {
            room.enemies = {"スライム", "ゴブリン"};
            room.treasures = {"小銭"};
        }
        else if (room.type == "宝物庫")
        {
            room.enemies = {"宝箱ミミック"};
            room.treasures = {"金貨", "魔法のアイテム"};
        }
        else if (room.type == "ボス部屋")
        {
            room.enemies = {"ドラゴン", "魔王"};
            room.treasures = {"伝説の武器", "大量の宝石"};
        }
        else if (room.type == "休憩室")
        {
            room.enemies = {};
            room.treasures = {"回復アイテム"};
        }
        
        co_yield room;
    }
}

// フィルタ機能付きジェネレーター
template<typename T, typename Predicate>
Generator<T> filter(Generator<T>& source, Predicate pred)
{
    for (auto value : source)
    {
        if (pred(value))
        {
            co_yield value;
        }
    }
}

// 変換機能付きジェネレーター
template<typename T, typename U, typename Transform>
Generator<U> map(Generator<T>& source, Transform transform)
{
    for (auto value : source)
    {
        co_yield transform(value);
    }
}

// 複数ジェネレーターの組み合わせ
template<typename T>
Generator<T> zip(Generator<T>& gen1, Generator<T>& gen2)
{
    auto it1 = gen1.begin();
    auto it2 = gen2.begin();
    auto end1 = gen1.end();
    auto end2 = gen2.end();
    
    while (it1 != end1 && it2 != end2)
    {
        co_yield *it1;
        co_yield *it2;
        ++it1;
        ++it2;
    }
}

int main()
{
    std::cout << "=== ジェネレーター（Generator）===" << std::endl;
    
    // 1. 基本的な範囲
    std::cout << "\n--- 基本的な範囲 ---" << std::endl;
    auto rangeGen = range(1, 10, 2);
    for (auto value : rangeGen)
    {
        std::cout << value << " ";
    }
    std::cout << std::endl;
    
    // 2. 素数生成（最初の10個）
    std::cout << "\n--- 素数生成 ---" << std::endl;
    auto primes = primeNumbers();
    int count = 0;
    for (auto prime : primes)
    {
        std::cout << prime << " ";
        if (++count >= 10) break;
    }
    std::cout << std::endl;
    
    // 3. ランダムイベント
    std::cout << "\n--- ランダムイベント ---" << std::endl;
    auto events = randomEventGenerator();
    count = 0;
    for (auto event : events)
    {
        std::cout << "イベント: " << event.type << " (重要度: " << event.severity 
                  << ") - " << event.description << std::endl;
        if (++count >= 5) break;
    }
    
    // 4. 音波生成（最初の10サンプル）
    std::cout << "\n--- 音波生成 ---" << std::endl;
    auto wave = sineWave(440.0f, 0.5f, 0.001f);  // ラの音、短時間
    count = 0;
    for (auto sample : wave)
    {
        std::cout << "サンプル " << count << ": " << sample << std::endl;
        if (++count >= 10) break;
    }
    
    // 5. フラクタルポイント（最初の5ポイント）
    std::cout << "\n--- フラクタルポイント ---" << std::endl;
    auto fractal = mandelbrotPoints(20, 20, 0.5f);
    count = 0;
    for (auto point : fractal)
    {
        std::cout << "ポイント: (" << point.x << ", " << point.y << ")" << std::endl;
        if (++count >= 5) break;
    }
    
    // 6. ダンジョン生成
    std::cout << "\n--- ダンジョン生成 ---" << std::endl;
    auto dungeon = dungeonGenerator(5);
    for (auto room : dungeon)
    {
        std::cout << "部屋 " << room.id << ": " << room.type 
                  << " (難易度: " << room.difficulty << ")" << std::endl;
        std::cout << "  敵: ";
        for (const auto& enemy : room.enemies)
        {
            std::cout << enemy << " ";
        }
        std::cout << std::endl;
        std::cout << "  宝物: ";
        for (const auto& treasure : room.treasures)
        {
            std::cout << treasure << " ";
        }
        std::cout << std::endl;
    }
    
    // 7. フィルタリング
    std::cout << "\n--- フィルタリング例 ---" << std::endl;
    auto numbers = range(1, 20);
    auto evenNumbers = filter(numbers, [](int x) { return x % 2 == 0; });
    
    std::cout << "偶数のみ: ";
    for (auto value : evenNumbers)
    {
        std::cout << value << " ";
    }
    std::cout << std::endl;
    
    // 8. 無限シーケンス（制限付き）
    std::cout << "\n--- 無限シーケンス ---" << std::endl;
    auto infinite = infiniteSequence(100);
    count = 0;
    for (auto value : infinite)
    {
        std::cout << value << " ";
        if (++count >= 8) break;
    }
    std::cout << std::endl;
    
    std::cout << "\n=== ジェネレーターの利点 ===" << std::endl;
    std::cout << "- 遅延評価：必要なときにのみ値を生成" << std::endl;
    std::cout << "- メモリ効率：全データを保持しない" << std::endl;
    std::cout << "- 無限シーケンス：無制限のデータ生成" << std::endl;
    std::cout << "- 合成可能：フィルタリングや変換を組み合わせ" << std::endl;
    std::cout << "- ゲーム開発：レベル生成、イベント、エフェクトに最適" << std::endl;
    
    return 0;
}

// まとめ：
// - ジェネレーターで効率的な無限シーケンス生成
// - co_yield で値を一つずつ生成
// - 遅延評価でメモリ効率を最大化
// - フィルタリングや変換で柔軟なデータ処理
// - ゲーム開発では動的コンテンツ生成に威力を発揮