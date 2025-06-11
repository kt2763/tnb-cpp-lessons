// 第2講: コンストラクタとデストラクタ
// オブジェクトの生成と破棄の際の初期化・終了処理を学ぶ

#include <iostream>
#include <string>
#include <vector>
#include <memory>

// === 基本的なコンストラクタとデストラクタ ===
class GameCharacter 
{
private:
    std::string name;
    int level;
    int hp;
    int maxHp;
    int* dynamicData;  // 動的メモリの例
    
public:
    // === デフォルトコンストラクタ ===
    GameCharacter() 
    {
        std::cout << "デフォルトコンストラクタが呼ばれました" << std::endl;
        name = "名無し";
        level = 1;
        maxHp = 100;
        hp = maxHp;
        dynamicData = new int(42);  // 動的メモリ確保
        std::cout << "キャラクター「" << name << "」が作成されました" << std::endl;
    }
    
    // === パラメータ付きコンストラクタ ===
    GameCharacter(const std::string& characterName) 
    {
        std::cout << "パラメータ付きコンストラクタが呼ばれました" << std::endl;
        name = characterName;
        level = 1;
        maxHp = 100;
        hp = maxHp;
        dynamicData = new int(42);
        std::cout << "キャラクター「" << name << "」が作成されました" << std::endl;
    }
    
    // === 複数パラメータのコンストラクタ ===
    GameCharacter(const std::string& characterName, int startLevel, int health) 
    {
        std::cout << "複数パラメータコンストラクタが呼ばれました" << std::endl;
        name = characterName;
        level = startLevel;
        maxHp = health;
        hp = maxHp;
        dynamicData = new int(level * 10);
        std::cout << "キャラクター「" << name << "」（Lv." << level << ", HP:" << hp << "）が作成されました" << std::endl;
    }
    
    // === デストラクタ ===
    ~GameCharacter() 
    {
        std::cout << "デストラクタが呼ばれました" << std::endl;
        std::cout << "キャラクター「" << name << "」を削除しています..." << std::endl;
        
        // 動的メモリの解放
        delete dynamicData;
        std::cout << "動的メモリを解放しました" << std::endl;
        
        std::cout << "キャラクター「" << name << "」が削除されました" << std::endl;
    }
    
    // ゲッター
    const std::string& getName() const { return name; }
    int getLevel() const { return level; }
    int getHp() const { return hp; }
    int getMaxHp() const { return maxHp; }
    
    void displayStatus() const 
    {
        std::cout << "名前: " << name << ", レベル: " << level 
                  << ", HP: " << hp << "/" << maxHp 
                  << ", データ: " << *dynamicData << std::endl;
    }
};

// === 初期化リストを使用したコンストラクタ ===
class Weapon 
{
private:
    const std::string name;        // const メンバ
    const int baseAttack;          // const メンバ
    int durability;
    int maxDurability;
    static int weaponCount;        // static メンバ

public:
    // 初期化リストを使用（const メンバの初期化に必要）
    Weapon(const std::string& weaponName, int attack, int dur) 
        : name(weaponName), baseAttack(attack), durability(dur), maxDurability(dur)
    {
        weaponCount++;
        std::cout << "武器「" << name << "」が作成されました（総数: " << weaponCount << "）" << std::endl;
    }
    
    // デストラクタ
    ~Weapon() 
    {
        weaponCount--;
        std::cout << "武器「" << name << "」が破棄されました（残り: " << weaponCount << "）" << std::endl;
    }
    
    // ゲッター
    const std::string& getName() const { return name; }
    int getBaseAttack() const { return baseAttack; }
    int getDurability() const { return durability; }
    int getMaxDurability() const { return maxDurability; }
    static int getWeaponCount() { return weaponCount; }
    
    void displayInfo() const 
    {
        std::cout << "武器: " << name << ", 攻撃力: " << baseAttack 
                  << ", 耐久度: " << durability << "/" << maxDurability << std::endl;
    }
    
    bool use() 
    {
        if (durability > 0) 
        {
            durability--;
            return true;
        }
        return false;
    }
};

// static メンバの定義
int Weapon::weaponCount = 0;

// === コンストラクタのオーバーロード ===
class Monster 
{
private:
    std::string name;
    int hp;
    int attack;
    int defense;
    std::string type;

public:
    // タイプ別のコンストラクタ
    Monster() : name("野良モンスター"), hp(50), attack(10), defense(5), type("一般") 
    {
        std::cout << "一般モンスターが出現！" << std::endl;
    }
    
    Monster(const std::string& monsterName) 
        : name(monsterName), hp(50), attack(10), defense(5), type("一般") 
    {
        std::cout << "「" << name << "」が出現！" << std::endl;
    }
    
