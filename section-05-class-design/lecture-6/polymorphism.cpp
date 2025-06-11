// 第6講: ポリモーフィズム
// 仮想関数と純粋仮想関数による多態性の実装と活用を学ぶ

#include <iostream>
#include <vector>
#include <memory>

// === 抽象基底クラス：ゲームオブジェクト ===
class GameObject 
{
protected:
    std::string name;
    double x, y;
    bool active;

public:
    GameObject(const std::string& objName, double posX = 0, double posY = 0) 
        : name(objName), x(posX), y(posY), active(true) {}
    
    virtual ~GameObject() = default;
    
    // 純粋仮想関数（抽象メソッド）
    virtual void update(double deltaTime) = 0;
    virtual void render() const = 0;
    virtual void onCollision(GameObject* other) = 0;
    
    // 通常の仮想関数
    virtual void setPosition(double newX, double newY) 
    {
        x = newX;
        y = newY;
    }
    
    // 非仮想関数
    const std::string& getName() const { return name; }
    double getX() const { return x; }
    double getY() const { return y; }
    bool isActive() const { return active; }
    void setActive(bool state) { active = state; }
};

// === 派生クラス1：プレイヤー ===
class Player : public GameObject 
{
private:
    int health;
    int maxHealth;
    double speed;

public:
    Player(const std::string& playerName, double posX = 0, double posY = 0) 
        : GameObject(playerName, posX, posY), health(100), maxHealth(100), speed(5.0) {}
    
    void update(double deltaTime) override 
    {
        std::cout << "プレイヤー「" << name << "」を更新中..." << std::endl;
        // プレイヤー固有の更新処理
    }
    
    void render() const override 
    {
        std::cout << "プレイヤー「" << name << "」を描画: (" << x << ", " << y << ")" << std::endl;
    }
    
    void onCollision(GameObject* other) override 
    {
        std::cout << "プレイヤー「" << name << "」が「" << other->getName() << "」と衝突！" << std::endl;
    }
    
    // プレイヤー固有のメソッド
    void takeDamage(int damage) 
    {
        health -= damage;
        if (health < 0) health = 0;
        std::cout << name << "は" << damage << "のダメージを受けた！（HP: " << health << "）" << std::endl;
    }
    
    int getHealth() const { return health; }
    bool isAlive() const { return health > 0; }
};

// === 派生クラス2：敵（基底クラス） ===
class Enemy : public GameObject 
{
protected:
    int health;
    int attackPower;
    double attackRange;

public:
    Enemy(const std::string& enemyName, double posX, double posY, int hp, int attack) 
        : GameObject(enemyName, posX, posY), health(hp), attackPower(attack), attackRange(2.0) {}
    
    void update(double deltaTime) override 
    {
        std::cout << "敵「" << name << "」のAI更新中..." << std::endl;
        // 基本的なAI処理
    }
    
    void render() const override 
    {
        std::cout << "敵「" << name << "」を描画: (" << x << ", " << y << ")" << std::endl;
    }
    
    void onCollision(GameObject* other) override 
    {
        std::cout << "敵「" << name << "」が「" << other->getName() << "」と衝突！" << std::endl;
    }
    
    // 仮想攻撃メソッド（派生クラスでオーバーライド可能）
    virtual void attack(Player* target) 
    {
        std::cout << name << "の基本攻撃！" << std::endl;
        target->takeDamage(attackPower);
    }
    
    int getHealth() const { return health; }
    int getAttackPower() const { return attackPower; }
    
    void takeDamage(int damage) 
    {
        health -= damage;
        if (health <= 0) 
        {
            health = 0;
            setActive(false);
            std::cout << name << "を倒した！" << std::endl;
        }
    }
};

// === 具象敵クラス1：ゴブリン ===
class Goblin : public Enemy 
{
public:
    Goblin(double posX, double posY) 
        : Enemy("ゴブリン", posX, posY, 30, 8) {}
    
    void update(double deltaTime) override 
    {
        Enemy::update(deltaTime);
        std::cout << "  ゴブリンは素早く動き回る" << std::endl;
    }
    
    void attack(Player* target) override 
    {
        std::cout << name << "の素早いナイフ攻撃！" << std::endl;
        target->takeDamage(attackPower);
    }
};

// === 具象敵クラス2：オーガ ===
class Ogre : public Enemy 
{
public:
    Ogre(double posX, double posY) 
        : Enemy("オーガ", posX, posY, 80, 15) {}
    
    void update(double deltaTime) override 
    {
        Enemy::update(deltaTime);
        std::cout << "  オーガはゆっくりと近づく" << std::endl;
    }
    
