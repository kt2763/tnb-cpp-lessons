// Lecture 9: 多重継承
// Multiple Inheritance in C++

#include <iostream>
#include <string>
#include <vector>
#include <memory>

// 基底インターフェースたち
class IMovable 
{
public:
    virtual ~IMovable() = default;
    virtual void move(int dx, int dy) = 0;
    virtual int getX() const = 0;
    virtual int getY() const = 0;
};

class ICombatant 
{
public:
    virtual ~ICombatant() = default;
    virtual void attack(ICombatant* target) = 0;
    virtual void takeDamage(int damage) = 0;
    virtual int getHealth() const = 0;
    virtual bool isAlive() const = 0;
};

class IInventoryHolder 
{
public:
    virtual ~IInventoryHolder() = default;
    virtual void addItem(const std::string& item) = 0;
    virtual bool hasItem(const std::string& item) const = 0;
    virtual void showInventory() const = 0;
};

class IInteractable 
{
public:
    virtual ~IInteractable() = default;
    virtual void interact() = 0;
    virtual std::string getInteractionText() const = 0;
};

// 多重継承の基本例：プレイヤー
class Player : public IMovable, public ICombatant, public IInventoryHolder 
{
private:
    std::string name;
    int x, y;  // 位置
    int health, maxHealth;
    int attackPower;
    std::vector<std::string> inventory;
    
public:
    Player(const std::string& n, int startX, int startY) 
        : name(n), x(startX), y(startY), health(100), maxHealth(100), attackPower(20) 
    {
        inventory.push_back("木の剣");
        inventory.push_back("パン");
        std::cout << "Player " << name << " が座標(" << x << ", " << y << ")に生成されました\n";
    }
    
    // IMovable の実装
    void move(int dx, int dy) override 
    {
        x += dx;
        y += dy;
        std::cout << name << "が座標(" << x << ", " << y << ")に移動しました\n";
    }
    
    int getX() const override { return x; }
    int getY() const override { return y; }
    
    // ICombatant の実装
    void attack(ICombatant* target) override 
    {
        if (target && target->isAlive()) {
            std::cout << name << "は攻撃しました！\n";
            target->takeDamage(attackPower);
        }
    }
    
    void takeDamage(int damage) override 
    {
        health -= damage;
        if (health < 0) health = 0;
        std::cout << name << "は" << damage << "のダメージを受けた！ "
                  << "残りHP: " << health << "/" << maxHealth << "\n";
    }
    
    int getHealth() const override { return health; }
    bool isAlive() const override { return health > 0; }
    
    // IInventoryHolder の実装
    void addItem(const std::string& item) override 
    {
        inventory.push_back(item);
        std::cout << item << " をインベントリに追加しました\n";
    }
    
    bool hasItem(const std::string& item) const override 
    {
        for (const auto& i : inventory) {
            if (i == item) return true;
        }
        return false;
    }
    
    void showInventory() const override 
    {
        std::cout << name << "のインベントリ: ";
        for (const auto& item : inventory) {
            std::cout << item << " ";
        }
        std::cout << "\n";
    }
    
    std::string getName() const { return name; }
};

// 商人：インタラクト可能でアイテムを持つ
class Merchant : public IMovable, public IInventoryHolder, public IInteractable 
{
private:
    std::string name;
    int x, y;
    std::vector<std::string> inventory;
    
public:
    Merchant(const std::string& n, int posX, int posY) 
        : name(n), x(posX), y(posY) 
    {
        // 商品の初期化
        inventory = {"体力ポーション", "魔力ポーション", "鉄の剣", "革の防具"};
        std::cout << "Merchant " << name << " が店を開きました\n";
    }
    
    void move(int dx, int dy) override 
    {
        x += dx;
        y += dy;
        std::cout << "Merchant " << name << "が移動しました\n";
    }
    
    int getX() const override { return x; }
    int getY() const override { return y; }
    
    void addItem(const std::string& item) override 
    {
        inventory.push_back(item);
        std::cout << item << " を商品に追加しました\n";
    }
    
    bool hasItem(const std::string& item) const override 
    {
        for (const auto& i : inventory) {
            if (i == item) return true;
        }
        return false;
    }
    
    void showInventory() const override 
    {
        std::cout << name << "の商品: ";
        for (const auto& item : inventory) {
            std::cout << item << " ";
        }
        std::cout << "\n";
    }
    
    void interact() override 
    {
        std::cout << name << ": "いらっしゃいませ！何かお入り用の物はありますか？"\n";
        showInventory();
    }
    
    std::string getInteractionText() const override 
    {
        return "話しかける";
    }
    
    // 商人特有の機能
    bool sellItem(const std::string& item, IInventoryHolder* buyer) 
    {
        if (hasItem(item)) {
            buyer->addItem(item);
            // 実際はインベントリから削除する必要がある
            std::cout << item << " を売りました！\n";
            return true;
        }
        return false;
    }
};

// ダイヤモンド継承の問題を解決する例
class GameObject 
{
protected:
    std::string name;
    int id;
    
public:
    GameObject(const std::string& n, int objectId) : name(n), id(objectId) 
    {
        std::cout << "GameObject " << name << " (ID: " << id << ") が作成されました\n";
    }
    
    virtual ~GameObject() 
    {
        std::cout << "GameObject " << name << " が破棄されました\n";
    }
    
    std::string getName() const { return name; }
    int getId() const { return id; }
};

