// 動的メモリ割り当て（new/delete）
// ゲーム開発における動的メモリ管理の実践

#include <iostream>
#include <vector>
#include <string>
#include <chrono>

using namespace std;

// 敵キャラクター構造体
struct Enemy
{
    int id;
    string name;
    int hp;
    float x, y, z;
    
    Enemy(int id, const string& name, int hp, float x, float y, float z)
        : id(id), name(name), hp(hp), x(x), y(y), z(z)
    {
        cout << "敵 " << name << " を生成（ID: " << id << "）\n";
    }
    
    ~Enemy()
    {
        cout << "敵 " << name << " を削除（ID: " << id << "）\n";
    }
};

// 基本的なnew/delete操作
void basicNewDelete()
{
    cout << "\n=== 基本的なnew/delete操作 ===\n";
    
    // 単一オブジェクトの動的生成
    int* playerScore = new int(1500);
    cout << "プレイヤースコア: " << *playerScore << "\n";
    
    // 単一オブジェクトの削除
    delete playerScore;
    playerScore = nullptr;  // ダングリングポインタを防ぐ
    
    // 複雑なオブジェクトの動的生成
    Enemy* boss = new Enemy(999, "ドラゴンロード", 5000, 0.0f, 0.0f, 0.0f);
    cout << "ボス戦開始！相手は " << boss->name << "（HP: " << boss->hp << "）\n";
    
    // オブジェクトの削除
    delete boss;
    boss = nullptr;
}

// 配列の動的割り当て
void arrayAllocation()
{
    cout << "\n=== 配列の動的割り当て ===\n";
    
    // 敵の数を実行時に決定
    int enemyCount;
    cout << "今回のステージの敵の数を入力: ";
    cin >> enemyCount;
    
    // 動的配列の確保（基本型）
    int* enemyIds = new int[enemyCount];
    float* enemyPositions = new float[enemyCount * 3];  // x,y,z座標
    
    // 配列の初期化
    for (int i = 0; i < enemyCount; i++)
    {
        enemyIds[i] = i + 1;
        enemyPositions[i * 3] = static_cast<float>(rand() % 100);
        enemyPositions[i * 3 + 1] = 0.0f;
        enemyPositions[i * 3 + 2] = static_cast<float>(rand() % 100);
    }
    
    cout << "生成された敵の位置:\n";
    for (int i = 0; i < enemyCount; i++)
    {
        cout << "敵ID " << enemyIds[i] << ": ("
             << enemyPositions[i * 3] << ", "
             << enemyPositions[i * 3 + 1] << ", "
             << enemyPositions[i * 3 + 2] << ")\n";
    }
    
    // 配列の削除（[]を忘れずに）
    delete[] enemyIds;
    delete[] enemyPositions;
    
    // オブジェクトの動的配列
    Enemy** enemies = new Enemy*[enemyCount];
    for (int i = 0; i < enemyCount; i++)
    {
        enemies[i] = new Enemy(i + 1, "ゴブリン" + to_string(i + 1), 50 + i * 10,
                              static_cast<float>(rand() % 100), 0.0f, static_cast<float>(rand() % 100));
    }
    
    cout << "\n戦闘開始！\n";
    
    // 各敵を削除
    for (int i = 0; i < enemyCount; i++)
    {
        delete enemies[i];
    }
    delete[] enemies;
}

// メモリ確保の失敗処理
void handleAllocationFailure()
{
    cout << "\n=== メモリ確保失敗の処理 ===\n";
    
    try
    {
        // 非常に大きなメモリを要求（失敗する可能性）
        cout << "巨大なゲームワールド（10億要素）の確保を試みます...\n";
        int* hugeArray = new int[1000000000];  // 約4GB
        cout << "確保成功（テスト環境による）\n";
        delete[] hugeArray;
    }
    catch (const bad_alloc& e)
    {
        cout << "メモリ確保失敗: " << e.what() << "\n";
        cout << "フォールバック: より小さなワールドを生成します\n";
        
        // フォールバック処理
        int* smallerArray = new int[1000];
        cout << "小さなワールド（1000要素）を生成しました\n";
        delete[] smallerArray;
    }
    
    // nothrowバージョンの使用
    cout << "\nnothrow版でのメモリ確保:\n";
    int* safeAlloc = new(nothrow) int[1000000000];
    if (safeAlloc == nullptr)
    {
        cout << "メモリ確保失敗（例外なし）- より小さなサイズで再試行\n";
        safeAlloc = new(nothrow) int[1000];
        if (safeAlloc)
        {
            cout << "小さなサイズでの確保に成功\n";
            delete[] safeAlloc;
        }
    }
    else
    {
        cout << "大きなサイズでの確保に成功\n";
        delete[] safeAlloc;
    }
}

// メモリプールの簡単な実装例
class SimpleMemoryPool
{
private:
    char* pool;
    size_t poolSize;
    size_t currentOffset;
    
public:
    SimpleMemoryPool(size_t size) : poolSize(size), currentOffset(0)
    {
        pool = new char[poolSize];
        cout << "メモリプール作成（サイズ: " << poolSize << " バイト）\n";
    }
    
    ~SimpleMemoryPool()
    {
        delete[] pool;
        cout << "メモリプールを削除\n";
    }
    
    void* allocate(size_t size)
    {
        if (currentOffset + size > poolSize)
        {
            cout << "プール容量不足！\n";
            return nullptr;
        }
        
        void* ptr = pool + currentOffset;
        currentOffset += size;
        cout << "プールから " << size << " バイト確保\n";
        return ptr;
    }
    
    void reset()
    {
        currentOffset = 0;
        cout << "メモリプールをリセット\n";
    }
    
    size_t getUsedMemory() const { return currentOffset; }
    size_t getAvailableMemory() const { return poolSize - currentOffset; }
};

void demonstrateMemoryPool()
{
    cout << "\n=== シンプルなメモリプール ===\n";
    
    SimpleMemoryPool gamePool(1024);  // 1KB のプール
    
    // プールからメモリを確保
    int* playerData = static_cast<int*>(gamePool.allocate(sizeof(int) * 10));
    float* enemyPositions = static_cast<float*>(gamePool.allocate(sizeof(float) * 20));
    
    if (playerData && enemyPositions)
    {
        // データの使用
        for (int i = 0; i < 10; i++)
        {
            playerData[i] = i * 100;
        }
        
        cout << "使用中メモリ: " << gamePool.getUsedMemory() << " バイト\n";
        cout << "残り容量: " << gamePool.getAvailableMemory() << " バイト\n";
        
        // ステージクリア時にプールをリセット
        gamePool.reset();
    }
}

int main()
{
    cout << "=== 動的メモリ割り当ての基礎 ===\n";
    
    // 基本的なnew/delete
    basicNewDelete();
    
    // 配列の動的割り当て
    arrayAllocation();
    
    // メモリ確保失敗の処理
    handleAllocationFailure();
    
    // メモリプールの例
    demonstrateMemoryPool();
    
    cout << "\n=== まとめ ===\n";
    cout << "1. new/delete は対で使用する\n";
    cout << "2. new[] には delete[] を使用\n";
    cout << "3. 削除後はポインタをnullptrに設定\n";
    cout << "4. メモリ確保失敗に備えた処理を実装\n";
    cout << "5. ゲームでは頻繁な動的確保を避け、プールを活用\n";
    
    return 0;
}

// ゲーム開発での動的メモリ活用例：
// - レベル読み込み時の地形データ確保
// - 敵の動的生成・削除
// - アイテムドロップの管理
// - UIエレメントの動的作成
// - エフェクトシステムでのパーティクル管理