#include <iostream>
#include <string>

// ❌ 型ごとに同じ関数を書くのは冗長
void print_value_int(int value)
{
    std::cout << "値: " << value << std::endl;
}

void print_value_double(double value)
{
    std::cout << "値: " << value << std::endl;
}

void print_value_string(const std::string& value)
{
    std::cout << "値: " << value << std::endl;
}

int main()
{
    print_value_int(42);
    print_value_double(3.14);
    print_value_string("ゴブリン");

    // ロジックは同じなのに3つも関数が必要→テンプレートで解決できる
    return 0;
}
