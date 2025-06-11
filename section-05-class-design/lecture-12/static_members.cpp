// Lecture 12: 静的メンバー
// Static Members in C++

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>

// 静的メンバーの基本例：ゲーム内のプレイヤー管理
class Player 
{
private:
    // インスタンスメンバー（各プレイヤー固有）
    std::string name;
    int level;
    int health;
    int playerId;
    
    // 静的メンバー変数（クラス全体で共有）
    static int totalPlayers;          // 作成されたプレイヤー総数
    static int nextPlayerId;          // 次に割り当てるID
    static std::string serverName;    // サーバー名
    static bool gameStarted;          // ゲーム開始フラグ
    
public:
    // コンストラクタ
    Player(const std::string& playerName, int startLevel = 1) 
        : name(playerName), level(startLevel), health(100)
    {
        playerId = nextPlayerId++;
        totalPlayers++;
        
        std::cout << "プレイヤー '" << name << "' (ID: " << playerId 
                  << ") がサーバー '" << serverName << "' に参加しました\n";
        std::cout << "現在のプレイヤー数: " << totalPlayers << "\n";
    }
    
    // デストラクタ
    ~Player() 
    {
        totalPlayers--;
        std::cout << "プレイヤー '" << name << "' がサーバーから退出しました\n";
        std::cout << "残りプレイヤー数: " << totalPlayers << "\n";
    }
    
    // インスタンスメンバー関数
    void showInfo() const 
    {
        std::cout << "=== プレイヤー情報 ===\n";
        std::cout << "名前: " << name << " (ID: " << playerId << ")\n";
        std::cout << "レベル: " << level << ", HP: " << health << "\n";
        std::cout << "サーバー: " << serverName << "\n";
    }
    
    void levelUp() 
    {
        level++;
        health += 20;
        std::cout << name << " がレベル " << level << " にアップ！\n";
    }
    
    // 静的メンバー関数（クラス全体の情報にアクセス）
    static int getTotalPlayers() 
    {
        return totalPlayers;
    }
    
    static void setServerName(const std::string& newServerName) 
    {
        serverName = newServerName;
        std::cout << "サーバー名を '" << serverName << "' に変更しました\n";
    }
    
    static std::string getServerName() 
    {
        return serverName;
    }
    
    static void startGame() 
    {
        if (!gameStarted) {
            gameStarted = true;
            std::cout << "ゲームが開始されました！\n";
            std::cout << "参加プレイヤー数: " << totalPlayers << "\n";
        } else {
            std::cout << "ゲームは既に開始されています\n";
        }
    }
    
    static bool isGameStarted() 
    {
        return gameStarted;
    }
    
    static void showServerStats() 
    {
        std::cout << "\n=== サーバー統計 ===\n";
        std::cout << "サーバー名: " << serverName << "\n";
        std::cout << "現在のプレイヤー数: " << totalPlayers << "\n";
        std::cout << "次のプレイヤーID: " << nextPlayerId << "\n";
        std::cout << "ゲーム状態: " << (gameStarted ? "開始済み" : "待機中") << "\n";
    }
    
    // ゲッター
    std::string getName() const { return name; }
    int getPlayerId() const { return playerId; }
    int getLevel() const { return level; }
};

// 静的メンバー変数の定義（クラス外で必須）
int Player::totalPlayers = 0;
int Player::nextPlayerId = 1001;
std::string Player::serverName = "GameServer-01";
bool Player::gameStarted = false;

// シングルトンパターンの例：ゲーム設定管理
class GameConfig 
{
private:
    // 静的なインスタンスポインタ
    static GameConfig* instance;
    
    // 設定値
    int maxPlayers;
    int mapSize;
    std::string gameMode;
    bool debugMode;
    
    // プライベートコンストラクタ（シングルトン）
    GameConfig() 
        : maxPlayers(100), mapSize(1000), gameMode("Normal"), debugMode(false)
    {
        std::cout << "GameConfig インスタンスが作成されました\n";
    }
    
