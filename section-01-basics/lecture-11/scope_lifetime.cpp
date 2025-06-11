// 第11講: スコープと変数の寿命
// 変数がどこで使えて、いつまで生きているかを理解する

#include <iostream>
#include <string>

// グローバル変数（ファイル全体で使用可能）
int globalPlayerCount = 0;
const int MAX_PLAYERS = 100;

// 関数レベルのスコープ
void demonstrateLocalScope() 
{
    int localVar = 42;  // この関数内でのみ使用可能
    std::cout << "関数内のローカル変数: " << localVar << std::endl;
    
    // ブロックスコープ
    if (true) 
    {
        int blockVar = 10;  // このブロック内でのみ使用可能
        std::cout << "ブロック内の変数: " << blockVar << std::endl;
        std::cout << "ブロック内からローカル変数: " << localVar << std::endl;  // アクセス可能
    }
    
    // std::cout << blockVar;  // エラー！ブロックを出たので使用不可
}

// ゲームの戦闘システムの例
void battleSystem() 
{
    std::cout << "\n=== 戦闘システム ===" << std::endl;
    
    int playerHP = 100;
    int enemyHP = 80;
    
    // 戦闘ループ
    for (int turn = 1; turn <= 5; ++turn) 
    {
        std::cout << "\nターン " << turn << std::endl;
        
        // ターン内でのみ有効な変数
        int playerDamage = 20 + (turn * 2);  // ターンごとに攻撃力上昇
        int enemyDamage = 15;
        
        enemyHP -= playerDamage;
        playerHP -= enemyDamage;
        
        std::cout << "プレイヤーは " << playerDamage << " のダメージを与えた" << std::endl;
        std::cout << "プレイヤーHP: " << playerHP << ", 敵HP: " << enemyHP << std::endl;
        
        if (enemyHP <= 0) 
        {
            std::cout << "敵を倒した！" << std::endl;
            break;
        }
        
        if (playerHP <= 0) 
        {
            std::cout << "プレイヤーが倒された..." << std::endl;
            break;
        }
    }
    
    // turn, playerDamage, enemyDamage はここでは使用不可
}

// 静的ローカル変数の例
int getNextPlayerID() 
{
    static int nextID = 1;  // 関数を抜けても値が保持される
    return nextID++;
}

// グローバル変数の操作
void addPlayer(const std::string& playerName) 
{
    if (globalPlayerCount < MAX_PLAYERS) 
    {
        globalPlayerCount++;
        std::cout << "プレイヤー「" << playerName << "」が参加しました。" << std::endl;
        std::cout << "現在のプレイヤー数: " << globalPlayerCount << std::endl;
    }
    else 
    {
        std::cout << "サーバーが満員です！" << std::endl;
    }
}

int main() 
{
    std::cout << "=== スコープと寿命の基本 ===" << std::endl;
    
    // メイン関数のローカル変数
    int mainVar = 100;
    
    std::cout << "メイン関数のローカル変数: " << mainVar << std::endl;
    std::cout << "グローバル変数: " << globalPlayerCount << std::endl;
    
    // 関数を呼び出してスコープを実演
    demonstrateLocalScope();
    
    // ブロックスコープの詳細例
    std::cout << "\n=== ブロックスコープの詳細 ===" << std::endl;
    
    int x = 10;
    std::cout << "外側のx: " << x << std::endl;
    
    {
        int x = 20;  // 同じ名前だが別の変数（シャドウイング）
        std::cout << "内側のx: " << x << std::endl;
        
        {
            int x = 30;  // さらに内側
            std::cout << "最も内側のx: " << x << std::endl;
        }
        
        std::cout << "内側に戻ったx: " << x << std::endl;
    }
    
    std::cout << "再び外側のx: " << x << std::endl;
    
    // ループでのスコープ
    std::cout << "\n=== ループでのスコープ ===" << std::endl;
    
    for (int i = 0; i < 3; ++i) 
    {
        int loopVar = i * 10;
        std::cout << "ループ " << i << ": loopVar = " << loopVar << std::endl;
    }
    
    // std::cout << i;  // エラー！iはforループ内でのみ有効
    
    // 静的変数の例
    std::cout << "\n=== 静的変数の例 ===" << std::endl;
    
    for (int i = 0; i < 5; ++i) 
    {
        std::cout << "プレイヤーID: " << getNextPlayerID() << std::endl;
    }
    
    // グローバル変数の使用例
    std::cout << "\n=== グローバル変数の使用 ===" << std::endl;
    
    addPlayer("太郎");
    addPlayer("花子");
    addPlayer("次郎");
    
    // 戦闘システムの実行
    battleSystem();
    
    // インベントリシステムでのスコープ例
    std::cout << "\n=== インベントリシステム ===" << std::endl;
    
    const int INVENTORY_SIZE = 10;
    int inventory[INVENTORY_SIZE] = {0};  // 0で初期化
    
    auto addItem = [&inventory](int itemID) 
    {
        for (int i = 0; i < INVENTORY_SIZE; ++i) 
        {
            if (inventory[i] == 0) 
            {  // 空きスロット
                inventory[i] = itemID;
                std::cout << "アイテム " << itemID << " をスロット " 
                          << (i + 1) << " に追加" << std::endl;
                return;
            }
        }
        std::cout << "インベントリが満杯です！" << std::endl;
    };
    
    addItem(101);  // ポーション
    addItem(102);  // エーテル
    addItem(201);  // 鉄の剣
    
    // スコープルールによる変数の隠蔽例
    std::cout << "\n=== 変数の隠蔽（シャドウイング） ===" << std::endl;
    
    int damage = 50;  // 外側のdamage
    std::cout << "基本ダメージ: " << damage << std::endl;
    
    {
        double damage = 75.5;  // 内側のdamage（型も違う）
        std::cout << "クリティカルダメージ: " << damage << std::endl;
        
        // 外側のdamageにアクセスするには...
        // C++では直接的な方法はない（::damage は使えない）
    }
    
    std::cout << "再び基本ダメージ: " << damage << std::endl;
    
    return 0;
}

/*
    スコープの種類:
    
    1. グローバルスコープ:
       - ファイル全体で使用可能
       - プログラム終了まで生存
       - 多用は避ける（名前衝突、保守性の問題）
    
    2. 関数スコープ:
       - 関数内でのみ使用可能
       - 関数終了時に破棄
    
    3. ブロックスコープ:
       - {} で囲まれた範囲内でのみ使用可能
       - ブロック終了時に破棄
    
    4. ループスコープ:
       - for文の初期化部で宣言された変数
       - ループ終了時に破棄
    
    特殊な変数:
    
    static変数:
    - 関数内で宣言してもプログラム終了まで生存
    - 初回実行時のみ初期化
    
    シャドウイング:
    - 内側のスコープで同名の変数を宣言
    - 外側の変数が一時的に隠される
    
    ベストプラクティス:
    - 変数のスコープは可能な限り狭く
    - グローバル変数は最小限に
    - 適切な名前付けで衝突を避ける
*/