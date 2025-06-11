// メモリデバッグツールの実装
// ゲーム開発でのメモリ問題検出システム

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <thread>
#include <mutex>

using namespace std;
using namespace chrono;

// メモリ確保情報を記録する構造体
struct AllocationInfo
{
    void* address;          // メモリアドレス
    size_t size;           // 確保サイズ
    string filename;       // ファイル名
    int line;              // 行番号
    string function;       // 関数名
    steady_clock::time_point timestamp; // 確保時刻
    bool isActive;         // アクティブフラグ
    size_t id;             // 確保ID
    
    AllocationInfo(void* addr, size_t sz, const string& file, int ln, const string& func)
        : address(addr), size(sz), filename(file), line(ln), function(func)
        , timestamp(steady_clock::now()), isActive(true)
    {
        static size_t nextId = 1;
        id = nextId++;
    }
};

// カスタムメモリトラッカー
class MemoryTracker
{
private:
    unordered_map<void*, unique_ptr<AllocationInfo>> allocations;
    size_t totalAllocated;
    size_t peakUsage;
    size_t allocationCount;
    size_t deallocationCount;
    size_t leakCount;
    bool trackingEnabled;
    mutable mutex trackerMutex;
    
    // デバッグ用ログファイル
    ofstream logFile;
    
public:
    MemoryTracker() 
        : totalAllocated(0), peakUsage(0), allocationCount(0), deallocationCount(0)
        , leakCount(0), trackingEnabled(true)
    {
        logFile.open("memory_debug.log");
        logFile << "=== メモリデバッグログ開始 ===\n";
        logFile << "時刻\t操作\tアドレス\tサイズ\tファイル:行\t関数\n";
        cout << "メモリトラッカーを初期化\n";
    }
    
    ~MemoryTracker()
    {
        generateLeakReport();
        logFile << "=== メモリデバッグログ終了 ===\n";
        logFile.close();
    }
    
    void* allocate(size_t size, const string& filename, int line, const string& function)
    {
        if (!trackingEnabled) return malloc(size);
        
        void* ptr = malloc(size);
        if (!ptr) return nullptr;
        
        lock_guard<mutex> lock(trackerMutex);
        
        auto info = make_unique<AllocationInfo>(ptr, size, filename, line, function);
        
        // ログに記録
        auto now = steady_clock::now();
        auto timeStr = to_string(duration_cast<milliseconds>(now.time_since_epoch()).count());
        
        logFile << timeStr << "\tALLOC\t" << ptr << "\t" << size << "\t"
                << filename << ":" << line << "\t" << function << "\n";
        logFile.flush();
        
        allocations[ptr] = move(info);
        totalAllocated += size;
        allocationCount++;
        
        if (totalAllocated > peakUsage)
        {
            peakUsage = totalAllocated;
        }
        
        cout << "メモリ確保 [ID:" << allocations[ptr]->id << "] " << ptr 
             << " (" << size << " bytes) - " << filename << ":" << line << "\n";
        
        return ptr;
    }
    
    void deallocate(void* ptr, const string& filename, int line, const string& function)
    {
        if (!trackingEnabled)
        {
            free(ptr);
            return;
        }
        
        if (!ptr) return;
        
        lock_guard<mutex> lock(trackerMutex);
        
        auto it = allocations.find(ptr);
        if (it != allocations.end())
        {
            size_t size = it->second->size;
            size_t id = it->second->id;
            
            // ログに記録
            auto now = steady_clock::now();
            auto timeStr = to_string(duration_cast<milliseconds>(now.time_since_epoch()).count());
            
            logFile << timeStr << "\tFREE\t" << ptr << "\t" << size << "\t"
                    << filename << ":" << line << "\t" << function << "\n";
            logFile.flush();
            
            totalAllocated -= size;
            deallocationCount++;
            
            cout << "メモリ解放 [ID:" << id << "] " << ptr 
                 << " (" << size << " bytes) - " << filename << ":" << line << "\n";
            
            allocations.erase(it);
            free(ptr);
        }
        else
        {
            cout << "警告: 未追跡のポインタを解放しようとしました: " << ptr 
                 << " - " << filename << ":" << line << "\n";
            
            logFile << "WARNING\tUNTRACKED_FREE\t" << ptr << "\t0\t"
                    << filename << ":" << line << "\t" << function << "\n";
            
            free(ptr); // 一応解放は行う
        }
    }
    
