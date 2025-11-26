#include <iostream>
using namespace std;

int main()
{
    int score;
    cout << "点数を入力: ";
    cin >> score;

    if (score >= 50 && score < 80)
    {
        cout << "追試ライン\n";
    }
}
