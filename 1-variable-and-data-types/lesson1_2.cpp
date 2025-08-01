#include <iostream>

int main()
{
    int height = 182;         // 整数
    double weight = 79.5;     // 誤差前提の浮動小数点数
    char initialLetter = 'T'; // 文字
    bool isMarried = true;    // 真偽値

    std::cout << "身長: " << height << "cm、体重は" << weight << "kgです。\n";
    std::cout << "頭文字は'" << initialLetter << "'です。\n";
    std::cout << "結婚しているか → " << std::boolalpha << isMarried << "\n";
    return 0;
}
