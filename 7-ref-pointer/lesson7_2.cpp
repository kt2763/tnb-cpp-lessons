#include <iostream>
using namespace std;

int main()
{
    int a = 10;
    int &r = a; // r は a の別名（参照）

    r = 99;                        // a が 99 に書き換わる
    cout << a << " " << r << "\n"; // 99 99

    // int& r2;         // ❌ 参照は必ず初期化が必要（未初期化はNG）
    // int& r3 = 5;     // ❌ 一時値には基本的に結べない（後述：const参照は可）
}