    void generateLeakReport() const
    {
        lock_guard<mutex> lock(trackerMutex);
        
        cout << "\n=== メモリリークレポート ===\n";
        
        if (allocations.empty())
        {
            cout << "メモリリークは検出されませんでした！\n";
        }
        else
        {
            cout << "検出されたメモリリーク: " << allocations.size() << "個\n\n";
            
            size_t totalLeakedBytes = 0;
            for (const auto& pair : allocations)
            {
                const auto& info = pair.second;
                totalLeakedBytes += info->size;
                
                auto elapsed = duration_cast<seconds>(steady_clock::now() - info->timestamp);
                
                cout << "リーク [ID:" << info->id << "] " << info->address
                     << " (" << info->size << " bytes)\n";
                cout << "  場所: " << info->filename << ":" << info->line << "\n";
                cout << "  関数: " << info->function << "\n";
                cout << "  確保からの経過時間: " << elapsed.count() << " 秒\n\n";
            }
            
            cout << "総リークメモリ: " << totalLeakedBytes << " bytes\n";
        }
        
        printStatistics();
    }
    
    void printStatistics() const
    {
        lock_guard<mutex> lock(trackerMutex);
        
        cout << "\n=== メモリ統計 ===\n";
        cout << "総確保回数: " << allocationCount << "\n";
        cout << "総解放回数: " << deallocationCount << "\n";
        cout << "現在の使用量: " << totalAllocated << " bytes\n";
        cout << "ピーク使用量: " << peakUsage << " bytes\n";
        cout << "アクティブな確保: " << allocations.size() << " 個\n";
        
        if (allocationCount > 0)
        {
            double leakRate = static_cast<double>(allocations.size()) / allocationCount * 100.0;
            cout << "リーク率: " << fixed << setprecision(2) << leakRate << "%\n";
        }
    }
    
    void printTopLeaks(size_t count = 5) const
    {
        lock_guard<mutex> lock(trackerMutex);
        
        cout << "\n=== 上位メモリリーク (トップ " << count << ") ===\n";
        
        vector<pair<void*, AllocationInfo*>> sortedLeaks;
        for (const auto& pair : allocations)
        {
            sortedLeaks.emplace_back(pair.first, pair.second.get());
        }
        
        // サイズでソート
        sort(sortedLeaks.begin(), sortedLeaks.end(),
            [](const auto& a, const auto& b) {
                return a.second->size > b.second->size;
            });
        
        for (size_t i = 0; i < min(count, sortedLeaks.size()); ++i)
        {
            const auto& info = sortedLeaks[i].second;
            cout << (i + 1) << ". [ID:" << info->id << "] " << info->address
                 << " (" << info->size << " bytes)\n";
            cout << "   " << info->filename << ":" << info->line 
                 << " in " << info->function << "\n";
        }
    }
    
    void enableTracking() { trackingEnabled = true; }
    void disableTracking() { trackingEnabled = false; }
    
    bool isValidPointer(void* ptr) const
    {
        lock_guard<mutex> lock(trackerMutex);
        return allocations.find(ptr) != allocations.end();
    }
    
    size_t getCurrentUsage() const
    {
        lock_guard<mutex> lock(trackerMutex);
        return totalAllocated;
    }
    
    size_t getPeakUsage() const
    {
        lock_guard<mutex> lock(trackerMutex);
        return peakUsage;
    }
};

// グローバルなメモリトラッカーインスタンス
static MemoryTracker g_memoryTracker;

// デバッグ用のnewとdeleteのオーバーロード
#define DEBUG_NEW(size) g_memoryTracker.allocate(size, __FILE__, __LINE__, __FUNCTION__)
#define DEBUG_DELETE(ptr) g_memoryTracker.deallocate(ptr, __FILE__, __LINE__, __FUNCTION__)

