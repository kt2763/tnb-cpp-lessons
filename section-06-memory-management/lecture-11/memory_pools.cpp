// メモリプールパターンの詳細実装
// ゲーム開発での高速メモリ管理システム

#include <iostream>
#include <vector>
#include <memory>
#include <chrono>
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <thread>
#include <mutex>

using namespace std;
using namespace chrono;

// 基本的なメモリプールの実装
class BasicMemoryPool
{
private:
    struct Block
    {
        Block* next;
    };
    
    char* memory;        // プールのメモリ領域
    Block* freeHead;     // フリーブロックのリスト
    size_t blockSize;    // 各ブロックのサイズ
    size_t blockCount;   // 総ブロック数
    size_t allocated;    // 確保済みブロック数
    string poolName;     // プール名
    
public:
    BasicMemoryPool(const string& name, size_t blockSize, size_t blockCount)
        : blockSize(blockSize), blockCount(blockCount), allocated(0), poolName(name)
    {
        // ブロックサイズは最低でもポインタのサイズ以上必要
        this->blockSize = max(blockSize, sizeof(Block));
        
        size_t totalSize = this->blockSize * blockCount;
        memory = static_cast<char*>(malloc(totalSize));
        
        if (!memory)
        {
            throw bad_alloc();
        }
        
        // フリーリストを初期化
        initializeFreeList();
        
        cout << "メモリプール '" << poolName << "' を作成\n";
        cout << "  ブロックサイズ: " << this->blockSize << " bytes\n";
        cout << "  ブロック数: " << blockCount << "\n";
        cout << "  総サイズ: " << totalSize << " bytes\n";
    }
    
    ~BasicMemoryPool()
    {
        free(memory);
        cout << "メモリプール '" << poolName << "' を解放\n";
        
        if (allocated > 0)
        {
            cout << "  警告: " << allocated << "個のブロックが未解放でした\n";
        }
    }
    
    // コピー禁止
    BasicMemoryPool(const BasicMemoryPool&) = delete;
    BasicMemoryPool& operator=(const BasicMemoryPool&) = delete;
    
    void* allocate()
    {
        if (!freeHead)
        {
            cout << "プール '" << poolName << "' の容量不足！\n";
            return nullptr;
        }
        
        void* ptr = freeHead;
        freeHead = freeHead->next;
        allocated++;
        
        cout << "プール '" << poolName << "' からブロックを確保: " << ptr 
             << " (使用中: " << allocated << "/" << blockCount << ")\n";
        
        return ptr;
    }
    
    void deallocate(void* ptr)
    {
        if (!ptr || !isFromThisPool(ptr))
        {
            cout << "無効なポインタを返却しようとしました: " << ptr << "\n";
            return;
        }
        
        Block* block = static_cast<Block*>(ptr);
        block->next = freeHead;
        freeHead = block;
        allocated--;
        
        cout << "プール '" << poolName << "' にブロックを返却: " << ptr 
             << " (使用中: " << allocated << "/" << blockCount << ")\n";
    }
    
    // 統計情報
    size_t getAllocatedCount() const { return allocated; }
    size_t getAvailableCount() const { return blockCount - allocated; }
    size_t getTotalCount() const { return blockCount; }
    size_t getBlockSize() const { return blockSize; }
    double getUsagePercentage() const { return static_cast<double>(allocated) / blockCount * 100.0; }
    
    void printStatistics() const
    {
        cout << "\n=== プール '" << poolName << "' の統計 ===\n";
        cout << "使用中ブロック: " << allocated << "/" << blockCount << "\n";
        cout << "使用率: " << getUsagePercentage() << "%\n";
        cout << "ブロックサイズ: " << blockSize << " bytes\n";
        cout << "総メモリ: " << blockSize * blockCount << " bytes\n";
    }
    
private:
    void initializeFreeList()
    {
        freeHead = reinterpret_cast<Block*>(memory);
        Block* current = freeHead;
        
        for (size_t i = 0; i < blockCount - 1; ++i)
        {
            current->next = reinterpret_cast<Block*>(memory + (i + 1) * blockSize);
            current = current->next;
        }
        current->next = nullptr;
    }
    
    bool isFromThisPool(void* ptr) const
    {
        char* charPtr = static_cast<char*>(ptr);
        return charPtr >= memory && charPtr < memory + (blockSize * blockCount);
    }
};

// ゲームオブジェクトの例
struct GameParticle
{
    float x, y, z;          // 位置
    float vx, vy, vz;       // 速度
    float life;             // 生存時間
    uint32_t color;         // 色
    bool active;            // アクティブフラグ
    
