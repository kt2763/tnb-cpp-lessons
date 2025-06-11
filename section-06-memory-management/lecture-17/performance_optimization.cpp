// メモリパフォーマンス最適化
// ゲーム開発でのメモリアクセス高速化技術

#include <iostream>
#include <vector>
#include <memory>
#include <chrono>
#include <algorithm>
#include <random>
#include <cassert>
#include <immintrin.h>  // SIMD命令
#include <thread>
#include <numeric>

using namespace std;
using namespace chrono;

// ベンチマーク用のタイマークラス
class BenchmarkTimer
{
private:
    high_resolution_clock::time_point startTime;
    string description;
    
public:
    BenchmarkTimer(const string& desc) : description(desc)
    {
        startTime = high_resolution_clock::now();
    }
    
    ~BenchmarkTimer()
    {
        auto endTime = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(endTime - startTime).count();
        cout << description << ": " << duration << " マイクロ秒\n";
    }
};

#define BENCHMARK(desc) BenchmarkTimer timer(desc)

// パフォーマンス比較: 配列 vs ベクター vs リスト
namespace ArrayVsVector
{
    struct GameEntity
    {
        float x, y, z;      // 位置
        float vx, vy, vz;   // 速度
        int health;         // 体力
        int id;             // ID
        bool active;        // アクティブフラグ
        
        GameEntity() : x(0), y(0), z(0), vx(0), vy(0), vz(0), health(100), id(0), active(true) {}
        
        GameEntity(float x, float y, float z, int id)
            : x(x), y(y), z(z), vx(0), vy(0), vz(0), health(100), id(id), active(true) {}
        
        void update(float deltaTime)
        {
            if (!active) return;
            
            x += vx * deltaTime;
            y += vy * deltaTime;
            z += vz * deltaTime;
            
            // 簡単な境界チェック
            if (x < -1000 || x > 1000 || y < -1000 || y > 1000)
            {
                active = false;
            }
        }
    };
    
    void compareContainerPerformance()
    {
        cout << "\n=== コンテナパフォーマンス比較 ===\n";
        
        const int entityCount = 100000;
        const int iterations = 100;
        
        // 1. C配列での処理
        {
            BENCHMARK("C配列");
            
            GameEntity* entities = new GameEntity[entityCount];
            
            // 初期化
            for (int i = 0; i < entityCount; ++i)
            {
                entities[i] = GameEntity(
                    static_cast<float>(i % 1000),
                    static_cast<float>(i % 1000),
                    0.0f,
                    i
                );
                entities[i].vx = static_cast<float>((i % 10) - 5);
                entities[i].vy = static_cast<float>((i % 10) - 5);
            }
            
            // 更新処理
            for (int iter = 0; iter < iterations; ++iter)
            {
                for (int i = 0; i < entityCount; ++i)
                {
                    entities[i].update(0.016f);
                }
            }
            
            delete[] entities;
        }
        
        // 2. std::vectorでの処理
        {
            BENCHMARK("std::vector");
            
            vector<GameEntity> entities;
            entities.reserve(entityCount);
            
            // 初期化
            for (int i = 0; i < entityCount; ++i)
            {
                entities.emplace_back(
                    static_cast<float>(i % 1000),
                    static_cast<float>(i % 1000),
                    0.0f,
                    i
                );
                entities[i].vx = static_cast<float>((i % 10) - 5);
                entities[i].vy = static_cast<float>((i % 10) - 5);
            }
            
            // 更新処理
            for (int iter = 0; iter < iterations; ++iter)
            {
                for (auto& entity : entities)
                {
                    entity.update(0.016f);
                }
            }
        }
        
        // 3. std::vector<unique_ptr>での処理
        {
            BENCHMARK("std::vector<unique_ptr>");
            
            vector<unique_ptr<GameEntity>> entities;
            entities.reserve(entityCount);
            
            // 初期化
            for (int i = 0; i < entityCount; ++i)
            {
                auto entity = make_unique<GameEntity>(
                    static_cast<float>(i % 1000),
                    static_cast<float>(i % 1000),
                    0.0f,
                    i
                );
                entity->vx = static_cast<float>((i % 10) - 5);
                entity->vy = static_cast<float>((i % 10) - 5);
                entities.push_back(move(entity));
            }
            
            // 更新処理
            for (int iter = 0; iter < iterations; ++iter)
            {
                for (auto& entity : entities)
                {
                    entity->update(0.016f);
                }
            }
        }
    }
}

