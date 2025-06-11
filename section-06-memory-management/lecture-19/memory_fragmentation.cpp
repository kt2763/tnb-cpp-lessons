// メモリ断片化の問題と対策
// ゲーム開発でのメモリ効率化技術

#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <algorithm>
#include <chrono>
#include <random>
#include <cassert>
#include <iomanip>

using namespace std;
using namespace chrono;

// メモリ断片化をシミュレートするアロケータ
class FragmentationSimulator
{
private:
    struct MemoryBlock
    {
        void* address;
        size_t size;
        bool allocated;
        
        MemoryBlock(void* addr, size_t sz, bool alloc = false)
            : address(addr), size(sz), allocated(alloc) {}
    };
    
    vector<MemoryBlock> blocks;
    size_t totalMemory;
    size_t usedMemory;
    size_t fragmentedMemory;
    int allocationCount;
    int deallocationCount;
    
public:
    FragmentationSimulator(size_t totalSize) 
        : totalMemory(totalSize), usedMemory(0), fragmentedMemory(0)
        , allocationCount(0), deallocationCount(0)
    {
        // 初期状態：全メモリが1つの空きブロック
        blocks.emplace_back(malloc(totalSize), totalSize, false);
        cout << "メモリシミュレータを初期化 (総容量: " << totalSize << " bytes)\n";
    }
    
    ~FragmentationSimulator()
    {
        for (auto& block : blocks)
        {
            if (block.address)
            {
                free(block.address);
            }
        }
        cout << "メモリシミュレータを終了\n";
    }
    
    void* allocate(size_t size)
    {
        // First Fit アルゴリズム：最初に見つかった十分な空きブロックを使用
        for (auto it = blocks.begin(); it != blocks.end(); ++it)
        {
            if (!it->allocated && it->size >= size)
            {
                void* result = it->address;
                
                // ブロックを分割
                if (it->size > size)
                {
                    void* remainingAddr = static_cast<char*>(it->address) + size;
                    size_t remainingSize = it->size - size;
                    
                    it->size = size;
                    it->allocated = true;
                    
                    // 残りの部分を新しい空きブロックとして挿入
                    blocks.insert(it + 1, MemoryBlock(remainingAddr, remainingSize, false));
                }
                else
                {
                    it->allocated = true;
                }
                
                usedMemory += size;
                allocationCount++;
                
                cout << "メモリ確保: " << size << " bytes at " << result 
                     << " (確保数: " << allocationCount << ")\n";
                
                return result;
            }
        }
        
        cout << "メモリ確保失敗: " << size << " bytes (断片化により確保不可)\n";
        return nullptr;
    }
    
    void deallocate(void* ptr)
    {
        if (!ptr) return;
        
        // 該当するブロックを見つける
        for (auto it = blocks.begin(); it != blocks.end(); ++it)
        {
            if (it->address == ptr && it->allocated)
            {
                it->allocated = false;
                usedMemory -= it->size;
                deallocationCount++;
                
                cout << "メモリ解放: " << it->size << " bytes at " << ptr 
                     << " (解放数: " << deallocationCount << ")\n";
                
                // 隣接する空きブロックと結合
                coalesceBlocks(it);
                return;
            }
        }
        
        cout << "警告: 無効なポインタの解放を試行: " << ptr << "\n";
    }
    
    void printMemoryMap() const
    {
        cout << "\n=== メモリマップ ===\n";
        cout << "アドレス\t\tサイズ\t状態\n";
        cout << "----------------------------------------\n";
        
        size_t freeSpace = 0;
        size_t freeBlocks = 0;
        size_t largestFreeBlock = 0;
        
        for (const auto& block : blocks)
        {
            cout << block.address << "\t" << block.size << "\t" 
                 << (block.allocated ? "確保済み" : "空き") << "\n";
            
            if (!block.allocated)
            {
                freeSpace += block.size;
                freeBlocks++;
                largestFreeBlock = max(largestFreeBlock, block.size);
            }
        }
        
        cout << "\n=== 統計情報 ===\n";
        cout << "総メモリ: " << totalMemory << " bytes\n";
        cout << "使用中メモリ: " << usedMemory << " bytes\n";
        cout << "空きメモリ: " << freeSpace << " bytes\n";
        cout << "空きブロック数: " << freeBlocks << "\n";
        cout << "最大空きブロック: " << largestFreeBlock << " bytes\n";
        cout << "断片化率: " << fixed << setprecision(2) 
             << calculateFragmentationRatio() << "%\n";
        cout << "確保回数: " << allocationCount << "\n";
        cout << "解放回数: " << deallocationCount << "\n\n";
    }
    
