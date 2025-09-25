#include <iostream>
using namespace std;

int &bad_ref()
{
    int x = 10;
    return x; // ローカル変数の参照を返しているのでダングリングになる
}

int *bad_ptr()
{
    int x = 10;
    return &x; // ローカル変数のアドレスを返しているのでダングリングになる
}

int main()
{
    int &ref = bad_ref(); // ダングリング参照呼出し
    cout << ref << endl;  // 未定義動作

    int *ptr = bad_ptr(); // ダングリングポインタ呼出し
    cout << *ptr << endl; // 未定義動作
    return 0;
}