// SoA (Structure of Arrays) vs AoS (Array of Structures) 比較
namespace SoAvsAoS
{
    // AoS: Array of Structures（通常の構造体配列）
    struct ParticleAoS
    {
        float x, y, z;        // 位置
        float vx, vy, vz;     // 速度
        float life;           // 生存時間
        uint32_t color;       // 色
    };
    
    // SoA: Structure of Arrays（配列の構造体）
    struct ParticlesSoA
    {
        vector<float> x, y, z;        // 位置配列
        vector<float> vx, vy, vz;     // 速度配列
        vector<float> life;           // 生存時間配列
        vector<uint32_t> color;       // 色配列
        
        void resize(size_t count)
        {
            x.resize(count); y.resize(count); z.resize(count);
            vx.resize(count); vy.resize(count); vz.resize(count);
            life.resize(count);
            color.resize(count);
        }
        
        size_t size() const { return x.size(); }
    };
    
    void compareDataLayouts()
    {
        cout << "\n=== データレイアウト比較 (SoA vs AoS) ===\n";
        
        const int particleCount = 100000;
        const int iterations = 1000;
        
        // AoS での処理
        {
            BENCHMARK("AoS (Array of Structures)");
            
            vector<ParticleAoS> particles(particleCount);
            
            // 初期化
            for (int i = 0; i < particleCount; ++i)
            {
                particles[i] = {
                    static_cast<float>(rand() % 1000),
                    static_cast<float>(rand() % 1000),
                    static_cast<float>(rand() % 1000),
                    static_cast<float>((rand() % 20) - 10),
                    static_cast<float>((rand() % 20) - 10),
                    static_cast<float>((rand() % 20) - 10),
                    1.0f,
                    0xFFFFFFFF
                };
            }
            
            // 位置更新（XYZのみアクセス）
            const float deltaTime = 0.016f;
            for (int iter = 0; iter < iterations; ++iter)
            {
                for (auto& particle : particles)
                {
                    particle.x += particle.vx * deltaTime;
                    particle.y += particle.vy * deltaTime;
                    particle.z += particle.vz * deltaTime;
                    particle.life -= deltaTime;
                }
            }
        }
        
        // SoA での処理
        {
            BENCHMARK("SoA (Structure of Arrays)");
            
            ParticlesSoA particles;
            particles.resize(particleCount);
            
            // 初期化
            for (int i = 0; i < particleCount; ++i)
            {
                particles.x[i] = static_cast<float>(rand() % 1000);
                particles.y[i] = static_cast<float>(rand() % 1000);
                particles.z[i] = static_cast<float>(rand() % 1000);
                particles.vx[i] = static_cast<float>((rand() % 20) - 10);
                particles.vy[i] = static_cast<float>((rand() % 20) - 10);
                particles.vz[i] = static_cast<float>((rand() % 20) - 10);
                particles.life[i] = 1.0f;
                particles.color[i] = 0xFFFFFFFF;
            }
            
            // 位置更新（XYZのみアクセス）
            const float deltaTime = 0.016f;
            for (int iter = 0; iter < iterations; ++iter)
            {
                for (size_t i = 0; i < particles.size(); ++i)
                {
                    particles.x[i] += particles.vx[i] * deltaTime;
                    particles.y[i] += particles.vy[i] * deltaTime;
                    particles.z[i] += particles.vz[i] * deltaTime;
                    particles.life[i] -= deltaTime;
                }
            }
        }
        
        // SoA + SIMD での処理
        {
            BENCHMARK("SoA + SIMD");
            
            ParticlesSoA particles;
            particles.resize(particleCount);
            
            // 初期化
            for (int i = 0; i < particleCount; ++i)
            {
                particles.x[i] = static_cast<float>(rand() % 1000);
                particles.y[i] = static_cast<float>(rand() % 1000);
                particles.z[i] = static_cast<float>(rand() % 1000);
                particles.vx[i] = static_cast<float>((rand() % 20) - 10);
                particles.vy[i] = static_cast<float>((rand() % 20) - 10);
                particles.vz[i] = static_cast<float>((rand() % 20) - 10);
                particles.life[i] = 1.0f;
                particles.color[i] = 0xFFFFFFFF;
            }
            
            const float deltaTime = 0.016f;
            const __m256 deltaSIMD = _mm256_set1_ps(deltaTime);
            
            for (int iter = 0; iter < iterations; ++iter)
            {
                size_t simdCount = (particles.size() / 8) * 8;
                
                // SIMD で8要素ずつ処理
                for (size_t i = 0; i < simdCount; i += 8)
                {
                    __m256 x = _mm256_load_ps(&particles.x[i]);
                    __m256 y = _mm256_load_ps(&particles.y[i]);
                    __m256 z = _mm256_load_ps(&particles.z[i]);
                    
                    __m256 vx = _mm256_load_ps(&particles.vx[i]);
                    __m256 vy = _mm256_load_ps(&particles.vy[i]);
                    __m256 vz = _mm256_load_ps(&particles.vz[i]);
                    
                    __m256 life = _mm256_load_ps(&particles.life[i]);
                    
                    // x += vx * deltaTime
                    x = _mm256_fmadd_ps(vx, deltaSIMD, x);
                    y = _mm256_fmadd_ps(vy, deltaSIMD, y);
                    z = _mm256_fmadd_ps(vz, deltaSIMD, z);
                    
                    // life -= deltaTime
                    life = _mm256_sub_ps(life, deltaSIMD);
                    
                    _mm256_store_ps(&particles.x[i], x);
                    _mm256_store_ps(&particles.y[i], y);
                    _mm256_store_ps(&particles.z[i], z);
                    _mm256_store_ps(&particles.life[i], life);
                }
                
                // 残りの要素をスカラで処理
                for (size_t i = simdCount; i < particles.size(); ++i)
                {
                    particles.x[i] += particles.vx[i] * deltaTime;
                    particles.y[i] += particles.vy[i] * deltaTime;
                    particles.z[i] += particles.vz[i] * deltaTime;
                    particles.life[i] -= deltaTime;
                }
            }
        }
    }
}