    double calculateFragmentationRatio() const
    {
        if (totalMemory == usedMemory) return 0.0;
        
        size_t freeSpace = totalMemory - usedMemory;
        size_t largestFreeBlock = 0;
        
        for (const auto& block : blocks)
        {
            if (!block.allocated)
            {
                largestFreeBlock = max(largestFreeBlock, block.size);
            }
        }
        
        if (freeSpace == 0) return 0.0;
        
        // 断片化率 = (空きメモリ - 最大空きブロック) / 空きメモリ * 100
        return static_cast<double>(freeSpace - largestFreeBlock) / freeSpace * 100.0;
    }
    
    size_t getLargestFreeBlock() const
    {
        size_t largest = 0;
        for (const auto& block : blocks)
        {
            if (!block.allocated)
            {
                largest = max(largest, block.size);
            }
        }
        return largest;
    }
    
private:
    void coalesceBlocks(vector<MemoryBlock>::iterator target)
    {
        // 前方のブロックと結合
        if (target != blocks.begin())
        {
            auto prev = target - 1;
            if (!prev->allocated && 
                static_cast<char*>(prev->address) + prev->size == target->address)
            {
                prev->size += target->size;
                blocks.erase(target);
                target = prev;
            }
        }
        
        // 後方のブロックと結合
        auto next = target + 1;
        if (next != blocks.end() && !next->allocated &&
            static_cast<char*>(target->address) + target->size == next->address)
        {
            target->size += next->size;
            blocks.erase(next);
        }
    }
};

// メモリプールによる断片化防止
class AntiFragmentationPool
{
private:
    struct PoolChunk
    {
        char* memory;
        size_t size;
        size_t used;
        vector<bool> slots;
        
        PoolChunk(size_t totalSize, size_t slotSize)
            : size(totalSize), used(0)
        {
            memory = static_cast<char*>(aligned_alloc(64, totalSize)); // 64バイトアライメント
            size_t slotCount = totalSize / slotSize;
            slots.resize(slotCount, false);
        }
        
        ~PoolChunk()
        {
            free(memory);
        }
        
        void* allocate(size_t slotSize)
        {
            for (size_t i = 0; i < slots.size(); ++i)
            {
                if (!slots[i])
                {
                    slots[i] = true;
                    used++;
                    return memory + i * slotSize;
                }
            }
            return nullptr;
        }
        
        void deallocate(void* ptr, size_t slotSize)
        {
            if (!ptr) return;
            
            size_t offset = static_cast<char*>(ptr) - memory;
            size_t slotIndex = offset / slotSize;
            
            if (slotIndex < slots.size() && slots[slotIndex])
            {
                slots[slotIndex] = false;
                used--;
            }
        }
        
        bool isFull() const { return used == slots.size(); }
        bool isEmpty() const { return used == 0; }
        double getUsageRatio() const { return static_cast<double>(used) / slots.size(); }
    };
    
    size_t slotSize;
    vector<unique_ptr<PoolChunk>> chunks;
    size_t chunkSize;
    size_t totalAllocated;
    
public:
    AntiFragmentationPool(size_t objectSize, size_t initialChunkSize = 1024 * 1024)
        : slotSize(max(objectSize, sizeof(void*))), chunkSize(initialChunkSize), totalAllocated(0)
    {
        addNewChunk();
        cout << "断片化防止プールを作成 (スロットサイズ: " << slotSize << " bytes)\n";
    }
    
    void* allocate()
    {
        // 空きスロットがあるチャンクを探す
        for (auto& chunk : chunks)
        {
            if (!chunk->isFull())
            {
                void* ptr = chunk->allocate(slotSize);
                if (ptr)
                {
                    totalAllocated++;
                    return ptr;
                }
            }
        }
        
        // 新しいチャンクを追加
        addNewChunk();
        if (!chunks.empty())
        {
            void* ptr = chunks.back()->allocate(slotSize);
            if (ptr)
            {
                totalAllocated++;
                return ptr;
            }
        }
        
        return nullptr;
    }
    
