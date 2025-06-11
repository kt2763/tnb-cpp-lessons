// Lecture 10: コンポジション vs 継承
// Composition vs Inheritance in C++

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>

// 継承を使ったアプローチ（旧来の方法）
namespace InheritanceApproach 
{
    // 基底クラス
    class Weapon 
    {
    protected:
        std::string name;
        int damage;
        int durability;
        
    public:
        Weapon(const std::string& n, int dmg, int dur)
            : name(n), damage(dmg), durability(dur) {}
        
        virtual ~Weapon() = default;
        virtual void attack() = 0;
        virtual std::string getType() const = 0;
        
        std::string getName() const { return name; }
        int getDamage() const { return damage; }
    };
    
    // 継承で各種武器を作成
    class Sword : public Weapon 
    {
    public:
        Sword(const std::string& name, int damage) 
            : Weapon(name, damage, 100) {}
        
        void attack() override 
        {
            std::cout << name << "で斬りつけた！ ダメージ: " << damage << "\n";
        }
        
        std::string getType() const override { return "Sword"; }
    };
    
    class Bow : public Weapon 
    {
    public:
        Bow(const std::string& name, int damage) 
            : Weapon(name, damage, 80) {}
        
        void attack() override 
        {
            std::cout << name << "で矢を放った！ ダメージ: " << damage << "\n";
        }
        
        std::string getType() const override { return "Bow"; }
    };
    
    class Staff : public Weapon 
    {
    public:
        Staff(const std::string& name, int damage) 
            : Weapon(name, damage, 60) {}
        
        void attack() override 
        {
            std::cout << name << "で魔法を放った！ ダメージ: " << damage << "\n";
        }
        
        std::string getType() const override { return "Staff"; }
    };
    
    // 継承ベースのプレイヤークラス
    class Player 
    {
    private:
        std::string name;
        std::unique_ptr<Weapon> weapon;
        int health;
        
    public:
        Player(const std::string& n) : name(n), health(100) {}
        
        void equipWeapon(std::unique_ptr<Weapon> w) 
        {
            weapon = std::move(w);
            std::cout << name << "は" << weapon->getName() << "を装備しました\n";
        }
        
        void attack() 
        {
            if (weapon) {
                weapon->attack();
            } else {
                std::cout << name << "は素手で攻撃した！\n";
            }
        }
    };
}

// コンポジションを使ったアプローチ（推奨方法）
namespace CompositionApproach 
{
    // コンポーネントクラスたち
    class AttackComponent 
    {
    private:
        std::string attackType;
        int baseDamage;
        std::string attackMessage;
        
    public:
        AttackComponent(const std::string& type, int damage, const std::string& msg)
            : attackType(type), baseDamage(damage), attackMessage(msg) {}
        
        void performAttack(const std::string& weaponName) const 
        {
            std::cout << weaponName << attackMessage << " ダメージ: " << baseDamage << "\n";
        }
        
        int getDamage() const { return baseDamage; }
        std::string getType() const { return attackType; }
    };
    
    class DurabilityComponent 
    {
    private:
        int maxDurability;
        int currentDurability;
        
    public:
        DurabilityComponent(int maxDur) 
            : maxDurability(maxDur), currentDurability(maxDur) {}
        
        bool use() 
        {
            if (currentDurability > 0) {
                currentDurability--;
                return true;
            }
            return false;
        }
        
        void repair(int amount) 
        {
            currentDurability = std::min(maxDurability, currentDurability + amount);
        }
        
        bool isBroken() const { return currentDurability <= 0; }
        float getDurabilityRatio() const 
        {
            return static_cast<float>(currentDurability) / maxDurability;
        }
    };
    
    class EnchantmentComponent 
    {
    private:
        std::map<std::string, int> enchantments;
        
    public:
        void addEnchantment(const std::string& name, int level) 
        {
            enchantments[name] = level;
            std::cout << name << " Lv." << level << "のエンチャントを付与しました\n";
        }
        
        int getEnchantmentBonus() const 
        {
            int bonus = 0;
            for (const auto& [name, level] : enchantments) {
                bonus += level * 5;  // レベル1ごとに+5ダメージ
            }
            return bonus;
        }
        
        void showEnchantments() const 
        {
            if (enchantments.empty()) {
                std::cout << "エンチャントなし\n";
                return;
            }
            std::cout << "エンチャント: ";
            for (const auto& [name, level] : enchantments) {
                std::cout << name << " Lv." << level << " ";
            }
            std::cout << "\n";
        }
    };
    
    // コンポジションベースの武器クラス
    class Weapon 
    {
    private:
        std::string name;
        AttackComponent attackComp;
        DurabilityComponent durabilityComp;
        EnchantmentComponent enchantmentComp;
        
