#include <iostream>
using namespace std;

int main()
{
    int age;
    cout << "年齢を入力: ";
    cin >> age;
    cout << "あなたの年齢は " << age << " 歳です\n";

    // 複数入力
    int a, b;
    cout << "2つの整数を入力: ";
    cin >> a >> b;
    cout << "合計 = " << a + b << "\n";
}