    void attack(Player* target) override 
    {
        std::cout << name << "の強力なクラブ攻撃！" << std::endl;
        target->takeDamage(attackPower * 1.5);  // より強力
    }
    
    // オーガ固有の能力
    void smash() 
    {
        std::cout << name << "が地面を叩き割った！範囲攻撃！" << std::endl;
    }
};

// === 具象敵クラス3：ドラゴン ===
class Dragon : public Enemy 
{
private:
    bool canFly;

public:
    Dragon(double posX, double posY) 
        : Enemy("ドラゴン", posX, posY, 200, 25), canFly(true) {}
    
    void update(double deltaTime) override 
    {
        Enemy::update(deltaTime);
        std::cout << "  ドラゴンは空中を旋回している" << std::endl;
    }
    
    void attack(Player* target) override 
    {
        if (canFly) 
        {
            std::cout << name << "の火炎ブレス！空からの攻撃！" << std::endl;
            target->takeDamage(attackPower * 2);
        }
        else 
        {
            std::cout << name << "の爪攻撃！" << std::endl;
            target->takeDamage(attackPower);
        }
    }
    
    void breatheFire() 
    {
        std::cout << name << "が強力な火炎ブレスを放った！" << std::endl;
    }
    
    void land() 
    {
        canFly = false;
        std::cout << name << "が地上に降りた！" << std::endl;
    }
};

// === アイテムクラス ===
class Item : public GameObject 
{
private:
    std::string type;
    int value;

public:
    Item(const std::string& itemName, const std::string& itemType, int itemValue, double posX, double posY) 
        : GameObject(itemName, posX, posY), type(itemType), value(itemValue) {}
    
    void update(double deltaTime) override 
    {
        // アイテムは特に更新処理なし
    }
    
    void render() const override 
    {
        std::cout << "アイテム「" << name << "」を描画: (" << x << ", " << y << ")" << std::endl;
    }
    
    void onCollision(GameObject* other) override 
    {
        if (dynamic_cast<Player*>(other)) 
        {
            std::cout << "プレイヤーが「" << name << "」を拾った！" << std::endl;
            setActive(false);
        }
    }
    
    const std::string& getType() const { return type; }
    int getValue() const { return value; }
};

// === ゲームエンジンクラス ===
class GameEngine 
{
private:
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    Player* player;  // プレイヤーへの参照

public:
    GameEngine() : player(nullptr) {}
    
    void addGameObject(std::unique_ptr<GameObject> obj) 
    {
        // プレイヤーの参照を保存
        if (auto* p = dynamic_cast<Player*>(obj.get())) 
        {
            player = p;
        }
        
        gameObjects.push_back(std::move(obj));
    }
    
    void update(double deltaTime) 
    {
        std::cout << "\n=== ゲーム更新処理 ===" << std::endl;
        
        for (auto& obj : gameObjects) 
        {
            if (obj->isActive()) 
            {
                obj->update(deltaTime);  // ポリモーフィズム！
            }
        }
        
        // 衝突判定
        checkCollisions();
        
        // 敵の攻撃処理
        processEnemyAttacks();
    }
    
    void render() const 
    {
        std::cout << "\n=== レンダリング処理 ===" << std::endl;
        
        for (const auto& obj : gameObjects) 
        {
            if (obj->isActive()) 
            {
                obj->render();  // ポリモーフィズム！
            }
        }
    }
    
    void processEnemyAttacks() 
    {
        if (!player || !player->isAlive()) return;
        
        for (auto& obj : gameObjects) 
        {
            if (obj->isActive()) 
            {
                // dynamic_cast で安全にキャスト
                if (auto* enemy = dynamic_cast<Enemy*>(obj.get())) 
                {
                    double distance = calculateDistance(player, enemy);
                    if (distance < 3.0) 
                    {
                        enemy->attack(player);  // ポリモーフィズム！
                    }
                }
            }
        }
    }
    
    void checkCollisions() 
    {
        for (size_t i = 0; i < gameObjects.size(); ++i) 
        {
            if (!gameObjects[i]->isActive()) continue;
            
            for (size_t j = i + 1; j < gameObjects.size(); ++j) 
            {
                if (!gameObjects[j]->isActive()) continue;
                
                double distance = calculateDistance(gameObjects[i].get(), gameObjects[j].get());
                if (distance < 1.0) 
                {
                    gameObjects[i]->onCollision(gameObjects[j].get());
                    gameObjects[j]->onCollision(gameObjects[i].get());
                }
            }
        }
    }
    