    public:
        Weapon(const std::string& n, const std::string& attackType, 
               int damage, const std::string& attackMsg, int durability)
            : name(n), 
              attackComp(attackType, damage, attackMsg),
              durabilityComp(durability) {}
        
        void attack() 
        {
            if (durabilityComp.isBroken()) {
                std::cout << name << "は壊れている！\n";
                return;
            }
            
            if (durabilityComp.use()) {
                int totalDamage = attackComp.getDamage() + enchantmentComp.getEnchantmentBonus();
                attackComp.performAttack(name);
                if (enchantmentComp.getEnchantmentBonus() > 0) {
                    std::cout << "エンチャントボーナス: +" 
                              << enchantmentComp.getEnchantmentBonus() 
                              << " (合計: " << totalDamage << ")\n";
                }
                
                float durabilityRatio = durabilityComp.getDurabilityRatio();
                if (durabilityRatio < 0.3f) {
                    std::cout << "警告: " << name << "の耐久度が低下しています！\n";
                }
            }
        }
        
        void repair(int amount) 
        {
            durabilityComp.repair(amount);
            std::cout << name << "を修理しました\n";
        }
        
        void addEnchantment(const std::string& enchName, int level) 
        {
            enchantmentComp.addEnchantment(enchName, level);
        }
        
        void showStatus() const 
        {
            std::cout << "=== " << name << " の状態 ===\n";
            std::cout << "タイプ: " << attackComp.getType() << "\n";
            std::cout << "基本ダメージ: " << attackComp.getDamage() << "\n";
            std::cout << "耐久度: " << (durabilityComp.getDurabilityRatio() * 100) << "%\n";
            enchantmentComp.showEnchantments();
            std::cout << "\n";
        }
        
        std::string getName() const { return name; }
        bool isBroken() const { return durabilityComp.isBroken(); }
    };
    
    // ファクトリ関数で武器を作成
    std::unique_ptr<Weapon> createSword(const std::string& name, int damage) 
    {
        return std::make_unique<Weapon>(name, "Melee", damage, "で斬りつけた！", 100);
    }
    
    std::unique_ptr<Weapon> createBow(const std::string& name, int damage) 
    {
        return std::make_unique<Weapon>(name, "Ranged", damage, "で矢を放った！", 80);
    }
    
    std::unique_ptr<Weapon> createStaff(const std::string& name, int damage) 
    {
        return std::make_unique<Weapon>(name, "Magic", damage, "で魔法を放った！", 60);
    }
    
    // プレイヤークラス（コンポジション使用）
    class Player 
    {
    private:
        std::string name;
        std::unique_ptr<Weapon> mainWeapon;
        std::unique_ptr<Weapon> subWeapon;
        int health;
        
    public:
        Player(const std::string& n) : name(n), health(100) {}
        
        void equipMainWeapon(std::unique_ptr<Weapon> weapon) 
        {
            mainWeapon = std::move(weapon);
            std::cout << name << "は" << mainWeapon->getName() << "をメイン武器に装備しました\n";
        }
        
        void equipSubWeapon(std::unique_ptr<Weapon> weapon) 
        {
            subWeapon = std::move(weapon);
            std::cout << name << "は" << subWeapon->getName() << "をサブ武器に装備しました\n";
        }
        
        void attackWithMain() 
        {
            if (mainWeapon && !mainWeapon->isBroken()) {
                std::cout << name << "はメイン武器で攻撃！\n";
                mainWeapon->attack();
            } else {
                std::cout << name << "のメイン武器が使えません！\n";
            }
        }
        
        void attackWithSub() 
        {
            if (subWeapon && !subWeapon->isBroken()) {
                std::cout << name << "はサブ武器で攻撃！\n";
                subWeapon->attack();
            } else {
                std::cout << name << "のサブ武器が使えません！\n";
            }
        }
        
        void showWeaponStatus() const 
        {
            std::cout << "\n=== " << name << "の武器状態 ===\n";
            if (mainWeapon) {
                std::cout << "[メイン]\n";
                mainWeapon->showStatus();
            }
            if (subWeapon) {
                std::cout << "[サブ]\n";
                subWeapon->showStatus();
            }
        }
        
        void repairWeapons() 
        {
            if (mainWeapon) mainWeapon->repair(50);
            if (subWeapon) subWeapon->repair(50);
        }
    };
}

// 柔軟性のデモンストレーション
namespace FlexibilityDemo 
{
    using namespace CompositionApproach;
    
    // 新しいコンポーネントを追加するのが簡単
    class ElementalComponent 
    {
    private:
        std::string element;
        int elementalDamage;
        
    public:
        ElementalComponent(const std::string& elem, int damage)
            : element(elem), elementalDamage(damage) {}
        
        void applyElementalEffect() const 
        {
            std::cout << element << "属性攻撃！ 追加ダメージ: " 
                      << elementalDamage << "\n";
        }
        
