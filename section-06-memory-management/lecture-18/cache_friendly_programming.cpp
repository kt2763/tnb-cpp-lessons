// キャッシュフレンドリープログラミング
// ゲーム開発でのメモリアクセス最適化技術

#include <iostream>
#include <vector>
#include <array>
#include <memory>
#include <chrono>
#include <algorithm>
#include <random>
#include <cassert>
#include <immintrin.h>
#include <thread>

using namespace std;
using namespace chrono;

// キャッシュライン情報の取得
namespace CacheInfo
{
    constexpr size_t CACHE_LINE_SIZE = 64; // 一般的なx86_64のキャッシュラインサイズ
    
    // キャッシュラインアライメント用のマクロ
    #define CACHE_ALIGNED alignas(CACHE_LINE_SIZE)
    
    void printCacheInfo()
    {
        cout << "=== キャッシュ情報 ===\n";
        cout << "想定キャッシュラインサイズ: " << CACHE_LINE_SIZE << " bytes\n";
        cout << "L1キャッシュ容量（推定）: 32KB\n";
        cout << "L2キャッシュ容量（推定）: 256KB\n";
        cout << "L3キャッシュ容量（推定）: 8MB\n\n";
    }
}

// False Sharing の問題と解決
namespace FalseSharingDemo
{
    // False Sharing が発生する構造体
    struct BadCounters
    {
        long counter1;
        long counter2;
        long counter3;
        long counter4;
    };
    
    // False Sharing を回避する構造体
    struct GoodCounters
    {
        CACHE_ALIGNED long counter1;
        CACHE_ALIGNED long counter2;
        CACHE_ALIGNED long counter3;
        CACHE_ALIGNED long counter4;
    };
    
    void demonstrateFalseSharing()
    {
        cout << "=== False Sharing のデモ ===\n";
        
        const int iterations = 10000000;
        
        cout << "BadCounters サイズ: " << sizeof(BadCounters) << " bytes\n";
        cout << "GoodCounters サイズ: " << sizeof(GoodCounters) << " bytes\n\n";
        
        // False Sharing が発生するケース
        {
            cout << "False Sharing ありのケース:\n";
            BadCounters counters = {0, 0, 0, 0};
            
            auto start = high_resolution_clock::now();
            
            vector<thread> threads;
            
            threads.emplace_back([&counters, iterations]() {
                for (int i = 0; i < iterations; ++i)
                {
                    counters.counter1++;
                }
            });
            
            threads.emplace_back([&counters, iterations]() {
                for (int i = 0; i < iterations; ++i)
                {
                    counters.counter2++;
                }
            });
            
            threads.emplace_back([&counters, iterations]() {
                for (int i = 0; i < iterations; ++i)
                {
                    counters.counter3++;
                }
            });
            
            threads.emplace_back([&counters, iterations]() {
                for (int i = 0; i < iterations; ++i)
                {
                    counters.counter4++;
                }
            });
            
            for (auto& t : threads)
            {
                t.join();
            }
            
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(end - start).count();
            
            cout << "実行時間: " << duration << " ms\n";
            cout << "結果: " << counters.counter1 << ", " << counters.counter2 
                 << ", " << counters.counter3 << ", " << counters.counter4 << "\n\n";
        }
        
        // False Sharing を回避するケース
        {
            cout << "False Sharing なしのケース:\n";
            GoodCounters counters = {0, 0, 0, 0};
            
            auto start = high_resolution_clock::now();
            
            vector<thread> threads;
            
            threads.emplace_back([&counters, iterations]() {
                for (int i = 0; i < iterations; ++i)
                {
                    counters.counter1++;
                }
            });
            
            threads.emplace_back([&counters, iterations]() {
                for (int i = 0; i < iterations; ++i)
                {
                    counters.counter2++;
                }
            });
            
            threads.emplace_back([&counters, iterations]() {
                for (int i = 0; i < iterations; ++i)
                {
                    counters.counter3++;
                }
            });
            
            threads.emplace_back([&counters, iterations]() {
                for (int i = 0; i < iterations; ++i)
                {
                    counters.counter4++;
                }
            });
            
            for (auto& t : threads)
            {
                t.join();
            }
            
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(end - start).count();
            
            cout << "実行時間: " << duration << " ms\n";
            cout << "結果: " << counters.counter1 << ", " << counters.counter2 
                 << ", " << counters.counter3 << ", " << counters.counter4 << "\n\n";
        }
    }
}

