// Lecture 11: フレンド関数とフレンドクラス
// Friend Functions and Friend Classes in C++

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>

// フレンド関数の基本例：ゲーム内のキャラクターステータス
class PlayerStats 
{
private:
    std::string playerName;
    int level;
    int experience;
    int health;
    int mana;
    int strength;
    int intelligence;
    
    // 秘密情報：ゲーム内でのみ使用
    double criticalChance;
    int secretSkillPoints;
    
public:
    PlayerStats(const std::string& name, int lvl)
        : playerName(name), level(lvl), experience(0), 
          health(100), mana(50), strength(10), intelligence(10),
          criticalChance(0.05), secretSkillPoints(0) 
    {
        std::cout << "プレイヤー " << playerName << " がレベル" << level << "で作成されました\n";
    }
    
    // 公開メソッド
    void showBasicStats() const 
    {
        std::cout << "=== " << playerName << " のステータス ===\n";
        std::cout << "Level: " << level << ", EXP: " << experience << "\n";
        std::cout << "HP: " << health << ", MP: " << mana << "\n";
        std::cout << "STR: " << strength << ", INT: " << intelligence << "\n";
    }
    
    void gainExperience(int exp) 
    {
        experience += exp;
        std::cout << playerName << "は" << exp << " EXPを獲得しました\n";
        checkLevelUp();
    }
    
    std::string getName() const { return playerName; }
    int getLevel() const { return level; }
    
private:
    void checkLevelUp() 
    {
        int requiredExp = level * 100;
        if (experience >= requiredExp) {
            level++;
            experience -= requiredExp;
            health += 20;
            mana += 10;
            strength += 2;
            intelligence += 2;
            secretSkillPoints += 5;
            std::cout << playerName << "はレベル" << level << "にアップ！\n";
        }
    }
    
    // フレンド関数の宣言
    friend void showDetailedStats(const PlayerStats& player);
    friend void debugPlayerInfo(const PlayerStats& player);
    friend double calculateCombatPower(const PlayerStats& player);
    
    // フレンドクラスの宣言
    friend class GameMaster;
    friend class SaveSystem;
};

// フレンド関数の定義：詳細ステータス表示
void showDetailedStats(const PlayerStats& player) 
{
    std::cout << "\n=== " << player.playerName << " の詳細ステータス ===\n";
    std::cout << "Level: " << player.level << " (EXP: " << player.experience << ")\n";
    std::cout << "HP: " << player.health << ", MP: " << player.mana << "\n";
    std::cout << "STR: " << player.strength << ", INT: " << player.intelligence << "\n";
    std::cout << "Critical Chance: " << (player.criticalChance * 100) << "%\n";
    std::cout << "Secret Skill Points: " << player.secretSkillPoints << "\n";
}

// デバッグ用フレンド関数
void debugPlayerInfo(const PlayerStats& player) 
{
    std::cout << "\n[DEBUG] " << player.playerName << " - Internal State:\n";
    std::cout << "Memory Address: " << &player << "\n";
    std::cout << "All Private Data Accessible via Friend Function\n";
    std::cout << "criticalChance: " << player.criticalChance << "\n";
    std::cout << "secretSkillPoints: " << player.secretSkillPoints << "\n";
}

// 戦闘力計算用フレンド関数
double calculateCombatPower(const PlayerStats& player) 
{
    double basePower = player.strength * 2.5 + player.intelligence * 1.8;
    double levelBonus = player.level * 10;
    double critBonus = player.criticalChance * 100;
    double skillBonus = player.secretSkillPoints * 3;
    
    return basePower + levelBonus + critBonus + skillBonus;
}

// フレンドクラスの例：ゲームマスター
class GameMaster 
{
private:
    std::string gmName;
    bool adminMode;
    
public:
    GameMaster(const std::string& name) : gmName(name), adminMode(true) 
    {
        std::cout << "GameMaster " << gmName << " がログインしました\n";
    }
    
    // プレイヤーの全データにアクセス可能
    void giveBonus(PlayerStats& player, int bonusExp, int bonusSkillPoints) 
    {
        if (!adminMode) {
            std::cout << "Admin権限がありません\n";
            return;
        }
        
        std::cout << "[GM] " << gmName << "が" << player.playerName 
                  << "にボーナスを付与しました\n";
        
        player.experience += bonusExp;
        player.secretSkillPoints += bonusSkillPoints;
        player.checkLevelUp();
    }
    
    void adjustPlayerStats(PlayerStats& player, int healthBonus, int manaBonus) 
    {
        if (!adminMode) return;
        
        std::cout << "[GM] ステータスを調整しました\n";
        player.health += healthBonus;
        player.mana += manaBonus;
    }
    
