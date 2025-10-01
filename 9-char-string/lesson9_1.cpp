#include <iostream>
#include <cstring>
using namespace std;

int main()
{
    char str1[20] = "Hello"; // null文字を含む
    char str2[] = " World";  // null文字を含む

    strcat(str1, str2);           // 文字列結合
    cout << sizeof(str1) << "\n"; // 20
    cout << str1 << "\n";         // Hello World

    for (int i = 0; i < sizeof(str1); i++)
    {
        cout << i << ": " << str1[i] << "\n";
        cout << i << ": " << (int)str1[i] << "\n";
    }
}
