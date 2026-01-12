#include <iostream>
using namespace std;

// 敵キャラクターのクラス
class Enemy
{
public:
    int hp;

    // コンストラクタ(クラス名と同じ名前、戻り値なし)
    Enemy()
    {
        hp = 100; // HPを初期化
        cout << "敵が生成されました! HP: " << hp << endl;
    }
};

int main()
{
    Enemy enemy1; // このタイミングでコンストラクタが呼ばれる
    Enemy enemy2; // 別のインスタンスでも呼ばれる
}
