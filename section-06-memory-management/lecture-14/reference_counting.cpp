// 参照カウントシステムの実装
// ゲーム開発でのシェアードリソース管理

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <atomic>
#include <thread>
#include <chrono>
#include <map>

using namespace std;
using namespace chrono;

// シンプルな参照カウントクラス
template<typename T>
class SimpleRefPtr
{
private:
    T* ptr;
    int* refCount;
    
public:
    // コンストラクタ
    explicit SimpleRefPtr(T* p = nullptr) : ptr(p), refCount(p ? new int(1) : nullptr)
    {
        if (ptr)
        {
            cout << "参照カウントポインタを作成: " << ptr << " (カウント: 1)\n";
        }
    }
    
    // コピーコンストラクタ
    SimpleRefPtr(const SimpleRefPtr& other) : ptr(other.ptr), refCount(other.refCount)
    {
        if (refCount)
        {
            (*refCount)++;
            cout << "参照カウントを増加: " << ptr << " (カウント: " << *refCount << ")\n";
        }
    }
    
    // コピー代入演算子
    SimpleRefPtr& operator=(const SimpleRefPtr& other)
    {
        if (this != &other)
        {
            release();
            
            ptr = other.ptr;
            refCount = other.refCount;
            
            if (refCount)
            {
                (*refCount)++;
                cout << "代入で参照カウントを増加: " << ptr << " (カウント: " << *refCount << ")\n";
            }
        }
        return *this;
    }
    
    // デストラクタ
    ~SimpleRefPtr()
    {
        release();
    }
    
    // 参照解除
    void release()
    {
        if (refCount)
        {
            (*refCount)--;
            cout << "参照カウントを減少: " << ptr << " (カウント: " << *refCount << ")\n";
            
            if (*refCount == 0)
            {
                cout << "オブジェクトを削除: " << ptr << "\n";
                delete ptr;
                delete refCount;
            }
            
            ptr = nullptr;
            refCount = nullptr;
        }
    }
    
    // アクセサ
    T* get() const { return ptr; }
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    
    // 参照カウント取得
    int use_count() const { return refCount ? *refCount : 0; }
    
    // 有効性チェック
    bool is_valid() const { return ptr != nullptr; }
    
    // リセット
    void reset(T* p = nullptr)
    {
        release();
        ptr = p;
        refCount = p ? new int(1) : nullptr;
        if (ptr)
        {
            cout << "リセットで新しいポイント: " << ptr << " (カウント: 1)\n";
        }
    }
};

// スレッドセーフな参照カウントクラス
template<typename T>
class ThreadSafeRefPtr
{
private:
    T* ptr;
    atomic<int>* refCount;
    
public:
    explicit ThreadSafeRefPtr(T* p = nullptr) : ptr(p), refCount(p ? new atomic<int>(1) : nullptr)
    {
        if (ptr)
        {
            cout << "スレッドセーフ参照カウントポインタを作成: " << ptr << "\n";
        }
    }
    
    ThreadSafeRefPtr(const ThreadSafeRefPtr& other) : ptr(other.ptr), refCount(other.refCount)
    {
        if (refCount)
        {
            refCount->fetch_add(1);
            cout << "スレッドセーフ参照カウントを増加: " << ptr << " (カウント: " << refCount->load() << ")\n";
        }
    }
    
    ThreadSafeRefPtr& operator=(const ThreadSafeRefPtr& other)
    {
        if (this != &other)
        {
            release();
            
            ptr = other.ptr;
            refCount = other.refCount;
            
            if (refCount)
            {
                refCount->fetch_add(1);
            }
        }
        return *this;
    }
    
    ~ThreadSafeRefPtr()
    {
        release();
    }
    
    void release()
    {
        if (refCount)
        {
            int count = refCount->fetch_sub(1);
            if (count == 1)  // 最後の参照
            {
                cout << "スレッドセーフオブジェクトを削除: " << ptr << "\n";
                delete ptr;
                delete refCount;
            }
            
            ptr = nullptr;
            refCount = nullptr;
        }
    }
    
    T* get() const { return ptr; }
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    
    int use_count() const { return refCount ? refCount->load() : 0; }
    bool is_valid() const { return ptr != nullptr; }
};

