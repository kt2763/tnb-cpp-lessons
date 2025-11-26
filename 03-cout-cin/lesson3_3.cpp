#include <iostream>
#include <string>
using namespace std;

int main()
{
    string name;
    cout << "名前を入力: ";
    cin >> name; // スペース以降は切り捨て
    cout << "Hello, " << name << "\n";

    // こちらで書けばスペースありもいける
    cout << "フルネームを入力: ";
    getline(cin, name);
    cout << "Hello, " << name << "\n";
}
