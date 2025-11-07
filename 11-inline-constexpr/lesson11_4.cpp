#include <iostream>
using namespace std;

constexpr float PI = 3.1415926535f;
constexpr int MAX_ENEMY = 32;

// 度をラジアンに変換
inline float Deg2Rad(float deg)
{
    return deg * PI / 180.0f;
}

int main()
{
    cout << Deg2Rad(30.0f) << "\n";
    return 0;
}
