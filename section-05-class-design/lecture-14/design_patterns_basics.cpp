// Lecture 14: デザインパターンの基礎
// Basic Design Patterns in C++

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

// 1. Singleton Pattern - ゲーム設定管理
class GameSettings 
{
private:
    static std::unique_ptr<GameSettings> instance;
    static std::once_flag instanceFlag;
    
    // 設定値
    std::string playerName;
    int difficulty;
    float volume;
    bool fullscreen;
    
    // プライベートコンストラクタ
    GameSettings() : playerName("Player"), difficulty(1), volume(0.8f), fullscreen(false) 
    {
        std::cout << "GameSettings インスタンスを作成しました\n";
    }
    
public:
    // スレッドセーフなインスタンス取得
    static GameSettings& getInstance() 
    {
        std::call_once(instanceFlag, []() {
            instance = std::unique_ptr<GameSettings>(new GameSettings());
        });
        return *instance;
    }
    
    // コピー禁止
    GameSettings(const GameSettings&) = delete;
    GameSettings& operator=(const GameSettings&) = delete;
    
    // 設定アクセサー
    void setPlayerName(const std::string& name) { playerName = name; }
    void setDifficulty(int diff) { difficulty = diff; }
    void setVolume(float vol) { volume = vol; }
    void setFullscreen(bool fs) { fullscreen = fs; }
    
    std::string getPlayerName() const { return playerName; }
    int getDifficulty() const { return difficulty; }
    float getVolume() const { return volume; }
    bool isFullscreen() const { return fullscreen; }
    
    void showSettings() const 
    {
        std::cout << "=== ゲーム設定 ===\n";
        std::cout << "プレイヤー名: " << playerName << "\n";
        std::cout << "難易度: " << difficulty << "\n";
        std::cout << "音量: " << volume << "\n";
        std::cout << "フルスクリーン: " << (fullscreen ? "ON" : "OFF") << "\n";
    }
};

std::unique_ptr<GameSettings> GameSettings::instance = nullptr;
std::once_flag GameSettings::instanceFlag;

// 2. Factory Pattern - 敵キャラクター生成
class Enemy 
{
protected:
    std::string name;
    int health;
    int damage;
    
public:
    Enemy(const std::string& n, int hp, int dmg) : name(n), health(hp), damage(dmg) {}
    virtual ~Enemy() = default;
    
    virtual void attack() const = 0;
    virtual std::string getType() const = 0;
    
    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getDamage() const { return damage; }
    
    void takeDamage(int dmg) 
    {
        health -= dmg;
        if (health < 0) health = 0;
        std::cout << name << " は " << dmg << " のダメージを受けた！ 残りHP: " << health << "\n";
    }
};

class Goblin : public Enemy 
{
public:
    Goblin() : Enemy("ゴブリン", 30, 8) {}
    
    void attack() const override 
    {
        std::cout << name << " がクラブで攻撃！ ダメージ: " << damage << "\n";
    }
    
    std::string getType() const override { return "Goblin"; }
};

class Orc : public Enemy 
{
public:
    Orc() : Enemy("オーク", 60, 15) {}
    
    void attack() const override 
    {
        std::cout << name << " が斧で強打！ ダメージ: " << damage << "\n";
    }
    
    std::string getType() const override { return "Orc"; }
};

class Dragon : public Enemy 
{
public:
    Dragon() : Enemy("ドラゴン", 200, 40) {}
    
    void attack() const override 
    {
        std::cout << name << " が炎のブレスを放つ！ ダメージ: " << damage << "\n";
    }
    
    std::string getType() const override { return "Dragon"; }
};

// ファクトリークラス
class EnemyFactory 
{
public:
    enum class EnemyType { GOBLIN, ORC, DRAGON };
    
    static std::unique_ptr<Enemy> createEnemy(EnemyType type) 
    {
        switch (type) {
            case EnemyType::GOBLIN:
                std::cout << "ゴブリンを生成\n";
                return std::make_unique<Goblin>();
            case EnemyType::ORC:
                std::cout << "オークを生成\n";
                return std::make_unique<Orc>();
            case EnemyType::DRAGON:
                std::cout << "ドラゴンを生成\n";
                return std::make_unique<Dragon>();
            default:
                return nullptr;
        }
    }
    
    static std::unique_ptr<Enemy> createRandomEnemy() 
    {
        int randomType = rand() % 3;
        return createEnemy(static_cast<EnemyType>(randomType));
    }
};

// 3. Observer Pattern - イベントシステム
class IEventObserver 
{
public:
    virtual ~IEventObserver() = default;
    virtual void onPlayerLevelUp(int newLevel) = 0;
    virtual void onEnemyDefeated(const std::string& enemyName) = 0;
    virtual void onItemFound(const std::string& itemName) = 0;
};