    Monster(const std::string& monsterName, const std::string& monsterType) 
        : name(monsterName), type(monsterType)
    {
        // タイプに応じて能力値を設定
        if (type == "ボス") 
        {
            hp = 200;
            attack = 30;
            defense = 20;
        }
        else if (type == "エリート") 
        {
            hp = 100;
            attack = 20;
            defense = 15;
        }
        else if (type == "雑魚") 
        {
            hp = 30;
            attack = 8;
            defense = 3;
        }
        else 
        {
            hp = 50;
            attack = 10;
            defense = 5;
            type = "一般";
        }
        
        std::cout << type << "モンスター「" << name << "」が出現！" << std::endl;
    }
    
    Monster(const std::string& monsterName, int health, int attackPower, int defensePower) 
        : name(monsterName), hp(health), attack(attackPower), defense(defensePower), type("カスタム")
    {
        std::cout << "カスタムモンスター「" << name << "」が出現！" << std::endl;
    }
    
    ~Monster() 
    {
        std::cout << "「" << name << "」が倒れた..." << std::endl;
    }
    
    void displayStatus() const 
    {
        std::cout << "[" << type << "] " << name 
                  << " - HP:" << hp << " ATK:" << attack << " DEF:" << defense << std::endl;
    }
};

// === RAIIパターンの実装例 ===
class GameResource 
{
private:
    std::string resourceName;
    bool isLoaded;
    int* resourceData;

public:
    // コンストラクタでリソースを取得
    GameResource(const std::string& name) : resourceName(name), isLoaded(false), resourceData(nullptr)
    {
        std::cout << "リソース「" << resourceName << "」の読み込みを開始..." << std::endl;
        
        // リソースの読み込みをシミュレート
        resourceData = new int[100];  // 仮のデータ
        for (int i = 0; i < 100; i++) 
        {
            resourceData[i] = i;
        }
        
        isLoaded = true;
        std::cout << "リソース「" << resourceName << "」の読み込み完了！" << std::endl;
    }
    
    // デストラクタでリソースを解放
    ~GameResource() 
    {
        std::cout << "リソース「" << resourceName << "」を解放中..." << std::endl;
        
        if (resourceData) 
        {
            delete[] resourceData;
            resourceData = nullptr;
        }
        
        isLoaded = false;
        std::cout << "リソース「" << resourceName << "」の解放完了" << std::endl;
    }
    
    bool isReady() const 
    {
        return isLoaded && resourceData != nullptr;
    }
    
    void useResource() const 
    {
        if (isReady()) 
        {
            std::cout << "リソース「" << resourceName << "」を使用中..." << std::endl;
        }
        else 
        {
            std::cout << "エラー: リソース「" << resourceName << "」は利用できません" << std::endl;
        }
    }
};

// === ゲームシーンクラス（複雑な初期化の例） ===
class GameScene 
{
private:
    std::string sceneName;
    std::vector<std::unique_ptr<GameCharacter>> characters;
    std::vector<std::unique_ptr<Monster>> monsters;
    std::unique_ptr<GameResource> backgroundMusic;
    std::unique_ptr<GameResource> backgroundImage;

public:
    GameScene(const std::string& name) : sceneName(name)
    {
        std::cout << "\n=== シーン「" << sceneName << "」の初期化開始 ===" << std::endl;
        
        // リソースの読み込み
        backgroundMusic = std::make_unique<GameResource>("bgm_" + sceneName + ".ogg");
        backgroundImage = std::make_unique<GameResource>("bg_" + sceneName + ".png");
        
        // キャラクターの生成
        characters.push_back(std::make_unique<GameCharacter>("主人公"));
        characters.push_back(std::make_unique<GameCharacter>("仲間", 5, 150));
        
        // モンスターの生成
        monsters.push_back(std::make_unique<Monster>("ゴブリン", "雑魚"));
        monsters.push_back(std::make_unique<Monster>("オーガ", "エリート"));
        
        std::cout << "=== シーン「" << sceneName << "」の初期化完了 ===" << std::endl;
    }
    
    ~GameScene() 
    {
        std::cout << "\n=== シーン「" << sceneName << "」の終了処理開始 ===" << std::endl;
        
        // vectorのクリア（unique_ptrのデストラクタが自動的に呼ばれる）
        monsters.clear();
        characters.clear();
        
        // unique_ptrのリセット（デストラクタが自動的に呼ばれる）
        backgroundImage.reset();
        backgroundMusic.reset();
        
        std::cout << "=== シーン「" << sceneName << "」の終了処理完了 ===" << std::endl;
    }
    
