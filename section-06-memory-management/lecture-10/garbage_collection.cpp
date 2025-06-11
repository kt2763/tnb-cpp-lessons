// ガベージコレクションの概念と実装
// C++でのシンプルなGCシステムの実装例

#include <iostream>
#include <vector>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <algorithm>

using namespace std;
using namespace chrono;

// シンプルなガベージコレクタの実装
class SimpleGC
{
private:
    struct GCObject
    {
        void* ptr;
        size_t size;
        bool marked;
        string typeName;
        vector<GCObject*> references;
        
        GCObject(void* p, size_t s, const string& type) 
            : ptr(p), size(s), marked(false), typeName(type) {}
    };
    
    vector<unique_ptr<GCObject>> objects;
    unordered_map<void*, GCObject*> ptrToObject;
    vector<GCObject*> roots;
    size_t totalAllocated;
    size_t gcThreshold;
    int gcCycles;
    
public:
    SimpleGC(size_t threshold = 1024 * 1024) // 1MBでGC実行
        : totalAllocated(0), gcThreshold(threshold), gcCycles(0)
    {
        cout << "シンプルGCを初期化 (闾値: " << gcThreshold << " bytes)\n";
    }
    
    ~SimpleGC()
    {
        cout << "\nGC終了時の統計:\n";
        cout << "GCサイクル数: " << gcCycles << "\n";
        cout << "残留オブジェクト数: " << objects.size() << "\n";
        cout << "残留メモリ: " << totalAllocated << " bytes\n";
        
        // 残留オブジェクトを全て解放
        cleanup();
    }
    
    template<typename T>
    T* allocate()
    {
        T* ptr = new T();
        
        auto gcObj = make_unique<GCObject>(ptr, sizeof(T), typeid(T).name());
        GCObject* objPtr = gcObj.get();
        
        objects.push_back(move(gcObj));
        ptrToObject[ptr] = objPtr;
        totalAllocated += sizeof(T);
        
        cout << "オブジェクトを確保: " << ptr << " (タイプ: " << typeid(T).name() << ", サイズ: " << sizeof(T) << " bytes)\n";
        
        // 闾値を超えたらGCを実行
        if (totalAllocated >= gcThreshold)
        {
            collect();
        }
        
        return ptr;
    }
    
    void addRoot(void* ptr)
    {
        auto it = ptrToObject.find(ptr);
        if (it != ptrToObject.end())
        {
            roots.push_back(it->second);
            cout << "ルートオブジェクトとして登録: " << ptr << "\n";
        }
    }
    
    void removeRoot(void* ptr)
    {
        auto it = ptrToObject.find(ptr);
        if (it != ptrToObject.end())
        {
            roots.erase(remove(roots.begin(), roots.end(), it->second), roots.end());
            cout << "ルートオブジェクトから削除: " << ptr << "\n";
        }
    }
    
    void addReference(void* from, void* to)
    {
        auto fromIt = ptrToObject.find(from);
        auto toIt = ptrToObject.find(to);
        
        if (fromIt != ptrToObject.end() && toIt != ptrToObject.end())
        {
            fromIt->second->references.push_back(toIt->second);
            cout << "参照を追加: " << from << " -> " << to << "\n";
        }
    }
    
    void collect()
    {
        cout << "\n=== ガベージコレクション開始 ===\n";
        auto start = high_resolution_clock::now();
        
        gcCycles++;
        
        // マークフェーズ: 全てのマークをクリア
        for (auto& obj : objects)
        {
            obj->marked = false;
        }
        
        // ルートからマークを開始
        cout << "マークフェーズ開始...\n";
        for (GCObject* root : roots)
        {
            markObject(root);
        }
        
        // スイープフェーズ: マークされていないオブジェクトを削除
        cout << "スイープフェーズ開始...\n";
        size_t freedMemory = 0;
        int freedObjects = 0;
        
        auto it = objects.begin();
        while (it != objects.end())
        {
            if (!(*it)->marked)
            {
                cout << "ガベージオブジェクトを削除: " << (*it)->ptr 
                     << " (タイプ: " << (*it)->typeName << ")\n";
                
                freedMemory += (*it)->size;
                freedObjects++;
                
                // ポインタマップから削除
                ptrToObject.erase((*it)->ptr);
                
                // 実際のメモリを解放
                delete static_cast<char*>((*it)->ptr);
                
                it = objects.erase(it);
            }
            else
            {
                ++it;
            }
        }
        
        totalAllocated -= freedMemory;
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        
        cout << "GC完了: " << freedObjects << "個のオブジェクト、" << freedMemory << " bytesを解放\n";
        cout << "GC時間: " << duration << " マイクロ秒\n";
        cout << "残りメモリ: " << totalAllocated << " bytes\n";
        cout << "=== ガベージコレクション終了 ===\n\n";
    }
    
