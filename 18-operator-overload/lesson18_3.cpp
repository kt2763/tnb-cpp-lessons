#include <iostream>
using namespace std;

class Vector2
{
public:
    float x;
    float y;

    Vector2(float x, float y) : x(x), y(y) {}

    // +=演算子(自分自身を変更する)
    Vector2& operator+=(const Vector2& other)
    {
        x += other.x;
        y += other.y;
        return *this; // 自分自身への参照を返す
    }

    // -=演算子
    Vector2& operator-=(const Vector2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    void print() const
    {
        cout << "(" << x << ", " << y << ")" << endl;
    }
};

int main()
{
    Vector2 pos(10.0f, 20.0f);
    Vector2 velocity(1.0f, 2.0f);

    pos += velocity; // posが更新される
    pos.print(); // (11, 22)

    pos -= velocity;
    pos.print(); // (10, 20)
}
