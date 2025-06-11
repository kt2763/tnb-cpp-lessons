// スタックとヒープの基礎
// ゲームにおけるメモリ管理の基本を理解する

#include <iostream>
#include <vector>
#include <string>

using namespace std;

// スタック上のゲームキャラクター構造体
struct StackCharacter
{
    int hp;           // 体力
    int mp;           // マジックポイント
    float x, y, z;    // 位置座標
};

// ヒープ上の大規模ゲームワールドデータ
class GameWorld
{
private:
    vector<int>* terrainData;     // 地形データ
    vector<string>* npcDialogues; // NPCの会話データ
    
public:
    GameWorld(int worldSize)
    {
        // ヒープ上に大量のデータを確保
        terrainData = new vector<int>(worldSize * worldSize);
        npcDialogues = new vector<string>();
        cout << "ゲームワールドをヒープ上に作成（サイズ: " << worldSize << "x" << worldSize << "）\n";
    }
    
    ~GameWorld()
    {
        delete terrainData;
        delete npcDialogues;
        cout << "ゲームワールドを解放\n";
    }
};

// スタックメモリの例
void demonstrateStack()
{
    cout << "\n=== スタックメモリの例 ===\n";
    
    // スタック上の変数（自動的に管理される）
    int playerLevel = 25;
    float playerSpeed = 5.5f;
    StackCharacter hero = {100, 50, 0.0f, 0.0f, 0.0f};
    
    cout << "プレイヤーレベル: " << playerLevel << "（スタック上）\n";
    cout << "ヒーローのHP: " << hero.hp << "（スタック上）\n";
    
    // スタック上の配列（サイズが固定）
    int inventory[10] = {0};  // 10個のアイテムスロット
    inventory[0] = 101;  // ポーション
    inventory[1] = 202;  // 剣
    
    cout << "インベントリサイズ: 10（固定配列、スタック上）\n";
    
    // 関数終了時に自動的にメモリが解放される
}

// ヒープメモリの例
void demonstrateHeap()
{
    cout << "\n=== ヒープメモリの例 ===\n";
    
    // ヒープ上に動的にメモリを確保
    int* enemyCount = new int(50);
    cout << "敵の数: " << *enemyCount << "（ヒープ上）\n";
    
    // 動的配列（実行時にサイズを決定）
    cout << "ダンジョンの部屋数を入力: ";
    int roomCount;
    cin >> roomCount;
    
    int* dungeonRooms = new int[roomCount];
    for (int i = 0; i < roomCount; i++)
    {
        dungeonRooms[i] = i + 1;
    }
    
    cout << "ダンジョンに" << roomCount << "個の部屋を作成（ヒープ上）\n";
    
    // 手動でメモリを解放する必要がある
    delete enemyCount;
    delete[] dungeonRooms;
    
    cout << "ヒープメモリを手動で解放完了\n";
}

// スタックとヒープのパフォーマンス比較
void comparePerformance()
{
    cout << "\n=== パフォーマンス比較 ===\n";
    
    const int iterations = 1000000;
    
    // スタック割り当て（高速）
    auto startStack = chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++)
    {
        int stackVar = i;
    }
    auto endStack = chrono::high_resolution_clock::now();
    
    // ヒープ割り当て（低速）
    auto startHeap = chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++)
    {
        int* heapVar = new int(i);
        delete heapVar;
    }
    auto endHeap = chrono::high_resolution_clock::now();
    
    auto stackTime = chrono::duration_cast<chrono::microseconds>(endStack - startStack).count();
    auto heapTime = chrono::duration_cast<chrono::microseconds>(endHeap - startHeap).count();
    
    cout << "スタック割り当て時間: " << stackTime << " マイクロ秒\n";
    cout << "ヒープ割り当て時間: " << heapTime << " マイクロ秒\n";
    cout << "ヒープはスタックの約" << heapTime / max(stackTime, 1LL) << "倍遅い\n";
}

int main()
{
    cout << "=== スタックとヒープの基礎 ===\n";
    
    // スタックメモリのデモ
    demonstrateStack();
    
    // ヒープメモリのデモ
    demonstrateHeap();
    
    // 大規模データ構造の例
    cout << "\n=== 大規模ゲームワールドの例 ===\n";
    {
        GameWorld world(100);  // 100x100のワールド
        // スコープを抜けると自動的にデストラクタが呼ばれる
    }
    
    // パフォーマンス比較
    comparePerformance();
    
    cout << "\n=== まとめ ===\n";
    cout << "スタック: 高速、自動管理、サイズ制限あり、ローカル変数向き\n";
    cout << "ヒープ: 低速、手動管理、大容量データ可、動的割り当て向き\n";
    
    return 0;
}

// 重要なポイント：
// 1. スタックは高速だが容量が限られている（通常1-8MB）
// 2. ヒープは大容量だが割り当て/解放が遅い
// 3. ゲームでは両方を適切に使い分けることが重要
// 4. 小さなデータはスタック、大きなデータはヒープを使用