#include <iostream>
using namespace std;

void ShowScore(int score, string name = "Player")
{
    cout << name << " のスコア: " << score << "\n";
}

void ShowScore(double score, string name = "Player")
{
    cout << name << " のスコア(小数): " << score << "\n";
}

int main()
{
    ShowScore(100);
    ShowScore(98.5, "Tanabe");
}
