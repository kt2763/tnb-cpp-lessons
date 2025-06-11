// 第1講: クラスの基本
// オブジェクト指向プログラミングの基礎となるクラスの概念と実装方法を学ぶ

#include <iostream>
#include <string>

// === 基本的なクラスの定義 ===
class Player 
{
    // プライベートメンバ（外部からアクセス不可）
private:
    std::string name;
    int level;
    int hp;
    int maxHp;
    int mp;
    int maxMp;
    int experience;

    // パブリックメンバ（外部からアクセス可能）
public:
    // コンストラクタ（オブジェクト作成時に呼ばれる）
    Player(const std::string& playerName) 
    {
        name = playerName;
        level = 1;
        maxHp = 100;
        hp = maxHp;
        maxMp = 50;
        mp = maxMp;
        experience = 0;
        
        std::cout << "プレイヤー「" << name << "」が冒険を始めた！" << std::endl;
    }
    
    // デストラクタ（オブジェクト破棄時に呼ばれる）
    ~Player() 
    {
        std::cout << "プレイヤー「" << name << "」の冒険が終了しました" << std::endl;
    }
    
    // ゲッター（プライベートメンバへの読み取り専用アクセス）
    std::string getName() const 
    {
        return name;
    }
    
    int getLevel() const 
    {
        return level;
    }
    
    int getHp() const 
    {
        return hp;
    }
    
    int getMaxHp() const 
    {
        return maxHp;
    }
    
    int getMp() const 
    {
        return mp;
    }
    
    int getMaxMp() const 
    {
        return maxMp;
    }
    
    int getExperience() const 
    {
        return experience;
    }
    
    // セッター（プライベートメンバへの書き込みアクセス）
    void setName(const std::string& newName) 
    {
        name = newName;
        std::cout << "プレイヤー名を「" << name << "」に変更しました" << std::endl;
    }
    
    // メンバ関数（オブジェクトの操作）
    void displayStatus() const 
    {
        std::cout << "=== " << name << "のステータス ===" << std::endl;
        std::cout << "レベル: " << level << std::endl;
        std::cout << "HP: " << hp << "/" << maxHp << std::endl;
        std::cout << "MP: " << mp << "/" << maxMp << std::endl;
        std::cout << "経験値: " << experience << std::endl;
        std::cout << "===============================" << std::endl;
    }
    
    void takeDamage(int damage) 
    {
        hp -= damage;
        if (hp < 0) 
        {
            hp = 0;
        }
        
        std::cout << name << "は" << damage << "のダメージを受けた！" << std::endl;
        std::cout << "残りHP: " << hp << "/" << maxHp << std::endl;
        
        if (hp == 0) 
        {
            std::cout << name << "は倒れてしまった..." << std::endl;
        }
    }
    
    void heal(int amount) 
    {
        int oldHp = hp;
        hp += amount;
        if (hp > maxHp) 
        {
            hp = maxHp;
        }
        
        int actualHealing = hp - oldHp;
        std::cout << name << "は" << actualHealing << "回復した！" << std::endl;
        std::cout << "現在HP: " << hp << "/" << maxHp << std::endl;
    }
    
    void gainExperience(int exp) 
    {
        experience += exp;
        std::cout << name << "は" << exp << "の経験値を獲得！" << std::endl;
        
        // レベルアップ判定
        int expNeededForNextLevel = level * 100;
        if (experience >= expNeededForNextLevel) 
        {
            levelUp();
        }
    }
    
    bool isAlive() const 
    {
        return hp > 0;
    }
    
private:
    // プライベートメソッド（クラス内部でのみ使用）
    void levelUp() 
    {
        experience -= level * 100;
        level++;
        
        // ステータス上昇
        int hpIncrease = 20;
        int mpIncrease = 10;
        
        maxHp += hpIncrease;
        hp += hpIncrease;  // レベルアップ時は追加HP分も回復
        maxMp += mpIncrease;
        mp += mpIncrease;  // レベルアップ時は追加MP分も回復
        
        std::cout << "★ レベルアップ！ " << name << "はレベル" << level << "になった！" << std::endl;
        std::cout << "  最大HPが" << hpIncrease << "上昇！（" << maxHp << "）" << std::endl;
        std::cout << "  最大MPが" << mpIncrease << "上昇！（" << maxMp << "）" << std::endl;
        
        // 再度レベルアップできるかチェック
        int expNeededForNextLevel = level * 100;
        if (experience >= expNeededForNextLevel) 
        {
            levelUp();  // 連続レベルアップ
        }
    }
};

// === 敵キャラクターのクラス ===
class Enemy 
{
private:
    std::string name;
    int hp;
    int maxHp;
    int attackPower;
    int defense;

public:
    Enemy(const std::string& enemyName, int health, int attack, int def) 
        : name(enemyName), hp(health), maxHp(health), attackPower(attack), defense(def)
    {
        std::cout << name << "が現れた！" << std::endl;
    }
    
    ~Enemy() 
    {
        std::cout << name << "との戦闘終了" << std::endl;
    }
    
    // ゲッター
    std::string getName() const { return name; }
    int getHp() const { return hp; }
    int getMaxHp() const { return maxHp; }
    int getAttackPower() const { return attackPower; }
    int getDefense() const { return defense; }
    
    void displayStatus() const 
    {
        std::cout << name << " - HP:" << hp << "/" << maxHp 
                  << " ATK:" << attackPower << " DEF:" << defense << std::endl;
    }
    
