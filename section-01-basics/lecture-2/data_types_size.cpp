// 第2講: データ型のサイズと範囲
// 各データ型が使用するメモリサイズと値の範囲を理解する

#include <iostream>
#include <limits>  // 数値型の限界値を調べるため

int main() 
{
    std::cout << "=== C++の基本データ型のサイズ ===" << std::endl;
    
    // sizeof演算子でデータ型のサイズ（バイト数）を調べる
    std::cout << "char のサイズ: " << sizeof(char) << " バイト" << std::endl;
    std::cout << "short のサイズ: " << sizeof(short) << " バイト" << std::endl;
    std::cout << "int のサイズ: " << sizeof(int) << " バイト" << std::endl;
    std::cout << "long のサイズ: " << sizeof(long) << " バイト" << std::endl;
    std::cout << "long long のサイズ: " << sizeof(long long) << " バイト" << std::endl;
    std::cout << "float のサイズ: " << sizeof(float) << " バイト" << std::endl;
    std::cout << "double のサイズ: " << sizeof(double) << " バイト" << std::endl;
    std::cout << "bool のサイズ: " << sizeof(bool) << " バイト" << std::endl;
    
    std::cout << "\n=== 整数型の範囲 ===" << std::endl;
    
    // 各整数型の最小値と最大値
    std::cout << "int の範囲: " 
              << std::numeric_limits<int>::min() << " ～ "
              << std::numeric_limits<int>::max() << std::endl;
    
    std::cout << "unsigned int の範囲: " 
              << std::numeric_limits<unsigned int>::min() << " ～ "
              << std::numeric_limits<unsigned int>::max() << std::endl;
    
    // ゲーム開発での実用例
    std::cout << "\n=== ゲーム開発での使い分け例 ===" << std::endl;
    
    // 小さな値には小さな型を使ってメモリを節約
    unsigned char monsterLevel = 99;  // 0～255で十分
    short playerGold = 9999;  // -32,768～32,767
    int playerExperience = 1000000;  // 大きな値が必要
    unsigned int playerID = 12345678;  // 負の値は不要
    
    std::cout << "モンスターレベル (unsigned char): " << static_cast<int>(monsterLevel) << std::endl;
    std::cout << "所持金 (short): " << playerGold << std::endl;
    std::cout << "経験値 (int): " << playerExperience << std::endl;
    std::cout << "プレイヤーID (unsigned int): " << playerID << std::endl;
    
    // オーバーフローの危険性
    std::cout << "\n=== オーバーフローの例 ===" << std::endl;
    unsigned char maxLevel = 255;
    std::cout << "最大レベル: " << static_cast<int>(maxLevel) << std::endl;
    maxLevel = maxLevel + 1;  // オーバーフロー！
    std::cout << "レベル+1後: " << static_cast<int>(maxLevel) << " (0に戻る！)" << std::endl;
    
    return 0;
}

/*
    型選択のガイドライン:
    
    1. メモリ使用量を考慮する
       - 大量のデータを扱う場合は、適切なサイズの型を選ぶ
       - 例: 1000体の敵のレベル → char型で十分（1バイト×1000）
    
    2. 値の範囲を考慮する
       - プレイヤーのHP: 0～9999 → unsigned short
       - ゲーム内通貨: 大きな値 → unsigned int または long long
    
    3. 符号の有無を考慮する
       - 負の値が不要な場合は unsigned を使う
       - 例: プレイヤーID、アイテム個数など
    
    4. 浮動小数点の精度
       - float: 約7桁の精度（ゲームの座標など）
       - double: 約15桁の精度（物理計算など）
*/