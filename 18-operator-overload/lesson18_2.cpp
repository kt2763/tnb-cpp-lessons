#include <iostream>
#include <cmath>
using namespace std;

class Vector2
{
public:
    float x;
    float y;

    Vector2(float x, float y) : x(x), y(y) {}

    // 加算
    Vector2 operator+(const Vector2& other) const
    {
        return Vector2(x + other.x, y + other.y);
    }

    // 減算
    Vector2 operator-(const Vector2& other) const
    {
        return Vector2(x - other.x, y - other.y);
    }

    // スカラー倍(数値をかける)
    Vector2 operator*(float scalar) const
    {
        return Vector2(x * scalar, y * scalar);
    }

    // 等価比較
    bool operator==(const Vector2& other) const
    {
        return (x == other.x) && (y == other.y);
    }

    // 不等価比較
    bool operator!=(const Vector2& other) const
    {
        return !(*this == other); // ==の結果を反転
    }

    void print() const
    {
        cout << "(" << x << ", " << y << ")" << endl;
    }
};

int main()
{
    Vector2 v1(1.0f, 2.0f);
    Vector2 v2(3.0f, 4.0f);

    // 加算
    Vector2 sum = v1 + v2;
    sum.print(); // (4, 6)

    // 減算
    Vector2 diff = v2 - v1;
    diff.print(); // (2, 2)

    // スカラー倍
    Vector2 scaled = v1 * 2.0f;
    scaled.print(); // (2, 4)

    // 比較
    if (v1 == v2)
    {
        cout << "同じベクトルです" << endl;
    }
    else
    {
        cout << "異なるベクトルです" << endl; // こちらが表示される
    }
}