        int getElementalDamage() const { return elementalDamage; }
        std::string getElement() const { return element; }
    };
    
    // コンポジションで簡単に機能拡張
    class EnhancedWeapon 
    {
    private:
        std::string name;
        AttackComponent attackComp;
        DurabilityComponent durabilityComp;
        EnchantmentComponent enchantmentComp;
        std::unique_ptr<ElementalComponent> elementalComp;
        
    public:
        EnhancedWeapon(const std::string& n, const std::string& attackType, 
                      int damage, const std::string& attackMsg, int durability)
            : name(n), 
              attackComp(attackType, damage, attackMsg),
              durabilityComp(durability) {}
        
        void addElementalComponent(const std::string& element, int damage) 
        {
            elementalComp = std::make_unique<ElementalComponent>(element, damage);
            std::cout << name << "に" << element << "属性を付与しました\n";
        }
        
        void attack() 
        {
            if (durabilityComp.isBroken()) {
                std::cout << name << "は壊れている！\n";
                return;
            }
            
            if (durabilityComp.use()) {
                int totalDamage = attackComp.getDamage() + enchantmentComp.getEnchantmentBonus();
                attackComp.performAttack(name);
                
                if (elementalComp) {
                    elementalComp->applyElementalEffect();
                    totalDamage += elementalComp->getElementalDamage();
                }
                
                if (enchantmentComp.getEnchantmentBonus() > 0 || elementalComp) {
                    std::cout << "合計ダメージ: " << totalDamage << "\n";
                }
            }
        }
    };
}

int main() 
{
    std::cout << "=== コンポジション vs 継承のデモ ===\n\n";
    
    // 継承アプローチのデモ
    std::cout << "1. 継承アプローチ\n";
    {
        using namespace InheritanceApproach;
        Player player("アリス");
        
        auto sword = std::make_unique<Sword>("鉄の剣", 30);
        player.equipWeapon(std::move(sword));
        player.attack();
        
        auto bow = std::make_unique<Bow>("エルフの弓", 25);
        player.equipWeapon(std::move(bow));
        player.attack();
    }
    
    std::cout << "\n---\n\n";
    
    // コンポジションアプローチのデモ
    std::cout << "2. コンポジションアプローチ\n";
    {
        using namespace CompositionApproach;
        Player player("ボブ");
        
        // 武器を作成して装備
        auto sword = createSword("ミスリルソード", 35);
        sword->addEnchantment("魂吸収", 2);
        sword->addEnchantment("炎上", 1);
        player.equipMainWeapon(std::move(sword));
        
        auto bow = createBow("シルバーボウ", 28);
        bow->addEnchantment("貫通", 3);
        player.equipSubWeapon(std::move(bow));
        
        // 攻撃テスト
        player.attackWithMain();
        player.attackWithSub();
        
        player.showWeaponStatus();
        
        // 耐久度を減らすために連続攻撃
        std::cout << "\n連続攻撃テスト:\n";
        for (int i = 0; i < 50; ++i) {
            player.attackWithMain();
        }
        
        std::cout << "\n修理後:\n";
        player.repairWeapons();
        player.attackWithMain();
    }
    
    std::cout << "\n---\n\n";
    
    // 柔軟性のデモ
    std::cout << "3. 柔軟性のデモ（属性システム追加）\n";
    {
        using namespace FlexibilityDemo;
        
        EnhancedWeapon flameStaff("炎の杖", "Magic", 20, "で炎の嘅を放った！", 80);
        flameStaff.addElementalComponent("炎", 15);
        
        EnhancedWeapon iceSword("氷の剣", "Melee", 25, "で凍りつく斬撃！", 120);
        iceSword.addElementalComponent("氷", 12);
        
        std::cout << "\n属性攻撃テスト:\n";
        flameStaff.attack();
        iceSword.attack();
    }
    
    return 0;
}

/*
学習ポイント：

コンポジションの利点：
1. 柔軽性 - 実行時にコンポーネントを変更できる
2. 再利用性 - コンポーネントを他のクラスでも使える
3. テスト容易性 - 各コンポーネントを独立してテスト可能
4. 単一責任の原則 - 各コンポーネントが一つの責任を持つ
5. 開放閉鎖の原則 - 拡張に開いているが修正には閉じている

継承の問題点：
1. 強い結合 - 基底クラスと派生クラスが密接に結合
2. 柔軟性の欠如 - コンパイル時に構造が決まる
3. 深い継承階層 - 複雑な階層構造になりやすい
4. 多重継承の問題 - ダイヤモンド問題など

使い分け：
- "is-a" 関係 → 継承（犬は動物である）
- "has-a" 関係 → コンポジション（車はエンジンを持つ）
- 行動の共有 → コンポジション（異なるクラスで同じ機能を使い回す）

モダンC++では "Composition over Inheritance" が推奨される
*/