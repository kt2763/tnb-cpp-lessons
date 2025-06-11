// メモリアライメントと最適化
// ゲーム開発でのパフォーマンス最適化技法

#include <iostream>
#include <chrono>
#include <vector>
#include <cstdint>
#include <memory>
#include <new>
#include <cassert>

using namespace std;
using namespace chrono;

// アライメントの基本デモ
void alignmentBasics()
{
    cout << "\n=== メモリアライメントの基本 ===\n";
    
    cout << "C++の基本型のアライメント要求:\n";
    cout << "char: " << alignof(char) << " bytes\n";
    cout << "short: " << alignof(short) << " bytes\n";
    cout << "int: " << alignof(int) << " bytes\n";
    cout << "long: " << alignof(long) << " bytes\n";
    cout << "float: " << alignof(float) << " bytes\n";
    cout << "double: " << alignof(double) << " bytes\n";
    cout << "long double: " << alignof(long double) << " bytes\n";
    cout << "void*: " << alignof(void*) << " bytes\n";
    
    cout << "\nシステムの最大アライメント: " << alignof(max_align_t) << " bytes\n";
}

// 適切でないアライメントの例
struct BadAlignment
{
    char a;     // 1バイト
    double b;   // 8バイト（パディングで空きができる）
    int c;      // 4バイト
    char d;     // 1バイト
};

// 最適化されたアライメントの例
struct GoodAlignment
{
    double b;   // 8バイト
    int c;      // 4バイト
    char a;     // 1バイト
    char d;     // 1バイト
    // 合計: 16バイト（パディングを最小化）
};

// ゲームキャラクターの悪いアライメント例
struct BadGameCharacter
{
    bool isAlive;           // 1バイト
    double positionX;       // 8バイト
    int health;             // 4バイト
    bool isVisible;         // 1バイト
    double positionY;       // 8バイト
    int mana;               // 4バイト
    char level;             // 1バイト
};

// ゲームキャラクターの最適化されたアライメント例
struct GoodGameCharacter
{
    double positionX;       // 8バイト
    double positionY;       // 8バイト
    int health;             // 4バイト
    int mana;               // 4バイト
    bool isAlive;           // 1バイト
    bool isVisible;         // 1バイト
    char level;             // 1バイト
    // 合計: 32バイト（パディングを最小化）
};

void structureSizeComparison()
{
    cout << "\n=== 構造体のサイズ比較 ===\n";
    
    cout << "BadAlignmentのサイズ: " << sizeof(BadAlignment) << " bytes\n";
    cout << "GoodAlignmentのサイズ: " << sizeof(GoodAlignment) << " bytes\n";
    
    cout << "\nBadGameCharacterのサイズ: " << sizeof(BadGameCharacter) << " bytes\n";
    cout << "GoodGameCharacterのサイズ: " << sizeof(GoodGameCharacter) << " bytes\n";
    
    cout << "\nメモリ節約率: " 
         << (sizeof(BadGameCharacter) - sizeof(GoodGameCharacter)) * 100.0 / sizeof(BadGameCharacter) 
         << "%\n";
    
    // メンバーのオフセットを確認
    cout << "\nBadGameCharacterのメンバーオフセット:\n";
    cout << "isAlive: " << offsetof(BadGameCharacter, isAlive) << "\n";
    cout << "positionX: " << offsetof(BadGameCharacter, positionX) << "\n";
    cout << "health: " << offsetof(BadGameCharacter, health) << "\n";
    cout << "isVisible: " << offsetof(BadGameCharacter, isVisible) << "\n";
    cout << "positionY: " << offsetof(BadGameCharacter, positionY) << "\n";
    cout << "mana: " << offsetof(BadGameCharacter, mana) << "\n";
    cout << "level: " << offsetof(BadGameCharacter, level) << "\n";
}

// 手動でのアライメント指定
struct alignas(16) AlignedGameObject
{
    float x, y, z;      // 位置
    float w;            // パディング用
};

struct alignas(32) HighlyAlignedData
{
    double matrix[4];   // 4x4マトリックスの一部
};