// メモリ破損検出用のガードパターン
class MemoryGuard
{
private:
    static const uint32_t GUARD_PATTERN = 0xDEADBEEF;
    static const size_t GUARD_SIZE = sizeof(uint32_t);
    
public:
    static void* allocateWithGuard(size_t size)
    {
        size_t totalSize = size + 2 * GUARD_SIZE;
        uint8_t* ptr = static_cast<uint8_t*>(DEBUG_NEW(totalSize));
        
        if (!ptr) return nullptr;
        
        // 前ガードを設定
        *reinterpret_cast<uint32_t*>(ptr) = GUARD_PATTERN;
        
        // 後ガードを設定
        *reinterpret_cast<uint32_t*>(ptr + GUARD_SIZE + size) = GUARD_PATTERN;
        
        return ptr + GUARD_SIZE; // ユーザー領域のポインタを返す
    }
    
    static void deallocateWithGuard(void* userPtr)
    {
        if (!userPtr) return;
        
        uint8_t* ptr = static_cast<uint8_t*>(userPtr) - GUARD_SIZE;
        
        // 前ガードをチェック
        uint32_t frontGuard = *reinterpret_cast<uint32_t*>(ptr);
        if (frontGuard != GUARD_PATTERN)
        {
            cout << "メモリ破損検出: 前ガードが壊れています at " << userPtr << "\n";
            cout << "期待値: 0x" << hex << GUARD_PATTERN << ", 実際値: 0x" << frontGuard << dec << "\n";
        }
        
        // 後ガードをチェック（サイズは分からないので推定）
        // 実際の実装では確保時にサイズを記録する必要がある
        
        DEBUG_DELETE(ptr);
    }
    
    static bool checkGuards(void* userPtr, size_t size)
    {
        if (!userPtr) return false;
        
        uint8_t* ptr = static_cast<uint8_t*>(userPtr) - GUARD_SIZE;
        
        uint32_t frontGuard = *reinterpret_cast<uint32_t*>(ptr);
        uint32_t backGuard = *reinterpret_cast<uint32_t*>(ptr + GUARD_SIZE + size);
        
        bool frontOk = (frontGuard == GUARD_PATTERN);
        bool backOk = (backGuard == GUARD_PATTERN);
        
        if (!frontOk)
        {
            cout << "前ガード破損検出 at " << userPtr << "\n";
        }
        
        if (!backOk)
        {
            cout << "後ガード破損検出 at " << userPtr << "\n";
        }
        
        return frontOk && backOk;
    }
};

// メモリパターン検査
class MemoryPatternChecker
{
public:
    static void fillPattern(void* ptr, size_t size, uint8_t pattern = 0xCC)
    {
        memset(ptr, pattern, size);
    }
    
    static bool checkPattern(void* ptr, size_t size, uint8_t pattern = 0xCC)
    {
        uint8_t* bytes = static_cast<uint8_t*>(ptr);
        for (size_t i = 0; i < size; ++i)
        {
            if (bytes[i] != pattern)
            {
                cout << "パターン不一致検出 at offset " << i << ": 期待値=0x" 
                     << hex << static_cast<int>(pattern) << ", 実際値=0x" 
                     << static_cast<int>(bytes[i]) << dec << "\n";
                return false;
            }
        }
        return true;
    }
    
    static void poisonMemory(void* ptr, size_t size)
    {
        fillPattern(ptr, size, 0xDD); // "Dead" pattern
    }
};

// ゲームオブジェクトの例（デバッグ機能付き）
class DebugGameObject
{
public:
    string name;
    int* healthData;
    char* textureData;
    size_t textureSize;
    
    DebugGameObject(const string& objName, size_t texSize = 1024)
        : name(objName), textureSize(texSize)
    {
        cout << "DebugGameObject '" << name << "' を作成中...\n";
        
        // デバッグ用メモリ確保
        healthData = static_cast<int*>(DEBUG_NEW(sizeof(int)));
        *healthData = 100;
        
        textureData = static_cast<char*>(MemoryGuard::allocateWithGuard(textureSize));
        if (textureData)
        {
            MemoryPatternChecker::fillPattern(textureData, textureSize, 0xAA);
        }
        
        cout << "DebugGameObject '" << name << "' 作成完了\n";
    }
    
