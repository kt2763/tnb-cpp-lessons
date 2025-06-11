// カスタムアロケータの実装
// ゲーム開発での特化されたメモリ管理

#include <iostream>
#include <vector>
#include <memory>
#include <chrono>
#include <cassert>
#include <cstdlib>
#include <new>

using namespace std;
using namespace chrono;

// シンプルなスタックアロケータ
class StackAllocator
{
private:
    char* memory;
    size_t size;
    size_t offset;
    
public:
    StackAllocator(size_t totalSize) : size(totalSize), offset(0)
    {
        memory = static_cast<char*>(malloc(totalSize));
        if (!memory)
        {
            throw bad_alloc();
        }
        cout << "ステックアロケータを初期化 (サイズ: " << totalSize << " bytes)\n";
    }
    
    ~StackAllocator()
    {
        free(memory);
        cout << "スタックアロケータを解放\n";
    }
    
    // コピー禁止
    StackAllocator(const StackAllocator&) = delete;
    StackAllocator& operator=(const StackAllocator&) = delete;
    
    void* allocate(size_t bytes, size_t alignment = sizeof(void*))
    {
        // アライメントを考慮したオフセット計算
        size_t alignedOffset = (offset + alignment - 1) & ~(alignment - 1);
        
        if (alignedOffset + bytes > size)
        {
            cout << "スタックアロケータの容量不足！\n";
            return nullptr;
        }
        
        void* ptr = memory + alignedOffset;
        offset = alignedOffset + bytes;
        
        cout << "スタックから " << bytes << " bytes 確保 (オフセット: " << alignedOffset << ")\n";
        return ptr;
    }
    
    void deallocate(void* ptr)
    {
        // スタックアロケータでは個別の解放は行わない
        cout << "スタックアロケータでは個別解放は無視されます\n";
    }
    
    void reset()
    {
        offset = 0;
        cout << "スタックアロケータをリセット\n";
    }
    
    size_t getUsedMemory() const { return offset; }
    size_t getAvailableMemory() const { return size - offset; }
    double getUsagePercentage() const { return static_cast<double>(offset) / size * 100.0; }
};

// ゲームオブジェクトプールアロケータ
template<typename T>
class PoolAllocator
{
private:
    struct FreeNode
    {
        FreeNode* next;
    };
    
    char* memory;
    FreeNode* freeHead;
    size_t blockSize;
    size_t blockCount;
    size_t allocatedBlocks;
    
public:
    PoolAllocator(size_t count) : blockCount(count), allocatedBlocks(0)
    {
        blockSize = max(sizeof(T), sizeof(FreeNode));
        size_t totalSize = blockSize * blockCount;
        
        memory = static_cast<char*>(malloc(totalSize));
        if (!memory)
        {
            throw bad_alloc();
        }
        
        // フリーリストを初期化
        freeHead = reinterpret_cast<FreeNode*>(memory);
        FreeNode* current = freeHead;
        
        for (size_t i = 0; i < blockCount - 1; ++i)
        {
            current->next = reinterpret_cast<FreeNode*>(memory + (i + 1) * blockSize);
            current = current->next;
        }
        current->next = nullptr;
        
        cout << "プールアロケータを初期化 (タイプ: " << typeid(T).name() 
             << ", ブロックサイズ: " << blockSize 
             << ", ブロック数: " << blockCount << ")\n";
    }
    
    ~PoolAllocator()
    {
        free(memory);
        cout << "プールアロケータを解放\n";
    }
    
    // コピー禁止
    PoolAllocator(const PoolAllocator&) = delete;
    PoolAllocator& operator=(const PoolAllocator&) = delete;
    
    T* allocate()
    {
        if (!freeHead)
        {
            cout << "プールアロケータの容量不足！\n";
            return nullptr;
        }
        
        T* ptr = reinterpret_cast<T*>(freeHead);
        freeHead = freeHead->next;
        allocatedBlocks++;
        
        cout << "プールからブロックを確保 (使用中: " << allocatedBlocks << "/" << blockCount << ")\n";
        return ptr;
    }
    
    void deallocate(T* ptr)
    {
        if (!ptr) return;
        
        FreeNode* node = reinterpret_cast<FreeNode*>(ptr);
        node->next = freeHead;
        freeHead = node;
        allocatedBlocks--;
        
        cout << "プールにブロックを返却 (使用中: " << allocatedBlocks << "/" << blockCount << ")\n";
    }
    