// メモリプールの最適化
namespace OptimizedMemoryPool
{
    // アライメント対応の高速メモリプール
    template<typename T, size_t Alignment = alignof(T)>
    class FastMemoryPool
    {
    private:
        struct alignas(Alignment) AlignedBlock
        {
            alignas(Alignment) char data[sizeof(T)];
            AlignedBlock* next;
        };
        
        AlignedBlock* freeList;
        vector<unique_ptr<AlignedBlock[]>> chunks;
        size_t chunkSize;
        size_t allocated;
        size_t totalCapacity;
        
    public:
        FastMemoryPool(size_t initialChunkSize = 1024)
            : freeList(nullptr), chunkSize(initialChunkSize), allocated(0), totalCapacity(0)
        {
            allocateNewChunk();
        }
        
        T* allocate()
        {
            if (!freeList)
            {
                allocateNewChunk();
            }
            
            AlignedBlock* block = freeList;
            freeList = freeList->next;
            allocated++;
            
            return reinterpret_cast<T*>(block);
        }
        
        void deallocate(T* ptr)
        {
            if (!ptr) return;
            
            AlignedBlock* block = reinterpret_cast<AlignedBlock*>(ptr);
            block->next = freeList;
            freeList = block;
            allocated--;
        }
        
        size_t getAllocatedCount() const { return allocated; }
        size_t getTotalCapacity() const { return totalCapacity; }
        
    private:
        void allocateNewChunk()
        {
            auto chunk = make_unique<AlignedBlock[]>(chunkSize);
            
            // フリーリストに追加
            for (size_t i = 0; i < chunkSize - 1; ++i)
            {
                chunk[i].next = &chunk[i + 1];
            }
            chunk[chunkSize - 1].next = freeList;
            freeList = &chunk[0];
            
            totalCapacity += chunkSize;
            chunks.push_back(move(chunk));
        }
    };
    
