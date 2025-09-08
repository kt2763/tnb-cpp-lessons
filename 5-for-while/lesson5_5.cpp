#include <iostream>
using namespace std;

int main()
{
    for (int i = 0; i < 10; i++)
    {
        if (i == 5)
            break; // 5でループ終了
        if (i % 2 == 0)
            continue; // 偶数をスキップ
        cout << i << "\n";
    }

    return 0;
}
