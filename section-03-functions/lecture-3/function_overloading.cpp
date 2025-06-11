// 第3講: 関数のオーバーロード
// 同じ関数名で異なる引数を持つ関数を定義する方法を学ぶ

#include <iostream>
#include <string>
#include <vector>

// === 基本的なオーバーロード ===
// 整数の攻撃
void attack(int damage) 
{
    std::cout << "通常攻撃: " << damage << " ダメージ" << std::endl;
}

// 整数と文字列（属性付き攻撃）
void attack(int damage, std::string element) 
{
    std::cout << element << "属性攻撃: " << damage << " ダメージ" << std::endl;
}

// 整数と倍率（クリティカル攻撃）
void attack(int damage, double multiplier) 
{
    int finalDamage = damage * multiplier;
    std::cout << "強化攻撃: " << finalDamage << " ダメージ (×" << multiplier << ")" << std::endl;
}

// === 異なる型での回復処理 ===
// HP回復（整数値）
void heal(int amount) 
{
    std::cout << "HPを " << amount << " 回復" << std::endl;
}

// HP回復（パーセント）
void heal(double percentage) 
{
    std::cout << "HPを " << (percentage * 100) << "% 回復" << std::endl;
}

// HP回復（アイテム名）
void heal(std::string itemName) 
{
    if (itemName == "ポーション") 
    {
        std::cout << itemName << "を使用: HP 50 回復" << std::endl;
    }
    else if (itemName == "ハイポーション") 
    {
        std::cout << itemName << "を使用: HP 150 回復" << std::endl;
    }
    else if (itemName == "エリクサー") 
    {
        std::cout << itemName << "を使用: HP/MP 全回復" << std::endl;
    }
}

// === キャラクター情報の表示 ===
// 名前のみ
void displayCharacter(std::string name) 
{
    std::cout << "キャラクター: " << name << std::endl;
}

// 名前とレベル
void displayCharacter(std::string name, int level) 
{
    std::cout << "キャラクター: " << name << " (Lv." << level << ")" << std::endl;
}

// 名前、レベル、職業
void displayCharacter(std::string name, int level, std::string job) 
{
    std::cout << "キャラクター: " << name << " (Lv." << level << " " << job << ")" << std::endl;
}

// === 実践的なゲーム関数 ===
// ダメージ計算のオーバーロード
// 基本ダメージ
int calculateDamage(int attackPower) 
{
    return attackPower;
}

// 防御力を考慮
int calculateDamage(int attackPower, int defense) 
{
    int damage = attackPower - defense;
    return damage > 0 ? damage : 1;  // 最低1ダメージ
}

// 属性相性を考慮
int calculateDamage(int attackPower, int defense, double elementMultiplier) 
{
    int baseDamage = attackPower - defense;
    if (baseDamage < 1) baseDamage = 1;
    return baseDamage * elementMultiplier;
}

// アイテム生成のオーバーロード
struct Item 
{
    std::string name;
    std::string type;
    int value;
    int quantity;
};

// アイテム名のみ
Item createItem(std::string name) 
{
    Item item;
    item.name = name;
    item.type = "その他";
    item.value = 10;
    item.quantity = 1;
    return item;
}

// アイテム名と個数
Item createItem(std::string name, int quantity) 
{
    Item item;
    item.name = name;
    item.type = "消耗品";
    item.value = 50;
    item.quantity = quantity;
    return item;
}

// アイテム名、タイプ、価値
Item createItem(std::string name, std::string type, int value) 
{
    Item item;
    item.name = name;
    item.type = type;
    item.value = value;
    item.quantity = 1;
    return item;
}

// 移動処理のオーバーロード
// 方向指定
void move(std::string direction) 
{
    std::cout << direction << "へ移動" << std::endl;
}

// 座標指定
void move(int x, int y) 
{
    std::cout << "座標(" << x << ", " << y << ")へ移動" << std::endl;
}

// 距離と角度指定
void move(double distance, double angle) 
{
    std::cout << "角度" << angle << "度の方向へ" << distance << "m移動" << std::endl;
}

// 敵生成のオーバーロード
class Enemy 
{
public:
    std::string name;
    int hp;
    int attack;
    int defense;
    
    void display() 
    {
        std::cout << name << " - HP:" << hp << " ATK:" << attack << " DEF:" << defense << std::endl;
    }
};

// 敵タイプのみ
Enemy createEnemy(std::string type) 
{
    Enemy enemy;
    
    if (type == "スライム") 
    {
        enemy.name = type;
        enemy.hp = 30;
        enemy.attack = 10;
        enemy.defense = 5;
    }
    else if (type == "ゴブリン") 
    {
        enemy.name = type;
        enemy.hp = 50;
        enemy.attack = 20;
        enemy.defense = 10;
    }
    
    return enemy;
}