    size_t getAllocatedCount() const { return allocatedBlocks; }
    size_t getAvailableCount() const { return blockCount - allocatedBlocks; }
    bool isFull() const { return allocatedBlocks == blockCount; }
    bool isEmpty() const { return allocatedBlocks == 0; }
};

// リニアアロケータ（シンプル版）
class LinearAllocator
{
private:
    char* memory;
    size_t size;
    size_t offset;
    
public:
    LinearAllocator(size_t totalSize) : size(totalSize), offset(0)
    {
        memory = static_cast<char*>(malloc(totalSize));
        if (!memory)
        {
            throw bad_alloc();
        }
        cout << "リニアアロケータを初期化 (サイズ: " << totalSize << " bytes)\n";
    }
    
    ~LinearAllocator()
    {
        free(memory);
        cout << "リニアアロケータを解放\n";
    }
    
    // コピー禁止
    LinearAllocator(const LinearAllocator&) = delete;
    LinearAllocator& operator=(const LinearAllocator&) = delete;
    
    void* allocate(size_t bytes, size_t alignment = sizeof(void*))
    {
        // アライメントを考慮したオフセット計算
        size_t alignedOffset = (offset + alignment - 1) & ~(alignment - 1);
        
        if (alignedOffset + bytes > size)
        {
            cout << "リニアアロケータの容量不足！\n";
            return nullptr;
        }
        
        void* ptr = memory + alignedOffset;
        offset = alignedOffset + bytes;
        
        cout << "リニアから " << bytes << " bytes 確保 (オフセット: " << alignedOffset << ")\n";
        return ptr;
    }
    
    void deallocate(void* ptr)
    {
        // リニアアロケータでは個別の解放は行わない
        cout << "リニアアロケータでは個別解放は無視されます\n";
    }
    
    void reset()
    {
        offset = 0;
        cout << "リニアアロケータをリセット\n";
    }
    
    size_t getUsedMemory() const { return offset; }
    size_t getAvailableMemory() const { return size - offset; }
};

// ゲームオブジェクトの例
struct GameParticle
{
    float x, y, z;
    float vx, vy, vz;
    float life;
    uint32_t color;
    
    GameParticle() : x(0), y(0), z(0), vx(0), vy(0), vz(0), life(1.0f), color(0xFFFFFFFF)
    {
        cout << "パーティクルを作成\n";
    }
    
    ~GameParticle()
    {
        cout << "パーティクルを削除\n";
    }
    
    void update(float deltaTime)
    {
        x += vx * deltaTime;
        y += vy * deltaTime;
        z += vz * deltaTime;
        life -= deltaTime;
    }
};

struct GameBullet
{
    float x, y, z;
    float dirX, dirY, dirZ;
    float speed;
    int damage;
    
    GameBullet() : x(0), y(0), z(0), dirX(0), dirY(1), dirZ(0), speed(100.0f), damage(10)
    {
        cout << "弾丸を作成\n";
    }
    
    ~GameBullet()
    {
        cout << "弾丸を削除\n";
    }
};

// STLコンテナで使用できるカスタムアロケータ
template<typename T>
class STLPoolAllocator
{
private:
    PoolAllocator<T>* pool;
    
public:
    using value_type = T;
    
    STLPoolAllocator(PoolAllocator<T>* poolPtr) : pool(poolPtr) {}
    
    template<typename U>
    STLPoolAllocator(const STLPoolAllocator<U>& other) : pool(nullptr) {}
    
    T* allocate(size_t n)
    {
        if (n != 1 || !pool)
        {
            throw bad_alloc();
        }
        return pool->allocate();
    }
    
    void deallocate(T* ptr, size_t n)
    {
        if (pool)
        {
            pool->deallocate(ptr);
        }
    }
    
    template<typename U>
    bool operator==(const STLPoolAllocator<U>&) const { return true; }
    
    template<typename U>
    bool operator!=(const STLPoolAllocator<U>&) const { return false; }
};