// 仮想継承でダイヤモンド問題を防ぐ
class MovableObject : virtual public GameObject 
{
protected:
    int x, y;
    int speed;
    
public:
    MovableObject(const std::string& n, int id, int startX, int startY, int spd)
        : GameObject(n, id), x(startX), y(startY), speed(spd) 
    {
        std::cout << "MovableObject が初期化されました\n";
    }
    
    virtual void move(int dx, int dy) 
    {
        x += dx * speed;
        y += dy * speed;
        std::cout << name << "が速度" << speed << "で移動しました\n";
    }
    
    int getX() const { return x; }
    int getY() const { return y; }
};

class CombatObject : virtual public GameObject 
{
protected:
    int health, maxHealth;
    int attackPower;
    
public:
    CombatObject(const std::string& n, int id, int hp, int atk)
        : GameObject(n, id), health(hp), maxHealth(hp), attackPower(atk) 
    {
        std::cout << "CombatObject が初期化されました\n";
    }
    
    virtual void attack(CombatObject* target) 
    {
        if (target && target->isAlive()) {
            std::cout << name << "は" << target->getName() << "を攻撃した！\n";
            target->takeDamage(attackPower);
        }
    }
    
    virtual void takeDamage(int damage) 
    {
        health -= damage;
        if (health < 0) health = 0;
        std::cout << name << "は" << damage << "のダメージを受けた！\n";
    }
    
    bool isAlive() const { return health > 0; }
    int getHealth() const { return health; }
};

// 仮想継承で統合されたクラス
class GameCharacter : public MovableObject, public CombatObject 
{
public:
    GameCharacter(const std::string& n, int id, int x, int y, int spd, int hp, int atk)
        : GameObject(n, id),  // 仮想継承では明示的に基底クラスを初期化
          MovableObject(n, id, x, y, spd),
          CombatObject(n, id, hp, atk) 
    {
        std::cout << "GameCharacter " << name << " が作成されました\n";
    }
    
    // 特別なアビリティ
    void specialAttack(CombatObject* target) 
    {
        if (target && target->isAlive()) {
            std::cout << name << "は特殊攻撃を放った！\n";
            target->takeDamage(attackPower * 2);
        }
    }
};

// 多重継承でユーティリティを組み合わせる例
template<typename T>
class Loggable 
{
public:
    void log(const std::string& message) 
    {
        std::cout << "[LOG] " << static_cast<T*>(this)->getName() 
                  << ": " << message << "\n";
    }
};

template<typename T>
class Serializable 
{
public:
    void save() 
    {
        std::cout << "[SAVE] " << static_cast<T*>(this)->getName() 
                  << " のデータを保存しました\n";
    }
    
    void load() 
    {
        std::cout << "[LOAD] " << static_cast<T*>(this)->getName() 
                  << " のデータを読み込みました\n";
    }
};

// Mixin パターンの例
class AdvancedPlayer : public Player, public Loggable<AdvancedPlayer>, public Serializable<AdvancedPlayer> 
{
public:
    AdvancedPlayer(const std::string& name, int x, int y)
        : Player(name, x, y) 
    {
        log("作成されました");
    }
    
    void move(int dx, int dy) override 
    {
        Player::move(dx, dy);
        log("移動しました");
    }
    
    void attack(ICombatant* target) override 
    {
        Player::attack(target);
        log("攻撃しました");
    }
};

int main() 
{
    std::cout << "=== 多重継承のデモ ===\n\n";
    
    // 基本的な多重継承
    std::cout << "1. 基本的な多重継承\n";
    Player player("勇者", 0, 0);
    Merchant merchant("アンナ", 10, 10);
    
    player.move(5, 5);
    player.showInventory();
    
    merchant.interact();
    merchant.sellItem("鉄の剣", &player);
    player.showInventory();
    
    std::cout << "\n---\n\n";
    
    // 仮想継承でダイヤモンド問題解決
    std::cout << "2. 仮想継承でダイヤモンド問題解決\n";
    GameCharacter hero("ナイト", 1, 0, 0, 2, 150, 30);
    GameCharacter monster("オーク", 2, 20, 20, 1, 80, 25);
    
    hero.move(1, 0);
    hero.attack(&monster);
    hero.specialAttack(&monster);
    
    std::cout << "\n---\n\n";
    
    // Mixin パターン
    std::cout << "3. Mixin パターンとユーティリティ\n";
    AdvancedPlayer advPlayer("マスター", 0, 0);
    
    advPlayer.move(3, 3);
    advPlayer.save();
    advPlayer.load();
    
    // インターフェースを使ったポリモーフィズム
    std::cout << "\n4. インターフェースを使ったポリモーフィズム\n";
    std::vector<IMovable*> movables = {&player, &merchant, &advPlayer};
    std::vector<IInventoryHolder*> inventoryHolders = {&player, &merchant, &advPlayer};
    
    std::cout << "全オブジェクトを移動:\n";
    for (auto* movable : movables) {
        movable->move(1, 1);
    }
    
    std::cout << "\n全インベントリを表示:\n";
    for (auto* holder : inventoryHolders) {
        holder->showInventory();
    }
    
    return 0;
}

/*
学習ポイント：
1. 多重継承 = 複数のクラスから同時に継承
2. インターフェースの組み合わせで柔軟な設計が可能
3. ダイヤモンド継承問題 = 同じ基底クラスが複数継承される問題
4. 仮想継承（virtual）でダイヤモンド問題を解決
5. Mixin パターンで機能をモジュラー化
6. CRTP (Curiously Recurring Template Pattern) で静的ポリモーフィズム
7. インターフェース分離の原則 (ISP) で設計を改善
8. 多重継承は慣習が必要だが、適切に使えば強力
*/