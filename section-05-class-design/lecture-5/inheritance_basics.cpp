// 第5講: 継承の基本
// クラスの継承によるコードの再利用と階層構造の設計を学ぶ

#include <iostream>
#include <string>
#include <vector>
#include <memory>

// === 基底クラス：ゲームキャラクター ===
class GameCharacter 
{
protected:  // 継承先からアクセス可能
    std::string name;
    int level;
    int hp;
    int maxHp;
    int mp;
    int maxMp;

public:
    GameCharacter(const std::string& characterName, int startLevel = 1) 
        : name(characterName), level(startLevel)
    {
        maxHp = 50 + level * 10;
        hp = maxHp;
        maxMp = 20 + level * 5;
        mp = maxMp;
        std::cout << "GameCharacter「" << name << "」を作成しました" << std::endl;
    }
    
    virtual ~GameCharacter() 
    {
        std::cout << "GameCharacter「" << name << "」を削除しました" << std::endl;
    }
    
    // 基本的なメソッド
    const std::string& getName() const { return name; }
    int getLevel() const { return level; }
    int getHp() const { return hp; }
    int getMaxHp() const { return maxHp; }
    int getMp() const { return mp; }
    int getMaxMp() const { return maxMp; }
    
    virtual void displayStatus() const 
    {
        std::cout << "名前: " << name << ", レベル: " << level 
                  << ", HP: " << hp << "/" << maxHp 
                  << ", MP: " << mp << "/" << maxMp << std::endl;
    }
    
    virtual void attack() const 
    {
        std::cout << name << "の基本攻撃！" << std::endl;
    }
    
    virtual void levelUp() 
    {
        level++;
        int hpIncrease = 10;
        int mpIncrease = 5;
        
        maxHp += hpIncrease;
        hp += hpIncrease;
        maxMp += mpIncrease;
        mp += mpIncrease;
        
        std::cout << name << "がレベル" << level << "に上がった！" << std::endl;
    }
    
    void takeDamage(int damage) 
    {
        hp -= damage;
        if (hp < 0) hp = 0;
        std::cout << name << "は" << damage << "のダメージを受けた！（HP: " << hp << "）" << std::endl;
    }
    
    bool isAlive() const 
    {
        return hp > 0;
    }
};

// === 派生クラス1：戦士 ===
class Warrior : public GameCharacter 
{
private:
    int armor;
    int strength;

public:
    Warrior(const std::string& name, int startLevel = 1) 
        : GameCharacter(name, startLevel), armor(10 + level * 2), strength(15 + level * 3)
    {
        // 戦士は基本HPが多い
        maxHp += 20;
        hp = maxHp;
        std::cout << "戦士「" << name << "」が参戦！" << std::endl;
    }
    
    ~Warrior() 
    {
        std::cout << "戦士「" << name << "」が戦線離脱" << std::endl;
    }
    
    void displayStatus() const override 
    {
        GameCharacter::displayStatus();  // 基底クラスのメソッドを呼び出し
        std::cout << "  職業: 戦士, 装甲: " << armor << ", 筋力: " << strength << std::endl;
    }
    
    void attack() const override 
    {
        std::cout << name << "の剣撃！強力な物理攻撃！" << std::endl;
    }
    
    void levelUp() override 
    {
        GameCharacter::levelUp();  // 基底クラスの処理を実行
        
        // 戦士固有の成長
        armor += 3;
        strength += 4;
        maxHp += 15;  // 追加HP増加
        hp += 15;
        
        std::cout << "  戦士として成長：装甲+" << 3 << ", 筋力+" << 4 << std::endl;
    }
    
    // 戦士固有のメソッド
    void shieldBlock() const 
    {
        std::cout << name << "は盾で攻撃を防いだ！" << std::endl;
    }
    
    void powerSlash() const 
    {
        std::cout << name << "の必殺技：パワースラッシュ！" << std::endl;
    }
    
    int getArmor() const { return armor; }
    int getStrength() const { return strength; }
};

// === 派生クラス2：魔法使い ===
class Mage : public GameCharacter 
{
private:
    int intelligence;
    int manaRegeneration;

public:
    Mage(const std::string& name, int startLevel = 1) 
        : GameCharacter(name, startLevel), intelligence(20 + level * 4), manaRegeneration(3 + level)
    {
        // 魔法使いは基本MPが多い
        maxMp += 30;
        mp = maxMp;
        std::cout << "魔法使い「" << name << "」が参戦！" << std::endl;
    }
    
    ~Mage() 
    {
        std::cout << "魔法使い「" << name << "」が戦線離脱" << std::endl;
    }
    
