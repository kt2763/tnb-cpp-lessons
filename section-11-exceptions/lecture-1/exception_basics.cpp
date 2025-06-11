// 例外処理の基本
// 例外（exception）はプログラムの異常な状態を処理するメカニズム

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// ゲームのキャラクタークラス
class GameCharacter
{
private:
    std::string name;
    int health;
    int maxHealth;
    int level;
    
public:
    GameCharacter(const std::string& n, int hp, int lvl) 
        : name(n), health(hp), maxHealth(hp), level(lvl)
    {
        // コンストラクタでの検証
        if (hp <= 0)
        {
            throw std::invalid_argument("体力は1以上である必要があります");
        }
        if (lvl < 1)
        {
            throw std::invalid_argument("レベルは1以上である必要があります");
        }
    }
    
    // ダメージを受ける関数
    void takeDamage(int damage)
    {
        if (damage < 0)
        {
            throw std::invalid_argument("ダメージは0以上である必要があります");
        }
        
        health -= damage;
        std::cout << name << " は " << damage << " のダメージを受けた！" << std::endl;
        
        if (health <= 0)
        {
            health = 0;
            throw std::runtime_error(name + " は倒れた！");
        }
    }
    
    // 回復する関数
    void heal(int amount)
    {
        if (amount < 0)
        {
            throw std::invalid_argument("回復量は0以上である必要があります");
        }
        
        health += amount;
        if (health > maxHealth)
        {
            health = maxHealth;
        }
        
        std::cout << name << " は " << amount << " HP回復した！" << std::endl;
    }
    
    void displayStatus() const
    {
        std::cout << "【" << name << "】 HP: " << health << "/" << maxHealth 
                  << " Lv: " << level << std::endl;
    }
};

// アイテムクラス
class Item
{
private:
    std::string name;
    int quantity;
    
public:
    Item(const std::string& n, int qty) : name(n), quantity(qty)
    {
        if (qty < 0)
        {
            throw std::invalid_argument("アイテム数は0以上である必要があります");
        }
    }
    
    void use(int amount = 1)
    {
        if (amount > quantity)
        {
            throw std::runtime_error("アイテムが不足しています！");
        }
        
        quantity -= amount;
        std::cout << name << " を " << amount << " 個使用しました" << std::endl;
    }
    
    int getQuantity() const { return quantity; }
    std::string getName() const { return name; }
};

// カスタム例外クラス
class GameException : public std::exception
{
private:
    std::string message;
    
public:
    explicit GameException(const std::string& msg) : message(msg) {}
    
    const char* what() const noexcept override
    {
        return message.c_str();
    }
};

// ゲームシステムでの例外処理
void battleSystem()
{
    try
    {
        // キャラクターの作成
        GameCharacter hero("勇者", 100, 15);
        GameCharacter enemy("スライム", 30, 5);
        
        hero.displayStatus();
        enemy.displayStatus();
        
        // 戦闘シミュレーション
        std::cout << "\n=== 戦闘開始！ ===" << std::endl;
        
        // 敵からのダメージ
        hero.takeDamage(20);
        hero.displayStatus();
        
        // 勇者の攻撃
        enemy.takeDamage(35); // これは例外を投げる（敵のHPが0になる）
        
        // ここは実行されない
        std::cout << "戦闘継続中..." << std::endl;
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "無効な引数エラー: " << e.what() << std::endl;
    }
    catch (const std::runtime_error& e)
    {
        std::cout << "\n戦闘終了: " << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "エラーが発生しました: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "不明なエラーが発生しました" << std::endl;
    }
}

// アイテム使用システム
void itemSystem()
{
    try
    {
        Item potion("ポーション", 3);
        std::cout << "\n現在の" << potion.getName() 
                  << "の数: " << potion.getQuantity() << std::endl;
        
        // ポーションを使用
        potion.use(2);
        std::cout << "残り: " << potion.getQuantity() << std::endl;
        
        // 在庫以上に使用しようとする
        potion.use(5); // 例外が発生
    }
    catch (const std::exception& e)
    {
        std::cerr << "アイテムエラー: " << e.what() << std::endl;
    }
}

// 例外の再スロー
void processCommand(const std::string& command)
{
    try
    {
        if (command == "attack")
        {
            throw GameException("攻撃コマンドは戦闘中のみ使用可能です");
        }
        else if (command == "escape")
        {
            throw std::runtime_error("逃走に失敗しました！");
        }
        else if (command.empty())
        {
            throw std::invalid_argument("コマンドが入力されていません");
        }
        
        std::cout << "コマンド '" << command << "' を実行しました" << std::endl;
    }
    catch (const GameException& e)
    {
        std::cerr << "ゲームエラー: " << e.what() << std::endl;
        throw; // 例外を再スロー
    }
    catch (const std::exception& e)
    {
        std::cerr << "コマンド処理エラー: " << e.what() << std::endl;
        // ログを記録してから再スロー
        std::cerr << "エラーログに記録しました" << std::endl;
        throw;
    }
}

int main()
{
    std::cout << "=== 例外処理の基本 ===" << std::endl;
    
    // 1. 基本的な例外処理
    try
    {
        int level = -1;
        if (level < 1)
        {
            throw std::runtime_error("レベルが不正です");
        }
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "エラー: " << e.what() << std::endl;
    }
    
    // 2. 戦闘システムでの例外処理
    battleSystem();
    
    // 3. アイテムシステムでの例外処理
    itemSystem();
    
    // 4. 例外の再スロー
    std::cout << "\n=== コマンド処理 ===" << std::endl;
    try
    {
        processCommand("attack");
    }
    catch (const std::exception& e)
    {
        std::cerr << "メイン関数でキャッチ: " << e.what() << std::endl;
    }
    
    // 5. 複数の例外型の処理
    std::cout << "\n=== 複数の例外型 ===" << std::endl;
    try
    {
        std::vector<int> inventory(5);
        
        // 範囲外アクセス
        std::cout << "アイテムスロット10: " << inventory.at(10) << std::endl;
    }
    catch (const std::out_of_range& e)
    {
        std::cerr << "範囲外エラー: " << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "その他のエラー: " << e.what() << std::endl;
    }
    
    std::cout << "\nプログラム正常終了" << std::endl;
    
    return 0;
}

// まとめ：
// - 例外はtryブロックで投げられ、catchブロックで処理される
// - 標準例外クラス（std::exception派生）を使用できる
// - カスタム例外クラスを作成できる
// - 例外は型によって異なるcatchブロックで処理される
// - 例外の再スロー（throw;）で上位の処理に委譲できる