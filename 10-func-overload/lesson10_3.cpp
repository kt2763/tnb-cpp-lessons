#include <iostream>
using namespace std;

void f(long x) { cout << "long\n"; }
void f(float x) { cout << "float\n"; }

int main()
{
    f(10); // int → long も int → float もどちらも「標準変換」レベル → 曖昧エラー
}
