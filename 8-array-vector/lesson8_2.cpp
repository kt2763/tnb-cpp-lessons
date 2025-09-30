#include <iostream>
#include <vector>
using namespace std;

int main()
{
    vector<int> scores = {10, 20, 30};

    scores.push_back(40); // 末尾に追加
    scores.push_back(50);

    for (int i = 0; i < scores.size(); i++)
    {
        cout << scores[i] << " "; // 添字アクセス
    }
    cout << "\n";

    for (int score : scores)
    {
        cout << score << " ";
    }
    cout << "\n";

    // 安全アクセス
    cout << scores.at(2) << "\n"; // 30
}