    double calculateDistance(GameObject* obj1, GameObject* obj2) const 
    {
        double dx = obj1->getX() - obj2->getX();
        double dy = obj1->getY() - obj2->getY();
        return std::sqrt(dx * dx + dy * dy);
    }
    
    void cleanup() 
    {
        gameObjects.erase(
            std::remove_if(gameObjects.begin(), gameObjects.end(),
                [](const std::unique_ptr<GameObject>& obj) {
                    return !obj->isActive();
                }),
            gameObjects.end()
        );
    }
    
    void displayGameObjectCount() const 
    {
        int players = 0, enemies = 0, items = 0;
        
        for (const auto& obj : gameObjects) 
        {
            if (dynamic_cast<const Player*>(obj.get())) players++;
            else if (dynamic_cast<const Enemy*>(obj.get())) enemies++;
            else if (dynamic_cast<const Item*>(obj.get())) items++;
        }
        
        std::cout << "ゲームオブジェクト数 - プレイヤー: " << players 
                  << ", 敵: " << enemies << ", アイテム: " << items << std::endl;
    }
};

int main() 
{
    std::cout << "=== ポリモーフィズムの実演 ===" << std::endl;
    
    GameEngine engine;
    
    // 様々なオブジェクトを作成
    engine.addGameObject(std::make_unique<Player>("勇者", 0, 0));
    engine.addGameObject(std::make_unique<Goblin>(5, 3));
    engine.addGameObject(std::make_unique<Ogre>(8, 7));
    engine.addGameObject(std::make_unique<Dragon>(12, 10));
    engine.addGameObject(std::make_unique<Item>("ポーション", "回復アイテム", 50, 2, 1));
    engine.addGameObject(std::make_unique<Item>("魔法の剣", "武器", 1000, 10, 5));
    
    engine.displayGameObjectCount();
    
    // ゲームループシミュレーション
    for (int frame = 1; frame <= 3; ++frame) 
    {
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "フレーム " << frame << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        engine.update(1.0/60.0);  // 60FPS
        engine.render();
        engine.cleanup();
    }
    
    // 動的キャストの実演
    std::cout << "\n=== 動的キャストの実演 ===" << std::endl;
    
    std::vector<std::unique_ptr<Enemy>> enemies;
    enemies.push_back(std::make_unique<Goblin>(0, 0));
    enemies.push_back(std::make_unique<Ogre>(0, 0));
    enemies.push_back(std::make_unique<Dragon>(0, 0));
    
    for (auto& enemy : enemies) 
    {
        // 基底クラスのポインタから派生クラス固有の機能を呼び出し
        if (auto* ogre = dynamic_cast<Ogre*>(enemy.get())) 
        {
            ogre->smash();  // オーガ固有の機能
        }
        else if (auto* dragon = dynamic_cast<Dragon*>(enemy.get())) 
        {
            dragon->breatheFire();  // ドラゴン固有の機能
            dragon->land();
        }
        else 
        {
            std::cout << enemy->getName() << "は特別な能力を持たない" << std::endl;
        }
    }
    
    return 0;
}

/*
    まとめ:
    
    1. ポリモーフィズムの基本:
       - 基底クラスのポインタで派生クラスのオブジェクトを操作
       - virtual 関数により実行時に適切なメソッドが呼ばれる
       - 同じインターフェースで異なる動作を実現
    
    2. 純粋仮想関数:
       - virtual void method() = 0; の構文
       - 抽象基底クラスの定義
       - 派生クラスで必ず実装が必要
    
    3. 動的キャスト（dynamic_cast）:
       - 実行時の型安全なキャスト
       - 失敗時はnullptrを返す
       - RTTI（Run-Time Type Information）を使用
    
    4. 仮想デストラクタ:
       - 基底クラスのデストラクタをvirtualに
       - 正しい順序での削除を保証
       - メモリリークの防止
    
    5. オーバーライドの指定:
       - override キーワードの使用
       - コンパイル時のエラー検出
       - 意図の明確化
    
    利点:
    - コードの拡張性
    - 統一されたインターフェース
    - プラグイン的な設計
    - 保守性の向上
    
    ゲーム開発での活用:
    - ゲームオブジェクトの統一管理
    - AIの種類別実装
    - 描画システムの抽象化
    - イベントハンドリング
    - プラグイン型のゲーム機能
    
    注意点:
    - 仮想関数は実行時コストがある
    - 適切な設計が重要
    - 過度の抽象化は避ける
    - パフォーマンスとのバランス
*/