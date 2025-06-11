/**
 * @file lambda_basics.cpp
 * @brief ラムダ式の基礎と構文
 * 
 * ラムダ式は無名関数オブジェクトを作成する便利な方法です。
 * ゲーム開発では、イベントハンドリングやコールバックシステムで頻繁に使用されます。
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

int main()
{
    // ラムダ式の基本構文：[キャプチャ](パラメータ) -> 戻り値型 { 本体 }
    std::cout << "=== ラムダ式の基礎 ===\n";
    
    // 1. 最もシンプルなラムダ式
    auto simpleLambda = []()
    {
        std::cout << "Hello from lambda!\n";
    };
    simpleLambda();
    
    // 2. パラメータを持つラムダ式（ダメージ計算）
    auto calculateDamage = [](int baseDamage, float multiplier) -> int
    {
        return static_cast<int>(baseDamage * multiplier);
    };
    
    int damage = calculateDamage(100, 1.5f);
    std::cout << "計算されたダメージ: " << damage << "\n";
    
    // 3. キャプチャを使用するラムダ式
    int playerLevel = 25;
    int enemyLevel = 30;
    
    // 値キャプチャ [=]
    auto checkLevelDifference = [=]()
    {
        int difference = enemyLevel - playerLevel;
        std::cout << "レベル差: " << difference << "\n";
        return difference > 10; // 危険な敵かどうか
    };
    
    bool isDangerous = checkLevelDifference();
    std::cout << "危険な敵: " << (isDangerous ? "はい" : "いいえ") << "\n\n";
    
    // 4. ゲームイベントハンドラーの例
    std::cout << "=== イベントハンドラーシステム ===\n";
    
    // イベントタイプの定義
    enum class GameEvent
    {
        PlayerLevelUp,
        ItemPickup,
        EnemyDefeated
    };
    
    // イベントハンドラーの登録
    std::vector<std::function<void(int)>> levelUpHandlers;
    
    // 複数のハンドラーを登録
    levelUpHandlers.push_back([](int newLevel)
    {
        std::cout << "レベルアップ！新しいレベル: " << newLevel << "\n";
    });
    
    levelUpHandlers.push_back([](int newLevel)
    {
        int hpBonus = newLevel * 10;
        std::cout << "HP +" << hpBonus << " ボーナス獲得！\n";
    });
    
    levelUpHandlers.push_back([](int newLevel)
    {
        if (newLevel % 5 == 0)
        {
            std::cout << "新しいスキルをアンロック！\n";
        }
    });
    
    // イベント発火
    int newPlayerLevel = 10;
    for (const auto& handler : levelUpHandlers)
    {
        handler(newPlayerLevel);
    }
    
    // 5. STLアルゴリズムでの使用例（アイテムフィルタリング）
    std::cout << "\n=== アイテムフィルタリング ===\n";
    
    struct Item
    {
        std::string name;
        int value;
        int requiredLevel;
    };
    
    std::vector<Item> inventory = 
    {
        {"ポーション", 50, 1},
        {"エリクサー", 500, 10},
        {"伝説の剣", 5000, 25},
        {"鉄の鎧", 300, 5},
        {"ドラゴンシールド", 3000, 20}
    };
    
    // プレイヤーが使用可能なアイテムをフィルタリング
    std::cout << "使用可能なアイテム（レベル " << playerLevel << "）:\n";
    std::for_each(inventory.begin(), inventory.end(),
        [playerLevel](const Item& item)
        {
            if (item.requiredLevel <= playerLevel)
            {
                std::cout << "- " << item.name 
                         << " (価値: " << item.value << ")\n";
            }
        });
    
    // 6. 条件付きアクション（戦闘システム）
    std::cout << "\n=== 戦闘アクション ===\n";
    
    struct CombatAction
    {
        std::string name;
        std::function<void(int&, int&)> execute;
    };
    
    std::vector<CombatAction> actions;
    
    // 通常攻撃
    actions.push_back({"通常攻撃", 
        [](int& playerHP, int& enemyHP)
        {
            enemyHP -= 50;
            std::cout << "敵に50のダメージ！\n";
        }});
    
    // 防御
    actions.push_back({"防御",
        [](int& playerHP, int& enemyHP)
        {
            std::cout << "防御態勢！次のダメージを半減\n";
        }});
    
    // スキル使用
    actions.push_back({"火炎魔法",
        [](int& playerHP, int& enemyHP)
        {
            enemyHP -= 120;
            playerHP -= 10; // MP消費をHPで代用
            std::cout << "火炎魔法で120のダメージ！（MP消費）\n";
        }});
    
    // 戦闘シミュレーション
    int playerHP = 100;
    int enemyHP = 200;
    
    std::cout << "初期状態 - プレイヤーHP: " << playerHP 
              << ", 敵HP: " << enemyHP << "\n";
    
    // ランダムにアクションを実行
    for (int i = 0; i < 3; ++i)
    {
        int actionIndex = i % actions.size();
        std::cout << "\nアクション: " << actions[actionIndex].name << "\n";
        actions[actionIndex].execute(playerHP, enemyHP);
        std::cout << "現在 - プレイヤーHP: " << playerHP 
                  << ", 敵HP: " << enemyHP << "\n";
    }
    
    return 0;
}

/**
 * まとめ：
 * - ラムダ式は [capture](parameters) -> return_type { body } の形式
 * - キャプチャで外部の変数にアクセス可能
 * - std::functionでラムダを保存・管理できる
 * - ゲームのイベントシステムやコールバックに最適
 * - STLアルゴリズムと組み合わせて強力な処理が可能
 */