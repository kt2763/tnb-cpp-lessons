// Future と Promise
// 非同期処理の結果を効率的に取得

#include <iostream>
#include <future>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <random>
#include <exception>

// 基本的なfutureの使用
int longRunningCalculation(int input)
{
    std::cout << "長時間計算開始: " << input << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    int result = input * input + 42;
    std::cout << "長時間計算完了: " << result << std::endl;
    return result;
}

// 例外を投げる関数
int riskyCalculation(int input)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    if (input < 0)
    {
        throw std::invalid_argument("負の値は処理できません");
    }
    
    return input * 10;
}

// ゲームでの使用例：リソースローダー
class GameResourceLoader
{
public:
    struct TextureData
    {
        std::string filename;
        int width, height;
        std::vector<uint8_t> data;
        
        TextureData(const std::string& name, int w, int h)
            : filename(name), width(w), height(h), data(w * h * 4, 255) {}
    };
    
    struct SoundData
    {
        std::string filename;
        int sampleRate;
        std::vector<int16_t> samples;
        
        SoundData(const std::string& name, int rate)
            : filename(name), sampleRate(rate), samples(rate, 0) {}
    };
    
    static std::future<TextureData> loadTextureAsync(const std::string& filename)
    {
        return std::async(std::launch::async, [filename]() {
            std::cout << "テクスチャ読み込み開始: " << filename << std::endl;
            
            // ファイル読み込みをシミュレート
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
            
            // ランダムなサイズ
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> sizeDist(256, 1024);
            
            int width = sizeDist(gen);
            int height = sizeDist(gen);
            
            std::cout << "テクスチャ読み込み完了: " << filename 
                      << " (" << width << "x" << height << ")" << std::endl;
            
            return TextureData(filename, width, height);
        });
    }
    
    static std::future<SoundData> loadSoundAsync(const std::string& filename)
    {
        return std::async(std::launch::async, [filename]() {
            std::cout << "サウンド読み込み開始: " << filename << std::endl;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(600));
            
            std::cout << "サウンド読み込み完了: " << filename << std::endl;
            return SoundData(filename, 44100);
        });
    }
    
    static std::future<bool> validateResourceAsync(const std::string& filename)
    {
        return std::async(std::launch::async, [filename]() {
            std::cout << "リソース検証開始: " << filename << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            
            // ランダムに失敗することがある
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(1, 10);
            
            bool valid = dist(gen) > 2;  // 80%の確率で成功
            
            std::cout << "リソース検証" << (valid ? "成功" : "失敗") 
                      << ": " << filename << std::endl;
            return valid;
        });
    }
};

// Promise を使ったカスタム非同期処理
class AsyncTaskManager
{
private:
    struct TaskResult
    {
        int taskId;
        std::string result;
        bool success;
    };
    
public:
    static std::future<TaskResult> executeTask(int taskId, int complexity)
    {
        auto promise = std::make_shared<std::promise<TaskResult>>();
        auto future = promise->get_future();
        
        // 別スレッドでタスク実行
        std::thread([promise, taskId, complexity]() {
            try
            {
                std::cout << "タスク " << taskId << " 実行開始 (複雑度: " << complexity << ")" << std::endl;
                
                // 複雑度に応じた処理時間
                std::this_thread::sleep_for(std::chrono::milliseconds(complexity * 100));
                
                // ランダムに失敗することがある
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dist(1, 10);
                
                if (dist(gen) <= 2)  // 20%の確率で失敗
                {
                    throw std::runtime_error("タスク実行中にエラーが発生");
                }
                
                TaskResult result;
                result.taskId = taskId;
                result.result = "タスク " + std::to_string(taskId) + " 完了";
                result.success = true;
                
                std::cout << "タスク " << taskId << " 完了" << std::endl;
                promise->set_value(result);
            }
            catch (const std::exception& e)
            {
                std::cout << "タスク " << taskId << " エラー: " << e.what() << std::endl;
                promise->set_exception(std::current_exception());
            }
        }).detach();
        
        return future;
    }
};

// バッチ処理システム
class BatchProcessor
{
public:
    template<typename Func, typename... Args>
    static auto processBatch(const std::vector<std::tuple<Args...>>& inputs, Func func)
        -> std::vector<std::future<decltype(std::apply(func, inputs[0]))>>
    {
        std::vector<std::future<decltype(std::apply(func, inputs[0]))>> futures;
        
        for (const auto& input : inputs)
        {
            futures.push_back(std::async(std::launch::async, [func, input]() {
                return std::apply(func, input);
            }));
        }
        
        return futures;
    }
    
    template<typename T>
    static std::vector<T> waitForAll(std::vector<std::future<T>>& futures)
    {
        std::vector<T> results;
        results.reserve(futures.size());
        
        for (auto& fut : futures)
        {
            try
            {
                results.push_back(fut.get());
            }
            catch (const std::exception& e)
            {
                std::cout << "バッチ処理エラー: " << e.what() << std::endl;
                // デフォルト値を使用するか、エラーを再スローするかを選択
                throw;
            }
        }
        
        return results;
    }
};

// ゲームの初期化システム
class GameInitializer
{
public:
    struct InitResult
    {
        bool success;
        std::string message;
        std::chrono::milliseconds duration;
    };
    
    static std::future<InitResult> initializeGraphics()
    {
        return std::async(std::launch::async, []() {
            auto start = std::chrono::steady_clock::now();
            
            std::cout << "グラフィックス初期化開始..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1200));
            
            auto end = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            std::cout << "グラフィックス初期化完了" << std::endl;
            return InitResult{true, "グラフィックス初期化成功", duration};
        });
    }
    
