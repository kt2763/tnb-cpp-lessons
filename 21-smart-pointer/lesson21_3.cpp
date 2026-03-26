#include <iostream>
#include <memory>

class Enemy
{
public:
    std::string name;
    int hp;

    Enemy(std::string n, int h) : name(n), hp(h)
    {
        std::cout << name << " が出現！" << std::endl;
    }

    ~Enemy()
    {
        std::cout << name << " が消滅..." << std::endl;
    }
};

// ゲームの各システムが同じ敵を参照するイメージ
void renderSystem(std::shared_ptr<Enemy> enemy)
{
    std::cout << "[Render] " << enemy->name << " を描画 (参照数:" << enemy.use_count() << ")" << std::endl;
}

void physicsSystem(std::shared_ptr<Enemy> enemy)
{
    std::cout << "[Physics] " << enemy->name << " の当たり判定 (参照数:" << enemy.use_count() << ")" << std::endl;
}

int main()
{
    // make_shared で生成
    auto enemy = std::make_shared<Enemy>("ドラゴン", 500);
    std::cout << "参照数: " << enemy.use_count() << std::endl; // 1

    {
        // shared_ptr をコピーすると参照カウントが増える
        auto anotherRef = enemy;
        std::cout << "参照数: " << enemy.use_count() << std::endl; // 2

        renderSystem(enemy);  // 参照数が一時的に増える
        physicsSystem(enemy);
    }
    // anotherRef がスコープを抜けると参照数が減る
    std::cout << "参照数: " << enemy.use_count() << std::endl; // 1

    // enemy がスコープを抜けると参照数が0になって自動解放
    return 0;
}
