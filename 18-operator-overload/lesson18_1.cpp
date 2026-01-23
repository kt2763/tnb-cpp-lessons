#include <iostream>
using namespace std;

// 2D座標を表すクラス
class Vector2
{
public:
    float x;
    float y;

    // コンストラクタ
    Vector2(float x, float y) : x(x), y(y) {}

    // +演算子のオーバーロード
    Vector2 operator+(const Vector2& other) const
    {
        return Vector2(x + other.x, y + other.y);
    }

    // 表示用の関数
    void print() const
    {
        cout << "(" << x << ", " << y << ")" << endl;
    }
};

int main()
{
    Vector2 pos1(1.0f, 2.0f);
    Vector2 pos2(3.0f, 4.0f);

    Vector2 result = pos1 + pos2; // operator+が呼ばれる
    result.print(); // (4, 6)
}