    void deallocate(void* ptr)
    {
        if (!ptr) return;
        
        for (auto& chunk : chunks)
        {
            char* chunkStart = chunk->memory;
            char* chunkEnd = chunkStart + chunk->size;
            
            if (static_cast<char*>(ptr) >= chunkStart && static_cast<char*>(ptr) < chunkEnd)
            {
                chunk->deallocate(ptr, slotSize);
                totalAllocated--;
                return;
            }
        }
    }
    
    void printStatistics() const
    {
        cout << "\n=== プール統計 ===\n";
        cout << "チャンク数: " << chunks.size() << "\n";
        cout << "総確保数: " << totalAllocated << "\n";
        cout << "スロットサイズ: " << slotSize << " bytes\n";
        
        size_t totalSlots = 0;
        size_t usedSlots = 0;
        
        for (size_t i = 0; i < chunks.size(); ++i)
        {
            const auto& chunk = chunks[i];
            totalSlots += chunk->slots.size();
            usedSlots += chunk->used;
            
            cout << "チャンク " << i << ": 使用率 " 
                 << fixed << setprecision(1) << chunk->getUsageRatio() * 100 << "%\n";
        }
        
        cout << "全体使用率: " << fixed << setprecision(1) 
             << static_cast<double>(usedSlots) / totalSlots * 100 << "%\n\n";
    }
    
private:
    void addNewChunk()
    {
        chunks.push_back(make_unique<PoolChunk>(chunkSize, slotSize));
        cout << "新しいチャンクを追加 (チャンク数: " << chunks.size() << ")\n";
    }
};

// 複数サイズ対応のプール管理
class MultiSizePoolManager
{
private:
    map<size_t, unique_ptr<AntiFragmentationPool>> pools;
    vector<size_t> standardSizes;
    
public:
    MultiSizePoolManager()
    {
        // 標準的なサイズを設定（2の累乗 + ゲーム固有サイズ）
        standardSizes = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
        
        cout << "マルチサイズプールマネージャーを初期化\n";
        cout << "対応サイズ: ";
        for (size_t size : standardSizes)
        {
            cout << size << " ";
        }
        cout << "bytes\n\n";
    }
    
    void* allocate(size_t size)
    {
        size_t poolSize = findBestFitSize(size);
        
        auto it = pools.find(poolSize);
        if (it == pools.end())
        {
            pools[poolSize] = make_unique<AntiFragmentationPool>(poolSize);
        }
        
        return pools[poolSize]->allocate();
    }
    
    void deallocate(void* ptr, size_t size)
    {
        size_t poolSize = findBestFitSize(size);
        
        auto it = pools.find(poolSize);
        if (it != pools.end())
        {
            it->second->deallocate(ptr);
        }
    }
    
    void printAllStatistics() const
    {
        cout << "=== マルチサイズプール統計 ===\n";
        cout << "アクティブプール数: " << pools.size() << "\n\n";
        
        for (const auto& pair : pools)
        {
            cout << "サイズ " << pair.first << " bytes プール:\n";
            pair.second->printStatistics();
        }
    }
    
private:
    size_t findBestFitSize(size_t requestedSize) const
    {
        // 最小のサイズ制限
        if (requestedSize <= 8) return 16;
        
        // 標準サイズから最適なものを選択
        for (size_t size : standardSizes)
        {
            if (size >= requestedSize)
            {
                return size;
            }
        }
        
        // 標準サイズを超える場合は、次の2の累乗に切り上げ
        size_t powerOfTwo = 1;
        while (powerOfTwo < requestedSize)
        {
            powerOfTwo *= 2;
        }
        return powerOfTwo;
    }
};

// ゲームオブジェクトの例
struct GameEntity
{
    float x, y, z;
    float vx, vy, vz;
    int health;
    int level;
    bool active;
    
    GameEntity() : x(0), y(0), z(0), vx(0), vy(0), vz(0), health(100), level(1), active(true) {}
};

struct GameParticle
{
    float x, y, z;
    float life;
    uint32_t color;
    
    GameParticle() : x(0), y(0), z(0), life(1.0f), color(0xFFFFFFFF) {}
};

