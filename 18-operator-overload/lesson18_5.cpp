#include <iostream>
using namespace std;

class Vector2
{
public:
    float x;
    float y;

    Vector2(float x, float y) : x(x), y(y) {}

    // <<演算子のオーバーロード(フレンド関数として定義)
    friend ostream& operator<<(ostream& os, const Vector2& vec)
    {
        os << "(" << vec.x << ", " << vec.y << ")";
        return os;
    }
};

int main()
{
    Vector2 pos(10.0f, 20.0f);
    Vector2 velocity(1.5f, 2.5f);

    // coutで直接出力できる
    cout << "位置: " << pos << endl;
    cout << "速度: " << velocity << endl;

    // 連続して出力もできる
    cout << "位置: " << pos << ", 速度: " << velocity << endl;
}