class EventManager 
{
private:
    std::vector<IEventObserver*> observers;
    
public:
    void addObserver(IEventObserver* observer) 
    {
        observers.push_back(observer);
        std::cout << "オブザーバーを追加しました\n";
    }
    
    void removeObserver(IEventObserver* observer) 
    {
        auto it = std::find(observers.begin(), observers.end(), observer);
        if (it != observers.end()) {
            observers.erase(it);
            std::cout << "オブザーバーを削除しました\n";
        }
    }
    
    void notifyPlayerLevelUp(int newLevel) 
    {
        std::cout << "\\n[イベント] プレイヤーがレベル " << newLevel << " にアップ！\n";
        for (auto* observer : observers) {
            observer->onPlayerLevelUp(newLevel);
        }
    }
    
    void notifyEnemyDefeated(const std::string& enemyName) 
    {
        std::cout << "\\n[イベント] " << enemyName << " を倒した！\n";
        for (auto* observer : observers) {
            observer->onEnemyDefeated(enemyName);
        }
    }
    
    void notifyItemFound(const std::string& itemName) 
    {
        std::cout << "\\n[イベント] " << itemName << " を発見！\n";
        for (auto* observer : observers) {
            observer->onItemFound(itemName);
        }
    }
};

// オブザーバーの具体実装
class UIObserver : public IEventObserver 
{
public:
    void onPlayerLevelUp(int newLevel) override 
    {
        std::cout << "[UI] レベルアップエフェクトを表示\n";
        std::cout << "[UI] ステータス画面を更新\n";
    }
    
    void onEnemyDefeated(const std::string& enemyName) override 
    {
        std::cout << "[UI] 勝利メッセージを表示: " << enemyName << " を倒した！\n";
    }
    
    void onItemFound(const std::string& itemName) override 
    {
        std::cout << "[UI] アイテム取得通知を表示: " << itemName << "\n";
    }
};

class AudioObserver : public IEventObserver 
{
public:
    void onPlayerLevelUp(int newLevel) override 
    {
        std::cout << "[AUDIO] レベルアップ効果音を再生\n";
        if (newLevel % 10 == 0) {
            std::cout << "[AUDIO] 特別なファンファーレを再生\n";
        }
    }
    
    void onEnemyDefeated(const std::string& enemyName) override 
    {
        std::cout << "[AUDIO] 勝利効果音を再生\n";
    }
    
    void onItemFound(const std::string& itemName) override 
    {
        std::cout << "[AUDIO] アイテム取得効果音を再生\n";
    }
};

class StatisticsObserver : public IEventObserver 
{
private:
    int totalLevels;
    int enemiesDefeated;
    int itemsFound;
    
public:
    StatisticsObserver() : totalLevels(0), enemiesDefeated(0), itemsFound(0) {}
    
    void onPlayerLevelUp(int newLevel) override 
    {
        totalLevels = newLevel;
        std::cout << "[STATS] 最高レベルを更新: " << totalLevels << "\n";
    }
    
    void onEnemyDefeated(const std::string& enemyName) override 
    {
        enemiesDefeated++;
        std::cout << "[STATS] 倒した敵の数: " << enemiesDefeated << "\n";
    }
    
    void onItemFound(const std::string& itemName) override 
    {
        itemsFound++;
        std::cout << "[STATS] 見つけたアイテム数: " << itemsFound << "\n";
    }
    
    void showStatistics() const 
    {
        std::cout << "\\n=== ゲーム統計 ===\n";
        std::cout << "最高レベル: " << totalLevels << "\n";
        std::cout << "倒した敵: " << enemiesDefeated << "\n";
        std::cout << "見つけたアイテム: " << itemsFound << "\n";
    }
};

// 4. Strategy Pattern - 戦闘戦略
class IAttackStrategy 
{
public:
    virtual ~IAttackStrategy() = default;
    virtual int calculateDamage(int baseDamage) const = 0;
    virtual std::string getAttackDescription() const = 0;
};

class NormalAttack : public IAttackStrategy 
{
public:
    int calculateDamage(int baseDamage) const override 
    {
        return baseDamage;
    }
    
    std::string getAttackDescription() const override 
    {
        return "通常攻撃";
    }
};

class CriticalAttack : public IAttackStrategy 
{
public:
    int calculateDamage(int baseDamage) const override 
    {
        return baseDamage * 2;  // 2倍ダメージ
    }
    
    std::string getAttackDescription() const override 
    {
        return "クリティカル攻撃";
    }
};

class MagicAttack : public IAttackStrategy 
{
public:
    int calculateDamage(int baseDamage) const override 
    {
        return baseDamage + 10;  // 魔法ボーナス
    }
    