    void showFullPlayerData(const PlayerStats& player) 
    {
        std::cout << "\n[GM] " << player.playerName << " の全データ:\n";
        std::cout << "Level: " << player.level << ", EXP: " << player.experience << "\n";
        std::cout << "HP: " << player.health << ", MP: " << player.mana << "\n";
        std::cout << "STR: " << player.strength << ", INT: " << player.intelligence << "\n";
        std::cout << "Critical: " << player.criticalChance << "\n";
        std::cout << "Secret Points: " << player.secretSkillPoints << "\n";
    }
    
    void resetPlayer(PlayerStats& player) 
    {
        if (!adminMode) return;
        
        std::cout << "[GM] " << player.playerName << " をリセットしました\n";
        player.level = 1;
        player.experience = 0;
        player.health = 100;
        player.mana = 50;
        player.strength = 10;
        player.intelligence = 10;
        player.secretSkillPoints = 0;
    }
};

// セーブシステムフレンドクラス
class SaveSystem 
{
public:
    // プレイヤーデータをファイルに保存
    static bool savePlayer(const PlayerStats& player, const std::string& filename) 
    {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Save failed: ファイルを開けません\n";
            return false;
        }
        
        // 全のプライベートメンバーにアクセス可能
        file << "PlayerData\n";
        file << player.playerName << "\n";
        file << player.level << "\n";
        file << player.experience << "\n";
        file << player.health << "\n";
        file << player.mana << "\n";
        file << player.strength << "\n";
        file << player.intelligence << "\n";
        file << player.criticalChance << "\n";
        file << player.secretSkillPoints << "\n";
        
        file.close();
        std::cout << "[SAVE] " << player.playerName << " のデータを" << filename << "に保存しました\n";
        return true;
    }
    
    // ファイルからプレイヤーデータを読み込み
    static bool loadPlayer(PlayerStats& player, const std::string& filename) 
    {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "Load failed: ファイルが見つかりません\n";
            return false;
        }
        
        std::string header;
        file >> header;
        if (header != "PlayerData") {
            std::cout << "Load failed: 無効なファイル形式\n";
            return false;
        }
        
        file >> player.playerName;
        file >> player.level;
        file >> player.experience;
        file >> player.health;
        file >> player.mana;
        file >> player.strength;
        file >> player.intelligence;
        file >> player.criticalChance;
        file >> player.secretSkillPoints;
        
        file.close();
        std::cout << "[LOAD] " << player.playerName << " のデータを" << filename << "から読み込みました\n";
        return true;
    }
};

// 演算子オーバーロードでフレンド関数を使う例
class Position 
{
private:
    double x, y;
    
public:
    Position(double xPos = 0.0, double yPos = 0.0) : x(xPos), y(yPos) {}
    
    void move(double dx, double dy) 
    {
        x += dx;
        y += dy;
    }
    
    void showPosition() const 
    {
        std::cout << "Position: (" << x << ", " << y << ")\n";
    }
    
    // 演算子オーバーロードでフレンド関数を使用
    friend Position operator+(const Position& a, const Position& b);
    friend Position operator-(const Position& a, const Position& b);
    friend double distance(const Position& a, const Position& b);
    friend std::ostream& operator<<(std::ostream& os, const Position& pos);
};

// フレンド演算子の実装
Position operator+(const Position& a, const Position& b) 
{
    return Position(a.x + b.x, a.y + b.y);
}

Position operator-(const Position& a, const Position& b) 
{
    return Position(a.x - b.x, a.y - b.y);
}