// メモリ断片化のデモンストレーション
void demonstrateFragmentation()
{
    cout << "\n=== メモリ断片化デモンストレーション ===\n";
    
    FragmentationSimulator simulator(10240); // 10KB のメモリ
    vector<void*> allocations;
    
    // Phase 1: 様々なサイズのメモリを確保
    cout << "\nPhase 1: 様々なサイズのメモリを確保\n";
    vector<size_t> sizes = {64, 128, 256, 32, 512, 64, 128, 256, 32, 1024};
    
    for (size_t size : sizes)
    {
        void* ptr = simulator.allocate(size);
        if (ptr)
        {
            allocations.push_back(ptr);
        }
    }
    
    simulator.printMemoryMap();
    
    // Phase 2: 一部のメモリを解放（断片化を作成）
    cout << "Phase 2: 一部のメモリを解放（断片化を作成）\n";
    // 奇数番目のブロックを解放
    for (size_t i = 1; i < allocations.size(); i += 2)
    {
        simulator.deallocate(allocations[i]);
        allocations[i] = nullptr;
    }
    
    simulator.printMemoryMap();
    
    // Phase 3: 大きなブロックの確保を試行
    cout << "Phase 3: 大きなブロックの確保を試行\n";
    void* largeBlock = simulator.allocate(512);
    if (largeBlock)
    {
        cout << "大きなブロックの確保に成功\n";
    }
    else
    {
        cout << "大きなブロックの確保に失敗（断片化が原因）\n";
        cout << "最大空きブロック: " << simulator.getLargestFreeBlock() << " bytes\n";
    }
    
    simulator.printMemoryMap();
    
    // 残りのメモリを解放
    for (void* ptr : allocations)
    {
        if (ptr)
        {
            simulator.deallocate(ptr);
        }
    }
    
    if (largeBlock)
    {
        simulator.deallocate(largeBlock);
    }
}

// プール vs 標準アロケータの比較
void comparePoolVsStandard()
{
    cout << "\n=== プール vs 標準アロケータ比較 ===\n";
    
    const int entityCount = 10000;
    const int iterations = 100;
    
    // 標準アロケータでのテスト
    {
        cout << "標準アロケータでのテスト:\n";
        
        auto start = high_resolution_clock::now();
        
        for (int iter = 0; iter < iterations; ++iter)
        {
            vector<GameEntity*> entities;
            
            // 確保
            for (int i = 0; i < entityCount; ++i)
            {
                entities.push_back(new GameEntity());
            }
            
            // ランダムに一部を削除（断片化を作成）
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<> dis(0, entityCount - 1);
            
            for (int i = 0; i < entityCount / 4; ++i)
            {
                int index = dis(gen);
                if (entities[index])
                {
                    delete entities[index];
                    entities[index] = nullptr;
                }
            }
            
            // 残りを削除
            for (auto* entity : entities)
            {
                delete entity;
            }
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start).count();
        cout << "実行時間: " << duration << " ms\n\n";
    }
    
    // プールアロケータでのテスト
    {
        cout << "プールアロケータでのテスト:\n";
        
        AntiFragmentationPool pool(sizeof(GameEntity));
        
        auto start = high_resolution_clock::now();
        
        for (int iter = 0; iter < iterations; ++iter)
        {
            vector<GameEntity*> entities;
            
            // 確保
            for (int i = 0; i < entityCount; ++i)
            {
                void* ptr = pool.allocate();
                if (ptr)
                {
                    entities.push_back(new(ptr) GameEntity());
                }
            }
            
            // ランダムに一部を削除
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<> dis(0, entityCount - 1);
            
            for (int i = 0; i < entityCount / 4; ++i)
            {
                int index = dis(gen);
                if (entities[index])
                {
                    entities[index]->~GameEntity();
                    pool.deallocate(entities[index]);
                    entities[index] = nullptr;
                }
            }
            
            // 残りを削除
            for (auto* entity : entities)
            {
                if (entity)
                {
                    entity->~GameEntity();
                    pool.deallocate(entity);
                }
            }
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start).count();
        cout << "実行時間: " << duration << " ms\n\n";
        
        pool.printStatistics();
    }
}

