#include <iostream>

int main()
{
    // スタックメモリ - 自動的に確保・解放される
    // プレイヤーのHPみたいな小さいデータに向いている
    int playerHP = 100;

    std::cout << "プレイヤーHP(スタック): " << playerHP << std::endl;

    // ヒープメモリは次のセクションで説明します
    // 敵キャラのような大きなデータや、数が変動するものに使います

    return 0;
}
// ここで playerHP は自動的に解放される