    static std::future<InitResult> initializeAudio()
    {
        return std::async(std::launch::async, []() {
            auto start = std::chrono::steady_clock::now();
            
            std::cout << "オーディオ初期化開始..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(600));
            
            auto end = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            std::cout << "オーディオ初期化完了" << std::endl;
            return InitResult{true, "オーディオ初期化成功", duration};
        });
    }
    
    static std::future<InitResult> initializeNetwork()
    {
        return std::async(std::launch::async, []() {
            auto start = std::chrono::steady_clock::now();
            
            std::cout << "ネットワーク初期化開始..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
            
            auto end = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            std::cout << "ネットワーク初期化完了" << std::endl;
            return InitResult{true, "ネットワーク初期化成功", duration};
        });
    }
};

// 実演関数
void demonstrateBasicFuture()
{
    std::cout << "\n=== 基本的なFuture ===" << std::endl;
    
    // std::asyncを使用
    auto future1 = std::async(std::launch::async, longRunningCalculation, 10);
    auto future2 = std::async(std::launch::async, longRunningCalculation, 20);
    
    std::cout << "他の処理を実行中..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // 結果を取得
    std::cout << "結果1: " << future1.get() << std::endl;
    std::cout << "結果2: " << future2.get() << std::endl;
}

void demonstrateExceptionHandling()
{
    std::cout << "\n=== 例外処理 ===" << std::endl;
    
    auto future1 = std::async(std::launch::async, riskyCalculation, 5);
    auto future2 = std::async(std::launch::async, riskyCalculation, -3);
    
    try
    {
        std::cout << "結果1: " << future1.get() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "エラー1: " << e.what() << std::endl;
    }
    
    try
    {
        std::cout << "結果2: " << future2.get() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "エラー2: " << e.what() << std::endl;
    }
}

void demonstrateResourceLoading()
{
    std::cout << "\n=== 並列リソース読み込み ===" << std::endl;
    
    auto textureTask = GameResourceLoader::loadTextureAsync("hero.png");
    auto soundTask = GameResourceLoader::loadSoundAsync("bgm.mp3");
    auto validationTask = GameResourceLoader::validateResourceAsync("config.json");
    
    std::cout << "他の初期化処理を実行中..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    
    // すべての結果を取得
    try
    {
        auto texture = textureTask.get();
        std::cout << "テクスチャ準備完了: " << texture.filename 
                  << " (" << texture.width << "x" << texture.height << ")" << std::endl;
        
        auto sound = soundTask.get();
        std::cout << "サウンド準備完了: " << sound.filename 
                  << " (" << sound.sampleRate << "Hz)" << std::endl;
        
        bool isValid = validationTask.get();
        std::cout << "設定ファイル検証: " << (isValid ? "成功" : "失敗") << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "リソース読み込みエラー: " << e.what() << std::endl;
    }
}

void demonstrateGameInitialization()
{
    std::cout << "\n=== ゲーム初期化 ===" << std::endl;
    
    auto graphicsInit = GameInitializer::initializeGraphics();
    auto audioInit = GameInitializer::initializeAudio();
    auto networkInit = GameInitializer::initializeNetwork();
    
    std::cout << "初期化処理を並列実行中..." << std::endl;
    
    // すべての初期化完了を待つ
    try
    {
        auto graphicsResult = graphicsInit.get();
        auto audioResult = audioInit.get();
        auto networkResult = networkInit.get();
        
        std::cout << "\n=== 初期化結果 ===" << std::endl;
        std::cout << graphicsResult.message << " (" 
                  << graphicsResult.duration.count() << "ms)" << std::endl;
        std::cout << audioResult.message << " (" 
                  << audioResult.duration.count() << "ms)" << std::endl;
        std::cout << networkResult.message << " (" 
                  << networkResult.duration.count() << "ms)" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "初期化エラー: " << e.what() << std::endl;
    }
}

int main()
{
    std::cout << "=== Future と Promise ===" << std::endl;
    
    // 1. 基本的なFuture
    demonstrateBasicFuture();
    
    // 2. 例外処理
    demonstrateExceptionHandling();
    
    // 3. リソース読み込み
    demonstrateResourceLoading();
    
    // 4. Promise を使ったタスク管理
    std::cout << "\n=== タスクマネージャー ===" << std::endl;
    std::vector<std::future<AsyncTaskManager::TaskResult>> taskFutures;
    
    for (int i = 1; i <= 5; ++i)
    {
        taskFutures.push_back(AsyncTaskManager::executeTask(i, i));
    }
    
    for (auto& fut : taskFutures)
    {
        try
        {
            auto result = fut.get();
            std::cout << "タスク結果: " << result.result << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cout << "タスクエラー: " << e.what() << std::endl;
        }
    }
    
    // 5. ゲーム初期化
    demonstrateGameInitialization();
    
    // 6. バッチ処理
    std::cout << "\n=== バッチ処理 ===" << std::endl;
    std::vector<std::tuple<int>> inputs = {{1}, {2}, {3}, {4}, {5}};
    
    auto batchFutures = BatchProcessor::processBatch(inputs, [](int x) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return x * x;
    });
    
    try
    {
        auto results = BatchProcessor::waitForAll(batchFutures);
        std::cout << "バッチ処理結果: ";
        for (auto result : results)
        {
            std::cout << result << " ";
        }
        std::cout << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "バッチ処理エラー: " << e.what() << std::endl;
    }
    
    return 0;
}

// まとめ：
// - std::futureで非同期処理の結果を取得
// - std::asyncで簡単に非同期タスクを実行
// - std::promiseで手動制御の非同期処理
// - 例外も非同期で伝播される
// - ゲーム開発では初期化やリソース読み込みに活用