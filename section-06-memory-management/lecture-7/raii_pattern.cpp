// RAIIパターンの活用
// ゲーム開発でのリソース管理の自動化

#include <iostream>
#include <memory>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <mutex>
#include <thread>

using namespace std;
using namespace chrono;

// ゲームアセット管理のRAIIクラス
class GameAsset
{
private:
    string filename;
    char* data;
    size_t size;
    bool isLoaded;
    
public:
    GameAsset(const string& filename) : filename(filename), data(nullptr), size(0), isLoaded(false)
    {
        load();
    }
    
    ~GameAsset()
    {
        unload();
    }
    
    // コピー禁止（リソースの二重管理を防ぐ）
    GameAsset(const GameAsset&) = delete;
    GameAsset& operator=(const GameAsset&) = delete;
    
    // ムーブセマンティクス
    GameAsset(GameAsset&& other) noexcept
        : filename(move(other.filename)), data(other.data), size(other.size), isLoaded(other.isLoaded)
    {
        other.data = nullptr;
        other.size = 0;
        other.isLoaded = false;
        cout << "アセット '" << filename << "' をムーブ\n";
    }
    
    GameAsset& operator=(GameAsset&& other) noexcept
    {
        if (this != &other)
        {
            unload();
            
            filename = move(other.filename);
            data = other.data;
            size = other.size;
            isLoaded = other.isLoaded;
            
            other.data = nullptr;
            other.size = 0;
            other.isLoaded = false;
        }
        return *this;
    }
    
    bool isValid() const { return isLoaded && data != nullptr; }
    size_t getSize() const { return size; }
    const string& getFilename() const { return filename; }
    
private:
    void load()
    {
        cout << "アセット '" << filename << "' をロード中...\n";
        
        // シミュレートされたファイルサイズ
        size = 1024 + (filename.length() * 100);
        data = new char[size];
        
        if (data)
        {
            isLoaded = true;
            cout << "アセット '" << filename << "' ロード完了 (サイズ: " << size << " bytes)\n";
        }
        else
        {
            cout << "アセット '" << filename << "' のロードに失敗\n";
        }
    }
    
    void unload()
    {
        if (data)
        {
            delete[] data;
            data = nullptr;
            cout << "アセット '" << filename << "' をアンロード\n";
        }
        isLoaded = false;
    }
};

// ファイル管理のRAIIクラス
class GameFileManager
{
private:
    ofstream file;
    string filename;
    bool isOpen;
    
public:
    GameFileManager(const string& filename) : filename(filename), isOpen(false)
    {
        file.open(filename);
        if (file.is_open())
        {
            isOpen = true;
            cout << "セーブファイル '" << filename << "' をオープン\n";
        }
        else
        {
            cout << "セーブファイル '" << filename << "' のオープンに失敗\n";
        }
    }
    
    ~GameFileManager()
    {
        if (isOpen)
        {
            file.close();
            cout << "セーブファイル '" << filename << "' をクローズ\n";
        }
    }
    
    // コピー禁止
    GameFileManager(const GameFileManager&) = delete;
    GameFileManager& operator=(const GameFileManager&) = delete;
    
    void writePlayerData(const string& playerName, int level, int score)
    {
        if (isOpen)
        {
            file << "Player: " << playerName << "\n";
            file << "Level: " << level << "\n";
            file << "Score: " << score << "\n";
            file << "---\n";
            cout << "プレイヤーデータを保存: " << playerName << "\n";
        }
    }
    
    bool isFileOpen() const { return isOpen; }
};

// タイマー管理のRAIIクラス
class GameTimer
{
private:
    steady_clock::time_point startTime;
    string timerName;
    
public:
    GameTimer(const string& name) : timerName(name), startTime(steady_clock::now())
    {
        cout << "タイマー '" << timerName << "' を開始\n";
    }
    
