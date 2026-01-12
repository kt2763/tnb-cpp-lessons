class Enemy
{
public:
    int hp;

    // デフォルトコンストラクタ
    Enemy()
    {
        hp = 100;
    }

    // 引数付きコンストラクタ
    Enemy(int h)
    {
        hp = h;
    }
};

int main()
{
    Enemy enemy1;      // デフォルトコンストラクタが呼ばれる
    Enemy enemy2(200); // 引数付きコンストラクタが呼ばれる
}