    void startScene() 
    {
        std::cout << "\n--- シーン「" << sceneName << "」開始 ---" << std::endl;
        
        if (backgroundMusic && backgroundMusic->isReady()) 
        {
            backgroundMusic->useResource();
        }
        
        if (backgroundImage && backgroundImage->isReady()) 
        {
            backgroundImage->useResource();
        }
        
        std::cout << "\nキャラクター一覧:" << std::endl;
        for (const auto& character : characters) 
        {
            character->displayStatus();
        }
        
        std::cout << "\nモンスター一覧:" << std::endl;
        for (const auto& monster : monsters) 
        {
            monster->displayStatus();
        }
    }
};

void demonstrateConstructors() 
{
    std::cout << "\n=== コンストラクタのデモンストレーション ===" << std::endl;
    
    // 各種コンストラクタの呼び出し
    GameCharacter char1;                              // デフォルトコンストラクタ
    GameCharacter char2("勇者");                      // パラメータ付きコンストラクタ
    GameCharacter char3("魔法使い", 5, 80);           // 複数パラメータコンストラクタ
    
    char1.displayStatus();
    char2.displayStatus();
    char3.displayStatus();
    
    std::cout << "\n=== 武器の作成 ===" << std::endl;
    Weapon sword("ロングソード", 25, 10);
    Weapon staff("魔法の杖", 15, 8);
    
    std::cout << "現在の武器数: " << Weapon::getWeaponCount() << std::endl;
    
    sword.displayInfo();
    staff.displayInfo();
    
    std::cout << "\n=== モンスターの作成 ===" << std::endl;
    Monster monster1;                                    // デフォルト
    Monster monster2("スライム");                        // 名前のみ
    Monster monster3("ドラゴン", "ボス");                // 名前とタイプ
    Monster monster4("カスタムモンスター", 300, 50, 30); // 完全カスタム
    
    monster1.displayStatus();
    monster2.displayStatus();
    monster3.displayStatus();
    monster4.displayStatus();
}

void demonstrateRAII() 
{
    std::cout << "\n=== RAIIパターンのデモンストレーション ===" << std::endl;
    
    {
        std::cout << "スコープ開始" << std::endl;
        GameResource texture("player_texture.png");
        GameResource sound("attack_sound.wav");
        
        if (texture.isReady()) 
        {
            texture.useResource();
        }
        
        if (sound.isReady()) 
        {
            sound.useResource();
        }
        
        std::cout << "スコープ終了直前" << std::endl;
    }  // ここでデストラクタが自動的に呼ばれ、リソースが解放される
    
    std::cout << "スコープ終了後" << std::endl;
}

int main() 
{
    std::cout << "=== コンストラクタとデストラクタの詳細 ===" << std::endl;
    
    // 基本的なコンストラクタ・デストラクタの動作
    demonstrateConstructors();
    
    // RAIIパターンの実演
    demonstrateRAII();
    
    // ゲームシーンによる複雑な初期化・終了処理
    std::cout << "\n=== 複雑な初期化・終了処理の例 ===" << std::endl;
    
    {
        GameScene battleScene("戦闘フィールド");
        battleScene.startScene();
        
        std::cout << "\n--- 何かゲーム処理 ---" << std::endl;
        
    }  // ここでGameSceneのデストラクタが呼ばれ、全てが適切に終了される
    
    std::cout << "\n最終的な武器数: " << Weapon::getWeaponCount() << std::endl;
    
    return 0;
}

/*
    まとめ:
    
    1. コンストラクタ:
       - オブジェクト作成時に自動的に呼ばれる
       - メンバ変数の初期化を行う
       - 複数のオーバーロードが可能
       - 初期化リストでconst/referenceメンバを初期化
    
    2. デストラクタ:
       - オブジェクト破棄時に自動的に呼ばれる
       - リソースの解放を行う
       - virtual指定で継承時の適切な呼び出しを保証
    
    3. RAIIパターン:
       - Resource Acquisition Is Initialization
       - リソースの取得と解放を自動化
       - 例外安全性を保証
       - メモリリーク防止
    
    4. ベストプラクティス:
       - 初期化リストの使用を推奨
       - 動的メモリはスマートポインタを使用
       - リソース管理はRAIIパターンで
       - デストラクタでの例外発生を避ける
    
    ゲーム開発での活用:
    - ゲームオブジェクトの生成・破棄
    - リソース（テクスチャ、音声）の管理
    - シーンの初期化・終了処理
    - 一時的なエフェクトの管理
    - セーブデータの読み込み・書き込み
*/