// ゲームアセットクラス
class GameTexture
{
public:
    string name;
    size_t width, height;
    char* pixelData;
    
    GameTexture(const string& name, size_t w, size_t h) 
        : name(name), width(w), height(h)
    {
        size_t dataSize = width * height * 4; // RGBA
        pixelData = new char[dataSize];
        // ダミーデータで初期化
        for (size_t i = 0; i < dataSize; ++i)
        {
            pixelData[i] = static_cast<char>(rand() % 256);
        }
        cout << "テクスチャ '" << name << "' を作成 (" << width << "x" << height << ")\n";
    }
    
    ~GameTexture()
    {
        delete[] pixelData;
        cout << "テクスチャ '" << name << "' を削除\n";
    }
    
    void printInfo() const
    {
        cout << "  テクスチャ: " << name << " (" << width << "x" << height << ")\n";
    }
};

class GameMesh
{
public:
    string name;
    vector<float> vertices;
    vector<int> indices;
    
    GameMesh(const string& name, size_t vertexCount, size_t indexCount)
        : name(name)
    {
        vertices.resize(vertexCount * 3); // x, y, z
        indices.resize(indexCount);
        
        // ダミーデータで初期化
        for (size_t i = 0; i < vertices.size(); ++i)
        {
            vertices[i] = static_cast<float>(rand()) / RAND_MAX;
        }
        
        for (size_t i = 0; i < indices.size(); ++i)
        {
            indices[i] = static_cast<int>(i % vertexCount);
        }
        
        cout << "メッシュ '" << name << "' を作成 (頂点: " << vertexCount << ", インデックス: " << indexCount << ")\n";
    }
    
    ~GameMesh()
    {
        cout << "メッシュ '" << name << "' を削除\n";
    }
    
    void printInfo() const
    {
        cout << "  メッシュ: " << name << " (頂点: " << vertices.size() / 3 << ", インデックス: " << indices.size() << ")\n";
    }
};

// ゲームオブジェクトクラス
class GameObject
{
public:
    string name;
    SimpleRefPtr<GameTexture> texture;
    SimpleRefPtr<GameMesh> mesh;
    
    GameObject(const string& name) : name(name)
    {
        cout << "ゲームオブジェクト '" << name << "' を作成\n";
    }
    
    ~GameObject()
    {
        cout << "ゲームオブジェクト '" << name << "' を削除\n";
    }
    
    void setTexture(SimpleRefPtr<GameTexture> tex)
    {
        texture = tex;
        cout << "'" << name << "' にテクスチャ '" << tex->name << "' を設定\n";
    }
    
    void setMesh(SimpleRefPtr<GameMesh> m)
    {
        mesh = m;
        cout << "'" << name << "' にメッシュ '" << m->name << "' を設定\n";
    }
    
    void printInfo() const
    {
        cout << "\n=== " << name << " の情報 ===\n";
        if (texture.is_valid())
        {
            texture->printInfo();
            cout << "    テクスチャ参照カウント: " << texture.use_count() << "\n";
        }
        
        if (mesh.is_valid())
        {
            mesh->printInfo();
            cout << "    メッシュ参照カウント: " << mesh.use_count() << "\n";
        }
    }
};

// シンプルな参照カウントのデモ
void simpleRefCountDemo()
{
    cout << "\n=== シンプルな参照カウントのデモ ===\n";
    
    {
        // テクスチャを作成
        SimpleRefPtr<GameTexture> sharedTexture(new GameTexture("shared_grass.png", 256, 256));
        cout << "初期参照カウント: " << sharedTexture.use_count() << "\n";
        
        {
            // 別のポインタで参照
            SimpleRefPtr<GameTexture> anotherRef = sharedTexture;
            cout << "コピー後の参照カウント: " << sharedTexture.use_count() << "\n";
            
            {
                // さらに別のポインタで参照
                SimpleRefPtr<GameTexture> thirdRef = anotherRef;
                cout << "さらにコピー後の参照カウント: " << sharedTexture.use_count() << "\n";
                
                cout << "\nthirdRefがスコープを抜けます...\n";
            }
            
            cout << "thirdRef破棄後の参照カウント: " << sharedTexture.use_count() << "\n";
            
            cout << "\nanotherRefがスコープを抜けます...\n";
        }
        
        cout << "anotherRef破棄後の参照カウント: " << sharedTexture.use_count() << "\n";
        
        cout << "\nsharedTextureがスコープを抜けます...\n";
    }
    
    cout << "すべての参照が破棄され、オブジェクトが自動削除されました\n";
}