    void printStatistics() const
    {
        cout << "\n=== GC統計 ===\n";
        cout << "総オブジェクト数: " << objects.size() << "\n";
        cout << "総メモリ使用量: " << totalAllocated << " bytes\n";
        cout << "ルートオブジェクト数: " << roots.size() << "\n";
        cout << "GCサイクル数: " << gcCycles << "\n";
        
        cout << "\nオブジェクト一覧:\n";
        for (const auto& obj : objects)
        {
            cout << "  " << obj->ptr << " - " << obj->typeName 
                 << " (サイズ: " << obj->size << ", マーク: " << obj->marked 
                 << ", 参照: " << obj->references.size() << ")\n";
        }
    }
    
private:
    void markObject(GCObject* obj)
    {
        if (!obj || obj->marked) return;
        
        obj->marked = true;
        cout << "オブジェクトをマーク: " << obj->ptr << " (" << obj->typeName << ")\n";
        
        // 参照先も再帰的にマーク
        for (GCObject* ref : obj->references)
        {
            markObject(ref);
        }
    }
    
    void cleanup()
    {
        for (auto& obj : objects)
        {
            delete static_cast<char*>(obj->ptr);
        }
        objects.clear();
        ptrToObject.clear();
        roots.clear();
        totalAllocated = 0;
    }
};

// ゲームオブジェクトの例
struct GameCharacter
{
    string name;
    int health;
    int level;
    GameCharacter* target;
    vector<GameCharacter*> allies;
    
    GameCharacter(const string& n = "無名", int h = 100, int l = 1) 
        : name(n), health(h), level(l), target(nullptr)
    {
        cout << "キャラクター '" << name << "' を作成\n";
    }
    
    ~GameCharacter()
    {
        cout << "キャラクター '" << name << "' を削除\n";
    }
    
    void setTarget(GameCharacter* t) { target = t; }
    void addAlly(GameCharacter* ally) { allies.push_back(ally); }
};

struct GameItem
{
    string name;
    int value;
    GameCharacter* owner;
    
    GameItem(const string& n = "アイテム", int v = 10) 
        : name(n), value(v), owner(nullptr)
    {
        cout << "アイテム '" << name << "' を作成\n";
    }
    
    ~GameItem()
    {
        cout << "アイテム '" << name << "' を削除\n";
    }
    
    void setOwner(GameCharacter* o) { owner = o; }
};

