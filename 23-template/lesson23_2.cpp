#include <iostream>
#include <string>

// ✅ テンプレートで一本化
template<typename T>
void print_value(const T& value)
{
    std::cout << "値: " << value << std::endl;
}

// 複数の型パラメータ
template<typename T>
T clamp(T value, T min_val, T max_val)
{
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

// 汎用 swap（標準の std::swap も同じ仕組み）
template<typename T>
void my_swap(T& a, T& b)
{
    T temp = a;
    a = b;
    b = temp;
}

int main()
{
    // 型推論で呼び出す
    print_value(42);
    print_value(3.14);
    print_value(std::string("ゴブリン"));

    // 明示的に型を指定することもできる
    print_value<int>(100);

    // clamp：HPを0〜100の範囲に収める
    int player_hp = clamp(120, 0, 100);
    std::cout << "HP: " << player_hp << std::endl; // 100

    // swap
    std::string a = "勇者";
    std::string b = "魔法使い";
    my_swap(a, b);
    std::cout << a << " / " << b << std::endl; // 魔法使い / 勇者

    return 0;
}