    // コピー禁止
    GameConfig(const GameConfig&) = delete;
    GameConfig& operator=(const GameConfig&) = delete;
    
public:
    // インスタンス取得
    static GameConfig* getInstance() 
    {
        if (instance == nullptr) {
            instance = new GameConfig();
        }
        return instance;
    }
    
    // インスタンス削除
    static void destroyInstance() 
    {
        delete instance;
        instance = nullptr;
        std::cout << "GameConfig インスタンスが削除されました\n";
    }
    
    // 設定アクセサー
    void setMaxPlayers(int max) { maxPlayers = max; }
    void setMapSize(int size) { mapSize = size; }
    void setGameMode(const std::string& mode) { gameMode = mode; }
    void setDebugMode(bool debug) { debugMode = debug; }
    
    int getMaxPlayers() const { return maxPlayers; }
    int getMapSize() const { return mapSize; }
    std::string getGameMode() const { return gameMode; }
    bool isDebugMode() const { return debugMode; }
    
    void showConfig() const 
    {
        std::cout << "\n=== ゲーム設定 ===\n";
        std::cout << "最大プレイヤー数: " << maxPlayers << "\n";
        std::cout << "マップサイズ: " << mapSize << "x" << mapSize << "\n";
        std::cout << "ゲームモード: " << gameMode << "\n";
        std::cout << "デバッグモード: " << (debugMode ? "ON" : "OFF") << "\n";
    }
};

// 静的メンバー変数の初期化
GameConfig* GameConfig::instance = nullptr;

// 静的メンバーでファクトリーパターン
class Weapon 
{
protected:
    std::string name;
    int damage;
    std::string type;
    
    Weapon(const std::string& n, int d, const std::string& t)
        : name(n), damage(d), type(t) {}
    
public:
    virtual ~Weapon() = default;
    
    virtual void attack() const 
    {
        std::cout << name << " で攻撃！ ダメージ: " << damage << "\n";
    }
    
    std::string getName() const { return name; }
    int getDamage() const { return damage; }
    std::string getType() const { return type; }
    
    // ファクトリーメソッド（静的）
    static std::unique_ptr<Weapon> createSword(const std::string& name, int damage);
    static std::unique_ptr<Weapon> createBow(const std::string& name, int damage);
    static std::unique_ptr<Weapon> createStaff(const std::string& name, int damage);
    
    // 武器カウンター
    static int getWeaponCount() { return weaponCount; }
    
private:
    static int weaponCount;
};

int Weapon::weaponCount = 0;

// 具体的な武器クラス
class Sword : public Weapon 
{
public:
    Sword(const std::string& name, int damage) : Weapon(name, damage, "Sword") 
    {
        weaponCount++;
    }
    
    void attack() const override 
    {
        std::cout << name << " で斬りつけた！ ダメージ: " << damage << "\n";
    }
};

class Bow : public Weapon 
{
public:
    Bow(const std::string& name, int damage) : Weapon(name, damage, "Bow") 
    {
        weaponCount++;
    }
    
    void attack() const override 
    {
        std::cout << name << " で矢を放った！ ダメージ: " << damage << "\n";
    }
};

class Staff : public Weapon 
{
public:
    Staff(const std::string& name, int damage) : Weapon(name, damage, "Staff") 
    {
        weaponCount++;
    }
    
    void attack() const override 
    {
        std::cout << name << " で魔法を唱えた！ ダメージ: " << damage << "\n";
    }
};

// ファクトリーメソッドの実装
std::unique_ptr<Weapon> Weapon::createSword(const std::string& name, int damage) 
{
    return std::make_unique<Sword>(name, damage);
}

std::unique_ptr<Weapon> Weapon::createBow(const std::string& name, int damage) 
{
    return std::make_unique<Bow>(name, damage);
}