// 敵タイプとレベル
Enemy createEnemy(std::string type, int level) 
{
    Enemy enemy = createEnemy(type);
    
    // レベルによる強化
    enemy.hp *= level;
    enemy.attack += (level - 1) * 5;
    enemy.defense += (level - 1) * 3;
    enemy.name = "Lv." + std::to_string(level) + " " + enemy.name;
    
    return enemy;
}

// カスタム敵
Enemy createEnemy(std::string name, int hp, int attack, int defense) 
{
    Enemy enemy;
    enemy.name = name;
    enemy.hp = hp;
    enemy.attack = attack;
    enemy.defense = defense;
    return enemy;
}

int main() 
{
    std::cout << "=== 関数オーバーロードの基本 ===" << std::endl;
    
    // 攻撃関数の呼び分け
    attack(50);                        // 通常攻撃
    attack(40, "火");                  // 属性攻撃
    attack(30, 2.5);                   // クリティカル攻撃
    
    // 回復関数の呼び分け
    std::cout << std::endl;
    heal(30);                          // 固定値回復
    heal(0.5);                         // 50%回復
    heal("エリクサー");                // アイテム使用
    
    // キャラクター表示の呼び分け
    std::cout << std::endl;
    displayCharacter("勇者");
    displayCharacter("魔法使い", 15);
    displayCharacter("戦士", 20, "バーサーカー");
    
    std::cout << "\n=== ゲーム開発での実践例 ===" << std::endl;
    
    // ダメージ計算
    int atk = 100;
    int def = 30;
    
    std::cout << "ダメージ計算:" << std::endl;
    std::cout << "基本: " << calculateDamage(atk) << std::endl;
    std::cout << "防御考慮: " << calculateDamage(atk, def) << std::endl;
    std::cout << "属性相性（弱点2倍）: " << calculateDamage(atk, def, 2.0) << std::endl;
    std::cout << "属性相性（耐性半減）: " << calculateDamage(atk, def, 0.5) << std::endl;
    
    // アイテム生成
    std::cout << "\nアイテム生成:" << std::endl;
    Item item1 = createItem("謎の石");
    Item item2 = createItem("ポーション", 5);
    Item item3 = createItem("伝説の剣", "武器", 10000);
    
    std::cout << item1.name << " (" << item1.type << ") 価値:" << item1.value 
              << "G ×" << item1.quantity << std::endl;
    std::cout << item2.name << " (" << item2.type << ") 価値:" << item2.value 
              << "G ×" << item2.quantity << std::endl;
    std::cout << item3.name << " (" << item3.type << ") 価値:" << item3.value 
              << "G ×" << item3.quantity << std::endl;
    
    // 移動処理
    std::cout << "\n移動コマンド:" << std::endl;
    move("北");
    move(10, 5);
    move(15.5, 45.0);
    
    // 敵の生成
    std::cout << "\n敵の生成:" << std::endl;
    Enemy enemy1 = createEnemy("スライム");
    Enemy enemy2 = createEnemy("ゴブリン", 3);
    Enemy enemy3 = createEnemy("魔王", 1000, 200, 150);
    
    enemy1.display();
    enemy2.display();
    enemy3.display();
    
    // 複雑な戦闘シミュレーション
    std::cout << "\n戦闘シミュレーション:" << std::endl;
    
    // プレイヤーの攻撃
    int playerAtk = 80;
    int enemyDef = 20;
    
    std::cout << "プレイヤーのターン:" << std::endl;
    attack(calculateDamage(playerAtk, enemyDef));
    attack(calculateDamage(playerAtk, enemyDef, 1.5), "雷");  // 属性攻撃
    
    // 敵の反撃
    std::cout << "\n敵のターン:" << std::endl;
    int enemyAtk = 50;
    int playerDef = 30;
    std::cout << "敵の";
    attack(calculateDamage(enemyAtk, playerDef));
    
    return 0;
}

/*
    まとめ:
    - 関数のオーバーロードにより、同じ関数名で異なる処理を実現
    - 引数の型、個数、順序が異なれば別の関数として定義可能
    - 戻り値の型だけが異なる場合はオーバーロードできない
    - コンパイラが引数から適切な関数を自動的に選択
    
    ゲーム開発での活用：
    - 攻撃処理（通常、属性、クリティカルなど）
    - 回復処理（固定値、割合、アイテムなど）
    - キャラクター生成（簡易版から詳細版まで）
    - 移動処理（方向、座標、極座標など）
    - ダメージ計算（単純から複雑な計算まで）
    
    利点：
    - 直感的なAPI設計が可能
    - 同じ概念に対して統一された関数名を使用
    - 使用する側が必要に応じて適切なバージョンを選択
    - コードの可読性と保守性が向上
*/