    GameParticle() : x(0), y(0), z(0), vx(0), vy(0), vz(0), life(1.0f), color(0xFFFFFFFF), active(false)
    {
        cout << "パーティクルを初期化\n";
    }
    
    ~GameParticle()
    {
        cout << "パーティクルを終了\n";
    }
    
    void initialize(float x, float y, float z, float vx, float vy, float vz)
    {
        this->x = x; this->y = y; this->z = z;
        this->vx = vx; this->vy = vy; this->vz = vz;
        this->life = 1.0f;
        this->active = true;
    }
    
    void update(float deltaTime)
    {
        if (!active) return;
        
        x += vx * deltaTime;
        y += vy * deltaTime;
        z += vz * deltaTime;
        life -= deltaTime;
        
        if (life <= 0.0f)
        {
            active = false;
        }
    }
};

struct GameBullet
{
    float x, y, z;
    float dirX, dirY, dirZ;
    float speed;
    int damage;
    bool active;
    
    GameBullet() : x(0), y(0), z(0), dirX(0), dirY(1), dirZ(0), speed(100.0f), damage(10), active(false)
    {
        cout << "弾丸を初期化\n";
    }
    
    ~GameBullet()
    {
        cout << "弾丸を終了\n";
    }
    
    void fire(float x, float y, float z, float dirX, float dirY, float dirZ)
    {
        this->x = x; this->y = y; this->z = z;
        this->dirX = dirX; this->dirY = dirY; this->dirZ = dirZ;
        this->active = true;
    }
    
    void update(float deltaTime)
    {
        if (!active) return;
        
        x += dirX * speed * deltaTime;
        y += dirY * speed * deltaTime;
        z += dirZ * speed * deltaTime;
    }
};

// スレッドセーフなメモリプール
class ThreadSafeMemoryPool
{
private:
    BasicMemoryPool pool;
    mutable mutex poolMutex;
    
public:
    ThreadSafeMemoryPool(const string& name, size_t blockSize, size_t blockCount)
        : pool(name + "_ThreadSafe", blockSize, blockCount)
    {
        cout << "スレッドセーフプールを作成: " << name << "\n";
    }
    
    void* allocate()
    {
        lock_guard<mutex> lock(poolMutex);
        return pool.allocate();
    }
    
    void deallocate(void* ptr)
    {
        lock_guard<mutex> lock(poolMutex);
        pool.deallocate(ptr);
    }
    
    void printStatistics() const
    {
        lock_guard<mutex> lock(poolMutex);
        pool.printStatistics();
    }
    
    size_t getAllocatedCount() const
    {
        lock_guard<mutex> lock(poolMutex);
        return pool.getAllocatedCount();
    }
};

// プールマネージャー
class PoolManager
{
private:
    vector<unique_ptr<BasicMemoryPool>> pools;
    
public:
    template<typename T>
    BasicMemoryPool* createPool(const string& name, size_t count)
    {
        auto pool = make_unique<BasicMemoryPool>(name, sizeof(T), count);
        BasicMemoryPool* poolPtr = pool.get();
        pools.push_back(move(pool));
        
        cout << "プールマネージャーに '" << name << "' を登録\n";
        return poolPtr;
    }
    
    void printAllStatistics() const
    {
        cout << "\n=== 全プールの統計 ===\n";
        for (const auto& pool : pools)
        {
            pool->printStatistics();
        }
    }
    
    size_t getTotalPools() const { return pools.size(); }
};

// プールを使用したオブジェクト管理
template<typename T>
class PooledObject
{
private:
    BasicMemoryPool* pool;
    T* object;
    
public:
    PooledObject(BasicMemoryPool* pool) : pool(pool), object(nullptr)
    {
        void* ptr = pool->allocate();
        if (ptr)
        {
            object = new(ptr) T();  // プレースメントnew
        }
    }
    
    ~PooledObject()
    {
        if (object && pool)
        {
            object->~T();  // デストラクタを明示呼び出し
            pool->deallocate(object);
        }
    }
    
    // コピー禁止
    PooledObject(const PooledObject&) = delete;
    PooledObject& operator=(const PooledObject&) = delete;
    
    // ムーブセマンティクス
    PooledObject(PooledObject&& other) noexcept
        : pool(other.pool), object(other.object)
    {
        other.pool = nullptr;
        other.object = nullptr;
    }
    