std::unique_ptr<Weapon> Weapon::createStaff(const std::string& name, int damage) 
{
    return std::make_unique<Staff>(name, damage);
}

// 静的データメンバーでキャッシュ管理
class ResourceManager 
{
private:
    static std::map<std::string, std::string> textureCache;
    static std::map<std::string, std::string> soundCache;
    static int cacheHits;
    static int cacheMisses;
    
public:
    // テクスチャリソース管理
    static std::string loadTexture(const std::string& filename) 
    {
        auto it = textureCache.find(filename);
        if (it != textureCache.end()) {
            cacheHits++;
            std::cout << "[CACHE HIT] テクスチャ '" << filename << "' をキャッシュから取得\n";
            return it->second;
        }
        
        cacheMisses++;
        std::cout << "[CACHE MISS] テクスチャ '" << filename << "' をロード中...\n";
        
        // 実際のロード処理をシミュレート
        std::string textureData = "TextureData_" + filename;
        textureCache[filename] = textureData;
        
        std::cout << "テクスチャ '" << filename << "' をキャッシュに保存\n";
        return textureData;
    }
    
    // サウンドリソース管理
    static std::string loadSound(const std::string& filename) 
    {
        auto it = soundCache.find(filename);
        if (it != soundCache.end()) {
            cacheHits++;
            std::cout << "[CACHE HIT] サウンド '" << filename << "' をキャッシュから取得\n";
            return it->second;
        }
        
        cacheMisses++;
        std::cout << "[CACHE MISS] サウンド '" << filename << "' をロード中...\n";
        
        std::string soundData = "SoundData_" + filename;
        soundCache[filename] = soundData;
        
        std::cout << "サウンド '" << filename << "' をキャッシュに保存\n";
        return soundData;
    }
    
    // キャッシュ統計
    static void showCacheStats() 
    {
        std::cout << "\n=== キャッシュ統計 ===\n";
        std::cout << "テクスチャキャッシュサイズ: " << textureCache.size() << "\n";
        std::cout << "サウンドキャッシュサイズ: " << soundCache.size() << "\n";
        std::cout << "キャッシュヒット: " << cacheHits << "\n";
        std::cout << "キャッシュミス: " << cacheMisses << "\n";
        
        if (cacheHits + cacheMisses > 0) {
            double hitRate = (double)cacheHits / (cacheHits + cacheMisses) * 100;
            std::cout << "ヒット率: " << hitRate << "%\n";
        }
    }
    
    // キャッシュクリア
    static void clearCache() 
    {
        textureCache.clear();
        soundCache.clear();
        std::cout << "キャッシュをクリアしました\n";
    }
};

// 静的メンバー変数の定義
std::map<std::string, std::string> ResourceManager::textureCache;
std::map<std::string, std::string> ResourceManager::soundCache;
int ResourceManager::cacheHits = 0;
int ResourceManager::cacheMisses = 0;

// 静的メンバーでIDジェネレーター
class IDGenerator 
{
private:
    static int currentId;
    
public:
    static int getNextId() 
    {
        return ++currentId;
    }
    
    static int getCurrentId() 
    {
        return currentId;
    }
    
    static void resetId() 
    {
        currentId = 0;
        std::cout << "ID カウンターをリセットしました\n";
    }
};

int IDGenerator::currentId = 0;