// ゲームオブジェクトでの共有リソース管理
void gameObjectSharingDemo()
{
    cout << "\n=== ゲームオブジェクトでの共有リソース管理 ===\n";
    
    // 共有リソースを作成
    SimpleRefPtr<GameTexture> grassTexture(new GameTexture("grass_texture.png", 512, 512));
    SimpleRefPtr<GameTexture> stoneTexture(new GameTexture("stone_texture.png", 512, 512));
    SimpleRefPtr<GameMesh> cubeMesh(new GameMesh("cube_mesh", 8, 12));
    
    vector<unique_ptr<GameObject>> gameObjects;
    
    // 複数のゲームオブジェクトでリソースを共有
    cout << "\n複数のゲームオブジェクトでリソースを共有...\n";
    
    for (int i = 0; i < 3; ++i)
    {
        auto obj = make_unique<GameObject>("草ブロック" + to_string(i + 1));
        obj->setTexture(grassTexture);
        obj->setMesh(cubeMesh);
        gameObjects.push_back(move(obj));
    }
    
    for (int i = 0; i < 2; ++i)
    {
        auto obj = make_unique<GameObject>("石ブロック" + to_string(i + 1));
        obj->setTexture(stoneTexture);
        obj->setMesh(cubeMesh);
        gameObjects.push_back(move(obj));
    }
    
    cout << "\nリソースの参照カウント状態:\n";
    cout << "grassTexture: " << grassTexture.use_count() << " 個のオブジェクトが参照\n";
    cout << "stoneTexture: " << stoneTexture.use_count() << " 個のオブジェクトが参照\n";
    cout << "cubeMesh: " << cubeMesh.use_count() << " 個のオブジェクトが参照\n";
    
    // 各オブジェクトの情報を表示
    for (const auto& obj : gameObjects)
    {
        obj->printInfo();
    }
    
    // 一部のオブジェクトを削除
    cout << "\n一部のゲームオブジェクトを削除...\n";
    gameObjects.erase(gameObjects.begin(), gameObjects.begin() + 2);
    
    cout << "\n削除後の参照カウント状態:\n";
    cout << "grassTexture: " << grassTexture.use_count() << " 個のオブジェクトが参照\n";
    cout << "stoneTexture: " << stoneTexture.use_count() << " 個のオブジェクトが参照\n";
    cout << "cubeMesh: " << cubeMesh.use_count() << " 個のオブジェクトが参照\n";
}

// std::shared_ptrとの比較
void stdSharedPtrComparison()
{
    cout << "\n=== std::shared_ptrとの比較 ===\n";
    
    {
        cout << "std::shared_ptrでのリソース管理:\n";
        
        auto stdTexture = make_shared<GameTexture>("std_texture.png", 256, 256);
        cout << "std::shared_ptr参照カウント: " << stdTexture.use_count() << "\n";
        
        {
            auto stdCopy = stdTexture;
            cout << "std::shared_ptrコピー後の参照カウント: " << stdTexture.use_count() << "\n";
            
            vector<shared_ptr<GameTexture>> stdVector;
            stdVector.push_back(stdTexture);
            stdVector.push_back(stdCopy);
            
            cout << "vectorに追加後の参照カウント: " << stdTexture.use_count() << "\n";
        }
        
        cout << "スコープ後の参照カウント: " << stdTexture.use_count() << "\n";
    }
    
    cout << "\nstd::shared_ptrのメリット:\n";
    cout << "1. 標準ライブラリで安全性が高い\n";
    cout << "2. weak_ptrとの連携で循環参照を解決\n";
    cout << "3. スレッドセーフな参照カウント\n";
    cout << "4. カスタムデリータのサポート\n";
}

