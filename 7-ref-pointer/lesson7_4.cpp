#include <iostream>
using namespace std;

int main()
{
    int x = 10;
    const int *p1 = &x; // 「const int を指すポインタ」= 中身の書換え禁止
    // *p1 = 20;              // エラー
    int *const p2 = &x; // 「constなポインタ」= ポインタ自体の付け替え禁止
    *p2 = 20;           // OK
    // p2 = &y;              // エラー
    const int *const p3 = &x; // 両方const
    // *p3 = 30;                 // エラー
    // p3 = &y;                  // エラー
}
