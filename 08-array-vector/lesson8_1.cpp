#include <iostream>
using namespace std;

int main()
{
    int scores[5] = {10, 20, 30, 40, 50};

    // 要素アクセス
    cout << scores[0] << "\n"; // 10
    cout << scores[4] << "\n"; // 50

    // ループで全要素表示
    for (int i = 0; i < 5; i++)
    {
        cout << scores[i] << " ";
    }
    cout << "\n";

    for (int score : scores)
    {
        cout << score << " ";
    }
    cout << "\n";
}
