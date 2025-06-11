// Lecture 7: 抽象クラスとインターフェース
// Abstract Classes and Interfaces in C++

#include <iostream>
#include <string>
#include <vector>
#include <memory>

// 抽象クラス：純粋仮想関数を持つクラス
class GameObject 
{
protected:
    std::string name;
    int x, y;  // 座標

public:
    GameObject(const std::string& n, int xPos, int yPos) 
        : name(n), x(xPos), y(yPos) {}
    
    // 純粋仮想関数 = 0で宣言
    virtual void update() = 0;  // 更新処理
    virtual void render() const = 0;  // 描画処理
    virtual std::string getType() const = 0;  // タイプ取得
    
    // 通常の仮想関数も持てる
    virtual ~GameObject() 
    {
        std::cout << name << "が破棄されました\n";
    }
    
    // 通常のメンバ関数
    void move(int dx, int dy) 
    {
        x += dx;
        y += dy;
    }
    
    std::string getName() const { return name; }
};

// インターフェースとして機能するクラス
class IDamageable 
{
public:
    virtual ~IDamageable() = default;
    virtual void takeDamage(int damage) = 0;
    virtual int getHealth() const = 0;
    virtual bool isAlive() const = 0;
};

// 複数のインターフェースを実装
class IInteractable 
{
public:
    virtual ~IInteractable() = default;
    virtual void interact() = 0;
    virtual std::string getInteractionText() const = 0;
};

// 具体的なクラス：プレイヤー
class Player : public GameObject, public IDamageable 
{
private:
    int health;
    int maxHealth;
    int level;
    
public:
    Player(const std::string& name, int x, int y) 
        : GameObject(name, x, y), health(100), maxHealth(100), level(1) {}
    
    // GameObjectの純粋仮想関数を実装
    void update() override 
    {
        // プレイヤーの更新処理
        std::cout << "プレイヤー " << name << " を更新中...\n";
    }
    
    void render() const override 
    {
        std::cout << "プレイヤー " << name << " を座標(" 
                  << x << ", " << y << ")に描画\n";
    }
    
    std::string getType() const override 
    {
        return "Player";
    }
    
    // IDamageableの実装
    void takeDamage(int damage) override 
    {
        health -= damage;
        if (health < 0) health = 0;
        std::cout << name << "は" << damage << "のダメージを受けた！ "
                  << "残りHP: " << health << "/" << maxHealth << "\n";
    }
    
    int getHealth() const override { return health; }
    
    bool isAlive() const override { return health > 0; }
    
    void levelUp() 
    {
        level++;
        maxHealth += 20;
        health = maxHealth;
        std::cout << name << "はレベル" << level << "になった！\n";
    }
};

// 具体的なクラス：敵
class Enemy : public GameObject, public IDamageable 
{
private:
    int health;
    int attackPower;
    
public:
    Enemy(const std::string& name, int x, int y, int hp, int atk)
        : GameObject(name, x, y), health(hp), attackPower(atk) {}
    
    void update() override 
    {
        // 敵のAI更新
        std::cout << "敵 " << name << " のAIを更新中...\n";
    }
    
    void render() const override 
    {
        std::cout << "敵 " << name << " を座標(" 
                  << x << ", " << y << ")に描画\n";
    }
    
    std::string getType() const override 
    {
        return "Enemy";
    }
    
    void takeDamage(int damage) override 
    {
        health -= damage;
        if (health < 0) health = 0;
        std::cout << name << "は" << damage << "のダメージ！ "
                  << "残りHP: " << health << "\n";
    }
    
    int getHealth() const override { return health; }
    bool isAlive() const override { return health > 0; }
    
    int getAttackPower() const { return attackPower; }
};

// 宝箱：インタラクト可能なオブジェクト
class TreasureChest : public GameObject, public IInteractable 
{
private:
    bool isOpen;
    std::string contents;
    
public:
    TreasureChest(int x, int y, const std::string& item)
        : GameObject("宝箱", x, y), isOpen(false), contents(item) {}
    
    void update() override 
    {
        // 宝箱は静的なので特に更新なし
    }
    
    void render() const override 
    {
        std::cout << (isOpen ? "開いた" : "閉じた") 
                  << "宝箱を座標(" << x << ", " << y << ")に描画\n";
    }
    
    std::string getType() const override 
    {
        return "TreasureChest";
    }
    
    void interact() override 
    {
        if (!isOpen) {
            isOpen = true;
            std::cout << "宝箱を開けた！ " << contents << "を手に入れた！\n";
        } else {
            std::cout << "この宝箱は既に開いている。\n";
        }
    }
    
    std::string getInteractionText() const override 
    {
        return isOpen ? "空の宝箱" : "宝箱を開ける";
    }
};

// ゲームワールドクラス
class GameWorld 
{
private:
    std::vector<std::unique_ptr<GameObject>> objects;
    
public:
    void addObject(std::unique_ptr<GameObject> obj) 
    {
        objects.push_back(std::move(obj));
    }
    
    void updateAll() 
    {
        std::cout << "\n=== ゲームワールド更新 ===\n";
        for (auto& obj : objects) {
            obj->update();
        }
    }
    
    void renderAll() const 
    {
        std::cout << "\n=== ゲームワールド描画 ===\n";
        for (const auto& obj : objects) {
            obj->render();
        }
    }
    
    // ダメージを与えられるオブジェクトを攻撃
    void damageAt(int x, int y, int damage) 
    {
        std::cout << "\n=== 座標(" << x << ", " << y << ")への攻撃 ===\n";
        for (auto& obj : objects) {
            // dynamic_castでIDamageableかチェック
            if (auto* damageable = dynamic_cast<IDamageable*>(obj.get())) {
                damageable->takeDamage(damage);
            }
        }
    }
};

int main() 
{
    std::cout << "=== 抽象クラスとインターフェースのデモ ===\n\n";
    
    // ゲームワールド作成
    GameWorld world;
    
    // ゲームオブジェクトを追加
    world.addObject(std::make_unique<Player>("勇者", 0, 0));
    world.addObject(std::make_unique<Enemy>("スライム", 5, 5, 30, 10));
    world.addObject(std::make_unique<Enemy>("ゴブリン", 10, 10, 50, 15));
    world.addObject(std::make_unique<TreasureChest>(15, 15, "伝説の剣"));
    
    // 全オブジェクトを更新・描画
    world.updateAll();
    world.renderAll();
    
    // ダメージテスト
    world.damageAt(5, 5, 25);
    
    // インターフェースの利点デモ
    std::cout << "\n=== インターフェースの利点 ===\n";
    
    // IDamageableのベクター
    std::vector<std::unique_ptr<IDamageable>> damageables;
    damageables.push_back(std::make_unique<Player>("戦士", 0, 0));
    damageables.push_back(std::make_unique<Enemy>("ドラゴン", 20, 20, 200, 50));
    
    // 全てのダメージ可能なオブジェクトに攻撃
    for (auto& d : damageables) {
        d->takeDamage(30);
        if (!d->isAlive()) {
            std::cout << "撃破！\n";
        }
    }
    
    return 0;
}

/*
学習ポイント：
1. 抽象クラス = 純粋仮想関数（= 0）を持つクラス
2. インスタンス化できない（new GameObject()は不可）
3. 派生クラスは全ての純粋仮想関数を実装する必要がある
4. インターフェース = 全てが純粋仮想関数のクラス（C++では慣例）
5. 多重継承でインターフェースを実装可能
6. dynamic_castでインターフェースの実装をチェック可能
*/