// シンプルGCのデモ
void simpleGCDemo()
{
    cout << "\n=== シンプルGCのデモ ===\n";
    
    SimpleGC gc(500); // 500バイトでGC実行
    
    // キャラクターを作成
    auto* hero = gc.allocate<GameCharacter>();
    hero->name = "勇者";
    hero->health = 100;
    hero->level = 10;
    
    auto* mage = gc.allocate<GameCharacter>();
    mage->name = "魔法使い";
    mage->health = 80;
    mage->level = 8;
    
    auto* enemy = gc.allocate<GameCharacter>();
    enemy->name = "オーク";
    enemy->health = 60;
    enemy->level = 5;
    
    // アイテムを作成
    auto* sword = gc.allocate<GameItem>();
    sword->name = "魔法の剣";
    sword->value = 100;
    
    auto* potion = gc.allocate<GameItem>();
    potion->name = "回復ポーション";
    potion->value = 50;
    
    // ルートオブジェクトとして登録
    gc.addRoot(hero);
    gc.addRoot(mage);
    
    // 参照関係を設定
    hero->setTarget(enemy);
    mage->addAlly(hero);
    hero->addAlly(mage);
    sword->setOwner(hero);
    potion->setOwner(mage);
    
    // GCシステムに参照関係を通知
    gc.addReference(hero, enemy);
    gc.addReference(mage, hero);
    gc.addReference(hero, mage);
    gc.addReference(sword, hero);
    gc.addReference(potion, mage);
    
    gc.printStatistics();
    
    // いくつかのオブジェクトを作成してGCを発動させる
    cout << "\n追加オブジェクトを作成してGCを発動...\n";
    
    for (int i = 0; i < 10; ++i)
    {
        auto* temp = gc.allocate<GameItem>();
        temp->name = "一時アイテム" + to_string(i);
        // これらはルートから達可能ではないのでGCされる
    }
    
    gc.printStatistics();
    
    // ルートオブジェクトを削除して再度GC
    cout << "\nルートオブジェクトを削除して手動GC...\n";
    gc.removeRoot(mage);
    gc.collect();
    
    gc.printStatistics();
}

// 参照カウントでのガベージコレクションシミュレーション
class ReferenceCountedGC
{
private:
    struct RCObject
    {
        void* ptr;
        size_t size;
        int refCount;
        string typeName;
        vector<RCObject*> references;
        
        RCObject(void* p, size_t s, const string& type) 
            : ptr(p), size(s), refCount(0), typeName(type) {}
    };
    
    vector<unique_ptr<RCObject>> objects;
    unordered_map<void*, RCObject*> ptrToObject;
    size_t totalAllocated;
    int collectionCycles;
    
public:
    ReferenceCountedGC() : totalAllocated(0), collectionCycles(0)
    {
        cout << "参照カウントGCを初期化\n";
    }
    
    ~ReferenceCountedGC()
    {
        cleanup();
    }
    
    template<typename T>
    T* allocate()
    {
        T* ptr = new T();
        
        auto rcObj = make_unique<RCObject>(ptr, sizeof(T), typeid(T).name());
        RCObject* objPtr = rcObj.get();
        
        objects.push_back(move(rcObj));
        ptrToObject[ptr] = objPtr;
        totalAllocated += sizeof(T);
        
        cout << "RCオブジェクトを確保: " << ptr << " (タイプ: " << typeid(T).name() << ")\n";
        
        return ptr;
    }
    
    void addReference(void* from, void* to)
    {
        auto fromIt = ptrToObject.find(from);
        auto toIt = ptrToObject.find(to);
        
        if (fromIt != ptrToObject.end() && toIt != ptrToObject.end())
        {
            fromIt->second->references.push_back(toIt->second);
            toIt->second->refCount++;
            cout << "参照を追加: " << from << " -> " << to 
                 << " (参照カウント: " << toIt->second->refCount << ")\n";
        }
    }
    
    void removeReference(void* from, void* to)
    {
        auto fromIt = ptrToObject.find(from);
        auto toIt = ptrToObject.find(to);
        
        if (fromIt != ptrToObject.end() && toIt != ptrToObject.end())
        {
            auto& refs = fromIt->second->references;
            refs.erase(remove(refs.begin(), refs.end(), toIt->second), refs.end());
            toIt->second->refCount--;
            
            cout << "参照を削除: " << from << " -> " << to 
                 << " (参照カウント: " << toIt->second->refCount << ")\n";
            
            // 参照カウントが0になったら即座解放
            if (toIt->second->refCount == 0)
            {
                deleteObject(toIt->second);
            }
        }
    }
    
    void collectCycles()
    {
        cout << "\n=== 循環参照のコレクション開始 ===\n";
        
        collectionCycles++;
        vector<RCObject*> candidates;
        
        // 参照カウントが0でないオブジェクトを候補として収集
        for (auto& obj : objects)
        {
            if (obj && obj->refCount > 0)
            {
                candidates.push_back(obj.get());
            }
        }
        
        cout << "循環参照の候補: " << candidates.size() << "個\n";
        
        // 簡単な循環検出（実際はMark&Sweepのような高度なアルゴリズムが必要）
        for (RCObject* candidate : candidates)
        {
            if (isInCycle(candidate))
            {
                cout << "循環参照を検出: " << candidate->ptr << "\n";
                deleteObject(candidate);
            }
        }
        
        cout << "=== 循環参照のコレクション終了 ===\n\n";
    }
    