// マルチスレッド環境でのテスト
void multithreadedRefCountDemo()
{
    cout << "\n=== マルチスレッド環境での参照カウント ===\n";
    
    ThreadSafeRefPtr<GameTexture> sharedTexture(new GameTexture("thread_safe_texture.png", 128, 128));
    
    auto workerThread = [&sharedTexture](int threadId) {
        for (int i = 0; i < 3; ++i)
        {
            ThreadSafeRefPtr<GameTexture> localRef = sharedTexture;
            cout << "スレッド " << threadId << " が参照を作成 (カウント: " << localRef.use_count() << ")\n";
            
            this_thread::sleep_for(milliseconds(50));
            
            cout << "スレッド " << threadId << " が参照を解除\n";
        }
    };
    
    vector<thread> threads;
    for (int i = 0; i < 3; ++i)
    {
        threads.emplace_back(workerThread, i);
    }
    
    for (auto& t : threads)
    {
        t.join();
    }
    
    cout << "\n最終参照カウント: " << sharedTexture.use_count() << "\n";
}

// パフォーマンス比較
void performanceComparison()
{
    cout << "\n=== パフォーマンス比較 ===\n";
    
    const int iterations = 10000;
    
    // 生ポインタのパフォーマンス
    {
        auto start = high_resolution_clock::now();
        
        GameTexture* rawTexture = new GameTexture("raw_perf_test.png", 64, 64);
        
        for (int i = 0; i < iterations; ++i)
        {
            GameTexture* ref = rawTexture;
            (void)ref; // 未使用警告を防ぐ
        }
        
        delete rawTexture;
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        
        cout << "生ポインタ (" << iterations << "回): " << duration << " マイクロ秒\n";
    }
    
    // カスタム参照カウントのパフォーマンス
    {
        auto start = high_resolution_clock::now();
        
        SimpleRefPtr<GameTexture> refTexture(new GameTexture("ref_perf_test.png", 64, 64));
        
        for (int i = 0; i < iterations; ++i)
        {
            SimpleRefPtr<GameTexture> ref = refTexture;
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        
        cout << "カスタム参照カウント (" << iterations << "回): " << duration << " マイクロ秒\n";
    }
    
    // std::shared_ptrのパフォーマンス
    {
        auto start = high_resolution_clock::now();
        
        auto stdTexture = make_shared<GameTexture>("std_perf_test.png", 64, 64);
        
        for (int i = 0; i < iterations; ++i)
        {
            auto ref = stdTexture;
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        
        cout << "std::shared_ptr (" << iterations << "回): " << duration << " マイクロ秒\n";
    }
}

int main()
{
    cout << "=== 参照カウントシステムの実装 ===\n";
    
    // シンプルな参照カウントのデモ
    simpleRefCountDemo();
    
    // ゲームオブジェクトでの共有リソース管理
    gameObjectSharingDemo();
    
    // std::shared_ptrとの比較
    stdSharedPtrComparison();
    
    // マルチスレッド環境でのテスト
    multithreadedRefCountDemo();
    
    // パフォーマンス比較
    performanceComparison();
    
    cout << "\n=== 参照カウントのメリット ===\n";
    cout << "1. 自動的なメモリ管理（ガベージコレクション）\n";
    cout << "2. 共有リソースの安全な管理\n";
    cout << "3. メモリリークの防止\n";
    cout << "4. 必要な間だけリソースを保持\n";
    
    cout << "\n=== 注意点 ===\n";
    cout << "1. 循環参照が発生するとメモリリークが起きる\n";
    cout << "2. 参照カウントのオーバーヘッドがある\n";
    cout << "3. マルチスレッド環境ではアトミック操作が必要\n";
    cout << "4. weak_ptrやスマートポインタと組み合わせて使用\n";
    
    cout << "\n=== ゲーム開発での活用例 ===\n";
    cout << "1. テクスチャ、メッシュ、サウンドなどのアセット共有\n";
    cout << "2. シーングラフやゲームオブジェクトの階層管理\n";
    cout << "3. リソースキャッシュシステム\n";
    cout << "4. イベントシステムでのコールバック管理\n";
    cout << "5. マルチプレイヤーゲームでの共有ゲーム状態\n";
    
    return 0;
}

// 参照カウントの重要なポイント：
// 1. オブジェクトへの参照数を記録し、0になったら自動削除
// 2. 共有リソースの管理に非常に有効
// 3. 循環参照を防ぐためのweak_ptrの併用が重要
// 4. マルチスレッド環境ではアトミック操作が必要
// 5. ゲームではメモリ消費やパフォーマンスとのバランスが重要