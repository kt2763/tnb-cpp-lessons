// スレッドの基礎
// std::threadを使った基本的な並行プログラミング

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <functional>
#include <string>

// 基本的なスレッド関数
void simpleTask(int id)
{
    std::cout << "スレッド " << id << " が開始されました" << std::endl;
    
    // 何らかの処理をシミュレート
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    std::cout << "スレッド " << id << " が完了しました" << std::endl;
}

// パラメータ付きの関数
void parameterizedTask(const std::string& name, int iterations)
{
    for (int i = 0; i < iterations; ++i)
    {
        std::cout << name << ": " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

// ゲームでの使用例：バックグラウンド処理
class GameEngine
{
private:
    bool running_;
    std::thread backgroundThread_;
    
public:
    GameEngine() : running_(false) {}
    
    void start()
    {
        running_ = true;
        backgroundThread_ = std::thread(&GameEngine::backgroundLoop, this);
        std::cout << "ゲームエンジンが開始されました" << std::endl;
    }
    
    void stop()
    {
        running_ = false;
        if (backgroundThread_.joinable())
        {
            backgroundThread_.join();
        }
        std::cout << "ゲームエンジンが停止しました" << std::endl;
    }
    
private:
    void backgroundLoop()
    {
        int frame = 0;
        while (running_)
        {
            // バックグラウンド処理（AI思考、物理演算など）
            processAI();
            updatePhysics();
            
            frame++;
            if (frame % 60 == 0)  // 1秒ごとに表示
            {
                std::cout << "バックグラウンド処理: フレーム " << frame << std::endl;
            }
            
            // 16ms = 60FPS相当
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }
    
    void processAI()
    {
        // AI処理のシミュレーション
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    
    void updatePhysics()
    {
        // 物理演算のシミュレーション
        std::this_thread::sleep_for(std::chrono::microseconds(50));
    }
};

// リソースローダー
class ResourceLoader
{
public:
    void loadTexture(const std::string& filename)
    {
        std::cout << "テクスチャ読み込み開始: " << filename << std::endl;
        
        // ファイル読み込みのシミュレーション
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
        
        std::cout << "テクスチャ読み込み完了: " << filename << std::endl;
    }
    
    void loadSound(const std::string& filename)
    {
        std::cout << "サウンド読み込み開始: " << filename << std::endl;
        
        // ファイル読み込みのシミュレーション
        std::this_thread::sleep_for(std::chrono::milliseconds(600));
        
        std::cout << "サウンド読み込み完了: " << filename << std::endl;
    }
    
    void loadModel(const std::string& filename)
    {
        std::cout << "3Dモデル読み込み開始: " << filename << std::endl;
        
        // ファイル読み込みのシミュレーション
        std::this_thread::sleep_for(std::chrono::milliseconds(1200));
        
        std::cout << "3Dモデル読み込み完了: " << filename << std::endl;
    }
};

// ラムダ式とスレッド
void demonstrateLambdaThreads()
{
    std::cout << "\n=== ラムダ式を使ったスレッド ===" << std::endl;
    
    // キャプチャ付きラムダ
    std::string message = "ラムダからのメッセージ";
    int count = 3;
    
    std::thread lambdaThread([message, count]() {
        for (int i = 0; i < count; ++i)
        {
            std::cout << message << " " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    });
    
    lambdaThread.join();
}

// 複数スレッドの管理
void demonstrateMultipleThreads()
{
    std::cout << "\n=== 複数スレッドの管理 ===" << std::endl;
    
    std::vector<std::thread> threads;
    
    // 複数のスレッドを作成
    for (int i = 0; i < 4; ++i)
    {
        threads.emplace_back([i]() {
            std::cout << "ワーカースレッド " << i << " 開始" << std::endl;
            
            // 何らかの処理
            for (int j = 0; j < 3; ++j)
            {
                std::cout << "  スレッド " << i << " : 処理 " << j << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
            
            std::cout << "ワーカースレッド " << i << " 完了" << std::endl;
        });
    }
    
    // すべてのスレッドの完了を待つ
    for (auto& t : threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
    
    std::cout << "すべてのワーカースレッドが完了しました" << std::endl;
}

// スレッドIDの取得
void demonstrateThreadInfo()
{
    std::cout << "\n=== スレッド情報 ===" << std::endl;
    
    std::cout << "メインスレッドID: " << std::this_thread::get_id() << std::endl;
    std::cout << "ハードウェアスレッド数: " << std::thread::hardware_concurrency() << std::endl;
    
    std::thread infoThread([]() {
        std::cout << "子スレッドID: " << std::this_thread::get_id() << std::endl;
    });
    
    infoThread.join();
}

// detachの使用例（注意が必要）
void demonstrateDetach()
{
    std::cout << "\n=== detachの使用例 ===" << std::endl;
    
    // 注意：detachしたスレッドはプログラム終了時に強制終了される
    std::thread detachedThread([]() {
        for (int i = 0; i < 5; ++i)
        {
            std::cout << "Detached thread: " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    });
    
    // スレッドをdetach（親スレッドから独立）
    detachedThread.detach();
    
    // メインスレッドはしばらく待機
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    std::cout << "メインスレッド終了準備" << std::endl;
}

// 例外処理とスレッド
void demonstrateExceptionHandling()
{
    std::cout << "\n=== スレッドでの例外処理 ===" << std::endl;
    
    std::thread exceptionThread([]() {
        try
        {
            std::cout << "例外をスローするスレッド" << std::endl;
            throw std::runtime_error("スレッド内でエラーが発生");
        }
        catch (const std::exception& e)
        {
            std::cout << "スレッド内でキャッチ: " << e.what() << std::endl;
        }
    });
    
    exceptionThread.join();
}

// ゲームでの実践例：並列リソース読み込み
void demonstrateParallelLoading()
{
    std::cout << "\n=== 並列リソース読み込み ===" << std::endl;
    
    ResourceLoader loader;
    std::vector<std::thread> loadingThreads;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // 複数のリソースを並列で読み込み
    loadingThreads.emplace_back([&loader]() {
        loader.loadTexture("player.png");
    });
    
    loadingThreads.emplace_back([&loader]() {
        loader.loadTexture("enemy.png");
    });
    
    loadingThreads.emplace_back([&loader]() {
        loader.loadSound("bgm.mp3");
    });
    
    loadingThreads.emplace_back([&loader]() {
        loader.loadModel("character.obj");
    });
    
    // すべての読み込み完了を待つ
    for (auto& thread : loadingThreads)
    {
        thread.join();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "全リソース読み込み完了: " << duration.count() << "ms" << std::endl;
}

int main()
{
    std::cout << "=== スレッドの基礎 ===" << std::endl;
    
    // 1. 基本的なスレッド作成
    std::cout << "\n--- 基本的なスレッド ---" << std::endl;
    std::thread t1(simpleTask, 1);
    std::thread t2(simpleTask, 2);
    
    // スレッドの完了を待つ
    t1.join();
    t2.join();
    
    // 2. パラメータ付きスレッド
    std::cout << "\n--- パラメータ付きスレッド ---" << std::endl;
    std::thread t3(parameterizedTask, "カウンター", 3);
    t3.join();
    
    // 3. クラスメンバー関数
    std::cout << "\n--- ゲームエンジンの例 ---" << std::endl;
    GameEngine engine;
    engine.start();
    
    // メインスレッドで他の処理
    for (int i = 0; i < 3; ++i)
    {
        std::cout << "メインスレッド処理: " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
    }
    
    engine.stop();
    
    // 4. ラムダ式
    demonstrateLambdaThreads();
    
    // 5. 複数スレッド管理
    demonstrateMultipleThreads();
    
    // 6. スレッド情報
    demonstrateThreadInfo();
    
    // 7. detachの使用例
    demonstrateDetach();
    
    // 8. 例外処理
    demonstrateExceptionHandling();
    
    // 9. 並列リソース読み込み
    demonstrateParallelLoading();
    
    std::cout << "\nプログラム終了" << std::endl;
    
    return 0;
}

// まとめ：
// - std::threadで並行処理を実現
// - join()で完了待ち、detach()で独立実行
// - ラムダ式やメンバー関数も使用可能
// - 複数スレッドを管理してパフォーマンス向上
// - ゲーム開発では背景処理やリソース読み込みに活用