void manualAlignmentDemo()
{
    cout << "\n=== 手動アライメント指定 ===\n";
    
    cout << "AlignedGameObjectのアライメント: " << alignof(AlignedGameObject) << " bytes\n";
    cout << "AlignedGameObjectのサイズ: " << sizeof(AlignedGameObject) << " bytes\n";
    
    cout << "HighlyAlignedDataのアライメント: " << alignof(HighlyAlignedData) << " bytes\n";
    cout << "HighlyAlignedDataのサイズ: " << sizeof(HighlyAlignedData) << " bytes\n";
    
    // アライメントされたメモリを動的に確保
    auto alignedPtr = make_unique<AlignedGameObject>();
    cout << "\n動的確保したアライメントオブジェクトのアドレス: " << alignedPtr.get() << "\n";
    cout << "アドレスの16バイト境界確認: " << (reinterpret_cast<uintptr_t>(alignedPtr.get()) % 16 == 0 ? "正しい" : "間違い") << "\n";
}

// キャッシュラインを意識したデータ構造
struct CacheFriendlyParticle
{
    float x, y, z;          // 位置（12バイト）
    float vx, vy, vz;       // 速度（12バイト）
    float life;             // 生存時間（4バイト）
    uint32_t color;         // 色（4バイト）
    // 合計: 32バイト（キャッシュラインサイズの倍数）
};

struct CacheUnfriendlyParticle
{
    double x, y, z;         // 位置（24バイト）
    bool isActive;          // アクティブフラグ（1バイト + 7バイトパディング）
    double vx, vy, vz;      // 速度（24バイト）
    double life;            // 生存時間（8バイト）
    uint32_t color;         // 色（4バイト）
    // 合計: 72バイト（キャッシュライン効率が悪い）
};

