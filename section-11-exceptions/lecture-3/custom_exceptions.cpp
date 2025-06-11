// カスタム例外クラスの設計
// ゲーム固有の例外階層を構築して、エラー処理を体系化

#include <iostream>
#include <exception>
#include <string>
#include <sstream>
#include <chrono>
#include <ctime>
#include <vector>
#include <map>

// ゲーム例外の基底クラス
class GameException : public std::exception
{
protected:
    std::string message;
    std::string errorCode;
    std::chrono::system_clock::time_point timestamp;
    
public:
    GameException(const std::string& msg, const std::string& code = "GAME_ERROR")
        : message(msg), errorCode(code), timestamp(std::chrono::system_clock::now())
    {
    }
    
    const char* what() const noexcept override
    {
        return message.c_str();
    }
    
    const std::string& getErrorCode() const noexcept
    {
        return errorCode;
    }
    
    std::string getTimestamp() const
    {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        return std::ctime(&time);
    }
    
    virtual std::string getDetailedMessage() const
    {
        std::stringstream ss;
        ss << "[" << errorCode << "] " << message;
        ss << " (発生時刻: " << getTimestamp() << ")";
        return ss.str();
    }
};

// プレイヤー関連の例外
class PlayerException : public GameException
{
protected:
    std::string playerName;
    int playerId;
    
public:
    PlayerException(const std::string& msg, const std::string& name, int id)
        : GameException(msg, "PLAYER_ERROR"), playerName(name), playerId(id)
    {
    }
    
    const std::string& getPlayerName() const noexcept { return playerName; }
    int getPlayerId() const noexcept { return playerId; }
};

// レベル不足例外
class InsufficientLevelException : public PlayerException
{
private:
    int requiredLevel;
    int currentLevel;
    
public:
    InsufficientLevelException(const std::string& playerName, int playerId,
                             int current, int required)
        : PlayerException("レベルが不足しています", playerName, playerId),
          requiredLevel(required), currentLevel(current)
    {
        std::stringstream ss;
        ss << "プレイヤー「" << playerName << "」のレベルが不足しています。"
           << "現在: Lv" << currentLevel << " / 必要: Lv" << requiredLevel;
        message = ss.str();
    }
    
    int getRequiredLevel() const noexcept { return requiredLevel; }
    int getCurrentLevel() const noexcept { return currentLevel; }
};

// アイテム関連の例外
class ItemException : public GameException
{
protected:
    std::string itemName;
    int itemId;
    
public:
    ItemException(const std::string& msg, const std::string& item, int id)
        : GameException(msg, "ITEM_ERROR"), itemName(item), itemId(id)
    {
    }
    
    const std::string& getItemName() const noexcept { return itemName; }
    int getItemId() const noexcept { return itemId; }
};

// アイテム不足例外
class InsufficientItemException : public ItemException
{
private:
    int requiredAmount;
    int currentAmount;
    
public:
    InsufficientItemException(const std::string& item, int id, 
                            int current, int required)
        : ItemException("アイテムが不足しています", item, id),
          requiredAmount(required), currentAmount(current)
    {
        std::stringstream ss;
        ss << "「" << itemName << "」が不足しています。"
           << "現在: " << currentAmount << "個 / 必要: " << requiredAmount << "個";
        message = ss.str();
    }
    
    int getRequiredAmount() const noexcept { return requiredAmount; }
    int getCurrentAmount() const noexcept { return currentAmount; }
};

// 戦闘関連の例外
class CombatException : public GameException
{
protected:
    std::string attackerName;
    std::string targetName;
    
public:
    CombatException(const std::string& msg, const std::string& attacker, 
                   const std::string& target)
        : GameException(msg, "COMBAT_ERROR"), 
          attackerName(attacker), targetName(target)
    {
    }
    
    const std::string& getAttackerName() const noexcept { return attackerName; }
    const std::string& getTargetName() const noexcept { return targetName; }
};

// 無効なターゲット例外
class InvalidTargetException : public CombatException
{
public:
    enum TargetError
    {
        OUT_OF_RANGE,
        ALREADY_DEFEATED,
        INVALID_TYPE,
        FRIENDLY_FIRE
    };
    
private:
    TargetError errorType;
    
public:
    InvalidTargetException(const std::string& attacker, const std::string& target,
                         TargetError error)
        : CombatException("無効なターゲットです", attacker, target),
          errorType(error)
    {
        std::stringstream ss;
        ss << attackerName << " は " << targetName << " を攻撃できません: ";
        
        switch (errorType)
        {
            case OUT_OF_RANGE:
                ss << "射程範囲外です";
                break;
            case ALREADY_DEFEATED:
                ss << "すでに倒されています";
                break;
            case INVALID_TYPE:
                ss << "攻撃対象として無効です";
                break;
            case FRIENDLY_FIRE:
                ss << "味方は攻撃できません";
                break;
        }
        
        message = ss.str();
    }
    
    TargetError getErrorType() const noexcept { return errorType; }
};

// ゲームシステムでの使用例
class Player
{
private:
    std::string name;
    int id;
    int level;
    std::map<std::string, int> inventory;
    
public:
    Player(const std::string& n, int i, int l) 
        : name(n), id(i), level(l) {}
    
    void useSkill(const std::string& skillName, int requiredLevel)
    {
        if (level < requiredLevel)
        {
            throw InsufficientLevelException(name, id, level, requiredLevel);
        }
        
        std::cout << name << " は " << skillName << " を使用した！" << std::endl;
    }
    