    ~DebugGameObject()
    {
        cout << "DebugGameObject '" << name << "' を削除中...\n";
        
        if (textureData)
        {
            // メモリパターンをチェック
            MemoryPatternChecker::checkPattern(textureData, textureSize, 0xAA);
            
            // ガードをチェック
            MemoryGuard::checkGuards(textureData, textureSize);
            
            MemoryGuard::deallocateWithGuard(textureData);
        }
        
        if (healthData)
        {
            DEBUG_DELETE(healthData);
        }
        
        cout << "DebugGameObject '" << name << "' 削除完了\n";
    }
    
    void corruptMemory()
    {
        cout << "意図的にメモリを破損させます（テスト用）\n";
        if (textureData && textureSize > 0)
        {
            textureData[0] = 0x99; // パターンを破壊
        }
    }
    
    void printDebugInfo() const
    {
        cout << "=== " << name << " デバッグ情報 ===\n";
        cout << "Health: " << (healthData ? *healthData : -1) << "\n";
        cout << "Texture size: " << textureSize << " bytes\n";
        cout << "Texture address: " << static_cast<void*>(textureData) << "\n";
        cout << "Health address: " << static_cast<void*>(healthData) << "\n";
    }
};

// メモリリークを意図的に作成する関数
void createMemoryLeaks()
{
    cout << "\n=== 意図的なメモリリーク作成（テスト用） ===\n";
    
    // リーク1: 単純な確保忘れ
    int* leakedInt = static_cast<int*>(DEBUG_NEW(sizeof(int) * 10));
    *leakedInt = 42;
    cout << "配列を確保したが解放しない: " << static_cast<void*>(leakedInt) << "\n";
    
    // リーク2: オブジェクトの削除忘れ
    DebugGameObject* leakedObject = new DebugGameObject("リークしたオブジェクト");
    cout << "オブジェクトを作成したが削除しない: " << static_cast<void*>(leakedObject) << "\n";
    
    // リーク3: 条件分岐での解放忘れ
    char* conditionalLeak = static_cast<char*>(DEBUG_NEW(512));
    if (conditionalLeak)
    {
        strcpy(conditionalLeak, "これはリークします");
        cout << "条件分岐で確保したが解放しない: " << static_cast<void*>(conditionalLeak) << "\n";
        // delete[] conditionalLeak; // この行をコメントアウトしてリークを作成
    }
}

// メモリ破損を意図的に作成する関数
void createMemoryCorruption()
{
    cout << "\n=== 意図的なメモリ破損作成（テスト用） ===\n";
    
    auto obj = make_unique<DebugGameObject>("破損テストオブジェクト", 256);
    obj->printDebugInfo();
    
    cout << "メモリ破損前のパターンチェック:\n";
    MemoryPatternChecker::checkPattern(obj->textureData, obj->textureSize, 0xAA);
    
    // 意図的に破損
    obj->corruptMemory();
    
    cout << "メモリ破損後のパターンチェック:\n";
    MemoryPatternChecker::checkPattern(obj->textureData, obj->textureSize, 0xAA);
}

// 正常なメモリ使用例
void normalMemoryUsage()
{
    cout << "\n=== 正常なメモリ使用例 ===\n";
    
    {
        auto obj1 = make_unique<DebugGameObject>("正常オブジェクト1", 512);
        auto obj2 = make_unique<DebugGameObject>("正常オブジェクト2", 1024);
        
        obj1->printDebugInfo();
        obj2->printDebugInfo();
        
        // 適切なメモリ管理の例
        vector<int*> tempAllocations;
        for (int i = 0; i < 5; ++i)
        {
            int* temp = static_cast<int*>(DEBUG_NEW(sizeof(int) * 100));
            tempAllocations.push_back(temp);
        }
        
        cout << "一時的な確保を " << tempAllocations.size() << " 個実行\n";
        
        // 適切に解放
        for (int* ptr : tempAllocations)
        {
            DEBUG_DELETE(ptr);
        }
        
        cout << "一時的な確保を全て解放\n";
    }
    
    cout << "正常なメモリ使用例完了\n";
}