    PooledObject& operator=(PooledObject&& other) noexcept
    {
        if (this != &other)
        {
            if (object && pool)
            {
                object->~T();
                pool->deallocate(object);
            }
            
            pool = other.pool;
            object = other.object;
            
            other.pool = nullptr;
            other.object = nullptr;
        }
        return *this;
    }
    
    T* get() const { return object; }
    T& operator*() const { return *object; }
    T* operator->() const { return object; }
    bool isValid() const { return object != nullptr; }
};

// メモリプールの基本デモ
void basicPoolDemo()
{
    cout << "\n=== 基本メモリプールのデモ ===\n";
    
    BasicMemoryPool particlePool("パーティクルプール", sizeof(GameParticle), 5);
    vector<GameParticle*> activeParticles;
    
    // パーティクルを順次生成
    cout << "\nパーティクルを生成中...\n";
    for (int i = 0; i < 7; ++i)  // 容量を超えてテスト
    {
        void* ptr = particlePool.allocate();
        if (ptr)
        {
            GameParticle* particle = new(ptr) GameParticle();
            particle->initialize(
                static_cast<float>(i * 10), 0.0f, 0.0f,
                static_cast<float>(i), 1.0f, 0.0f
            );
            activeParticles.push_back(particle);
        }
        else
        {
            cout << "パーティクル" << i << "の生成に失敗\n";
        }
    }
    
    particlePool.printStatistics();
    
    // パーティクルを更新
    cout << "\nパーティクルを更新中...\n";
    for (auto* particle : activeParticles)
    {
        particle->update(0.016f);  // 60FPSでのフレーム時間
    }
    
    // 非アクティブなパーティクルを削除
    cout << "\n非アクティブパーティクルを削除中...\n";
    auto it = activeParticles.begin();
    while (it != activeParticles.end())
    {
        if (!(*it)->active)
        {
            (*it)->~GameParticle();
            particlePool.deallocate(*it);
            it = activeParticles.erase(it);
        }
        else
        {
            ++it;
        }
    }
    
    particlePool.printStatistics();
    
    // 残りのパーティクルをクリーンアップ
    cout << "\n残りのパーティクルをクリーンアップ...\n";
    for (auto* particle : activeParticles)
    {
        particle->~GameParticle();
        particlePool.deallocate(particle);
    }
    activeParticles.clear();
    
    particlePool.printStatistics();
}

// RAIIラッパーを使用したデモ
void raiiPoolDemo()
{
    cout << "\n=== RAIIプールラッパーのデモ ===\n";
    
    PoolManager manager;
    auto* bulletPool = manager.createPool<GameBullet>("弾丸プール", 10);
    
    vector<PooledObject<GameBullet>> bullets;
    
    // 弾丸を発射
    cout << "\n弾丸を発射中...\n";
    for (int i = 0; i < 5; ++i)
    {
        bullets.emplace_back(bulletPool);
        if (bullets.back().isValid())
        {
            bullets.back()->fire(
                0.0f, 0.0f, 0.0f,
                static_cast<float>(i) * 0.2f, 1.0f, 0.0f
            );
        }
    }
    
    manager.printAllStatistics();
    
    // 弾丸を更新
    cout << "\n弾丸を更新中...\n";
    for (auto& bullet : bullets)
    {
        if (bullet.isValid())
        {
            bullet->update(0.016f);
        }
    }
    
    // スコープを抜けると自動的にクリーンアップされる
    cout << "\nスコープ終了時に自動クリーンアップ...\n";
}

// マルチスレッド環境でのデモ
void multithreadedPoolDemo()
{
    cout << "\n=== マルチスレッドプールのデモ ===\n";
    
    ThreadSafeMemoryPool safePool("マルチスレッドパーティクル", sizeof(GameParticle), 20);
    
    auto workerThread = [&safePool](int threadId, int particleCount) {
        vector<GameParticle*> particles;
        
        // パーティクルを確保
        for (int i = 0; i < particleCount; ++i)
        {
            void* ptr = safePool.allocate();
            if (ptr)
            {
                GameParticle* particle = new(ptr) GameParticle();
                particle->initialize(
                    static_cast<float>(threadId * 100 + i), 0.0f, 0.0f,
                    1.0f, 1.0f, 0.0f
                );
                particles.push_back(particle);
            }
        }
        
        cout << "スレッド" << threadId << ": " << particles.size() << "個のパーティクルを確保\n";
        
        // 少し待ってから解放
        this_thread::sleep_for(milliseconds(100));
        
        for (auto* particle : particles)
        {
            particle->~GameParticle();
            safePool.deallocate(particle);
        }
        
        cout << "スレッド" << threadId << ": パーティクルを解放完了\n";
    };
    
    // 複数スレッドでパーティクルを作成/削除
    vector<thread> threads;
    for (int i = 0; i < 4; ++i)
    {
        threads.emplace_back(workerThread, i, 3);
    }
    
    // 全スレッドの終了を待つ
    for (auto& t : threads)
    {
        t.join();
    }
    
    safePool.printStatistics();
}