void cachePerformanceTest()
{
    cout << "\n=== キャッシュパフォーマンステスト ===\n";
    
    const size_t particleCount = 1000000;
    const int iterations = 100;
    
    cout << "CacheFriendlyParticleのサイズ: " << sizeof(CacheFriendlyParticle) << " bytes\n";
    cout << "CacheUnfriendlyParticleのサイズ: " << sizeof(CacheUnfriendlyParticle) << " bytes\n";
    
    // キャッシュフレンドリーなパーティクルのテスト
    {
        vector<CacheFriendlyParticle> particles(particleCount);
        
        auto start = high_resolution_clock::now();
        
        for (int iter = 0; iter < iterations; ++iter)
        {
            for (auto& p : particles)
            {
                p.x += p.vx * 0.016f;
                p.y += p.vy * 0.016f;
                p.z += p.vz * 0.016f;
                p.life -= 0.016f;
            }
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        
        cout << "\nCacheFriendlyParticleの処理時間: " << duration << " マイクロ秒\n";
    }
    
    // キャッシュアンフレンドリーなパーティクルのテスト
    {
        vector<CacheUnfriendlyParticle> particles(particleCount);
        
        auto start = high_resolution_clock::now();
        
        for (int iter = 0; iter < iterations; ++iter)
        {
            for (auto& p : particles)
            {
                p.x += p.vx * 0.016;
                p.y += p.vy * 0.016;
                p.z += p.vz * 0.016;
                p.life -= 0.016;
            }
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        
        cout << "CacheUnfriendlyParticleの処理時間: " << duration << " マイクロ秒\n";
    }
}

// カスタムアロケータのアライメント対応
class AlignedAllocator
{
public:
    static void* allocate(size_t size, size_t alignment)
    {
        cout << "アライメントされたメモリを確保: " << size << " bytes, " << alignment << " bytes境界\n";
        
        void* ptr = aligned_alloc(alignment, size);
        if (!ptr)
        {
            throw bad_alloc();
        }
        
        cout << "確保されたアドレス: " << ptr << "\n";
        cout << "アライメント確認: " << (reinterpret_cast<uintptr_t>(ptr) % alignment == 0 ? "正しい" : "間違い") << "\n";
        
        return ptr;
    }
    
    static void deallocate(void* ptr)
    {
        if (ptr)
        {
            cout << "アライメントされたメモリを解放: " << ptr << "\n";
            free(ptr);
        }
    }
};

void customAlignedAllocationDemo()
{
    cout << "\n=== カスタムアライメントアロケータ ===\n";
    
    // 64バイト境界でメモリを確保
    void* ptr1 = AlignedAllocator::allocate(sizeof(HighlyAlignedData), 64);
    
    if (ptr1)
    {
        auto* data = reinterpret_cast<HighlyAlignedData*>(ptr1);
        // データを使用
        for (int i = 0; i < 4; ++i)
        {
            data->matrix[i] = i * 1.5;
        }
        
        cout << "データの使用例: matrix[0] = " << data->matrix[0] << "\n";
        
        AlignedAllocator::deallocate(ptr1);
    }
    
    // ゲームオブジェクトの配列をアライメントして確保
    size_t objectCount = 1000;
    size_t totalSize = sizeof(AlignedGameObject) * objectCount;
    
    void* ptr2 = AlignedAllocator::allocate(totalSize, alignof(AlignedGameObject));
    
    if (ptr2)
    {
        auto* objects = reinterpret_cast<AlignedGameObject*>(ptr2);
        
        // 配列の初期化
        for (size_t i = 0; i < objectCount; ++i)
        {
            new(&objects[i]) AlignedGameObject();
            objects[i].x = static_cast<float>(i);
            objects[i].y = static_cast<float>(i * 2);
            objects[i].z = static_cast<float>(i * 3);
        }
        
        cout << "\nアライメントされたオブジェクト配列を初期化完了\n";
        cout << "最初のオブジェクトの値: (" << objects[0].x << ", " << objects[0].y << ", " << objects[0].z << ")\n";
        
        // デストラクタを呼び出す必要はない（POD型）
        
        AlignedAllocator::deallocate(ptr2);
    }
}

// SIMD命令での活用例
void simdAlignmentDemo()
{
    cout << "\n=== SIMD命令でのアライメント活用 ===\n";
    
    cout << "SIMD命令ではデータが適切にアライメントされていることが重要です\n";
    cout << "16バイト境界: SSE命令用\n";
    cout << "32バイト境界: AVX命令用\n";
    cout << "64バイト境界: AVX-512命令用\n";
    
    // 16バイトアライメントでfloat配列を確保（SSE用）
    alignas(16) float vector4[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    
    cout << "\nSSE用のvector4のアドレス: " << vector4 << "\n";
    cout << "16バイト境界確認: " << (reinterpret_cast<uintptr_t>(vector4) % 16 == 0 ? "正しい" : "間違い") << "\n";
    
    // 32バイトアライメントでfloat配列を確保（AVX用）
    alignas(32) float vector8[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    
    cout << "AVX用のvector8のアドレス: " << vector8 << "\n";
    cout << "32バイト境界確認: " << (reinterpret_cast<uintptr_t>(vector8) % 32 == 0 ? "正しい" : "間違い") << "\n";
    
    cout << "\n注意: 実際のSIMD命令の使用には適切なヘッダーとライブラリが必要です\n";
}

int main()
{
    cout << "=== メモリアライメントと最適化 ===\n";
    
    // アライメントの基本
    alignmentBasics();
    
    // 構造体のサイズ比較
    structureSizeComparison();
    
    // 手動アライメント指定
    manualAlignmentDemo();
    
    // キャッシュパフォーマンステスト
    cachePerformanceTest();
    
    // カスタムアライメントアロケータ
    customAlignedAllocationDemo();
    
    // SIMDでのアライメント活用
    simdAlignmentDemo();
    
    cout << "\n=== メモリアライメントのメリット ===\n";
    cout << "1. CPUキャッシュの効率的な利用\n";
    cout << "2. SIMD命令の正しい実行\n";
    cout << "3. メモリアクセスの高速化\n";
    cout << "4. メモリ使用量の最適化\n";
    cout << "5. ハードウェアのパフォーマンスを最大限活用\n";
    
    cout << "\n=== ゲーム開発での活用例 ===\n";
    cout << "1. パーティクルシステムのデータ構造最適化\n";
    cout << "2. ゲームオブジェクトのメモリレイアウト最適化\n";
    cout << "3. ベクトルやマトリックス演算のSIMD最適化\n";
    cout << "4. オーディオサンプルやテクスチャデータの効率的処理\n";
    cout << "5. メッシュやアニメーションデータのキャッシュ効率向上\n";
    
    cout << "\n=== パフォーマンス最適化のポイント ===\n";
    cout << "1. 構造体のメンバーをサイズ順に並べる\n";
    cout << "2. 頻繁にアクセスするデータを近くに配置\n";
    cout << "3. キャッシュラインサイズ（64バイト）を意識したデータ構造\n";
    cout << "4. SIMD命令を使用する際は適切なアライメントを確保\n";
    cout << "5. メモリプールやオブジェクトプールでメモリ断片化を防ぐ\n";
    
    return 0;
}

// メモリアライメントの重要なポイント：
// 1. CPUアーキテクチャに合わせたアライメントでパフォーマンス向上
// 2. キャッシュラインの効率的な利用でメモリアクセスを最適化
// 3. SIMD命令での並列処理で計算パフォーマンスを向上
// 4. 構造体のパディングを最小化してメモリ使用量を削減
// 5. ゲームエンジンでは特にリアルタイム性が重要なので積極的に活用