// マルチスレッドでのメモリ使用テスト
void multithreadedMemoryTest()
{
    cout << "\n=== マルチスレッドメモリテスト ===\n";
    
    auto worker = [](int threadId) {
        for (int i = 0; i < 10; ++i)
        {
            DebugGameObject obj("スレッド" + to_string(threadId) + "_オブジェクト" + to_string(i), 256);
            this_thread::sleep_for(milliseconds(10));
        }
    };
    
    vector<thread> threads;
    for (int i = 0; i < 3; ++i)
    {
        threads.emplace_back(worker, i);
    }
    
    for (auto& t : threads)
    {
        t.join();
    }
    
    cout << "マルチスレッドテスト完了\n";
}

int main()
{
    cout << "=== メモリデバッグツールの実装 ===\n";
    
    // 正常なメモリ使用例
    normalMemoryUsage();
    
    // 現在の統計を表示
    g_memoryTracker.printStatistics();
    
    // 意図的なメモリリーク作成
    createMemoryLeaks();
    
    // 意図的なメモリ破損作成
    createMemoryCorruption();
    
    // マルチスレッドテスト
    multithreadedMemoryTest();
    
    // 最終統計とリークレポート
    cout << "\n=== 最終メモリ状態 ===\n";
    g_memoryTracker.printTopLeaks();
    g_memoryTracker.printStatistics();
    
    cout << "\n=== メモリデバッグツールの機能 ===\n";
    cout << "1. メモリ確保/解放の追跡と記録\n";
    cout << "2. メモリリークの検出と報告\n";
    cout << "3. ダブルフリーや無効ポインタの検出\n";
    cout << "4. メモリ破損の検出（ガードパターン）\n";
    cout << "5. メモリ使用量の統計情報\n";
    cout << "6. 詳細なログファイル出力\n";
    cout << "7. マルチスレッド対応\n";
    
    cout << "\n=== ゲーム開発での活用例 ===\n";
    cout << "1. デバッグビルドでのメモリリーク検出\n";
    cout << "2. アセットローダーのメモリ管理検証\n";
    cout << "3. ゲームオブジェクトのライフサイクル監視\n";
    cout << "4. パフォーマンステスト時のメモリ使用量測定\n";
    cout << "5. メモリプールの効率性検証\n";
    cout << "6. 長時間実行でのメモリリーク検出\n";
    
    cout << "\n=== 外部ツールとの連携 ===\n";
    cout << "1. Valgrind (Linux): メモリエラー検出\n";
    cout << "2. Dr. Memory (Windows): メモリ問題の詳細解析\n";
    cout << "3. AddressSanitizer: コンパイラ内蔵のメモリ検査\n";
    cout << "4. Visual Studio Diagnostic Tools: リアルタイム監視\n";
    cout << "5. Intel Inspector: メモリとスレッドの問題検出\n";
    
    cout << "\n=== ベストプラクティス ===\n";
    cout << "1. デバッグビルドでは常にメモリ追跡を有効化\n";
    cout << "2. 定期的なメモリリークテストの実行\n";
    cout << "3. CI/CDパイプラインにメモリテストを組み込み\n";
    cout << "4. メモリ使用量の異常検知アラートを設定\n";
    cout << "5. 本番環境では軽量なメモリ監視のみ実行\n";
    
    // プログラム終了時にメモリトラッカーのデストラクタが呼ばれ、
    // 最終的なリークレポートが生成される
    
    return 0;
}

// メモリデバッグの重要なポイント：
// 1. 開発段階での早期発見が重要（本番で発見は手遅れ）
// 2. 自動化されたテストでメモリ問題を継続的に検出
// 3. パフォーマンスとのバランスを考慮したツール選択
// 4. 複数のツールを組み合わせて包括的な検証を実施
// 5. チーム全体でメモリ管理のベストプラクティスを共有