double distance(const Position& a, const Position& b) 
{
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

std::ostream& operator<<(std::ostream& os, const Position& pos) 
{
    os << "(" << pos.x << ", " << pos.y << ")";
    return os;
}

// フレンド関数の注意点と代替手段のデモ
class BankAccount 
{
private:
    std::string accountNumber;
    double balance;
    std::string ownerName;
    
public:
    BankAccount(const std::string& number, const std::string& owner, double initialBalance)
        : accountNumber(number), ownerName(owner), balance(initialBalance) {}
    
    // 通常のメソッド
    void deposit(double amount) 
    {
        if (amount > 0) {
            balance += amount;
            std::cout << amount << "円を入金しました\n";
        }
    }
    
    bool withdraw(double amount) 
    {
        if (amount > 0 && balance >= amount) {
            balance -= amount;
            std::cout << amount << "円を出金しました\n";
            return true;
        }
        return false;
    }
    
    double getBalance() const { return balance; }
    
    // フレンド関数で銀行間送金
    friend bool transfer(BankAccount& from, BankAccount& to, double amount);
    
    // より安全な代替案：access関数で制御
    static bool transferSafe(BankAccount& from, BankAccount& to, double amount) 
    {
        if (from.balance >= amount && amount > 0) {
            from.balance -= amount;
            to.balance += amount;
            std::cout << "送金完了: " << amount << "円\n";
            return true;
        }
        std::cout << "送金失敗: 残高不足\n";
        return false;
    }
};

bool transfer(BankAccount& from, BankAccount& to, double amount) 
{
    // フレンド関数は全プライベートメンバーにアクセス可能
    if (from.balance >= amount && amount > 0) {
        from.balance -= amount;
        to.balance += amount;
        std::cout << "送金完了: " << from.ownerName << " → " 
                  << to.ownerName << " (" << amount << "円)\n";
        return true;
    }
    std::cout << "送金失敗\n";
    return false;
}

int main() 
{
    std::cout << "=== フレンド関数とフレンドクラスのデモ ===\n\n";
    
    // プレイヤーステータスのテスト
    std::cout << "1. フレンド関数の例\n";
    PlayerStats player("アルフ", 1);
    
    player.showBasicStats();
    player.gainExperience(150);
    
    // フレンド関数で詳細情報表示
    showDetailedStats(player);
    debugPlayerInfo(player);
    
    double combatPower = calculateCombatPower(player);
    std::cout << player.getName() << "の戦闘力: " << combatPower << "\n";
    
    std::cout << "\n---\n\n";
    
    // フレンドクラスのテスト
    std::cout << "2. フレンドクラスの例\n";
    GameMaster gm("AdminUser");
    
    gm.showFullPlayerData(player);
    gm.giveBonus(player, 200, 10);
    gm.adjustPlayerStats(player, 50, 30);
    gm.showFullPlayerData(player);
    
    // セーブ/ロードシステム
    SaveSystem::savePlayer(player, "player_save.dat");
    
    gm.resetPlayer(player);
    std::cout << "\nリセット後:\n";
    showDetailedStats(player);
    
    SaveSystem::loadPlayer(player, "player_save.dat");
    std::cout << "\nロード後:\n";
    showDetailedStats(player);
    
    std::cout << "\n---\n\n";
    
    // 演算子オーバーロードとフレンド関数
    std::cout << "3. 演算子オーバーロードとフレンド関数\n";
    Position pos1(3.0, 4.0);
    Position pos2(1.0, 2.0);
    
    std::cout << "Position 1: " << pos1 << "\n";
    std::cout << "Position 2: " << pos2 << "\n";
    
    Position sum = pos1 + pos2;
    Position diff = pos1 - pos2;
    double dist = distance(pos1, pos2);
    
    std::cout << "Sum: " << sum << "\n";
    std::cout << "Difference: " << diff << "\n";
    std::cout << "Distance: " << dist << "\n";
    
    std::cout << "\n---\n\n";
    
    // 銀行口座の例
    std::cout << "4. フレンド関数の注意点\n";
    BankAccount account1("123-456", "田中", 10000);
    BankAccount account2("789-012", "佐藤", 5000);
    
    std::cout << "送金前 - 田中: " << account1.getBalance() 
              << "円, 佐藤: " << account2.getBalance() << "円\n";
    
    transfer(account1, account2, 3000);
    
    std::cout << "送金後 - 田中: " << account1.getBalance() 
              << "円, 佐藤: " << account2.getBalance() << "円\n";
    
    return 0;
}

/*
学習ポイント：

フレンド関数の特徴：
1. クラスのプライベートメンバーにアクセス可能
2. クラスのメンバー関数ではない（グローバル関数）
3. クラス内でfriendキーワードで宣言する
4. 相互のfriend関係は存在しない（単方向）
5. 継承されない

フレンドクラスの特徴：
1. クラス全体がフレンドになる
2. そのクラスの全メンバー関数がプライベートメンバーにアクセス可能
3. 密接な関係のクラス間で使用

使用場面：
1. 演算子オーバーロード（特に二項演算子）
2. シリアライゼーション（セーブ/ロード）
3. テスト用コード
4. デバッグ用機能
5. 特殊なアクセス権限が必要な機能

注意点：
1. カプセル化を破ることになる
2. クラス間の結合度が高くなる
3. メンテナンス性が低下する可能性
4. 代替手段（publicメソッド、getter/setter）を検討する

フレンド関数/クラスは必要最小限に使用し、
適切な設計でカプセル化を維持することが重要
*/