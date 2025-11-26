#include <iostream>
using namespace std;

void by_value(int v) { v = 100; } // コピーを変更（元は変わらない）
void by_ref(int &r) { r = 100; }  // 元を変更
void by_const_ref(const int &r) { /* 読み取り専用, rを変えようとするエラー */ }
void by_ptr(int *p)
{
    if (p)
    {
        *p = 100;
    }
} // pが非nullなら元を変更

int main()
{
    int a = 1, b = 1, c = 1, d = 1;

    by_value(a);     // 変わらない
    by_ref(b);       // 100
    by_const_ref(c); // 読み取りのみ
    by_ptr(&d);      // 100

    cout << a << " " << b << " " << c << " " << d << "\n";
}