// パフォーマンス比較
void performanceComparison()
{
    cout << "\n=== パフォーマンス比較 ===\n";
    
    const int iterations = 10000;
    
    // 標準のnew/delete
    {
        auto start = high_resolution_clock::now();
        
        vector<GameParticle*> particles;
        
        for (int i = 0; i < iterations; ++i)
        {
            particles.push_back(new GameParticle());
        }
        
        for (auto* particle : particles)
        {
            delete particle;
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        
        cout << "標準new/delete: " << duration << " マイクロ秒\n";
    }
    
    // メモリプール
    {
        BasicMemoryPool pool("パフォーマンステスト", sizeof(GameParticle), iterations);
        
        auto start = high_resolution_clock::now();
        
        vector<GameParticle*> particles;
        
        for (int i = 0; i < iterations; ++i)
        {
            void* ptr = pool.allocate();
            if (ptr)
            {
                particles.push_back(new(ptr) GameParticle());
            }
        }
        
        for (auto* particle : particles)
        {
            particle->~GameParticle();
            pool.deallocate(particle);
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        
        cout << "メモリプール: " << duration << " マイクロ秒\n";
    }
    
    // RAIIラッパー
    {
        BasicMemoryPool pool("RAIIテスト", sizeof(GameParticle), iterations);
        
        auto start = high_resolution_clock::now();
        
        vector<PooledObject<GameParticle>> particles;
        particles.reserve(iterations);
        
        for (int i = 0; i < iterations; ++i)
        {
            particles.emplace_back(&pool);
        }
        
        // スコープを抜けると自動的にクリーンアップ
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        
        cout << "RAIIラッパー: " << duration << " マイクロ秒\n";
    }
}

int main()
{
    cout << "=== メモリプールパターンの詳細実装 ===\n";
    
    // 基本的なプールのデモ
    basicPoolDemo();
    
    // RAIIラッパーのデモ
    raiiPoolDemo();
    
    // マルチスレッドのデモ
    multithreadedPoolDemo();
    
    // パフォーマンス比較
    performanceComparison();
    
    cout << "\n=== メモリプールのメリット ===\n";
    cout << "1. 高速なメモリ割り当てと解放\n";
    cout << "2. メモリ断片化の減少\n";
    cout << "3. キャッシュ効率の向上（連続メモリ割り当て）\n";
    cout << "4. メモリ使用量の予測可能性\n";
    cout << "5. リアルタイム性の向上（ガベージコレクションなし）\n";
    
    cout << "\n=== ゲーム開発での活用例 ===\n";
    cout << "1. パーティクルシステム: 爆発、炎、煙などのエフェクト\n";
    cout << "2. 弾丸システム: プレイヤーや敵の弾丸管理\n";
    cout << "3. ゲームオブジェクト: 敵、アイテム、NPCなど\n";
    cout << "4. UIエレメント: ボタン、ラベル、ウィンドウなど\n";
    cout << "5. オーディオサンプル: 音声エフェクトの再生管理\n";
    cout << "6. ネットワークパケット: 通信データのバッファ管理\n";
    
    cout << "\n=== 設計指針 ===\n";
    cout << "1. オブジェクトのサイズと生存期間を考慮したプールサイズの決定\n";
    cout << "2. スレッドセーフティが必要な場合はミューテックスを使用\n";
    cout << "3. RAIIパターンで自動的なメモリ管理を実現\n";
    cout << "4. プールの使用率を監視して最適化\n";
    cout << "5. デバッグモードでのメモリリーク検出機能を追加\n";
    
    return 0;
}

// メモリプールの重要なポイント：
// 1. オブジェクトのサイズが同じで、頻繁に生成/削除される場合に最適
// 2. メモリの事前確保で、実行時のメモリ割り当てコストを削減
// 3. 連続メモリでキャッシュ効率を向上
// 4. フリーリストで高速な割り当て/解放を実現
// 5. マルチスレッド環境ではロック突端を最小限に抑える設計が重要