    void displayStatus() const override 
    {
        GameCharacter::displayStatus();
        std::cout << "  職業: 魔法使い, 知力: " << intelligence 
                  << ", MP回復: " << manaRegeneration << std::endl;
    }
    
    void attack() const override 
    {
        if (mp >= 10) 
        {
            std::cout << name << "のファイアボール！魔法攻撃！" << std::endl;
            // 実際にはMPを消費する処理
        }
        else 
        {
            std::cout << name << "のMP不足...杖で殴った" << std::endl;
        }
    }
    
    void levelUp() override 
    {
        GameCharacter::levelUp();
        
        // 魔法使い固有の成長
        intelligence += 5;
        manaRegeneration += 2;
        maxMp += 20;  // 追加MP増加
        mp += 20;
        
        std::cout << "  魔法使いとして成長：知力+" << 5 << ", MP回復+" << 2 << std::endl;
    }
    
    // 魔法使い固有のメソッド
    void castHeal() const 
    {
        std::cout << name << "は回復魔法を唱えた！" << std::endl;
    }
    
    void meteorStorm() const 
    {
        std::cout << name << "の必殺技：メテオストーム！" << std::endl;
    }
    
    int getIntelligence() const { return intelligence; }
    int getManaRegeneration() const { return manaRegeneration; }
};

// === 派生クラス3：盗賊 ===
class Thief : public GameCharacter 
{
private:
    int agility;
    int stealth;

public:
    Thief(const std::string& name, int startLevel = 1) 
        : GameCharacter(name, startLevel), agility(18 + level * 3), stealth(12 + level * 2)
    {
        std::cout << "盗賊「" << name << "」が参戦！" << std::endl;
    }
    
    ~Thief() 
    {
        std::cout << "盗賊「" << name << "」が戦線離脱" << std::endl;
    }
    
    void displayStatus() const override 
    {
        GameCharacter::displayStatus();
        std::cout << "  職業: 盗賊, 敏捷: " << agility << ", 隠密: " << stealth << std::endl;
    }
    
    void attack() const override 
    {
        std::cout << name << "の素早い短剣攻撃！" << std::endl;
    }
    
    void levelUp() override 
    {
        GameCharacter::levelUp();
        
        // 盗賊固有の成長
        agility += 4;
        stealth += 3;
        
        std::cout << "  盗賊として成長：敏捷+" << 4 << ", 隠密+" << 3 << std::endl;
    }
    
    // 盗賊固有のメソッド
    void hide() const 
    {
        std::cout << name << "は影に隠れた！" << std::endl;
    }
    
    void backstab() const 
    {
        std::cout << name << "の必殺技：バックスタブ！" << std::endl;
    }
    
    void pickLock() const 
    {
        std::cout << name << "は鍵を開けた！" << std::endl;
    }
    
    int getAgility() const { return agility; }
    int getStealth() const { return stealth; }
};

// === さらなる継承：上級戦士 ===
class Paladin : public Warrior 
{
private:
    int faith;

public:
    Paladin(const std::string& name, int startLevel = 1) 
        : Warrior(name, startLevel), faith(10 + level * 2)
    {
        // パラディンは追加でMPも持つ
        maxMp += 15;
        mp = maxMp;
        std::cout << "聖騎士「" << name << "」が降臨！" << std::endl;
    }
    
    ~Paladin() 
    {
        std::cout << "聖騎士「" << name << "」が昇天" << std::endl;
    }
    
    void displayStatus() const override 
    {
        Warrior::displayStatus();  // Warriorのメソッドを呼び出し
        std::cout << "  上級職: 聖騎士, 信仰: " << faith << std::endl;
    }
    
    void attack() const override 
    {
        std::cout << name << "の聖なる剣撃！光の力が宿る！" << std::endl;
    }
    
    void levelUp() override 
    {
        Warrior::levelUp();  // Warriorの成長処理
        
        // パラディン固有の成長
        faith += 3;
        maxMp += 10;
        mp += 10;
        
        std::cout << "  聖騎士として成長：信仰+" << 3 << std::endl;
    }
    
    // パラディン固有のメソッド
    void holyShield() const 
    {
        std::cout << name << "は聖なる護りを発動した！" << std::endl;
    }
    
    void divineJudgment() const 
    {
        std::cout << name << "の奥義：神罰！" << std::endl;
    }
    
    int getFaith() const { return faith; }
};

// === パーティ管理クラス ===
class Party 
{
private:
    std::vector<std::unique_ptr<GameCharacter>> members;

public:
    void addMember(std::unique_ptr<GameCharacter> character) 
    {
        std::cout << character->getName() << "がパーティに加わった！" << std::endl;
        members.push_back(std::move(character));
    }
    
