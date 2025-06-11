// 第1講: 条件演算子（三項演算子）
// if文の代替として使える条件演算子の使い方を学ぶ

#include <iostream>
#include <string>

int main() 
{
    std::cout << "=== 条件演算子の基本 ===" << std::endl;
    
    // 基本的な条件演算子
    int playerLevel = 8;
    std::string status = (playerLevel >= 10) ? "上級者" : "初心者";
    std::cout << "プレイヤーのステータス: " << status << std::endl;
    
    // 数値の代入での使用
    int enemyHP = 50;
    int damage = 30;
    int remainingHP = (enemyHP > damage) ? (enemyHP - damage) : 0;
    std::cout << "敵の残りHP: " << remainingHP << std::endl;
    
    // ネストした条件演算子
    int score = 85;
    std::string rank = (score >= 90) ? "S" : 
                      (score >= 80) ? "A" : 
                      (score >= 70) ? "B" : 
                      (score >= 60) ? "C" : "D";
    std::cout << "スコアランク: " << rank << std::endl;
    
    std::cout << "\n=== ゲーム開発での使用例 ===" << std::endl;
    
    // クリティカルダメージの計算
    int baseDamage = 100;
    bool isCritical = true;
    double criticalMultiplier = 1.5;
    int finalDamage = isCritical ? (baseDamage * criticalMultiplier) : baseDamage;
    std::cout << "与えたダメージ: " << finalDamage << (isCritical ? " (クリティカル！)" : "") << std::endl;
    
    // アイテムドロップ率の表示
    int luckStat = 15;
    double dropRate = 0.1;  // 基本ドロップ率10%
    double finalDropRate = (luckStat > 10) ? (dropRate * 1.5) : dropRate;
    std::cout << "アイテムドロップ率: " << (finalDropRate * 100) << "%" << std::endl;
    
    // 移動速度の計算
    bool isRunning = true;
    bool hasSpeedBoost = true;
    int baseSpeed = 5;
    int moveSpeed = hasSpeedBoost ? (isRunning ? baseSpeed * 3 : baseSpeed * 1.5) : 
                                   (isRunning ? baseSpeed * 2 : baseSpeed);
    std::cout << "現在の移動速度: " << moveSpeed << std::endl;
    
    // ショップでの価格計算
    int itemPrice = 1000;
    int playerGold = 1500;
    bool hasDiscount = true;
    int finalPrice = hasDiscount ? (itemPrice * 0.8) : itemPrice;
    std::string canBuy = (playerGold >= finalPrice) ? "購入可能" : "ゴールド不足";
    std::cout << "アイテム価格: " << finalPrice << "G (" << canBuy << ")" << std::endl;
    
    // 表示メッセージの切り替え
    int questProgress = 3;
    int questTotal = 5;
    std::string questStatus = (questProgress >= questTotal) ? "クエスト完了！" : 
                             "進行中: " + std::to_string(questProgress) + "/" + std::to_string(questTotal);
    std::cout << questStatus << std::endl;
    
    return 0;
}

/*
    まとめ:
    - 条件演算子（? :）は、簡単な条件分岐を1行で書ける便利な演算子
    - 構文: 条件式 ? 真の場合の値 : 偽の場合の値
    - if-else文より簡潔に書けるが、複雑な処理には不向き
    - ゲーム開発では、ダメージ計算、ステータス表示、価格計算などで活用
    - ネストも可能だが、可読性を考慮して使用すること
    - 戻り値が必要な場面で特に有効（変数への代入、関数の引数など）
*/