// データレイアウトの最適化
namespace DataLayoutOptimization
{
    // 悪いデータレイアウト：ホットデータとコールドデータが混在
    struct BadGameEntity
    {
        string name;          // コールドデータ（大きい）
        float x, y, z;        // ホットデータ
        vector<int> skills;   // コールドデータ（大きい）
        float vx, vy, vz;     // ホットデータ
        int health;           // ホットデータ
        string description;   // コールドデータ（大きい）
        bool active;          // ホットデータ
        map<string, int> stats; // コールドデータ（大きい）
        
        BadGameEntity() : x(0), y(0), z(0), vx(0), vy(0), vz(0), health(100), active(true) {}
    };
    
    // 良いデータレイアウト：ホットデータを集約
    struct GoodGameEntity
    {
        // ホットデータブロック（頻繁にアクセス）
        float x, y, z;        // 位置（12 bytes）
        float vx, vy, vz;     // 速度（12 bytes）
        int health;           // 体力（4 bytes）
        bool active;          // アクティブフラグ（1 byte）
        char padding[3];      // パディング（3 bytes）
        // 合計32 bytes（キャッシュライン半分）
        
        // コールドデータブロック（稀にアクセス）
        string name;
        vector<int> skills;
        string description;
        map<string, int> stats;
        
        GoodGameEntity() : x(0), y(0), z(0), vx(0), vy(0), vz(0), health(100), active(true) {}
    };
    
    void compareDataLayouts()
    {
        cout << "=== データレイアウト最適化比較 ===\n";
        
        const int entityCount = 100000;
        const int iterations = 1000;
        
        cout << "BadGameEntity サイズ: " << sizeof(BadGameEntity) << " bytes\n";
        cout << "GoodGameEntity サイズ: " << sizeof(GoodGameEntity) << " bytes\n\n";
        
        // 悪いレイアウトでの処理
        {
            cout << "悪いデータレイアウトでの処理:\n";
            
            vector<BadGameEntity> entities(entityCount);
            
            auto start = high_resolution_clock::now();
            
            for (int iter = 0; iter < iterations; ++iter)
            {
                for (auto& entity : entities)
                {
                    if (entity.active)
                    {
                        entity.x += entity.vx * 0.016f;
                        entity.y += entity.vy * 0.016f;
                        entity.z += entity.vz * 0.016f;
                        
                        if (entity.health > 0)
                        {
                            entity.health--;
                        }
                    }
                }
            }
            
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end - start).count();
            cout << "実行時間: " << duration << " マイクロ秒\n\n";
        }
        
        // 良いレイアウトでの処理
        {
            cout << "良いデータレイアウトでの処理:\n";
            
            vector<GoodGameEntity> entities(entityCount);
            
            auto start = high_resolution_clock::now();
            
            for (int iter = 0; iter < iterations; ++iter)
            {
                for (auto& entity : entities)
                {
                    if (entity.active)
                    {
                        entity.x += entity.vx * 0.016f;
                        entity.y += entity.vy * 0.016f;
                        entity.z += entity.vz * 0.016f;
                        
                        if (entity.health > 0)
                        {
                            entity.health--;
                        }
                    }
                }
            }
            
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end - start).count();
            cout << "実行時間: " << duration << " マイクロ秒\n\n";
        }
    }
}

// SoA (Structure of Arrays) の詳細実装
namespace AdvancedSoA
{
    // 3Dベクターの演算を高速化するSoA実装
    class Vector3SoA
    {
    private:
        CACHE_ALIGNED vector<float> x_data;
        CACHE_ALIGNED vector<float> y_data;
        CACHE_ALIGNED vector<float> z_data;
        
    public:
        void resize(size_t count)
        {
            x_data.resize(count);
            y_data.resize(count);
            z_data.resize(count);
        }
        
        void set(size_t index, float x, float y, float z)
        {
            x_data[index] = x;
            y_data[index] = y;
            z_data[index] = z;
        }
        
        void get(size_t index, float& x, float& y, float& z) const
        {
            x = x_data[index];
            y = y_data[index];
            z = z_data[index];
        }
        
