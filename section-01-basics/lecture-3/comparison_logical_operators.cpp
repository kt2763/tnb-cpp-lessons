// 第3講: 比較演算子と論理演算子
// 条件判定に使用する演算子

#include <iostream>

int main() 
{
    // 比較演算子
    int playerLevel = 15;
    int requiredLevel = 10;
    int playerHP = 30;
    int maxHP = 100;
    
    std::cout << "=== 比較演算子 ===" << std::endl;
    std::cout << "プレイヤーレベル: " << playerLevel << std::endl;
    std::cout << "必要レベル: " << requiredLevel << std::endl;
    
    // 各種比較演算子
    std::cout << "レベル == 必要レベル: " << (playerLevel == requiredLevel) << std::endl;  // false (0)
    std::cout << "レベル != 必要レベル: " << (playerLevel != requiredLevel) << std::endl;  // true (1)
    std::cout << "レベル > 必要レベル: " << (playerLevel > requiredLevel) << std::endl;    // true
    std::cout << "レベル >= 必要レベル: " << (playerLevel >= requiredLevel) << std::endl;  // true
    std::cout << "レベル < 必要レベル: " << (playerLevel < requiredLevel) << std::endl;    // false
    std::cout << "レベル <= 必要レベル: " << (playerLevel <= requiredLevel) << std::endl;  // false
    
    // 論理演算子
    std::cout << "\n=== 論理演算子 ===" << std::endl;
    
    bool hasKey = true;
    bool hasMap = false;
    bool isDaytime = true;
    
    // AND演算子 (&&): 両方が真の場合のみ真
    bool canOpenTreasure = (playerLevel >= 10) && hasKey;
    std::cout << "宝箱を開けられる（レベル10以上 AND 鍵を持っている）: " 
              << (canOpenTreasure ? "はい" : "いいえ") << std::endl;
    
    // OR演算子 (||): どちらかが真なら真
    bool canNavigate = hasMap || isDaytime;
    std::cout << "移動できる（地図を持っている OR 昼間）: " 
              << (canNavigate ? "はい" : "いいえ") << std::endl;
    
    // NOT演算子 (!): 真偽を反転
    bool isNight = !isDaytime;
    std::cout << "夜である: " << (isNight ? "はい" : "いいえ") << std::endl;
    
    // ゲームでの実用例
    std::cout << "\n=== ゲームでの条件判定例 ===" << std::endl;
    
    // 複雑な条件の組み合わせ
    int playerMoney = 500;
    int itemPrice = 300;
    int playerInventorySpace = 3;
    int maxInventorySpace = 10;
    
    // アイテムを購入できるか？
    bool canBuyItem = (playerMoney >= itemPrice) && 
                      (playerInventorySpace < maxInventorySpace);
    
    std::cout << "所持金: " << playerMoney << "G, アイテム価格: " << itemPrice << "G" << std::endl;
    std::cout << "インベントリ: " << playerInventorySpace << "/" << maxInventorySpace << std::endl;
    std::cout << "アイテムを購入できる: " << (canBuyItem ? "はい" : "いいえ") << std::endl;
    
    // クエスト受注条件
    bool hasCompletedTutorial = true;
    int playerRank = 3;  // 1:初級, 2:中級, 3:上級
    bool isQuestAvailable = true;
    
    bool canAcceptQuest = hasCompletedTutorial && 
                         (playerRank >= 2) && 
                         isQuestAvailable;
    
    std::cout << "\n上級クエストを受注できる: " << (canAcceptQuest ? "はい" : "いいえ") << std::endl;
    
    // 生存判定
    bool isAlive = (playerHP > 0) && (playerHP <= maxHP);
    bool needsHealing = (playerHP > 0) && (playerHP < (maxHP * 0.3));  // HP30%未満
    
    std::cout << "\nプレイヤーHP: " << playerHP << "/" << maxHP << std::endl;
    std::cout << "生存している: " << (isAlive ? "はい" : "いいえ") << std::endl;
    std::cout << "回復が必要: " << (needsHealing ? "はい" : "いいえ") << std::endl;
    
    return 0;
}

/*
    演算子の優先順位と結合性:
    1. ! (NOT)
    2. 比較演算子 (<, >, <=, >=, ==, !=)
    3. && (AND)
    4. || (OR)
    
    短絡評価（ショートサーキット）:
    - && の左側が false なら、右側は評価されない
    - || の左側が true なら、右側は評価されない
    
    ゲーム開発でのベストプラクティス:
    - 複雑な条件は変数に分けて可読性を高める
    - 条件の順序を工夫して処理を最適化する
    - マジックナンバーは定数にする（例: 0.3 → LOW_HP_THRESHOLD）
*/