    struct TestObject
    {
        alignas(32) float data[8]; // 32バイトアライメント
        int id;
        
        TestObject(int i = 0) : id(i)
        {
            for (int j = 0; j < 8; ++j)
            {
                data[j] = static_cast<float>(i + j);
            }
        }
    };
    
    void compareMemoryPools()
    {
        cout << "\n=== メモリプール最適化比較 ===\n";
        
        const int objectCount = 50000;
        const int iterations = 10;
        
        // 標準のnew/delete
        {
            BENCHMARK("標準 new/delete");
            
            for (int iter = 0; iter < iterations; ++iter)
            {
                vector<TestObject*> objects;
                
                // 確保
                for (int i = 0; i < objectCount; ++i)
                {
                    objects.push_back(new TestObject(i));
                }
                
                // 解放
                for (auto* obj : objects)
                {
                    delete obj;
                }
            }
        }
        
        // 最適化されたメモリプール
        {
            BENCHMARK("最適化メモリプール");
            
            FastMemoryPool<TestObject, 32> pool;
            
            for (int iter = 0; iter < iterations; ++iter)
            {
                vector<TestObject*> objects;
                
                // 確保
                for (int i = 0; i < objectCount; ++i)
                {
                    TestObject* obj = pool.allocate();
                    new(obj) TestObject(i); // プレースメントnew
                    objects.push_back(obj);
                }
                
                // 解放
                for (auto* obj : objects)
                {
                    obj->~TestObject(); // デストラクタ呼び出し
                    pool.deallocate(obj);
                }
            }
        }
    }
}

// キャッシュ効率の最適化
namespace CacheOptimization
{
    struct CacheFriendlyGameObject
    {
        // ホットデータ（頻繁にアクセス）を先頭に配置
        float x, y, z;          // 位置（12 bytes）
        bool active;            // アクティブフラグ（1 byte）
        char padding1[3];       // パディング（3 bytes）
        
        // 中程度のアクセス頻度
        float vx, vy, vz;       // 速度（12 bytes）
        int health;             // 体力（4 bytes）
        
        // コールドデータ（稀にアクセス）を末尾に配置
        string name;            // 名前
        vector<int> inventory;  // アイテム
        
        CacheFriendlyGameObject() : x(0), y(0), z(0), active(true), vx(0), vy(0), vz(0), health(100) {}
    };
    
    struct CacheUnfriendlyGameObject
    {
        // データが分散配置
        string name;            // 名前（大きなオブジェクト）
        float x;                // 位置X
        vector<int> inventory;  // アイテム（大きなオブジェクト）
        float y;                // 位置Y
        bool active;            // アクティブフラグ
        float z;                // 位置Z
        int health;             // 体力
        float vx, vy, vz;       // 速度
        
        CacheUnfriendlyGameObject() : x(0), y(0), z(0), active(true), vx(0), vy(0), vz(0), health(100) {}
    };
    
