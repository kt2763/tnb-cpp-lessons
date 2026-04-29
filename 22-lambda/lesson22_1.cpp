#include <iostream>

int add_normal(int a, int b)
{
    return a + b;
}

int main()
{
    // --- 通常の関数 ---
    std::cout << add_normal(3, 5) << std::endl; // 8

    // --- ラムダ式：変数に代入して使う ---
    auto add = [](int a, int b) -> int
        {
            return a + b;
        };

    std::cout << add(3, 5) << std::endl; // 8

    // --- 戻り値型は省略できる ---
    auto greet = [](const std::string& name)
        {
            std::cout << "こんにちは、" << name << "さん！" << std::endl;
        };

    greet("たなべ");

    // --- その場で呼び出す（即時実行ラムダ）---
    int result = [](int x) { return x * x; }(5);
    std::cout << "5の二乗: " << result << std::endl; // 25

    return 0;
}
