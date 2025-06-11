// 第3講: 算術演算子
// 基本的な計算を行うための演算子

#include <iostream>

int main() 
{
    // 基本的な算術演算子
    int a = 10;
    int b = 3;
    
    std::cout << "=== 基本的な算術演算 ===" << std::endl;
    std::cout << "a = " << a << ", b = " << b << std::endl;
    std::cout << "加算 (a + b): " << (a + b) << std::endl;  // 13
    std::cout << "減算 (a - b): " << (a - b) << std::endl;  // 7
    std::cout << "乗算 (a * b): " << (a * b) << std::endl;  // 30
    std::cout << "除算 (a / b): " << (a / b) << std::endl;  // 3 (整数除算)
    std::cout << "剰余 (a % b): " << (a % b) << std::endl;  // 1
    
    // 浮動小数点数での除算
    double x = 10.0;
    double y = 3.0;
    std::cout << "\n浮動小数点の除算 (10.0 / 3.0): " << (x / y) << std::endl;  // 3.33333...
    
    // ゲーム開発での実用例
    std::cout << "\n=== ゲーム開発での使用例 ===" << std::endl;
    
    // ダメージ計算
    int baseDamage = 50;
    int attackPower = 20;
    double criticalMultiplier = 1.5;
    
    int normalDamage = baseDamage + attackPower;
    int criticalDamage = static_cast<int>((baseDamage + attackPower) * criticalMultiplier);
    
    std::cout << "通常ダメージ: " << normalDamage << std::endl;
    std::cout << "クリティカルダメージ: " << criticalDamage << std::endl;
    
    // 経験値とレベルアップ
    int currentExp = 850;
    int expPerLevel = 100;
    int currentLevel = currentExp / expPerLevel;  // 8
    int expToNextLevel = expPerLevel - (currentExp % expPerLevel);  // 50
    
    std::cout << "\n現在のレベル: " << currentLevel << std::endl;
    std::cout << "次のレベルまで: " << expToNextLevel << " EXP" << std::endl;
    
    // アイテムドロップ率の計算
    int randomNumber = 73;  // 0-99の乱数と仮定
    int dropRate = 25;  // 25%の確率
    bool itemDropped = (randomNumber % 100) < dropRate;
    
    std::cout << "\nアイテムドロップ判定: " << (itemDropped ? "ドロップ！" : "ドロップなし") << std::endl;
    
    // 複合代入演算子
    std::cout << "\n=== 複合代入演算子 ===" << std::endl;
    int playerScore = 100;
    std::cout << "初期スコア: " << playerScore << std::endl;
    
    playerScore += 50;  // playerScore = playerScore + 50
    std::cout << "50点追加後: " << playerScore << std::endl;
    
    playerScore -= 30;  // playerScore = playerScore - 30
    std::cout << "30点減点後: " << playerScore << std::endl;
    
    playerScore *= 2;   // playerScore = playerScore * 2
    std::cout << "2倍ボーナス後: " << playerScore << std::endl;
    
    playerScore /= 4;   // playerScore = playerScore / 4
    std::cout << "4分の1に減少後: " << playerScore << std::endl;
    
    return 0;
}

/*
    演算子の優先順位:
    1. 括弧 ()
    2. 乗算 *, 除算 /, 剰余 %
    3. 加算 +, 減算 -
    
    注意点:
    - 整数同士の除算は整数になる（小数部分は切り捨て）
    - 0での除算はエラー（実行時エラー）
    - 剰余演算子 % は整数型でのみ使用可能
    
    ゲーム開発での活用:
    - ダメージ計算: 基本値 + ボーナス値
    - レベル計算: 経験値 / 必要経験値
    - 確率判定: 乱数 % 100 < 確率値
    - スコア計算: 基本点 * コンボ数
*/