        // SIMD を使用したベクター加算
        void add(const Vector3SoA& other)
        {
            assert(x_data.size() == other.x_data.size());
            
            const size_t count = x_data.size();
            const size_t simd_count = (count / 8) * 8;
            
            // SIMD で8要素ずつ処理
            for (size_t i = 0; i < simd_count; i += 8)
            {
                __m256 x1 = _mm256_load_ps(&x_data[i]);
                __m256 y1 = _mm256_load_ps(&y_data[i]);
                __m256 z1 = _mm256_load_ps(&z_data[i]);
                
                __m256 x2 = _mm256_load_ps(&other.x_data[i]);
                __m256 y2 = _mm256_load_ps(&other.y_data[i]);
                __m256 z2 = _mm256_load_ps(&other.z_data[i]);
                
                __m256 result_x = _mm256_add_ps(x1, x2);
                __m256 result_y = _mm256_add_ps(y1, y2);
                __m256 result_z = _mm256_add_ps(z1, z2);
                
                _mm256_store_ps(&x_data[i], result_x);
                _mm256_store_ps(&y_data[i], result_y);
                _mm256_store_ps(&z_data[i], result_z);
            }
            
            // 残りの要素をスカラで処理
            for (size_t i = simd_count; i < count; ++i)
            {
                x_data[i] += other.x_data[i];
                y_data[i] += other.y_data[i];
                z_data[i] += other.z_data[i];
            }
        }
        
        // SIMD を使用したスカラ乗算
        void scale(float scalar)
        {
            const size_t count = x_data.size();
            const size_t simd_count = (count / 8) * 8;
            
            __m256 scale_vec = _mm256_set1_ps(scalar);
            
            for (size_t i = 0; i < simd_count; i += 8)
            {
                __m256 x = _mm256_load_ps(&x_data[i]);
                __m256 y = _mm256_load_ps(&y_data[i]);
                __m256 z = _mm256_load_ps(&z_data[i]);
                
                x = _mm256_mul_ps(x, scale_vec);
                y = _mm256_mul_ps(y, scale_vec);
                z = _mm256_mul_ps(z, scale_vec);
                
                _mm256_store_ps(&x_data[i], x);
                _mm256_store_ps(&y_data[i], y);
                _mm256_store_ps(&z_data[i], z);
            }
            
            for (size_t i = simd_count; i < count; ++i)
            {
                x_data[i] *= scalar;
                y_data[i] *= scalar;
                z_data[i] *= scalar;
            }
        }
        
        size_t size() const { return x_data.size(); }
        
        // デバッグ用の出力
        void print(size_t start, size_t count) const
        {
            for (size_t i = start; i < min(start + count, size()); ++i)
            {
                cout << "Vector[" << i << "]: (" << x_data[i] << ", " << y_data[i] << ", " << z_data[i] << ")\n";
            }
        }
    };
    
    // 従来のAoS実装
    struct Vector3AoS
    {
        float x, y, z;
        
        Vector3AoS(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
        
        Vector3AoS& operator+=(const Vector3AoS& other)
        {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }
        
        Vector3AoS& operator*=(float scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }
    };
    
    void compareSoAvsAoS()
    {
        cout << "=== 高度なSoA vs AoS比較 ===\n";
        
        const int vectorCount = 1000000;
        const int iterations = 100;
        
        // AoS での処理
        {
            cout << "AoS (Array of Structures):\n";
            
            vector<Vector3AoS> vectors1(vectorCount);
            vector<Vector3AoS> vectors2(vectorCount);
            
            // 初期化
            for (int i = 0; i < vectorCount; ++i)
            {
                vectors1[i] = Vector3AoS(static_cast<float>(i), static_cast<float>(i * 2), static_cast<float>(i * 3));
                vectors2[i] = Vector3AoS(static_cast<float>(i % 100), static_cast<float>((i * 2) % 100), static_cast<float>((i * 3) % 100));
            }
            
            auto start = high_resolution_clock::now();
            
            for (int iter = 0; iter < iterations; ++iter)
            {
                for (int i = 0; i < vectorCount; ++i)
                {
                    vectors1[i] += vectors2[i];
                    vectors1[i] *= 0.99f;
                }
            }
            
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end - start).count();
            cout << "実行時間: " << duration << " マイクロ秒\n\n";
        }
        
        // SoA での処理
        {
            cout << "SoA (Structure of Arrays) + SIMD:\n";
            
            Vector3SoA vectors1, vectors2;
            vectors1.resize(vectorCount);
            vectors2.resize(vectorCount);
            
            // 初期化
            for (int i = 0; i < vectorCount; ++i)
            {
                vectors1.set(i, static_cast<float>(i), static_cast<float>(i * 2), static_cast<float>(i * 3));
                vectors2.set(i, static_cast<float>(i % 100), static_cast<float>((i * 2) % 100), static_cast<float>((i * 3) % 100));
            }
            
            auto start = high_resolution_clock::now();
            
            for (int iter = 0; iter < iterations; ++iter)
            {
                vectors1.add(vectors2);
                vectors1.scale(0.99f);
            }
            
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end - start).count();
            cout << "実行時間: " << duration << " マイクロ秒\n\n";
        }
    }
}

