#include <iostream>

int main()
{
    const double PI = 3.141592; // 変更不可
    double radius = 5.0;        // 半径
    double area = PI * radius * radius;

    std::cout << "円の面積 = " << area << "\n";
    // PI = 4.0;  // ← コンパイルエラーになる例
    return 0;
}
