#include <iostream>
using namespace std;

// 関数の宣言
int add(int a, int b)
{
    return a + b;
}

int main()
{
    int sum = add(3, 5); // 関数を呼び出す
    cout << "合計 = " << sum << "\n";
}
