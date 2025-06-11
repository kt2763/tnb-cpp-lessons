// メモリリークとデバッグ手法
// ゲーム開発でのメモリリークの発見と修正

#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <string>
#include <chrono>

using namespace std;

// メモリリーク検出用のシンプルなトラッカー
class MemoryTracker
{
private:
    static map<void*, size_t> allocations;
    static size_t totalAllocated;
    static size_t totalFreed;
    
public:
    static void recordAllocation(void* ptr, size_t size)
    {
        allocations[ptr] = size;
        totalAllocated += size;
        cout << "[MEM] 確保: " << ptr << " (サイズ: " << size << " バイト)\n";
    }
    
    static void recordDeallocation(void* ptr)
    {
        auto it = allocations.find(ptr);
        if (it != allocations.end())
        {
            totalFreed += it->second;
            cout << "[MEM] 解放: " << ptr << " (サイズ: " << it->second << " バイト)\n";
            allocations.erase(it);
        }
        else
        {
            cout << "[MEM] 警告: 未登録のポインタを解放しようとしました: " << ptr << "\n";
        }
    }
    
    static void printReport()
    {
        cout << "\n=== メモリレポート ===\n";
        cout << "総確保量: " << totalAllocated << " バイト\n";
        cout << "総解放量: " << totalFreed << " バイト\n";
        cout << "リーク量: " << (totalAllocated - totalFreed) << " バイト\n";
        
        if (!allocations.empty())
        {
            cout << "\nリークしたポインタ:";
            for (const auto& pair : allocations)
            {
                cout << "\n  " << pair.first << " (サイズ: " << pair.second << " バイト)";
            }
            cout << "\n";
        }
        else
        {
            cout << "メモリリークなし！\n";
        }
    }
};

// 静的メンバの初期化
map<void*, size_t> MemoryTracker::allocations;
size_t MemoryTracker::totalAllocated = 0;
size_t MemoryTracker::totalFreed = 0;

// カスタムnew/delete演算子（デバッグ用）
void* operator new(size_t size)
{
    void* ptr = malloc(size);
    if (ptr)
    {
        MemoryTracker::recordAllocation(ptr, size);
        return ptr;
    }
    throw bad_alloc();
}

void operator delete(void* ptr) noexcept
{
    if (ptr)
    {
        MemoryTracker::recordDeallocation(ptr);
        free(ptr);
    }
}

void* operator new[](size_t size)
{
    void* ptr = malloc(size);
    if (ptr)
    {
        MemoryTracker::recordAllocation(ptr, size);
        return ptr;
    }
    throw bad_alloc();
}

void operator delete[](void* ptr) noexcept
{
    if (ptr)
    {
        MemoryTracker::recordDeallocation(ptr);
        free(ptr);
    }
}

// ゲームアセットクラス
class GameAsset
{
public:
    string name;
    char* data;
    size_t size;
    
    GameAsset(const string& name, size_t size) : name(name), size(size)
    {
        data = new char[size];
        cout << "アセット '" << name << "' を読み込み (サイズ: " << size << " バイト)\n";
    }
    
    ~GameAsset()
    {
        delete[] data;
        cout << "アセット '" << name << "' を解放\n";
    }
};

// メモリリークの一般的なパターン
void demonstrateCommonLeaks()
{
    cout << "\n=== 一般的なメモリリークのパターン ===\n";
    
    // パターン1: 単純な削除忘れ
    cout << "\n1. 単純な削除忘れ:\n";
    int* forgottenPointer = new int(42);
    cout << "ポインタを作成したが削除し忘れ...\n";
    // delete forgottenPointer; // この行がコメントアウトされている！
    
    // パターン2: 配列の削除方法間違い
    cout << "\n2. 配列の削除方法間違い:\n";
    int* arrayPtr = new int[10];
    // delete arrayPtr;   // 間違い！delete[] を使うべき
    delete[] arrayPtr;    // 正しい方法
    
    // パターン3: 例外時のリーク
    cout << "\n3. 例外時のメモリリーク:\n";
    GameAsset* asset = nullptr;
    try
    {
        asset = new GameAsset("テクスチャ", 1024);
        // 例外が発生する可能性のある処理
        throw runtime_error("ロードエラー");
        delete asset;  // この行は実行されない
    }
    catch (const exception& e)
    {
        cout << "例外発生: " << e.what() << "\n";
        delete asset;  // 例外ハンドラでクリーンアップ
    }
}

// ゲームオブジェクトのメモリ管理の悪い例
class BadGameObject
{
public:
    int* stats;
    char* textureData;
    
    BadGameObject()
    {
        stats = new int[10];  // HP, MPなどのステータス
        textureData = new char[1024];  // テクスチャデータ
    }
    
    // デストラクタがない！メモリリークの原因
};

// 改善されたゲームオブジェクト
class GoodGameObject
{
public:
    int* stats;
    char* textureData;
    
    GoodGameObject()
    {
        stats = new int[10];
        textureData = new char[1024];
    }
    