// スタックアロケータのデモ
void stackAllocatorDemo()
{
    cout << "\n=== スタックアロケータのデモ ===\n";
    
    StackAllocator stackAlloc(1024);
    
    // 小さなデータを順次確保
    int* playerHealth = static_cast<int*>(stackAlloc.allocate(sizeof(int)));
    float* playerPosition = static_cast<float*>(stackAlloc.allocate(sizeof(float) * 3));
    char* playerName = static_cast<char*>(stackAlloc.allocate(32));
    
    if (playerHealth && playerPosition && playerName)
    {
        *playerHealth = 100;
        playerPosition[0] = 10.0f; // x
        playerPosition[1] = 5.0f;  // y
        playerPosition[2] = 0.0f;  // z
        strcpy(playerName, "勇者");
        
        cout << "\nプレイヤーデータ:\n";
        cout << "体力: " << *playerHealth << "\n";
        cout << "位置: (" << playerPosition[0] << ", " << playerPosition[1] << ", " << playerPosition[2] << ")\n";
        cout << "名前: " << playerName << "\n";
    }
    
    cout << "\n使用率: " << stackAlloc.getUsagePercentage() << "%\n";
    
    // スタックをリセット（ゲームシーン切り替え時など）
    stackAlloc.reset();
    
    cout "リセット後の使用率: " << stackAlloc.getUsagePercentage() << "%\n";
}

// プールアロケータのデモ
void poolAllocatorDemo()
{
    cout << "\n=== プールアロケータのデモ ===\n";
    
    PoolAllocator<GameParticle> particlePool(5);
    vector<GameParticle*> activeParticles;
    
    cout << "\nパーティクルを順次生成:\n";
    
    // パーティクルを生成
    for (int i = 0; i < 7; ++i) // 容量を超えてテスト
    {
        GameParticle* particle = particlePool.allocate();
        if (particle)
        {
            new(particle) GameParticle();  // プレースメントnew
            particle->x = static_cast<float>(i * 10);
            particle->vx = static_cast<float>(i * 5);
            activeParticles.push_back(particle);
        }
        else
        {
            cout << "パーティクル" << i << "の生成に失敗\n";
        }
    }
    
    cout << "\n一部のパーティクルを削除:\n";
    
    // 最初の2個を削除
    for (int i = 0; i < 2 && !activeParticles.empty(); ++i)
    {
        GameParticle* particle = activeParticles.back();
        activeParticles.pop_back();
        
        particle->~GameParticle();  // デストラクタを明示呼び出し
        particlePool.deallocate(particle);
    }
    
    cout << "\n新しいパーティクルを再生成:\n";
    
    // 新しいパーティクルを生成
    for (int i = 0; i < 3; ++i)
    {
        GameParticle* particle = particlePool.allocate();
        if (particle)
        {
            new(particle) GameParticle();
            particle->x = static_cast<float>(i * 20 + 100);
            activeParticles.push_back(particle);
        }
    }
    
    // 残りのパーティクルをクリーンアップ
    cout << "\n残りのパーティクルをクリーンアップ:\n";
    for (GameParticle* particle : activeParticles)
    {
        particle->~GameParticle();
        particlePool.deallocate(particle);
    }
    activeParticles.clear();
}

// リニアアロケータのデモ
void linearAllocatorDemo()
{
    cout << "\n=== リニアアロケータのデモ ===\n";
    
    LinearAllocator linearAlloc(2048);
    
    // レベルデータをロードするシナリオ
    cout << "\nレベルデータをロード中...\n";
    
    // 地形データを確保
    float* terrainHeights = static_cast<float*>(linearAlloc.allocate(sizeof(float) * 100));
    
    // 敵のポジションを確保
    float* enemyPositions = static_cast<float*>(linearAlloc.allocate(sizeof(float) * 60)); // 20体のx,y,z
    
    // アイテムの情報を確保
    int* itemTypes = static_cast<int*>(linearAlloc.allocate(sizeof(int) * 50));
    
    if (terrainHeights && enemyPositions && itemTypes)
    {
        // データを初期化
        for (int i = 0; i < 100; ++i)
        {
            terrainHeights[i] = static_cast<float>(rand() % 50);
        }
        
        for (int i = 0; i < 60; i += 3)
        {
            enemyPositions[i] = static_cast<float>(rand() % 1000);     // x
            enemyPositions[i + 1] = 0.0f;                              // y
            enemyPositions[i + 2] = static_cast<float>(rand() % 1000); // z
        }
        
        for (int i = 0; i < 50; ++i)
        {
            itemTypes[i] = rand() % 10; // 0-9のアイテムタイプ
        }
        
        cout << "レベルデータのロード完了\n";
        cout << "使用メモリ: " << linearAlloc.getUsedMemory() << " / " 
             << (linearAlloc.getUsedMemory() + linearAlloc.getAvailableMemory()) << " bytes\n";
    }
    
    // レベル終了時にリセット
    cout << "\nレベル終了 - リニアアロケータをリセット\n";
    linearAlloc.reset();
}

