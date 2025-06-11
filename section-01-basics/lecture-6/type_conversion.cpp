// 第6講: 型変換
// 異なるデータ型間での値の変換

#include <iostream>
#include <string>

int main() 
{
    std::cout << "=== 暗黙的な型変換（自動変換） ===" << std::endl;
    
    // 小さい型から大きい型への変換（安全）
    int playerLevel = 42;
    double levelAsDouble = playerLevel;  // int → double（自動変換）
    
    std::cout << "レベル（int）: " << playerLevel << std::endl;
    std::cout << "レベル（double）: " << levelAsDouble << std::endl;
    
    // 演算時の暗黙的変換
    int damage = 100;
    double multiplier = 1.5;
    double totalDamage = damage * multiplier;  // int が double に変換される
    
    std::cout << "\nダメージ計算: " << damage << " × " << multiplier 
              << " = " << totalDamage << std::endl;
    
    // 精度の損失を伴う暗黙的変換（注意が必要）
    double playerPositionX = 10.7;
    int gridX = playerPositionX;  // 小数部分が切り捨てられる！
    
    std::cout << "\n実際の位置: " << playerPositionX << std::endl;
    std::cout << "グリッド位置: " << gridX << " (小数部分が失われた！)" << std::endl;
    
    std::cout << "\n=== 明示的な型変換（キャスト） ===" << std::endl;
    
    // static_cast（推奨される方法）
    double expRate = 0.75;
    int expPercentage = static_cast<int>(expRate * 100);
    
    std::cout << "経験値獲得率: " << expPercentage << "%" << std::endl;
    
    // ゲームでの実用例：ダメージ計算
    double baseDamage = 50.0;
    double criticalMultiplier = 2.5;
    int finalDamage = static_cast<int>(baseDamage * criticalMultiplier);
    
    std::cout << "\n基本ダメージ: " << baseDamage << std::endl;
    std::cout << "クリティカル倍率: " << criticalMultiplier << std::endl;
    std::cout << "最終ダメージ: " << finalDamage << std::endl;
    
    // 文字と数値の変換
    std::cout << "\n=== 文字と数値の変換 ===" << std::endl;
    
    char rankChar = 'A';
    int rankValue = static_cast<int>(rankChar);  // ASCII値
    
    std::cout << "ランク: " << rankChar << std::endl;
    std::cout << "ASCII値: " << rankValue << std::endl;
    
    // 数値を文字に変換
    int scoreRank = 65;  // 'A'のASCII値
    char gradeChar = static_cast<char>(scoreRank);
    
    std::cout << "\nスコア値: " << scoreRank << std::endl;
    std::cout << "評価: " << gradeChar << std::endl;
    
    // 文字列と数値の変換（C++11以降）
    std::cout << "\n=== 文字列と数値の変換 ===" << std::endl;
    
    // 数値から文字列へ
    int playerScore = 12500;
    std::string scoreText = std::to_string(playerScore);
    std::cout << "スコア表示: " << scoreText << " points" << std::endl;
    
    // 文字列から数値へ
    std::string inputLevel = "25";
    int convertedLevel = std::stoi(inputLevel);
    std::cout << "入力されたレベル: " << convertedLevel << std::endl;
    
    // 浮動小数点数の変換
    std::string damageText = "123.45";
    double convertedDamage = std::stod(damageText);
    std::cout << "ダメージ値: " << convertedDamage << std::endl;
    
    // 型変換の実践例
    std::cout << "\n=== ゲームでの型変換実践例 ===" << std::endl;
    
    // HPバーの表示（割合を計算）
    int currentHP = 75;
    int maxHP = 120;
    double hpRatio = static_cast<double>(currentHP) / maxHP;
    int hpBarLength = static_cast<int>(hpRatio * 20);  // 20文字分のバー
    
    std::cout << "HP: " << currentHP << "/" << maxHP << std::endl;
    std::cout << "HP: [";
    for (int i = 0; i < 20; ++i) 
    {
        if (i < hpBarLength) 
        {
            std::cout << "=";
        }
        else 
        {
            std::cout << " ";
        }
    }
    std::cout << "] " << static_cast<int>(hpRatio * 100) << "%" << std::endl;
    
    // Cスタイルのキャスト（非推奨）
    std::cout << "\n=== Cスタイルキャスト（非推奨） ===" << std::endl;
    
    double oldStyleValue = 3.14;
    int oldStyleInt = (int)oldStyleValue;  // 古い方法
    
    std::cout << "元の値: " << oldStyleValue << std::endl;
    std::cout << "変換後: " << oldStyleInt << " (Cスタイル - 使わない方が良い)" << std::endl;
    
    return 0;
}

/*
    型変換の種類:
    
    1. 暗黙的変換（自動変換）:
       - 安全な方向（小→大）は自動
       - データ損失の可能性がある場合も自動（注意！）
    
    2. 明示的変換（キャスト）:
       - static_cast<型>(値) : 最も一般的（推奨）
       - dynamic_cast : ポインタの安全な変換
       - const_cast : const属性の除去
       - reinterpret_cast : 低レベル変換
    
    文字列変換関数（C++11以降）:
    - std::to_string() : 数値→文字列
    - std::stoi() : 文字列→int
    - std::stod() : 文字列→double
    - std::stof() : 文字列→float
    
    注意点:
    - 精度の損失（double→int）
    - オーバーフロー（大きい値→小さい型）
    - 符号の問題（unsigned→signed）
    
    ゲーム開発での使用:
    - ダメージ計算の端数処理
    - UI表示用の文字列変換
    - 座標の整数化（グリッド配置）
    - パーセンテージ表示
*/