    void useItem(const std::string& itemName, int amount)
    {
        auto it = inventory.find(itemName);
        int currentAmount = (it != inventory.end()) ? it->second : 0;
        
        if (currentAmount < amount)
        {
            throw InsufficientItemException(itemName, 1001, currentAmount, amount);
        }
        
        inventory[itemName] -= amount;
        std::cout << name << " は " << itemName << " を " 
                  << amount << " 個使用した！" << std::endl;
    }
    
    void addItem(const std::string& itemName, int amount)
    {
        inventory[itemName] += amount;
    }
    
    const std::string& getName() const { return name; }
    int getLevel() const { return level; }
};

// 戦闘システム
class CombatSystem
{
public:
    void attack(Player& attacker, Player& target, float distance)
    {
        const float MAX_RANGE = 10.0f;
        
        // 射程チェック
        if (distance > MAX_RANGE)
        {
            throw InvalidTargetException(attacker.getName(), target.getName(),
                                       InvalidTargetException::OUT_OF_RANGE);
        }
        
        // 味方攻撃チェック（実際のゲームではチーム情報を確認）
        if (attacker.getName() == target.getName())
        {
            throw InvalidTargetException(attacker.getName(), target.getName(),
                                       InvalidTargetException::FRIENDLY_FIRE);
        }
        
        std::cout << attacker.getName() << " は " << target.getName() 
                  << " を攻撃した！" << std::endl;
    }
};

// エラーハンドリングシステム
class ErrorHandler
{
private:
    std::vector<std::string> errorLog;
    
public:
    void handleException(const GameException& e)
    {
        // エラーログに記録
        errorLog.push_back(e.getDetailedMessage());
        
        // エラータイプに応じた処理
        if (auto* playerEx = dynamic_cast<const PlayerException*>(&e))
        {
            std::cerr << "プレイヤーエラー [ID: " << playerEx->getPlayerId() 
                     << "]: " << e.what() << std::endl;
        }
        else if (auto* itemEx = dynamic_cast<const ItemException*>(&e))
        {
            std::cerr << "アイテムエラー [" << itemEx->getItemName() 
                     << "]: " << e.what() << std::endl;
        }
        else if (auto* combatEx = dynamic_cast<const CombatException*>(&e))
        {
            std::cerr << "戦闘エラー: " << e.what() << std::endl;
        }
        else
        {
            std::cerr << "ゲームエラー: " << e.what() << std::endl;
        }
    }
    
    void displayErrorLog() const
    {
        std::cout << "\n=== エラーログ ===" << std::endl;
        for (const auto& error : errorLog)
        {
            std::cout << error << std::endl;
        }
    }
};

int main()
{
    std::cout << "=== カスタム例外クラスの設計 ===" << std::endl;
    
    ErrorHandler errorHandler;
    
    // プレイヤーの作成
    Player hero("勇者", 1001, 15);
    Player wizard("魔法使い", 1002, 20);
    Player enemy("ゴブリン", 2001, 5);
    
    // アイテムを追加
    hero.addItem("ポーション", 3);
    
    // 1. レベル不足の例外
    std::cout << "\n--- スキル使用テスト ---" << std::endl;
    try
    {
        hero.useSkill("メテオストライク", 30);
    }
    catch (const InsufficientLevelException& e)
    {
        errorHandler.handleException(e);
        std::cout << "必要レベル: " << e.getRequiredLevel() 
                  << " / 現在レベル: " << e.getCurrentLevel() << std::endl;
    }
    
    // 2. アイテム不足の例外
    std::cout << "\n--- アイテム使用テスト ---" << std::endl;
    try
    {
        hero.useItem("ポーション", 2);  // 成功
        hero.useItem("ポーション", 3);  // 失敗（不足）
    }
    catch (const InsufficientItemException& e)
    {
        errorHandler.handleException(e);
        std::cout << "必要数: " << e.getRequiredAmount() 
                  << " / 所持数: " << e.getCurrentAmount() << std::endl;
    }
    
    // 3. 戦闘関連の例外
    std::cout << "\n--- 戦闘テスト ---" << std::endl;
    CombatSystem combat;
    
    try
    {
        combat.attack(hero, enemy, 5.0f);    // 成功
        combat.attack(hero, wizard, 15.0f);  // 射程外
    }
    catch (const InvalidTargetException& e)
    {
        errorHandler.handleException(e);
        
        switch (e.getErrorType())
        {
            case InvalidTargetException::OUT_OF_RANGE:
                std::cout << "ターゲットが遠すぎます！" << std::endl;
                break;
            case InvalidTargetException::FRIENDLY_FIRE:
                std::cout << "味方を攻撃することはできません！" << std::endl;
                break;
            default:
                break;
        }
    }
    
    // 4. 一般的なゲーム例外
    std::cout << "\n--- 一般的な例外 ---" << std::endl;
    try
    {
        throw GameException("セーブデータが破損しています", "SAVE_ERROR");
    }
    catch (const GameException& e)
    {
        errorHandler.handleException(e);
        std::cout << "エラーコード: " << e.getErrorCode() << std::endl;
    }
    
    // エラーログの表示
    errorHandler.displayErrorLog();
    
    return 0;
}

// まとめ：
// - 例外クラスの階層構造でエラーを体系的に管理
// - 基底クラスから派生して特定のエラータイプを作成
// - 例外に追加情報（タイムスタンプ、エラーコード等）を含める
// - dynamic_castで例外の型を判定して適切な処理を実行
// - エラーハンドリングシステムでログ管理と統一的な処理