    ~GameTimer()
    {
        auto endTime = steady_clock::now();
        auto duration = duration_cast<milliseconds>(endTime - startTime);
        cout << "タイマー '" << timerName << "' 終了: " << duration.count() << "ms\n";
    }
    
    // コピー禁止
    GameTimer(const GameTimer&) = delete;
    GameTimer& operator=(const GameTimer&) = delete;
};

// ミューテックス管理のRAIIクラス
class GameMutexLock
{
private:
    mutex& mtx;
    bool isLocked;
    
public:
    GameMutexLock(mutex& m, const string& operation) : mtx(m), isLocked(false)
    {
        mtx.lock();
        isLocked = true;
        cout << "ミューテックスをロック: " << operation << "\n";
    }
    
    ~GameMutexLock()
    {
        if (isLocked)
        {
            mtx.unlock();
            cout << "ミューテックスをアンロック\n";
        }
    }
    
    // コピー禁止
    GameMutexLock(const GameMutexLock&) = delete;
    GameMutexLock& operator=(const GameMutexLock&) = delete;
};

// ゲームシーン管理のRAIIクラス
class GameScene
{
private:
    string sceneName;
    vector<unique_ptr<GameAsset>> assets;
    bool isActive;
    
public:
    GameScene(const string& name) : sceneName(name), isActive(false)
    {
        cout << "\n=== シーン '" << sceneName << "' を初期化 ===\n";
        loadScene();
    }
    
    ~GameScene()
    {
        cout << "=== シーン '" << sceneName << "' を終了 ===\n";
        unloadScene();
    }
    
    void addAsset(const string& filename)
    {
        if (isActive)
        {
            assets.push_back(make_unique<GameAsset>(filename));
        }
    }
    
    void processScene()
    {
        if (isActive)
        {
            GameTimer timer(sceneName + "_processing");
            
            cout << "\nシーン '" << sceneName << "' を処理中...\n";
            
            // ゲームロジックのシミュレーション
            this_thread::sleep_for(milliseconds(100));
            
            cout << "シーン処理完了\n";
        }
    }
    
private:
    void loadScene()
    {
        GameTimer timer(sceneName + "_loading");
        
        // シーンの基本アセットをロード
        assets.push_back(make_unique<GameAsset>(sceneName + "_background.png"));
        assets.push_back(make_unique<GameAsset>(sceneName + "_music.ogg"));
        
        isActive = true;
        cout << "シーン '" << sceneName << "' のロード完了\n";
    }
    
    void unloadScene()
    {
        isActive = false;
        assets.clear();  // 自動的にすべてのアセットがアンロードされる
        cout << "シーン '" << sceneName << "' のアンロード完了\n";
    }
};

// リソースプールのRAII実装
template<typename T>
class ResourcePool
{
private:
    vector<unique_ptr<T>> pool;
    vector<T*> available;
    size_t maxSize;
    string poolName;
    
public:
    ResourcePool(const string& name, size_t max) : poolName(name), maxSize(max)
    {
        cout << "リソースプール '" << poolName << "' を作成 (最大サイズ: " << maxSize << ")\n";
        
        // プールを事前に初期化
        for (size_t i = 0; i < maxSize; ++i)
        {
            auto resource = make_unique<T>();
            available.push_back(resource.get());
            pool.push_back(move(resource));
        }
    }
    
    ~ResourcePool()
    {
        cout << "リソースプール '" << poolName << "' を解放\n";
    }
    
    // RAIIリソースハンドル
    class ResourceHandle
    {
    private:
        T* resource;
        ResourcePool<T>* pool;
        
    public:
        ResourceHandle(T* res, ResourcePool<T>* p) : resource(res), pool(p)
        {
            cout << "リソースを取得\n";
        }
        
        ~ResourceHandle()
        {
            if (resource && pool)
            {
                pool->returnResource(resource);
                cout << "リソースを返却\n";
            }
        }
        
        // コピー禁止
        ResourceHandle(const ResourceHandle&) = delete;
        ResourceHandle& operator=(const ResourceHandle&) = delete;
        