// マルチサイズプールのデモ
void demonstrateMultiSizePool()
{
    cout << "\n=== マルチサイズプールデモ ===\n";
    
    MultiSizePoolManager manager;
    vector<pair<void*, size_t>> allocations;
    
    // 様々なサイズのオブジェクトを確保
    vector<size_t> requestSizes = {
        sizeof(GameEntity),    // 36 bytes
        sizeof(GameParticle),  // 20 bytes
        100,                   // カスタムサイズ
        500,                   // カスタムサイズ
        1500,                  // 大きなオブジェクト
        10,                    // 小さなオブジェクト
        2000                   // 非常に大きなオブジェクト
    };
    
    cout << "様々なサイズのオブジェクトを確保:\n";
    for (size_t size : requestSizes)
    {
        for (int i = 0; i < 100; ++i)
        {
            void* ptr = manager.allocate(size);
            if (ptr)
            {
                allocations.emplace_back(ptr, size);
            }
        }
        cout << "サイズ " << size << " bytes: 100個確保\n";
    }
    
    manager.printAllStatistics();
    
    // 一部を解放
    cout << "一部のオブジェクトを解放中...\n";
    for (size_t i = 0; i < allocations.size(); i += 3)
    {
        manager.deallocate(allocations[i].first, allocations[i].second);
        allocations[i].first = nullptr;
    }
    
    manager.printAllStatistics();
    
    // 残りを解放
    for (const auto& alloc : allocations)
    {
        if (alloc.first)
        {
            manager.deallocate(alloc.first, alloc.second);
        }
    }
    
    cout << "全オブジェクト解放後:\n";
    manager.printAllStatistics();
}

int main()
{
    cout << "=== メモリ断片化の問題と対策 ===\n";
    
    // メモリ断片化のデモンストレーション
    demonstrateFragmentation();
    
    // プール vs 標準アロケータの比較
    comparePoolVsStandard();
    
    // マルチサイズプールのデモ
    demonstrateMultiSizePool();
    
    cout << "=== メモリ断片化の問題 ===\n";
    cout << "1. 外部断片化: 空きメモリが分散し、大きなブロックが確保できない\n";
    cout << "2. 内部断片化: 確保されたメモリの一部が未使用のまま残る\n";
    cout << "3. パフォーマンス低下: メモリ検索時間の増加\n";
    cout << "4. メモリ効率の悪化: 利用可能メモリの減少\n";
    
    cout << "\n=== 断片化の対策 ===\n";
    cout << "1. メモリプール: 同サイズオブジェクト用の専用アロケータ\n";
    cout << "2. スラブアロケータ: 複数サイズ対応のプール管理\n";
    cout << "3. バディシステム: 2の累乗サイズでの効率的な管理\n";
    cout << "4. コンパクション: 使用中メモリの再配置による断片化解消\n";
    cout << "5. ガベージコレクション: 自動的なメモリ回収と整理\n";
    
    cout << "\n=== ゲーム開発での具体例 ===\n";
    cout << "1. エンティティシステム: エンティティ用のプール\n";
    cout << "2. パーティクルシステム: パーティクル用の高速プール\n";
    cout << "3. オーディオシステム: サウンドバッファ用のプール\n";
    cout << "4. レンダリング: 頂点バッファやテクスチャのプール\n";
    cout << "5. スクリプトシステム: スクリプトオブジェクト用のGC\n";
    
    cout << "\n=== 設計指針 ===\n";
    cout << "1. オブジェクトのライフサイクルを分析\n";
    cout << "2. サイズ毎にプールを分離\n";
    cout << "3. 長期間使用するオブジェクトと短期間のオブジェクトを区別\n";
    cout << "4. プールサイズを適切に設定（メモリ使用量とのバランス）\n";
    cout << "5. 定期的なメモリ使用状況の監視\n";
    
    cout << "\n=== パフォーマンス上の利点 ===\n";
    cout << "1. 高速な確保・解放（O(1)）\n";
    cout << "2. キャッシュ効率の向上（局所性）\n";
    cout << "3. 予測可能なメモリ使用量\n";
    cout << "4. ガベージコレクションの負荷軽減\n";
    cout << "5. リアルタイム性の向上（一定時間での確保・解放）\n";
    
    return 0;
}

// メモリ断片化対策の重要なポイント：
// 1. ゲーム開発では特にリアルタイム性が重要で、断片化は致命的な問題となる
// 2. 事前にオブジェクトのサイズとライフサイクルを分析し、適切なプールを設計
// 3. プールアロケータは断片化を防ぐが、メモリ使用量が増加する可能性
// 4. 複数のアロケータを組み合わせて、用途に応じた最適化を実施
// 5. 開発中は定期的にメモリ使用状況を監視し、プール設定を調整する