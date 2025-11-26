#include <iostream>
using namespace std;

int main()
{
    int a = 10;
    int *p = &a; // p は a のアドレスを保持

    *p = 77;                        // アドレス経由で a を書き換え
    cout << a << " " << *p << "\n"; // 77 77

    p = nullptr; // 何も指さない状態
    // *p = 1;     // ❌ 実行時クラッシュの危険（ヌルポインタ参照）
}