    void displayParty() const 
    {
        std::cout << "\n=== パーティメンバー ===" << std::endl;
        for (const auto& member : members) 
        {
            member->displayStatus();
            std::cout << std::endl;
        }
    }
    
    void partyAttack() const 
    {
        std::cout << "\n=== パーティ全員で攻撃！ ===" << std::endl;
        for (const auto& member : members) 
        {
            member->attack();
        }
    }
    
    void levelUpAll() 
    {
        std::cout << "\n=== パーティ全員レベルアップ！ ===" << std::endl;
        for (auto& member : members) 
        {
            member->levelUp();
        }
    }
    
    size_t size() const 
    {
        return members.size();
    }
};

void demonstrateInheritance() 
{
    std::cout << "\n=== 継承の基本デモンストレーション ===" << std::endl;
    
    // 各職業のキャラクター作成
    Warrior warrior("アーサー", 3);
    Mage mage("マーリン", 3);
    Thief thief("ロビン", 3);
    Paladin paladin("ガラハッド", 3);
    
    std::cout << "\n--- 初期ステータス ---" << std::endl;
    warrior.displayStatus();
    mage.displayStatus();
    thief.displayStatus();
    paladin.displayStatus();
    
    std::cout << "\n--- 各職業の攻撃 ---" << std::endl;
    warrior.attack();
    mage.attack();
    thief.attack();
    paladin.attack();
    
    std::cout << "\n--- 職業固有スキル ---" << std::endl;
    warrior.shieldBlock();
    warrior.powerSlash();
    
    mage.castHeal();
    mage.meteorStorm();
    
    thief.hide();
    thief.backstab();
    thief.pickLock();
    
    paladin.holyShield();
    paladin.divineJudgment();
    
    std::cout << "\n--- レベルアップ ---" << std::endl;
    warrior.levelUp();
    mage.levelUp();
    thief.levelUp();
    paladin.levelUp();
}

void demonstratePolymorphism() 
{
    std::cout << "\n=== ポリモーフィズムのデモンストレーション ===" << std::endl;
    
    Party adventureParty;
    
    // 様々な職業をパーティに追加
    adventureParty.addMember(std::make_unique<Warrior>("テスト戦士"));
    adventureParty.addMember(std::make_unique<Mage>("テスト魔法使い"));
    adventureParty.addMember(std::make_unique<Thief>("テスト盗賊"));
    adventureParty.addMember(std::make_unique<Paladin>("テスト聖騎士"));
    
    // 基底クラスのポインタ経由で派生クラスのメソッドが呼ばれる
    adventureParty.displayParty();
    adventureParty.partyAttack();
    adventureParty.levelUpAll();
    
    std::cout << "\nレベルアップ後のステータス:" << std::endl;
    adventureParty.displayParty();
}

int main() 
{
    std::cout << "=== 継承の基本と応用 ===" << std::endl;
    
    // 基本的な継承の動作確認
    demonstrateInheritance();
    
    // ポリモーフィズムの確認
    demonstratePolymorphism();
    
    return 0;
}

/*
    まとめ:
    
    1. 継承の基本:
       - class Derived : public Base の構文
       - protected メンバは継承先からアクセス可能
       - コンストラクタは基底→派生の順で呼ばれる
       - デストラクタは派生→基底の順で呼ばれる
    
    2. メソッドのオーバーライド:
       - virtual キーワードで仮想関数を定義
       - override キーワードで明示的にオーバーライド
       - 基底クラスのメソッドを Base::method() で呼び出し可能
    
    3. 継承の種類:
       - public継承: is-a関係（Warrior is a GameCharacter）
       - protected継承: 制限付きis-a関係
       - private継承: implemented-in-terms-of関係
    
    4. 多重継承:
       - 複数の基底クラスから継承可能
       - ダイヤモンド継承問題に注意
       - virtual継承で解決
    
    5. ポリモーフィズム:
       - 基底クラスのポインタで派生クラスを操作
       - 実行時に適切なメソッドが呼ばれる
       - virtual デストラクタが重要
    
    ベストプラクティス:
    - デストラクタはvirtualに
    - 継承よりもコンポジションを検討
    - LSP（リスコフの置換原則）を守る
    - 深い継承階層は避ける
    - 適切なアクセス指定子を使用
    
    ゲーム開発での活用:
    - キャラクターのクラス体系
    - ゲームオブジェクトの階層
    - UI要素の継承構造
    - 武器・アイテムの分類
    - 敵AIの種類分け
*/