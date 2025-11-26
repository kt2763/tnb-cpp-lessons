#include <iostream>
using namespace std;

int main()
{
    int score = 75;
    string result = (score >= 60) ? "合格" : "不合格";
    cout << "結果: " << result << "\n";
}
