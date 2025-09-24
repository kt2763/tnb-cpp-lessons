#include <iostream>
using namespace std;

int main()
{
    int x = 42;  // 変数x
    int *p = &x; // pはxのアドレスを保持, &はアドレス演算子

    cout << "xの値 = " << x << "\n"; // 42
    cout << "pの中身(アドレス) = " << p << "\n";
    cout << "*pの値 = " << *p << "\n"; // 42 (pが指す場所の中身)

    *p = 99;                               // pが指す場所の中身を変更 → xが99に書き換わる
    cout << "xの新しい値 = " << x << "\n"; // 99
}