    void compareCacheEfficiency()
    {
        cout << "\n=== キャッシュ効率比較 ===\n";
        
        const int objectCount = 100000;
        const int iterations = 1000;
        
        cout << "CacheFriendlyGameObject サイズ: " << sizeof(CacheFriendlyGameObject) << " bytes\n";
        cout << "CacheUnfriendlyGameObject サイズ: " << sizeof(CacheUnfriendlyGameObject) << " bytes\n";
        
        // キャッシュフレンドリーなアクセス
        {
            BENCHMARK("キャッシュフレンドリー");
            
            vector<CacheFriendlyGameObject> objects(objectCount);
            
            for (int iter = 0; iter < iterations; ++iter)
            {
                for (auto& obj : objects)
                {
                    if (obj.active)
                    {
                        obj.x += obj.vx * 0.016f;
                        obj.y += obj.vy * 0.016f;
                        obj.z += obj.vz * 0.016f;
                    }
                }
            }
        }
        
        // キャッシュアンフレンドリーなアクセス
        {
            BENCHMARK("キャッシュアンフレンドリー");
            
            vector<CacheUnfriendlyGameObject> objects(objectCount);
            
            for (int iter = 0; iter < iterations; ++iter)
            {
                for (auto& obj : objects)
                {
                    if (obj.active)
                    {
                        obj.x += obj.vx * 0.016f;
                        obj.y += obj.vy * 0.016f;
                        obj.z += obj.vz * 0.016f;
                    }
                }
            }
        }
        
        // ランダムアクセス vs シーケンシャルアクセス
        cout << "\n--- アクセスパターン比較 ---\n";
        
        vector<CacheFriendlyGameObject> objects(objectCount);
        vector<int> indices(objectCount);
        iota(indices.begin(), indices.end(), 0);
        
        // シーケンシャルアクセス
        {
            BENCHMARK("シーケンシャルアクセス");
            
            for (int iter = 0; iter < iterations; ++iter)
            {
                for (int i = 0; i < objectCount; ++i)
                {
                    objects[i].x += objects[i].vx * 0.016f;
                }
            }
        }
        
        // ランダムアクセス
        {
            BENCHMARK("ランダムアクセス");
            
            random_device rd;
            mt19937 gen(rd());
            
            for (int iter = 0; iter < iterations; ++iter)
            {
                shuffle(indices.begin(), indices.end(), gen);
                
                for (int idx : indices)
                {
                    objects[idx].x += objects[idx].vx * 0.016f;
                }
            }
        }
    }
}

// メモリプリフェッチの活用
namespace PrefetchOptimization
{
    void comparePrefetchEfficiency()
    {
        cout << "\n=== プリフェッチ最適化比較 ===\n";
        
        const int arraySize = 1000000;
        const int stride = 64; // キャッシュラインサイズ
        
        vector<int> data(arraySize);
        iota(data.begin(), data.end(), 0);
        
        // プリフェッチなし
        {
            BENCHMARK("プリフェッチなし");
            
            long long sum = 0;
            for (size_t i = 0; i < data.size(); i += stride)
            {
                sum += data[i];
            }
            
            // コンパイラの最適化を防ぐ
            volatile long long result = sum;
            (void)result;
        }
        
        // プリフェッチあり
        {
            BENCHMARK("プリフェッチあり");
            
            long long sum = 0;
            for (size_t i = 0; i < data.size(); i += stride)
            {
                // 次のアクセス位置をプリフェッチ
                if (i + stride * 8 < data.size())
                {
                    _mm_prefetch(reinterpret_cast<const char*>(&data[i + stride * 8]), _MM_HINT_T0);
                }
                
                sum += data[i];
            }
            
            // コンパイラの最適化を防ぐ
            volatile long long result = sum;
            (void)result;
        }
    }
}

// マルチスレッド対応の最適化
namespace MultiThreadOptimization
{
    void compareThreadingStrategies()
    {
        cout << "\n=== マルチスレッド最適化比較 ===\n";
        
        const int dataSize = 10000000;
        vector<float> input(dataSize);
        vector<float> output(dataSize);
        
        // データ初期化
        for (int i = 0; i < dataSize; ++i)
        {
            input[i] = static_cast<float>(i);
        }
        
        // シングルスレッド処理
        {
            BENCHMARK("シングルスレッド");
            
            for (int i = 0; i < dataSize; ++i)
            {
                output[i] = sqrt(input[i]) + sin(input[i] * 0.001f);
            }
        }
        
        // マルチスレッド処理（単純分割）
        {
            BENCHMARK("マルチスレッド（単純分割）");
            
            const int numThreads = thread::hardware_concurrency();
            const int chunkSize = dataSize / numThreads;
            
            vector<thread> threads;
            
            for (int t = 0; t < numThreads; ++t)
            {
                int start = t * chunkSize;
                int end = (t == numThreads - 1) ? dataSize : start + chunkSize;
                
                threads.emplace_back([&input, &output, start, end]() {
                    for (int i = start; i < end; ++i)
                    {
                        output[i] = sqrt(input[i]) + sin(input[i] * 0.001f);
                    }
                });
            }
            
            for (auto& t : threads)
            {
                t.join();
            }
        }
        
        // マルチスレッド処理（NUMA考慮）
        {
            BENCHMARK("マルチスレッド（NUMA考慮）");
            
            const int numThreads = thread::hardware_concurrency();
            const int chunkSize = 64 * 1024; // 64KB チャンク
            
            vector<thread> threads;
            atomic<int> nextChunk{0};
            
            for (int t = 0; t < numThreads; ++t)
            {
                threads.emplace_back([&input, &output, &nextChunk, chunkSize, dataSize]() {
                    int chunk;
                    while ((chunk = nextChunk.fetch_add(1)) * chunkSize < dataSize)
                    {
                        int start = chunk * chunkSize;
                        int end = min(start + chunkSize, dataSize);
                        
                        for (int i = start; i < end; ++i)
                        {
                            output[i] = sqrt(input[i]) + sin(input[i] * 0.001f);
                        }
                    }
                });
            }
            
            for (auto& t : threads)
            {
                t.join();
            }
        }
    }
}