// パフォーマンス比較
void performanceComparison()
{
    cout << "\n=== パフォーマンス比較 ===\n";
    
    const int iterations = 10000;
    
    // 標準のnew/delete
    {
        auto start = high_resolution_clock::now();
        
        vector<GameBullet*> bullets;
        
        for (int i = 0; i < iterations; ++i)
        {
            bullets.push_back(new GameBullet());
        }
        
        for (GameBullet* bullet : bullets)
        {
            delete bullet;
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        
        cout << "標準new/delete: " << duration << " マイクロ秒\n";
    }
    
    // プールアロケータ
    {
        PoolAllocator<GameBullet> bulletPool(iterations);
        
        auto start = high_resolution_clock::now();
        
        vector<GameBullet*> bullets;
        
        for (int i = 0; i < iterations; ++i)
        {
            GameBullet* bullet = bulletPool.allocate();
            if (bullet)
            {
                new(bullet) GameBullet();
                bullets.push_back(bullet);
            }
        }
        
        for (GameBullet* bullet : bullets)
        {
            bullet->~GameBullet();
            bulletPool.deallocate(bullet);
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        
        cout << "プールアロケータ: " << duration << " マイクロ秒\n";
    }
}

// STLコンテナでの使用例
void stlIntegrationDemo()
{
    cout << "\n=== STLコンテナでの使用例 ===\n";
    
    PoolAllocator<GameParticle> particlePool(10);
    
    // カスタムアロケータを使用したvector
    using ParticleVector = vector<GameParticle*, STLPoolAllocator<GameParticle*>>;
    STLPoolAllocator<GameParticle*> allocator(&particlePool);
    
    cout "カスタムアロケータを使用したSTLコンテナの例をシミュレート\n";
    cout << "（実際は、コンテナ自体のアロケータとオブジェクトのアロケータを別々に管理することが多い）\n";
    
    // シンプルな例として、手動でプールを管理
    vector<GameParticle*> particles;
    
    for (int i = 0; i < 5; ++i)
    {
        GameParticle* particle = particlePool.allocate();
        if (particle)
        {
            new(particle) GameParticle();
            particles.push_back(particle);
        }
    }
    
    cout << "生成されたパーティクル数: " << particles.size() << "\n";
    
    // クリーンアップ
    for (GameParticle* particle : particles)
    {
        particle->~GameParticle();
        particlePool.deallocate(particle);
    }
}

int main()
{
    cout << "=== カスタムアロケータの実装 ===\n";
    
    // スタックアロケータのデモ
    stackAllocatorDemo();
    
    // プールアロケータのデモ
    poolAllocatorDemo();
    
    // リニアアロケータのデモ
    linearAllocatorDemo();
    
    // パフォーマンス比較
    performanceComparison();
    
    // STL統合の例
    stlIntegrationDemo();
    
    cout << "\n=== カスタムアロケータのメリット ===\n";
    cout << "1. メモリ断片化の減少\n";
    cout << "2. キャッシュ効率の向上\n";
    cout << "3. 割り当て/解放の高速化\n";
    cout << "4. メモリ使用量の予測可能性\n";
    cout << "5. リアルタイム性の向上\n";
    
    cout << "\n=== ゲーム開発での活用例 ===\n";
    cout << "1. パーティクルシステムのオブジェクトプール\n";
    cout << "2. ゲームオブジェクトの動的生成/削除\n";
    cout << "3. オーディオサンプルやエフェクトの一時バッファ\n";
    cout << "4. レベルデータのロードとアンロード\n";
    cout << "5. メモリ制限が厳しいコンソールやモバイルゲーム\n";
    
    cout << "\n=== 選択指針 ===\n";
    cout << "1. スタックアロケータ: 一時的データ、フレーム単位の処理\n";
    cout << "2. プールアロケータ: 同サイズオブジェクトの頻繁な生成/削除\n";
    cout << "3. リニアアロケータ: 順次データ、レベルロードなど\n";
    cout << "4. フリーリストアロケータ: 可変サイズデータの管理\n";
    
    return 0;
}

// カスタムアロケータ設計の重要なポイント：
// 1. 使用パターンに合わせたアロケータタイプの選択
// 2. アライメントやパディングを考慮した設計
// 3. マルチスレッド環境でのスレッドセーフティ
// 4. メモリリークやオーバーフローの検出機能
// 5. パフォーマンスとメモリ使用量のバランス