int main() 
{
    std::cout << "=== 静的メンバーのデモ ===\n\n";
    
    // 静的メンバー変数と関数の基本使用
    std::cout << "1. 静的メンバー変数と関数の基本\n";
    Player::setServerName("MyGameServer");
    Player::showServerStats();
    
    std::cout << "\nプレイヤー作成:\n";
    {
        Player player1("勇者", 5);
        Player player2("魔法使い", 3);
        Player player3("戦士", 7);
        
        Player::startGame();
        Player::showServerStats();
        
        player1.showInfo();
        player2.levelUp();
    } // ここでプレイヤーがスコープを抜ける
    
    std::cout << "\nプレイヤー削除後:\n";
    Player::showServerStats();
    
    std::cout << "\n---\n\n";
    
    // シングルトンパターン
    std::cout << "2. シングルトンパターン\n";
    GameConfig* config1 = GameConfig::getInstance();
    GameConfig* config2 = GameConfig::getInstance();
    
    std::cout << "同じインスタンス？ " << (config1 == config2 ? "はい" : "いいえ") << "\n";
    
    config1->setMaxPlayers(50);
    config1->setGameMode("Hard");
    config1->setDebugMode(true);
    
    config2->showConfig(); // config1と同じ設定が表示される
    
    GameConfig::destroyInstance();
    
    std::cout << "\n---\n\n";
    
    // ファクトリーパターン
    std::cout << "3. ファクトリーパターン\n";
    std::vector<std::unique_ptr<Weapon>> weapons;
    
    weapons.push_back(Weapon::createSword("エクスカリバー", 50));
    weapons.push_back(Weapon::createBow("エルフの弓", 35));
    weapons.push_back(Weapon::createStaff("賢者の杖", 40));
    
    std::cout << "武器作成完了。総武器数: " << Weapon::getWeaponCount() << "\n";
    
    std::cout << "\n攻撃テスト:\n";
    for (const auto& weapon : weapons) {
        weapon->attack();
    }
    
    std::cout << "\n---\n\n";
    
    // リソース管理とキャッシュ
    std::cout << "4. リソース管理とキャッシュ\n";
    
    // 初回ロード（キャッシュミス）
    ResourceManager::loadTexture("player.png");
    ResourceManager::loadTexture("enemy.png");
    ResourceManager::loadSound("attack.wav");
    
    // 2回目ロード（キャッシュヒット）
    ResourceManager::loadTexture("player.png");
    ResourceManager::loadSound("attack.wav");
    ResourceManager::loadTexture("background.jpg");
    
    ResourceManager::showCacheStats();
    
    std::cout << "\n---\n\n";
    
    // IDジェネレーター
    std::cout << "5. IDジェネレーター\n";
    std::cout << "次のID: " << IDGenerator::getNextId() << "\n";
    std::cout << "次のID: " << IDGenerator::getNextId() << "\n";
    std::cout << "次のID: " << IDGenerator::getNextId() << "\n";
    std::cout << "現在のID: " << IDGenerator::getCurrentId() << "\n";
    
    IDGenerator::resetId();
    std::cout << "リセット後の次のID: " << IDGenerator::getNextId() << "\n";
    
    return 0;
}

/*
学習ポイント：

静的メンバー変数の特徴：
1. クラスの全インスタンスで共有される
2. プログラム開始時に初期化される
3. プログラム終了時に削除される
4. クラス外で定義が必要（宣言とは別）
5. インスタンスが存在しなくてもアクセス可能

静的メンバー関数の特徴：
1. インスタンスなしで呼び出し可能
2. 静的メンバー変数のみアクセス可能
3. thisポインタが存在しない
4. 仮想関数にできない
5. クラス名::関数名() で呼び出し

主な使用場面：
1. インスタンス数のカウント
2. シングルトンパターン
3. ファクトリーパターン
4. ユーティリティ関数
5. グローバルな設定管理
6. キャッシュシステム
7. IDジェネレーター

注意点：
1. 初期化順序の問題（異なる翻訳単位間）
2. スレッドセーフティの考慮が必要
3. テストの難しさ（グローバル状態）
4. 破棄タイミングの制御困難

モダンC++での代替案：
1. namespace内の変数・関数
2. 関数内static変数（遅延初期化）
3. std::call_once（スレッドセーフ初期化）
4. constexpr変数（コンパイル時定数）

静的メンバーは適切に使えば強力だが、
グローバル状態を作るため慎重に設計する必要がある
*/