    void printStatistics() const
    {
        cout << "\n=== RC-GC統計 ===\n";
        cout << "総オブジェクト数: " << objects.size() << "\n";
        cout << "総メモリ使用量: " << totalAllocated << " bytes\n";
        cout << "サイクルコレクション数: " << collectionCycles << "\n";
        
        cout << "\nオブジェクト一覧:\n";
        for (const auto& obj : objects)
        {
            if (obj)
            {
                cout << "  " << obj->ptr << " - " << obj->typeName 
                     << " (参照カウント: " << obj->refCount 
                     << ", 参照先: " << obj->references.size() << ")\n";
            }
        }
    }
    
private:
    void deleteObject(RCObject* obj)
    {
        if (!obj) return;
        
        cout << "RCオブジェクトを削除: " << obj->ptr << " (" << obj->typeName << ")\n";
        
        // 参照先の参照カウントを減らす
        for (RCObject* ref : obj->references)
        {
            ref->refCount--;
            if (ref->refCount == 0)
            {
                deleteObject(ref);
            }
        }
        
        // 実際のメモリを解放
        delete static_cast<char*>(obj->ptr);
        totalAllocated -= obj->size;
        ptrToObject.erase(obj->ptr);
        
        // objectsベクタから削除
        auto it = find_if(objects.begin(), objects.end(),
            [obj](const unique_ptr<RCObject>& ptr) { return ptr.get() == obj; });
        if (it != objects.end())
        {
            objects.erase(it);
        }
    }
    
    bool isInCycle(RCObject* obj)
    {
        // 簡化された循環検出（実際はもっと複雑）
        for (RCObject* ref : obj->references)
        {
            if (ref == obj) return true; // 自分自身への参照
            
            // 直接の相互参照をチェック
            for (RCObject* refRef : ref->references)
            {
                if (refRef == obj) return true;
            }
        }
        return false;
    }
    
    void cleanup()
    {
        for (auto& obj : objects)
        {
            if (obj)
            {
                delete static_cast<char*>(obj->ptr);
            }
        }
        objects.clear();
        ptrToObject.clear();
        totalAllocated = 0;
    }
};

void referenceCountedGCDemo()
{
    cout << "\n=== 参照カウントGCのデモ ===\n";
    
    ReferenceCountedGC rcgc;
    
    // キャラクターを作成
    auto* playerA = rcgc.allocate<GameCharacter>();
    playerA->name = "プレイヤーA";
    
    auto* playerB = rcgc.allocate<GameCharacter>();
    playerB->name = "プレイヤーB";
    
    auto* weapon = rcgc.allocate<GameItem>();
    weapon->name = "伝説の剣";
    
    // 相互参照を作成（循環参照）
    rcgc.addReference(playerA, playerB);
    rcgc.addReference(playerB, playerA);
    rcgc.addReference(playerA, weapon);
    
    rcgc.printStatistics();
    
    // 一方の参照を削除
    cout << "\n一方の参照を削除...\n";
    rcgc.removeReference(playerA, playerB);
    
    rcgc.printStatistics();
    
    // 循環参照のコレクション
    rcgc.collectCycles();
    
    rcgc.printStatistics();
}

// メモリ使用量監視システム
class MemoryMonitor
{
private:
    size_t totalAllocated;
    size_t peakUsage;
    int allocationCount;
    int deallocationCount;
    steady_clock::time_point startTime;
    
public:
    MemoryMonitor() : totalAllocated(0), peakUsage(0), allocationCount(0), deallocationCount(0)
    {
        startTime = steady_clock::now();
        cout << "メモリ監視を開始\n";
    }
    