// メモリプリフェッチの詳細活用
namespace PrefetchOptimization
{
    // プリフェッチ距離の最適化
    template<int PrefetchDistance>
    void processArrayWithPrefetch(const vector<float>& input, vector<float>& output)
    {
        const size_t size = input.size();
        
        for (size_t i = 0; i < size; ++i)
        {
            // プリフェッチ
            if (i + PrefetchDistance < size)
            {
                _mm_prefetch(reinterpret_cast<const char*>(&input[i + PrefetchDistance]), _MM_HINT_T0);
                _mm_prefetch(reinterpret_cast<const char*>(&output[i + PrefetchDistance]), _MM_HINT_T0);
            }
            
            // 計算処理（重い処理をシミュレート）
            output[i] = sqrt(input[i]) + sin(input[i] * 0.001f) + cos(input[i] * 0.001f);
        }
    }
    
    void optimizePrefetchDistance()
    {
        cout << "=== プリフェッチ距離最適化 ===\n";
        
        const int arraySize = 1000000;
        vector<float> input(arraySize);
        vector<float> output(arraySize);
        
        // データ初期化
        for (int i = 0; i < arraySize; ++i)
        {
            input[i] = static_cast<float>(i);
        }
        
        // プリフェッチなし
        {
            cout << "プリフェッチなし:\n";
            auto start = high_resolution_clock::now();
            
            for (size_t i = 0; i < input.size(); ++i)
            {
                output[i] = sqrt(input[i]) + sin(input[i] * 0.001f) + cos(input[i] * 0.001f);
            }
            
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end - start).count();
            cout << "実行時間: " << duration << " マイクロ秒\n\n";
        }
        
        // 異なるプリフェッチ距離でテスト
        vector<int> distances = {16, 32, 64, 128, 256};
        
        for (int distance : distances)
        {
            cout << "プリフェッチ距離 " << distance << ":\n";
            auto start = high_resolution_clock::now();
            
            switch (distance)
            {
            case 16:  processArrayWithPrefetch<16>(input, output); break;
            case 32:  processArrayWithPrefetch<32>(input, output); break;
            case 64:  processArrayWithPrefetch<64>(input, output); break;
            case 128: processArrayWithPrefetch<128>(input, output); break;
            case 256: processArrayWithPrefetch<256>(input, output); break;
            }
            
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end - start).count();
            cout << "実行時間: " << duration << " マイクロ秒\n\n";
        }
    }
}

// キャッシュ効率的なデータ構造
namespace CacheEfficientDataStructures
{
    // キャッシュ効率的な動的配列
    template<typename T, size_t ChunkSize = 64 / sizeof(T)>
    class CacheEfficientVector
    {
    private:
        vector<array<T, ChunkSize>> chunks;
        size_t totalSize;
        
    public:
        CacheEfficientVector() : totalSize(0) {}
        
        void push_back(const T& value)
        {
            size_t chunkIndex = totalSize / ChunkSize;
            size_t elementIndex = totalSize % ChunkSize;
            
            if (chunkIndex >= chunks.size())
            {
                chunks.emplace_back();
            }
            
            chunks[chunkIndex][elementIndex] = value;
            totalSize++;
        }
        
        T& operator[](size_t index)
        {
            size_t chunkIndex = index / ChunkSize;
            size_t elementIndex = index % ChunkSize;
            return chunks[chunkIndex][elementIndex];
        }
        
        const T& operator[](size_t index) const
        {
            size_t chunkIndex = index / ChunkSize;
            size_t elementIndex = index % ChunkSize;
            return chunks[chunkIndex][elementIndex];
        }
        
        size_t size() const { return totalSize; }
        
        // 効率的なイテレーション
        template<typename Func>
        void forEach(Func func)
        {
            for (size_t chunkIndex = 0; chunkIndex < chunks.size(); ++chunkIndex)
            {
                size_t elementsInChunk = min(ChunkSize, totalSize - chunkIndex * ChunkSize);
                
                for (size_t elementIndex = 0; elementIndex < elementsInChunk; ++elementIndex)
                {
                    func(chunks[chunkIndex][elementIndex]);
                }
            }
        }
    };
    
