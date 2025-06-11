// 第2講: 変数とデータ型
// 変数は、データを保存するための「箱」のようなもの

#include <iostream>

int main() 
{
    // 整数型（int）: 整数を保存する
    // ゲームでは、プレイヤーのレベルやスコアなどに使用
    int playerLevel = 1;
    int playerHP = 100;
    int enemyCount = 5;
    
    // 浮動小数点型（double, float）: 小数を保存する
    // ゲームでは、座標やダメージ倍率などに使用
    double playerPositionX = 10.5;
    double playerPositionY = 20.3;
    float damageMultiplier = 1.5f;  // f は float型を示す
    
    // 文字型（char）: 1文字を保存する
    char playerRank = 'A';
    char enemyType = 'B';  // B = ボス
    
    // 真偽値型（bool）: true（真）かfalse（偽）を保存する
    // ゲームでは、フラグ管理に使用
    bool isGameOver = false;
    bool hasKey = true;
    bool canJump = true;
    
    // 変数の値を表示
    std::cout << "=== ゲームステータス ===" << std::endl;
    std::cout << "プレイヤーレベル: " << playerLevel << std::endl;
    std::cout << "プレイヤーHP: " << playerHP << std::endl;
    std::cout << "現在位置: (" << playerPositionX << ", " << playerPositionY << ")" << std::endl;
    std::cout << "ランク: " << playerRank << std::endl;
    std::cout << "鍵を持っている: " << (hasKey ? "はい" : "いいえ") << std::endl;
    
    // 変数の値を変更
    playerLevel = 2;  // レベルアップ！
    playerHP = playerHP - 30;  // ダメージを受けた
    hasKey = false;  // 鍵を使った
    
    std::cout << "\n=== 戦闘後のステータス ===" << std::endl;
    std::cout << "プレイヤーレベル: " << playerLevel << std::endl;
    std::cout << "プレイヤーHP: " << playerHP << std::endl;
    std::cout << "鍵を持っている: " << (hasKey ? "はい" : "いいえ") << std::endl;
    
    return 0;
}

/*
    データ型の選び方:
    - int: 整数値（プレイヤーのレベル、アイテム数など）
    - double/float: 小数値（座標、確率など）
    - char: 1文字（ランク、方向キーなど）
    - bool: 真偽値（フラグ、状態管理など）
    
    変数名のルール:
    1. 意味のある名前をつける（playerHP は hp より分かりやすい）
    2. 小文字で始める（キャメルケース推奨）
    3. 数字で始めることはできない
    4. 特殊文字は使えない（_ は使用可能）
*/