    void takeDamage(int damage) 
    {
        int actualDamage = damage - defense;
        if (actualDamage < 1) 
        {
            actualDamage = 1;  // 最低1ダメージ
        }
        
        hp -= actualDamage;
        if (hp < 0) 
        {
            hp = 0;
        }
        
        std::cout << name << "は" << actualDamage << "のダメージを受けた！" << std::endl;
        
        if (hp == 0) 
        {
            std::cout << name << "を倒した！" << std::endl;
        }
    }
    
    bool isAlive() const 
    {
        return hp > 0;
    }
    
    int attack() const 
    {
        std::cout << name << "の攻撃！" << std::endl;
        return attackPower;
    }
};

// === 武器クラス ===
class Weapon 
{
private:
    std::string name;
    int attackBonus;
    int durability;
    int maxDurability;

public:
    Weapon(const std::string& weaponName, int attack, int dur) 
        : name(weaponName), attackBonus(attack), durability(dur), maxDurability(dur)
    {
        std::cout << "武器「" << name << "」を装備した！" << std::endl;
    }
    
    std::string getName() const { return name; }
    int getAttackBonus() const { return attackBonus; }
    int getDurability() const { return durability; }
    int getMaxDurability() const { return maxDurability; }
    
    void displayInfo() const 
    {
        std::cout << "武器: " << name << " 攻撃力+" << attackBonus 
                  << " 耐久度:" << durability << "/" << maxDurability << std::endl;
    }
    
    bool use() 
    {
        if (durability > 0) 
        {
            durability--;
            std::cout << name << "を使用！（耐久度: " << durability << "/" << maxDurability << "）" << std::endl;
            
            if (durability == 0) 
            {
                std::cout << name << "が壊れてしまった！" << std::endl;
                return false;
            }
            return true;
        }
        else 
        {
            std::cout << name << "は壊れているため使用できない" << std::endl;
            return false;
        }
    }
    
    void repair(int amount) 
    {
        durability += amount;
        if (durability > maxDurability) 
        {
            durability = maxDurability;
        }
        
        std::cout << name << "を修理した！耐久度: " << durability << "/" << maxDurability << std::endl;
    }
    
    bool isBroken() const 
    {
        return durability == 0;
    }
};

// === 戦闘システムクラス ===
class BattleSystem 
{
public:
    static void battle(Player& player, Enemy& enemy) 
    {
        std::cout << "\n=== 戦闘開始！ ===" << std::endl;
        std::cout << player.getName() << " vs " << enemy.getName() << std::endl;
        
        int turn = 1;
        while (player.isAlive() && enemy.isAlive()) 
        {
            std::cout << "\n--- ターン " << turn << " ---" << std::endl;
            
            // プレイヤーの攻撃
            std::cout << player.getName() << "の攻撃！" << std::endl;
            int playerDamage = 30;  // 基本攻撃力
            enemy.takeDamage(playerDamage);
            
            if (!enemy.isAlive()) 
            {
                break;
            }
            
            // 敵の攻撃
            int enemyDamage = enemy.attack();
            player.takeDamage(enemyDamage);
            
            turn++;
        }
        
        // 戦闘結果
        if (player.isAlive()) 
        {
            std::cout << "\n勝利！" << std::endl;
            player.gainExperience(50);
        }
        else 
        {
            std::cout << "\n敗北..." << std::endl;
        }
        
        std::cout << "=== 戦闘終了 ===" << std::endl;
    }
};

int main() 
{
    std::cout << "=== クラスの基本実装例 ===" << std::endl;
    
    // プレイヤーオブジェクトの作成
    Player hero("勇者アレクサンダー");
    hero.displayStatus();
    
    // 武器の作成と装備
    Weapon sword("鋼の剣", 15, 10);
    sword.displayInfo();
    
    // 敵の作成
    Enemy goblin("ゴブリン", 40, 15, 5);
    goblin.displayStatus();
    
    std::cout << "\n=== ゲームプレイシミュレーション ===" << std::endl;
    
    // 戦闘前の準備
    std::cout << "\n戦闘前の状態:" << std::endl;
    hero.displayStatus();
    
    // 戦闘実行
    BattleSystem::battle(hero, goblin);
    
    // 戦闘後の処理
    std::cout << "\n戦闘後の状態:" << std::endl;
    hero.displayStatus();
    
    // 回復
    std::cout << "\n回復処理:" << std::endl;
    hero.heal(30);
    
    // 武器の使用
    std::cout << "\n武器の使用:" << std::endl;
    for (int i = 0; i < 12; i++) 
    {
        if (!sword.use()) 
        {
            break;
        }
    }
    
    // 武器の修理
    std::cout << "\n武器の修理:" << std::endl;
    sword.repair(5);
    
    // 追加の経験値獲得でレベルアップテスト
    std::cout << "\n追加の経験値獲得:" << std::endl;
    hero.gainExperience(200);  // 大量の経験値で連続レベルアップ
    
    hero.displayStatus();
    
    return 0;
}

/*
    まとめ:
    - クラスはデータ（メンバ変数）と操作（メンバ関数）をまとめたもの
    - privateメンバは外部からアクセス不可、publicメンバはアクセス可能
    - コンストラクタでオブジェクトの初期化、デストラクタで終了処理
    - ゲッター/セッターでprivateメンバへの制御されたアクセス
    - constメンバ関数は状態を変更しない関数
    - カプセル化により、データの整合性と安全性を保つ
    
    ゲーム開発での活用:
    - Playerクラス: プレイヤーの状態管理
    - Enemyクラス: 敵キャラクターの定義
    - Weaponクラス: アイテムや装備の管理
    - BattleSystemクラス: ゲームロジックの整理
    
    オブジェクト指向の利点:
    - コードの再利用性
    - 保守性の向上
    - 理解しやすい構造
    - 現実世界のモデリングが可能
*/