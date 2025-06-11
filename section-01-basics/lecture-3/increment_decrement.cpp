// 第3講: インクリメント・デクリメント演算子
// 値を1ずつ増減させる演算子

#include <iostream>

int main() 
{
    std::cout << "=== インクリメント演算子 (++) ===" << std::endl;
    
    // 前置インクリメント
    int playerLevel = 10;
    std::cout << "現在のレベル: " << playerLevel << std::endl;
    std::cout << "++playerLevel の値: " << ++playerLevel << std::endl;  // 11 (先に増加)
    std::cout << "演算後のレベル: " << playerLevel << std::endl;  // 11
    
    // 後置インクリメント
    int enemyCount = 5;
    std::cout << "\n敵の数: " << enemyCount << std::endl;
    std::cout << "enemyCount++ の値: " << enemyCount++ << std::endl;  // 5 (値を返してから増加)
    std::cout << "演算後の敵の数: " << enemyCount << std::endl;  // 6
    
    std::cout << "\n=== デクリメント演算子 (--) ===" << std::endl;
    
    // 前置デクリメント
    int playerHP = 100;
    std::cout << "現在のHP: " << playerHP << std::endl;
    std::cout << "--playerHP の値: " << --playerHP << std::endl;  // 99 (先に減少)
    std::cout << "演算後のHP: " << playerHP << std::endl;  // 99
    
    // 後置デクリメント
    int remainingAmmo = 10;
    std::cout << "\n残弾数: " << remainingAmmo << std::endl;
    std::cout << "remainingAmmo-- の値: " << remainingAmmo-- << std::endl;  // 10 (値を返してから減少)
    std::cout << "演算後の残弾数: " << remainingAmmo << std::endl;  // 9
    
    // ゲーム開発での実用例
    std::cout << "\n=== ゲーム開発での使用例 ===" << std::endl;
    
    // コンボカウンター
    int comboCount = 0;
    std::cout << "コンボ開始！" << std::endl;
    std::cout << "攻撃1: コンボ " << ++comboCount << std::endl;  // 1
    std::cout << "攻撃2: コンボ " << ++comboCount << std::endl;  // 2
    std::cout << "攻撃3: コンボ " << ++comboCount << std::endl;  // 3
    
    // アイテム使用
    int potionCount = 3;
    std::cout << "\nポーション所持数: " << potionCount << std::endl;
    
    if (potionCount > 0) 
    {
        std::cout << "ポーションを使用しました！ 残り: " << --potionCount << std::endl;
    }
    
    // 敵の撃破カウント
    int defeatedEnemies = 0;
    int totalEnemies = 5;
    
    std::cout << "\n=== 敵撃破シミュレーション ===" << std::endl;
    std::cout << "敵を倒した！ 撃破数: " << ++defeatedEnemies << "/" << totalEnemies << std::endl;
    std::cout << "敵を倒した！ 撃破数: " << ++defeatedEnemies << "/" << totalEnemies << std::endl;
    
    // 前置と後置の違いを示す例
    std::cout << "\n=== 前置と後置の違い ===" << std::endl;
    int score1 = 10;
    int score2 = 10;
    
    int result1 = ++score1 * 2;  // (10+1) * 2 = 22
    int result2 = score2++ * 2;  // 10 * 2 = 20, その後score2は11になる
    
    std::cout << "前置: ++score1 * 2 = " << result1 << " (score1 = " << score1 << ")" << std::endl;
    std::cout << "後置: score2++ * 2 = " << result2 << " (score2 = " << score2 << ")" << std::endl;
    
    return 0;
}

/*
    インクリメント・デクリメント演算子の使い分け:
    
    前置 (++x, --x):
    - 先に値を変更してから、その値を使用
    - 単独で使う場合は前置を推奨（わずかに効率的）
    
    後置 (x++, x--):
    - 現在の値を使用してから、値を変更
    - 配列のインデックスなどで現在の値が必要な場合に使用
    
    ゲーム開発での使用場面:
    - スコアカウンター: score++
    - レベルアップ: ++playerLevel
    - 残機減少: --lives
    - 弾薬消費: ammo--
    - ターンカウント: ++turnCount
*/