int main()
{
    cout << "=== メモリパフォーマンス最適化 ===\n";
    
    // コンテナパフォーマンス比較
    ArrayVsVector::compareContainerPerformance();
    
    // データレイアウト最適化
    SoAvsAoS::compareDataLayouts();
    
    // メモリプール最適化
    OptimizedMemoryPool::compareMemoryPools();
    
    // キャッシュ効率最適化
    CacheOptimization::compareCacheEfficiency();
    
    // プリフェッチ最適化
    PrefetchOptimization::comparePrefetchEfficiency();
    
    // マルチスレッド最適化
    MultiThreadOptimization::compareThreadingStrategies();
    
    cout << "\n=== メモリパフォーマンス最適化のポイント ===\n";
    cout << "1. データ局所性の向上（キャッシュ効率）\n";
    cout << "2. SoA（Structure of Arrays）レイアウトの活用\n";
    cout << "3. SIMD命令による並列処理\n";
    cout << "4. メモリアライメントの最適化\n";
    cout << "5. プリフェッチによる先読み最適化\n";
    cout << "6. NUMA（Non-Uniform Memory Access）の考慮\n";
    cout << "7. 効率的なメモリプールの設計\n";
    
    cout << "\n=== ゲーム開発での活用例 ===\n";
    cout << "1. パーティクルシステム: SoA + SIMD で大量のパーティクル処理\n";
    cout << "2. エンティティシステム: コンポーネントのメモリレイアウト最適化\n";
    cout << "3. 物理シミュレーション: 並列処理とキャッシュ効率の両立\n";
    cout << "4. レンダリング: 頂点データやマテリアルの効率的な配置\n";
    cout << "5. AI処理: 大量のエージェントの並列更新処理\n";
    cout << "6. オーディオ処理: サンプルデータのSIMD処理\n";
    
    cout << "\n=== 最適化の指針 ===\n";
    cout << "1. プロファイリング結果に基づいた最適化\n";
    cout << "2. ホットパス（頻繁に実行される処理）の特定\n";
    cout << "3. メモリアクセスパターンの分析\n";
    cout << "4. ハードウェア特性に応じた最適化\n";
    cout << "5. 可読性とのバランスを考慮した設計\n";
    cout << "6. 継続的なパフォーマンス測定と改善\n";
    
    cout << "\n=== 注意点 ===\n";
    cout << "1. 過度な最適化は可読性を損なう可能性\n";
    cout << "2. ハードウェア依存の最適化は移植性に影響\n";
    cout << "3. マイクロ最適化よりもアルゴリズム最適化を優先\n";
    cout << "4. 実際の使用環境でのテストが重要\n";
    cout << "5. 最適化の効果を定量的に測定する\n";
    
    return 0;
}

// メモリパフォーマンス最適化の重要なポイント：
// 1. メモリ階層（CPU→L1→L2→L3→RAM）を理解し、各レベルでの最適化を実施
// 2. データ構造とアルゴリズムの選択がパフォーマンスに与える影響は大きい
// 3. 現代のCPUは並列処理とSIMD命令を活用することで大幅な高速化が可能
// 4. プロファイリングツールを使用してボトルネックを特定してから最適化
// 5. ゲーム開発では特にリアルタイム性が重要なので継続的な最適化が必要