    ~MemoryMonitor()
    {
        auto endTime = steady_clock::now();
        auto duration = duration_cast<seconds>(endTime - startTime).count();
        
        cout << "\n=== メモリ監視レポート ===\n";
        cout << "監視時間: " << duration << " 秒\n";
        cout << "総確保回数: " << allocationCount << "\n";
        cout << "総解放回数: " << deallocationCount << "\n";
        cout << "現在の使用量: " << totalAllocated << " bytes\n";
        cout << "ピーク使用量: " << peakUsage << " bytes\n";
        cout << "リーク可能性: " << (totalAllocated > 0 ? "あり" : "なし") << "\n";
    }
    
    void recordAllocation(size_t size)
    {
        totalAllocated += size;
        allocationCount++;
        
        if (totalAllocated > peakUsage)
        {
            peakUsage = totalAllocated;
        }
        
        cout << "メモリ確保: +" << size << " bytes (総使用量: " << totalAllocated << " bytes)\n";
    }
    
    void recordDeallocation(size_t size)
    {
        totalAllocated -= size;
        deallocationCount++;
        
        cout << "メモリ解放: -" << size << " bytes (総使用量: " << totalAllocated << " bytes)\n";
    }
    
    void printCurrentStatus() const
    {
        cout << "現在のメモリ使用量: " << totalAllocated << " bytes\n";
    }
};

void memoryMonitorDemo()
{
    cout << "\n=== メモリ監視システムのデモ ===\n";
    
    MemoryMonitor monitor;
    
    // シミュレートされたメモリ確保と解放
    for (int i = 0; i < 5; ++i)
    {
        size_t size = (i + 1) * 100;
        monitor.recordAllocation(size);
    }
    
    monitor.printCurrentStatus();
    
    // 一部を解放
    monitor.recordDeallocation(200);
    monitor.recordDeallocation(300);
    
    monitor.printCurrentStatus();
}

int main()
{
    cout << "=== ガベージコレクションの概念と実装 ===\n";
    
    // シンプルGCのデモ
    simpleGCDemo();
    
    // 参照カウントGCのデモ
    referenceCountedGCDemo();
    
    // メモリ監視システムのデモ
    memoryMonitorDemo();
    
    cout << "\n=== ガベージコレクションの種類 ===\n";
    cout << "1. Mark & Sweep: ルートから達可能なオブジェクトをマークし、未マークを削除\n";
    cout << "2. Reference Counting: 参照カウントが0になったら即座解放\n";
    cout << "3. Generational: 若い世代と古い世代で異なる戦略を使用\n";
    cout << "4. Incremental: GC処理を少しずつ実行して一時停止を減らす\n";
    cout << "5. Concurrent: メインプログラムと並行してGCを実行\n";
    
    cout << "\n=== メリットとデメリット ===\n";
    cout << "メリット:\n";
    cout << "1. メモリリークの防止\n";
    cout << "2. プログラマの負担軽減\n";
    cout << "3. 循環参照の自動解決\n";
    
    cout << "デメリット:\n";
    cout << "1. パフォーマンスのオーバーヘッド\n";
    cout << "2. 一時停止（Stop-the-World）の発生\n";
    cout << "3. メモリ使用量の増加\n";
    cout << "4. 非決定的な解放タイミング\n";
    
    cout << "\n=== C++でのGC活用案 ===\n";
    cout << "1. スクリプトシステムの組み込み\n";
    cout << "2. 動的コンテンツの管理（モデル、テクスチャ等）\n";
    cout << "3. ゲームオブジェクトのライフサイクル管理\n";
    cout << "4. メモリプールと組み合わせたハイブリッドアプローチ\n";
    cout << "5. デバッグモードでのメモリリーク検出\n";
    
    return 0;
}

// ガベージコレクションの重要なポイント：
// 1. C++はGCを標準でサポートしていないが、必要に応じて実装可能
// 2. ゲームではリアルタイム性が重要なので、一時停止を最小限にする必要
// 3. smart_ptrやRAIIなどのC++の機能を組み合わせて使用することが多い
// 4. パフォーマンスと使いやすさのバランスを考慮した設計が重要
// 5. デバッグやテスト時のメモリリーク検出には非常に有効