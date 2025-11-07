#include <iostream>
using namespace std;

inline int add(int a, int b)
{
    return a + b;
}

int main()
{
    cout << add(1, 2) << "\n"; // ← ここに1 + 2が展開されるイメージ
    return 0;
}