    std::string getAttackDescription() const override 
    {
        return "魔法攻撃";
    }
};

class Player 
{
private:
    std::string name;
    int health;
    int baseDamage;
    std::unique_ptr<IAttackStrategy> attackStrategy;
    
public:
    Player(const std::string& playerName) 
        : name(playerName), health(100), baseDamage(20),
          attackStrategy(std::make_unique<NormalAttack>()) {}
    
    void setAttackStrategy(std::unique_ptr<IAttackStrategy> strategy) 
    {
        attackStrategy = std::move(strategy);
        std::cout << name << " の攻撃戦略を " << attackStrategy->getAttackDescription() << " に変更\n";
    }
    
    void attack(Enemy& target) 
    {
        int damage = attackStrategy->calculateDamage(baseDamage);
        std::cout << name << " は " << attackStrategy->getAttackDescription() 
                  << " で攻撃！\n";
        target.takeDamage(damage);
    }
    
    std::string getName() const { return name; }
    int getHealth() const { return health; }
};

// 5. Command Pattern - ゲームコマンド
class ICommand 
{
public:
    virtual ~ICommand() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string getDescription() const = 0;
};

class MoveCommand : public ICommand 
{
private:
    int& x, & y;
    int dx, dy;
    
public:
    MoveCommand(int& posX, int& posY, int deltaX, int deltaY)
        : x(posX), y(posY), dx(deltaX), dy(deltaY) {}
    
    void execute() override 
    {
        x += dx;
        y += dy;
        std::cout << "移動: (" << x << ", " << y << ")\n";
    }
    
    void undo() override 
    {
        x -= dx;
        y -= dy;
        std::cout << "移動を取り消し: (" << x << ", " << y << ")\n";
    }
    
    std::string getDescription() const override 
    {
        return "移動(" + std::to_string(dx) + ", " + std::to_string(dy) + ")";
    }
};

class AttackCommand : public ICommand 
{
private:
    Player& attacker;
    Enemy& target;
    int previousHealth;
    
public:
    AttackCommand(Player& player, Enemy& enemy) 
        : attacker(player), target(enemy), previousHealth(enemy.getHealth()) {}
    
    void execute() override 
    {
        previousHealth = target.getHealth();
        attacker.attack(target);
    }
    
    void undo() override 
    {
        // 実際のゲームでは、より複雑な復元ロジックが必要
        std::cout << "攻撃を取り消し（HPを復元）\n";
    }
    
    std::string getDescription() const override 
    {
        return attacker.getName() + " が " + target.getName() + " を攻撃";
    }
};

class CommandManager 
{
private:
    std::vector<std::unique_ptr<ICommand>> history;
    size_t currentIndex;
    
public:
    CommandManager() : currentIndex(0) {}
    
    void executeCommand(std::unique_ptr<ICommand> command) 
    {
        // 現在位置以降の履歴を削除
        history.erase(history.begin() + currentIndex, history.end());
        
        // コマンド実行
        command->execute();
        
        // 履歴に追加
        history.push_back(std::move(command));
        currentIndex = history.size();
    }
    
    void undo() 
    {
        if (currentIndex > 0) {
            --currentIndex;
            history[currentIndex]->undo();
            std::cout << "Undo: " << history[currentIndex]->getDescription() << "\n";
        } else {
            std::cout << "取り消すコマンドがありません\n";
        }
    }
    
    void redo() 
    {
        if (currentIndex < history.size()) {
            history[currentIndex]->execute();
            std::cout << "Redo: " << history[currentIndex]->getDescription() << "\n";
            ++currentIndex;
        } else {
            std::cout << "やり直すコマンドがありません\n";
        }
    }
    
    void showHistory() const 
    {
        std::cout << "\\n=== コマンド履歴 ===\n";
        for (size_t i = 0; i < history.size(); ++i) {
            std::string marker = (i < currentIndex) ? "[実行済み] " : "[未実行] ";
            std::cout << marker << history[i]->getDescription() << "\n";
        }
    }
};

