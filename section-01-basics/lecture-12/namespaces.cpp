// 第12講: 名前空間（namespace）
// 名前の衝突を避け、コードを整理する仕組み

#include <iostream>
#include <string>

// グローバル名前空間の関数
void displayMessage() 
{
    std::cout << "グローバル名前空間の関数です" << std::endl;
}

// ゲーム関連の名前空間
namespace Game 
{
    const int MAX_LEVEL = 99;
    int playerCount = 0;
    
    void startGame() 
    {
        std::cout << "ゲームを開始します！" << std::endl;
    }
    
    void displayMessage() 
    {
        std::cout << "Game名前空間の関数です" << std::endl;
    }
    
    // 入れ子の名前空間
    namespace Player 
    {
        struct Stats 
        {
            int level;
            int hp;
            int mp;
            std::string name;
        };
        
        void levelUp(Stats& player) 
        {
            if (player.level < MAX_LEVEL) 
            {
                player.level++;
                player.hp += 10;
                player.mp += 5;
                std::cout << player.name << " がレベルアップ！ Lv." << player.level << std::endl;
            }
        }
        
        void displayStatus(const Stats& player) 
        {
            std::cout << "=== " << player.name << " のステータス ===" << std::endl;
            std::cout << "レベル: " << player.level << std::endl;
            std::cout << "HP: " << player.hp << std::endl;
            std::cout << "MP: " << player.mp << std::endl;
        }
    }
    
    // アイテム関連の名前空間
    namespace Item 
    {
        enum class Type 
        {
            POTION,
            WEAPON,
            ARMOR,
            ACCESSORY
        };
        
        struct ItemData 
        {
            int id;
            std::string name;
            Type type;
            int value;
        };
        
        void usePotion(Game::Player::Stats& player, int healAmount) 
        {
            player.hp += healAmount;
            std::cout << player.name << " はポーションを使用した！ HP+" << healAmount << std::endl;
        }
        
        void displayItem(const ItemData& item) 
        {
            std::cout << "アイテム: " << item.name << " (ID:" << item.id << ")" << std::endl;
        }
    }
}

// 別のプロジェクトやライブラリの名前空間（名前が衝突する例）
namespace Graphics 
{
    void displayMessage() 
    {
        std::cout << "Graphics名前空間の関数です" << std::endl;
    }
    
    struct Color 
    {
        int red, green, blue;
    };
    
    void setBackgroundColor(const Color& color) 
    {
        std::cout << "背景色を設定: RGB(" 
                  << color.red << ", " << color.green << ", " << color.blue << ")" << std::endl;
    }
}

// using宣言とusing指示文の例
namespace Math 
{
    const double PI = 3.14159;
    
    double calculateArea(double radius) 
    {
        return PI * radius * radius;
    }
    
    double calculateDistance(double x1, double y1, double x2, double y2) 
    {
        double dx = x2 - x1;
        double dy = y2 - y1;
        return std::sqrt(dx * dx + dy * dy);
    }
}

int main() 
{
    std::cout << "=== 名前空間の基本 ===" << std::endl;
    
    // 名前空間を指定して関数を呼び出し
    displayMessage();           // グローバル
    Game::displayMessage();     // Game名前空間
    Graphics::displayMessage(); // Graphics名前空間
    
    // 名前空間内の定数を使用
    std::cout << "最大レベル: " << Game::MAX_LEVEL << std::endl;
    
    // プレイヤーを作成
    Game::Player::Stats hero = {1, 100, 50, "勇者"};
    Game::Player::displayStatus(hero);
    
    // レベルアップ
    Game::Player::levelUp(hero);
    Game::Player::displayStatus(hero);
    
    // アイテムの使用
    Game::Item::usePotion(hero, 30);
    Game::Player::displayStatus(hero);
    
    // アイテム情報の表示
    Game::Item::ItemData sword = {101, "鉄の剣", Game::Item::Type::WEAPON, 200};
    Game::Item::displayItem(sword);
    
    std::cout << "\n=== using宣言の使用 ===" << std::endl;
    
    // using宣言：特定の名前のみを導入
    using Game::Player::Stats;
    using Game::Player::levelUp;
    
    Stats mage = {1, 80, 100, "魔法使い"};
    levelUp(mage);  // Game::Player:: を省略可能
    
    // 別の名前空間も使いたい場合
    using Graphics::Color;
    Color bgColor = {50, 100, 150};
    Graphics::setBackgroundColor(bgColor);
    
    std::cout << "\n=== using指示文の使用 ===" << std::endl;
    
    // using指示文：名前空間全体を導入（注意して使用）
    {
        using namespace Math;
        
        double radius = 5.0;
        double area = calculateArea(radius);  // Math:: を省略
        std::cout << "半径 " << radius << " の円の面積: " << area << std::endl;
        
        double distance = calculateDistance(0, 0, 3, 4);
        std::cout << "距離: " << distance << std::endl;
    }
    
    // 名前空間エイリアス
    std::cout << "\n=== 名前空間エイリアス ===" << std::endl;
    
    namespace GP = Game::Player;  // 長い名前に短いエイリアスを作成
    
    GP::Stats archer = {5, 150, 75, "弓使い"};
    GP::displayStatus(archer);
    
    // 無名名前空間の例（このファイル内でのみ有効）
    namespace 
    {
        const int SECRET_CODE = 12345;
        
        void internalFunction() 
        {
            std::cout << "内部関数：シークレットコード = " << SECRET_CODE << std::endl;
        }
    }
    
    internalFunction();  // 無名名前空間の関数は直接呼び出し可能
    
    // 実際のゲーム開発例
    std::cout << "\n=== ゲーム開発での名前空間活用例 ===" << std::endl;
    
    // 複数のプレイヤーを管理
    std::vector<Game::Player::Stats> party;
    party.push_back({"戦士", 10, 200, 80});
    party.push_back({"僧侶", 8, 150, 120});
    party.push_back({"盗賊", 12, 180, 90});
    
    std::cout << "パーティメンバー:" << std::endl;
    for (const auto& member : party) 
    {
        Game::Player::displayStatus(member);
    }
    
    return 0;
}

/*
    名前空間の使い方:
    
    基本構文:
    namespace 名前空間名 {
        // 宣言や定義
    }
    
    アクセス方法:
    - 名前空間名::識別子
    - using宣言: using 名前空間名::識別子;
    - using指示文: using namespace 名前空間名;
    
    メリット:
    - 名前の衝突を防ぐ
    - コードの整理と分類
    - 大規模プロジェクトでの管理
    
    注意点:
    - using namespace std; はヘッダファイルで使わない
    - グローバルスコープでのusing指示文は慎重に
    - 深すぎる入れ子は避ける
    
    ゲーム開発での活用:
    - システム別の分類（Player, Enemy, Item, Graphics等）
    - ライブラリとの名前衝突回避
    - 大規模プロジェクトでのモジュール分割
    
    ベストプラクティス:
    - 短すぎず長すぎない名前
    - 機能や責任で論理的に分割
    - エイリアスで長い名前を短縮
*/