// 第5講: リテラル
// プログラム中に直接書かれる値

#include <iostream>
#include <iomanip>  // 出力フォーマット用

int main() 
{
    std::cout << "=== 整数リテラル ===" << std::endl;
    
    // 10進数リテラル
    int decimal = 100;
    std::cout << "10進数: " << decimal << std::endl;
    
    // 8進数リテラル（0で始まる）
    int octal = 0144;  // 8進数で144 = 10進数で100
    std::cout << "8進数 0144 = " << octal << std::endl;
    
    // 16進数リテラル（0xで始まる）
    int hexadecimal = 0x64;  // 16進数で64 = 10進数で100
    std::cout << "16進数 0x64 = " << hexadecimal << std::endl;
    
    // 2進数リテラル（C++14以降、0bで始まる）
    int binary = 0b1100100;  // 2進数で1100100 = 10進数で100
    std::cout << "2進数 0b1100100 = " << binary << std::endl;
    
    // ゲームでの使用例：ビットフラグ
    std::cout << "\n=== ビットフラグの例 ===" << std::endl;
    const int ITEM_EQUIPPED = 0b00000001;  // 装備中
    const int ITEM_CURSED   = 0b00000010;  // 呪われている
    const int ITEM_RARE     = 0b00000100;  // レアアイテム
    const int ITEM_QUEST    = 0b00001000;  // クエストアイテム
    
    int swordStatus = ITEM_EQUIPPED | ITEM_RARE;  // 装備中のレアアイテム
    std::cout << "剣のステータス: " << std::bitset<8>(swordStatus) << std::endl;
    
    // 浮動小数点リテラル
    std::cout << "\n=== 浮動小数点リテラル ===" << std::endl;
    
    float damageFloat = 10.5f;     // f接尾辞でfloat型
    double damageDouble = 10.5;     // 接尾辞なしはdouble型
    long double damageLong = 10.5L; // L接尾辞でlong double型
    
    std::cout << "float型: " << damageFloat << "f" << std::endl;
    std::cout << "double型: " << damageDouble << std::endl;
    std::cout << "long double型: " << damageLong << "L" << std::endl;
    
    // 指数表記
    double largeNumber = 1.5e6;   // 1.5 × 10^6 = 1,500,000
    double smallNumber = 3.2e-4;  // 3.2 × 10^-4 = 0.00032
    
    std::cout << "\n大きな数（経験値）: " << largeNumber << std::endl;
    std::cout << "小さな数（確率）: " << smallNumber << std::endl;
    
    // 文字リテラル
    std::cout << "\n=== 文字リテラル ===" << std::endl;
    
    char grade = 'A';
    char newline = '\n';
    char tab = '\t';
    
    std::cout << "ランク: " << grade << std::endl;
    std::cout << "特殊文字の例:" << newline;
    std::cout << "項目1" << tab << "項目2" << tab << "項目3" << std::endl;
    
    // 文字列リテラル
    std::cout << "\n=== 文字列リテラル ===" << std::endl;
    
    const char* gameTitle = "勇者の冒険";
    std::string questName = "魔王討伐";
    
    std::cout << "ゲームタイトル: " << gameTitle << std::endl;
    std::cout << "クエスト名: " << questName << std::endl;
    
    // Raw文字列リテラル（C++11以降）
    std::string filePath = R"(C:\Games\RPG\save\player.dat)";
    std::string asciiArt = R"(
    /\_/\  
   ( o.o ) 
    > ^ <  
    )";
    
    std::cout << "\nファイルパス: " << filePath << std::endl;
    std::cout << "アスキーアート:" << asciiArt << std::endl;
    
    // ユーザー定義リテラル（C++11以降）
    std::cout << "\n=== 数値区切り（C++14以降） ===" << std::endl;
    
    long long goldAmount = 1'000'000;      // 100万ゴールド
    int binaryMask = 0b1111'0000'1111'0000;  // 見やすいビットパターン
    
    std::cout << "所持金: " << goldAmount << " ゴールド" << std::endl;
    std::cout << "ビットマスク: " << std::hex << binaryMask << std::dec << std::endl;
    
    // nullptr リテラル（C++11以降）
    int* noWeapon = nullptr;  // 武器を装備していない
    
    if (noWeapon == nullptr) 
    {
        std::cout << "\n武器を装備していません！" << std::endl;
    }
    
    return 0;
}

/*
    リテラルの種類:
    
    整数リテラル:
    - 10進数: 123
    - 8進数: 0123 (0で始まる)
    - 16進数: 0x7B (0xで始まる)
    - 2進数: 0b1111011 (0bで始まる、C++14以降)
    
    浮動小数点リテラル:
    - float: 1.23f
    - double: 1.23 (デフォルト)
    - long double: 1.23L
    
    文字リテラル:
    - 通常文字: 'A'
    - エスケープシーケンス: '\n', '\t', '\\'
    
    文字列リテラル:
    - 通常: "Hello"
    - Raw文字列: R"(Hello\nWorld)" (エスケープ不要)
    
    その他:
    - bool: true, false
    - ポインタ: nullptr (C++11以降)
    
    ゲーム開発での活用:
    - 16進数: カラーコード (0xFF0000 = 赤)
    - 2進数: ビットフラグ、権限管理
    - 指数表記: 大きな数値（経験値など）
    - Raw文字列: ファイルパス、SQL文
*/