        // ムーブセマンティクス
        ResourceHandle(ResourceHandle&& other) noexcept
            : resource(other.resource), pool(other.pool)
        {
            other.resource = nullptr;
            other.pool = nullptr;
        }
        
        ResourceHandle& operator=(ResourceHandle&& other) noexcept
        {
            if (this != &other)
            {
                if (resource && pool)
                {
                    pool->returnResource(resource);
                }
                
                resource = other.resource;
                pool = other.pool;
                
                other.resource = nullptr;
                other.pool = nullptr;
            }
            return *this;
        }
        
        T* get() const { return resource; }
        T& operator*() const { return *resource; }
        T* operator->() const { return resource; }
        bool isValid() const { return resource != nullptr; }
    };
    
    ResourceHandle acquire()
    {
        if (available.empty())
        {
            cout << "リソースプール '" << poolName << "' が空です\n";
            return ResourceHandle(nullptr, this);
        }
        
        T* resource = available.back();
        available.pop_back();
        return ResourceHandle(resource, this);
    }
    
    void returnResource(T* resource)
    {
        available.push_back(resource);
    }
    
    size_t availableCount() const { return available.size(); }
    size_t totalCount() const { return pool.size(); }
};

// ゲームエフェクトクラス（プール用）
class GameEffect
{
public:
    string effectName;
    bool isActive;
    
    GameEffect() : effectName("未設定"), isActive(false) {}
    
    void initialize(const string& name)
    {
        effectName = name;
        isActive = true;
        cout << "エフェクト '" << effectName << "' を初期化\n";
    }
    
    void play()
    {
        if (isActive)
        {
            cout << "エフェクト '" << effectName << "' を再生\n";
        }
    }
    
    void reset()
    {
        effectName = "未設定";
        isActive = false;
        cout << "エフェクトをリセット\n";
    }
};

// RAIIパターンの基本例
void basicRAIIDemo()
{
    cout << "\n=== RAIIパターンの基本例 ===\n";
    
    {
        cout << "\nスコープ開始\n";
        
        // アセット管理
        GameAsset playerTexture("player_sprite.png");
        GameAsset weaponModel("sword_model.obj");
        
        // ファイル管理
        GameFileManager saveFile("game_save.dat");
        if (saveFile.isFileOpen())
        {
            saveFile.writePlayerData("勇者", 25, 15000);
            saveFile.writePlayerData("魔法使い", 20, 12000);
        }
        
        // タイマー管理
        {
            GameTimer levelTimer("レベルクリア時間");
            this_thread::sleep_for(milliseconds(50));
        }
        
        cout "スコープ終了直前\n";
    }
    
    cout << "スコープ外 - すべてのリソースが自動的に解放された\n";
}

// シーン管理の例
void sceneManagementDemo()
{
    cout << "\n=== シーン管理の例 ===\n";
    
    {
        GameScene menuScene("メインメニュー");
        menuScene.addAsset("button_texture.png");
        menuScene.addAsset("menu_bgm.ogg");
        menuScene.processScene();
    }
    
    {
        GameScene gameplayScene("バトルシーン");
        gameplayScene.addAsset("battle_background.png");
        gameplayScene.addAsset("battle_music.ogg");
        gameplayScene.addAsset("effect_explosion.png");
        gameplayScene.processScene();
    }
    
    cout << "\nすべてのシーンが正しくクリーンアップされた\n";
}