    void compareDynamicArrays()
    {
        cout << "=== キャッシュ効率的な動的配列比較 ===\n";
        
        const int elementCount = 1000000;
        
        // 標準のvector
        {
            cout << "標準のstd::vector:\n";
            
            vector<int> data;
            data.reserve(elementCount);
            
            auto start = high_resolution_clock::now();
            
            for (int i = 0; i < elementCount; ++i)
            {
                data.push_back(i);
            }
            
            int sum = 0;
            for (const auto& value : data)
            {
                sum += value;
            }
            
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end - start).count();
            cout << "実行時間: " << duration << " マイクロ秒, 合計: " << sum << "\n\n";
        }
        
        // キャッシュ効率的なvector
        {
            cout << "キャッシュ効率的なvector:\n";
            
            CacheEfficientVector<int> data;
            
            auto start = high_resolution_clock::now();
            
            for (int i = 0; i < elementCount; ++i)
            {
                data.push_back(i);
            }
            
            int sum = 0;
            data.forEach([&sum](int value) {
                sum += value;
            });
            
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end - start).count();
            cout << "実行時間: " << duration << " マイクロ秒, 合計: " << sum << "\n\n";
        }
    }
}

int main()
{
    cout << "=== キャッシュフレンドリープログラミング ===\n\n";
    
    // キャッシュ情報の表示
    CacheInfo::printCacheInfo();
    
    // False Sharing のデモ
    FalseSharingDemo::demonstrateFalseSharing();
    
    // データレイアウト最適化
    DataLayoutOptimization::compareDataLayouts();
    
    // 高度なSoA実装
    AdvancedSoA::compareSoAvsAoS();
    
    // プリフェッチ最適化
    PrefetchOptimization::optimizePrefetchDistance();
    
    // キャッシュ効率的なデータ構造
    CacheEfficientDataStructures::compareDynamicArrays();
    
    cout << "=== キャッシュフレンドリープログラミングの原則 ===\n";
    cout << "1. データ局所性の最大化\n";
    cout << "   - 関連するデータを近くに配置\n";
    cout << "   - ホットデータとコールドデータの分離\n";
    cout << "2. シーケンシャルアクセスパターンの採用\n";
    cout << "   - ランダムアクセスを避ける\n";
    cout << "   - プリフェッチャーが予測しやすいパターン\n";
    cout << "3. False Sharing の回避\n";
    cout << "   - キャッシュラインアライメントの活用\n";
    cout << "   - スレッド間でのデータ分離\n";
    cout << "4. SIMD に適したデータレイアウト\n";
    cout << "   - SoA (Structure of Arrays) の活用\n";
    cout << "   - 適切なメモリアライメント\n";
    
    cout << "\n=== ゲーム開発での具体例 ===\n";
    cout << "1. エンティティコンポーネントシステム\n";
    cout << "   - コンポーネント毎にSoAレイアウト\n";
    cout << "   - システム毎の効率的な反復処理\n";
    cout << "2. パーティクルシステム\n";
    cout << "   - 位置、速度、色などを分離配置\n";
    cout << "   - SIMD による並列更新処理\n";
    cout << "3. 物理シミュレーション\n";
    cout << "   - 剛体の位置・回転情報の連続配置\n";
    cout << "   - 効率的な衝突検出処理\n";
    cout << "4. レンダリングシステム\n";
    cout << "   - 頂点データの最適な配置\n";
    cout << "   - ドローコールの最小化\n";
    
    cout << "\n=== パフォーマンス測定のベストプラクティス ===\n";
    cout << "1. 実際のハードウェアでの測定\n";
    cout << "2. ワーキングセットサイズの考慮\n";
    cout << "3. キャッシュミス率の監視\n";
    cout << "4. プロファイラーを使用した詳細分析\n";
    cout << "5. 複数回の測定による平均化\n";
    
    cout << "\n=== 注意点 ===\n";
    cout << "1. 可読性とのバランス\n";
    cout << "2. ハードウェア依存性の考慮\n";
    cout << "3. 最適化の効果測定\n";
    cout << "4. 維持管理コストの評価\n";
    
    return 0;
}

// キャッシュフレンドリープログラミングの重要なポイント：
// 1. CPU キャッシュ階層の理解（L1、L2、L3キャッシュの特性）
// 2. メモリアクセスパターンがパフォーマンスに与える影響は非常に大きい
// 3. False Sharing は並列処理において特に重要な問題
// 4. データ構造の設計時にキャッシュ効率を考慮することが重要
// 5. プロファイリングツールを使用してキャッシュミス率を監視する