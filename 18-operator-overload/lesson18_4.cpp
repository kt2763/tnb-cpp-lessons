#include <iostream>
using namespace std;

class Vector2
{
public:
    float x;
    float y;

    Vector2(float x, float y) : x(x), y(y) {}

    // メンバ関数版(vector * scalar)
    Vector2 operator*(float scalar) const
    {
        return Vector2(x * scalar, y * scalar);
    }

    // フレンド関数版(scalar * vector)
    friend Vector2 operator*(float scalar, const Vector2& vec)
    {
        return Vector2(vec.x * scalar, vec.y * scalar);
    }

    void print() const
    {
        cout << "(" << x << ", " << y << ")" << endl;
    }
};

int main()
{
    Vector2 v(1.0f, 2.0f);

    Vector2 result1 = v * 3.0f;    // メンバ関数が呼ばれる
    Vector2 result2 = 3.0f * v;    // フレンド関数が呼ばれる

    result1.print(); // (3, 6)
    result2.print(); // (3, 6)
}