int main() 
{
    std::cout << "=== デザインパターンの基礎デモ ===\n\n";
    
    // 1. Singleton Pattern
    std::cout << "1. Singleton Pattern - ゲーム設定\n";
    GameSettings& settings1 = GameSettings::getInstance();
    GameSettings& settings2 = GameSettings::getInstance();
    
    std::cout << "同一インスタンス？ " << (&settings1 == &settings2 ? "はい" : "いいえ") << "\n";
    
    settings1.setPlayerName("勇者アレックス");
    settings1.setDifficulty(3);
    settings2.showSettings();  // settings1と同じ設定が表示される
    
    std::cout << "\n---\n\n";
    
    // 2. Factory Pattern
    std::cout << "2. Factory Pattern - 敵生成\n";
    std::vector<std::unique_ptr<Enemy>> enemies;
    
    enemies.push_back(EnemyFactory::createEnemy(EnemyFactory::EnemyType::GOBLIN));
    enemies.push_back(EnemyFactory::createEnemy(EnemyFactory::EnemyType::ORC));
    enemies.push_back(EnemyFactory::createEnemy(EnemyFactory::EnemyType::DRAGON));
    
    std::cout << "\n生成された敵:\n";
    for (const auto& enemy : enemies) {
        std::cout << "- " << enemy->getName() << " (HP: " << enemy->getHealth() << ")\n";
    }
    
    std::cout << "\n---\n\n";
    
    // 3. Observer Pattern
    std::cout << "3. Observer Pattern - イベントシステム\n";
    EventManager eventManager;
    UIObserver uiObserver;
    AudioObserver audioObserver;
    StatisticsObserver statsObserver;
    
    eventManager.addObserver(&uiObserver);
    eventManager.addObserver(&audioObserver);
    eventManager.addObserver(&statsObserver);
    
    // イベント発生
    eventManager.notifyPlayerLevelUp(5);
    eventManager.notifyEnemyDefeated("ゴブリン");
    eventManager.notifyItemFound("魔法の剣");
    eventManager.notifyPlayerLevelUp(10);
    
    statsObserver.showStatistics();
    
    std::cout << "\n---\n\n";
    
    // 4. Strategy Pattern
    std::cout << "4. Strategy Pattern - 戦闘戦略\n";
    Player player("戦士");
    auto goblin = EnemyFactory::createEnemy(EnemyFactory::EnemyType::GOBLIN);
    
    // 通常攻撃
    player.attack(*goblin);
    
    // クリティカル攻撃に変更
    player.setAttackStrategy(std::make_unique<CriticalAttack>());
    player.attack(*goblin);
    
    // 魔法攻撃に変更
    player.setAttackStrategy(std::make_unique<MagicAttack>());
    player.attack(*goblin);
    
    std::cout << "\n---\n\n";
    
    // 5. Command Pattern
    std::cout << "5. Command Pattern - コマンド管理\n";
    CommandManager cmdManager;
    int playerX = 0, playerY = 0;
    
    std::cout << "初期位置: (" << playerX << ", " << playerY << ")\n";
    
    // 移動コマンド実行\n    cmdManager.executeCommand(std::make_unique<MoveCommand>(playerX, playerY, 3, 0));\n    cmdManager.executeCommand(std::make_unique<MoveCommand>(playerX, playerY, 0, 2));\n    cmdManager.executeCommand(std::make_unique<MoveCommand>(playerX, playerY, -1, 1));\n    \n    cmdManager.showHistory();\n    \n    // アンドゥ・リドゥテスト\n    std::cout << \"\\nアンドゥテスト:\n\";\n    cmdManager.undo();\n    cmdManager.undo();\n    \n    std::cout << \"\\nリドゥテスト:\n\";\n    cmdManager.redo();\n    \n    cmdManager.showHistory();\n    \n    return 0;\n}\n\n/*\n学習ポイント：\n\n今回実装したデザインパターン：\n\n1. Singleton Pattern（シングルトン）:\n   - 目的: クラスのインスタンスを1つに制限\n   - 使用場面: ゲーム設定、ログ管理、リソース管理\n   - 実装のポイント: スレッドセーフティ、遅延初期化\n\n2. Factory Pattern（ファクトリ）:\n   - 目的: オブジェクト生成を抽象化\n   - 使用場面: 敵キャラクター生成、UI要素作成\n   - 利点: 生成ロジックの集約、型安全性\n\n3. Observer Pattern（オブザーバー）:\n   - 目的: オブジェクト間の疎結合な通知システム\n   - 使用場面: イベントシステム、MVC\n   - 利点: 発行者と購読者の分離\n\n4. Strategy Pattern（ストラテジ）:\n   - 目的: アルゴリズムの動的な切り替え\n   - 使用場面: 戦闘システム、AI行動\n   - 利点: アルゴリズムの独立性、拡張性\n\n5. Command Pattern（コマンド）:\n   - 目的: 操作をオブジェクトとしてカプセル化\n   - 使用場面: undo/redo、マクロ、キューイング\n   - 利点: 操作の記録・再実行が可能\n\nデザインパターンの重要性：\n1. 再利用可能な解決策\n2. チーム開発での共通言語\n3. 保守性の向上\n4. 設計の品質向上\n\n注意点：\n1. 過度な適用は避ける（過剰設計）\n2. 要件に適したパターンを選択\n3. パフォーマンスへの影響を考慮\n4. シンプルな解決策で十分な場合もある\n\nこれらのパターンを理解することで、\nより良い設計とコードの再利用性が向上する\n*/