#include <iostream>
using namespace std;

class Enemy
{
public:
    string name;

    // コンストラクタ
    Enemy(string n) : name(n)
    {
        cout << name << "が出現しました!" << endl;
    }

    // デストラクタ(~をつける、引数なし、戻り値なし)
    ~Enemy()
    {
        cout << name << "が倒されました!" << endl;
    }
};

int main()
{
    Enemy slime("スライム");
    // ここでスコープを抜けるとデストラクタが呼ばれる
}