// リソースプールの例
void resourcePoolDemo()
{
    cout << "\n=== リソースプールの例 ===\n";
    
    ResourcePool<GameEffect> effectPool("エフェクトプール", 3);
    
    cout << "\nプールからリソースを取得:\n";
    
    {
        auto effect1 = effectPool.acquire();
        if (effect1.isValid())
        {
            effect1->initialize("爆発エフェクト");
            effect1->play();
        }
        
        auto effect2 = effectPool.acquire();
        if (effect2.isValid())
        {
            effect2->initialize("火球エフェクト");
            effect2->play();
        }
        
        cout << "利用可能リソース: " << effectPool.availableCount() << "/" << effectPool.totalCount() << "\n";
        
        {
            auto effect3 = effectPool.acquire();
            if (effect3.isValid())
            {
                effect3->initialize("氷エフェクト");
                effect3->play();
            }
            
            // プールが空になった状態で新しいリソースを要求
            auto effect4 = effectPool.acquire();
            if (!effect4.isValid())
            {
                cout << "プールが空のため、リソースを取得できません\n";
            }
            
            cout << "利用可能リソース: " << effectPool.availableCount() << "/" << effectPool.totalCount() << "\n";
        }
        
        cout << "\neffect3がスコープを抜けた後の利用可能リソース: " 
             << effectPool.availableCount() << "/" << effectPool.totalCount() << "\n";
    }
    
    cout << "\nすべてのリソースが返却された後の利用可能リソース: " 
         << effectPool.availableCount() << "/" << effectPool.totalCount() << "\n";
}

// マルチスレッドでのRAII
void multithreadedRAIIDemo()
{
    cout << "\n=== マルチスレッドでのRAII ===\n";
    
    mutex gameMutex;
    int sharedResource = 0;
    
    auto workerThread = [&gameMutex, &sharedResource](int threadId) {
        {
            GameMutexLock lock(gameMutex, "スレッド" + to_string(threadId) + "の処理");
            
            int oldValue = sharedResource;
            this_thread::sleep_for(milliseconds(50));
            sharedResource = oldValue + threadId;
            
            cout << "スレッド" << threadId << ": " << oldValue << " -> " << sharedResource << "\n";
        }
        // スコープを抜けると自動的にミューテックスがアンロックされる
    };
    
    vector<thread> threads;
    for (int i = 1; i <= 3; ++i)
    {
        threads.emplace_back(workerThread, i);
    }
    
    for (auto& t : threads)
    {
        t.join();
    }
    
    cout << "最終的な共有リソースの値: " << sharedResource << "\n";
}

int main()
{
    cout << "=== RAIIパターンの活用 ===\n";
    
    // 基本的なRAIIの例
    basicRAIIDemo();
    
    // シーン管理の例
    sceneManagementDemo();
    
    // リソースプールの例
    resourcePoolDemo();
    
    // マルチスレッドでのRAII
    multithreadedRAIIDemo();
    
    cout << "\n=== RAIIパターンのメリット ===\n";
    cout << "1. リソースのライフサイクルをスコープと結び付け\n";
    cout << "2. 例外が発生してもリソースが確実に解放される\n";
    cout << "3. メモリリークやリソースリークを防止\n";
    cout << "4. コードの可読性と保守性を向上\n";
    cout << "5. マルチスレッド環境での安全性向上\n";
    
    cout << "\n=== ゲーム開発での活用例 ===\n";
    cout << "1. アセットマネージャーでのリソース管理\n";
    cout << "2. シーンやレベルのライフサイクル管理\n";
    cout << "3. グラフィックスリソース（テクスチャ、バッファ等）の管理\n";
    cout << "4. オーディオシステムでのサウンドリソース管理\n";
    cout << "5. ネットワーク接続やファイルI/Oの管理\n";
    cout << "6. ゲームオブジェクトプールやパーティクルシステム\n";
    cout << "7. ロック管理やスレッドセーフなリソースアクセス\n";
    
    return 0;
}

// RAIIパターンの重要なポイント：
// 1. コンストラクタでリソースを取得（Resource Acquisition Is Initialization）
// 2. デストラクタでリソースを解放（スコープベースの管理）
// 3. コピーや代入を適切に制限（リソースの二重解放を防ぐ）
// 4. 例外安全性を確保（スタックアンワインド時のクリーンアップ）
// 5. ムーブセマンティクスで効率的なリソース移動を実現