    ~GoodGameObject()  // デストラクタでメモリを解放
    {
        delete[] stats;
        delete[] textureData;
        cout << "GameObjectのメモリを正しく解放\n";
    }
    
    // コピーコンストラクタと代入演算子を禁止（深いコピー問題を防ぐ）
    GoodGameObject(const GoodGameObject&) = delete;
    GoodGameObject& operator=(const GoodGameObject&) = delete;
};

// 循環参照の例
class Player;
class Guild
{
public:
    string name;
    vector<Player*> members;  // メンバーへのポインタ
    
    Guild(const string& name) : name(name) {}
    
    ~Guild()
    {
        cout << "ギルド '" << name << "' を解散\n";
        // メンバーのポインタを削除しない（他で管理していると仮定）
    }
};

class Player
{
public:
    string name;
    Guild* guild;  // 所属ギルドへのポインタ
    
    Player(const string& name) : name(name), guild(nullptr) {}
    
    ~Player()
    {
        cout << "プレイヤー '" << name << "' がログアウト\n";
    }
    
    void joinGuild(Guild* g)
    {
        guild = g;
        g->members.push_back(this);
    }
};

void demonstrateCircularReference()
{
    cout << "\n=== 循環参照の例 ===\n";
    
    Guild* dragonGuild = new Guild("ドラゴンギルド");
    Player* hero = new Player("勇者");
    Player* mage = new Player("魔法使い");
    
    hero->joinGuild(dragonGuild);
    mage->joinGuild(dragonGuild);
    
    cout << "ギルドとプレイヤーが相互参照している状態\n";
    
    // 正しい削除順序：先に関係を解除してからオブジェクトを削除
    dragonGuild->members.clear();
    delete hero;
    delete mage;
    delete dragonGuild;
}

// メモリリーク検出のベストプラクティス
void bestPracticesDemo()
{
    cout << "\n=== メモリリーク防止のベストプラクティス ===\n";
    
    cout << "\n1. RAIIパターンの使用:\n";
    {
        GoodGameObject obj;  // スタック上に作成
        cout << "オブジェクトを使用中...\n";
        // スコープを抜けると自動的にデストラクタが呼ばれる
    }
    
    cout << "\n2. スマートポインタの使用:\n";
    {
        auto smartAsset = make_unique<GameAsset>("スマートアセット", 512);
        cout << "スマートポインタで管理中...\n";
        // 自動的に削除される
    }
    
    cout << "\n3. コンテナの使用:\n";
    {
        vector<int> gameData(1000);  // 自動的にメモリ管理
        cout << "vectorでデータを管理中 (サイズ: " << gameData.size() << ")\n";
        // vectorのデストラクタが自動的にメモリを解放
    }
}

// メモリリークシミュレーション
void simulateMemoryLeaks()
{
    cout << "\n=== メモリリークシミュレーション ===\n";
    
    cout "意図的なリークを作成中...\n";
    
    // リーク1: 単純なポインタリーク
    int* leakedInt = new int(123);
    
    // リーク2: 配列リーク
    char* leakedArray = new char[500];
    
    // リーク3: オブジェクトリーク
    BadGameObject* leakedObject = new BadGameObject();
    
    cout << "リークを作成完了（意図的）\n";
    
    // これらを削除しないことで意図的にリークさせる
}

int main()
{
    cout << "=== メモリリークとデバッグ手法 ===\n";
    
    // 一般的なメモリリークのパターン
    demonstrateCommonLeaks();
    
    // 循環参照の問題
    demonstrateCircularReference();
    
    // ベストプラクティスのデモ
    bestPracticesDemo();
    
    // メモリリークを意図的に作成
    simulateMemoryLeaks();
    
    // メモリトラッカーのレポートを表示
    MemoryTracker::printReport();
    
    cout << "\n=== メモリリーク検出ツール ===\n";
    cout << "1. Valgrind (例: valgrind --leak-check=full ./program)\n";
    cout << "2. AddressSanitizer (例: g++ -fsanitize=address -g program.cpp)\n";
    cout "3. Visual Studio Diagnostic Tools\n";
    cout << "4. Dr. Memory (Windows)\n";
    cout << "5. カスタムメモリトラッカー（上記の例）\n";
    
    cout << "\n=== まとめ ===\n";
    cout << "1. newとdelete、new[]とdelete[]を必ず対で使用\n";
    cout << "2. RAIIパターンで自動的なメモリ管理を実現\n";
    cout << "3. スマートポインタやSTLコンテナを積極的に使用\n";
    cout << "4. 循環参照や例外時のリークに注意\n";
    cout << "5. メモリリーク検出ツールを積極的に活用\n";
    
    return 0;
}

// ゲーム開発でのメモリリークの一般的な原因：
// 1. アセットローダーでのリソース解放忘れ
// 2. ゲームオブジェクトのライフサイクル管理不備
// 3. UIシステムでのイベントリスナーの解除忘れ
// 4. サウンドやテクスチャのキャッシュ管理の不備
// 5. マルチスレッド環境での競合状態