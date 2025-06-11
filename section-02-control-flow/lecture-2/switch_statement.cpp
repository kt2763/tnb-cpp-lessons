// 第2講: switch文
// 多方向分岐を効率的に記述するswitch文の使い方を学ぶ

#include <iostream>
#include <string>

int main() 
{
    std::cout << "=== switch文の基本構文 ===" << std::endl;
    
    // 基本的なswitch文
    int menuChoice = 2;
    
    switch (menuChoice) 
    {
        case 1:
            std::cout << "ゲームスタート" << std::endl;
            break;
        case 2:
            std::cout << "コンティニュー" << std::endl;
            break;
        case 3:
            std::cout << "オプション" << std::endl;
            break;
        case 4:
            std::cout << "終了" << std::endl;
            break;
        default:
            std::cout << "無効な選択です" << std::endl;
            break;
    }
    
    // breakを使わないフォールスルー
    int difficulty = 3;
    std::cout << "\n難易度による報酬倍率:" << std::endl;
    
    switch (difficulty) 
    {
        case 3:
            std::cout << "ハード: 経験値3倍" << std::endl;
            // breakなし - 意図的なフォールスルー
        case 2:
            std::cout << "ノーマル: ゴールド2倍" << std::endl;
            // breakなし
        case 1:
            std::cout << "イージー: 基本報酬" << std::endl;
            break;
        default:
            std::cout << "不明な難易度" << std::endl;
    }
    
    std::cout << "\n=== ゲーム開発での使用例 ===" << std::endl;
    
    // キャラクタークラスによるステータス設定
    enum class CharacterClass 
    {
        Warrior = 1,
        Mage = 2,
        Archer = 3,
        Thief = 4
    };
    
    CharacterClass playerClass = CharacterClass::Mage;
    int hp = 0, mp = 0, attack = 0, defense = 0;
    
    switch (playerClass) 
    {
        case CharacterClass::Warrior:
            hp = 150;
            mp = 30;
            attack = 80;
            defense = 100;
            std::cout << "戦士を選択しました！" << std::endl;
            break;
            
        case CharacterClass::Mage:
            hp = 70;
            mp = 150;
            attack = 120;
            defense = 40;
            std::cout << "魔法使いを選択しました！" << std::endl;
            break;
            
        case CharacterClass::Archer:
            hp = 100;
            mp = 50;
            attack = 90;
            defense = 60;
            std::cout << "弓使いを選択しました！" << std::endl;
            break;
            
        case CharacterClass::Thief:
            hp = 90;
            mp = 60;
            attack = 110;
            defense = 50;
            std::cout << "盗賊を選択しました！" << std::endl;
            break;
    }
    
    std::cout << "HP: " << hp << ", MP: " << mp 
              << ", 攻撃力: " << attack << ", 防御力: " << defense << std::endl;
    
    // アイテムタイプによる効果
    int itemType = 2;  // 1:回復薬, 2:MP回復薬, 3:解毒薬
    int currentHP = 50, maxHP = 100;
    int currentMP = 20, maxMP = 50;
    bool isPoisoned = true;
    
    std::cout << "\nアイテム使用前 - HP: " << currentHP << "/" << maxHP 
              << ", MP: " << currentMP << "/" << maxMP 
              << (isPoisoned ? " [毒]" : "") << std::endl;
    
    switch (itemType) 
    {
        case 1:  // HP回復薬
            currentHP = std::min(currentHP + 50, maxHP);
            std::cout << "HPを50回復しました！" << std::endl;
            break;
            
        case 2:  // MP回復薬
            currentMP = std::min(currentMP + 30, maxMP);
            std::cout << "MPを30回復しました！" << std::endl;
            break;
            
        case 3:  // 解毒薬
            if (isPoisoned) 
            {
                isPoisoned = false;
                std::cout << "毒を治療しました！" << std::endl;
            }
            else 
            {
                std::cout << "毒状態ではありません。" << std::endl;
            }
            break;
            
        default:
            std::cout << "不明なアイテムです。" << std::endl;
    }
    
    std::cout << "アイテム使用後 - HP: " << currentHP << "/" << maxHP 
              << ", MP: " << currentMP << "/" << maxMP 
              << (isPoisoned ? " [毒]" : "") << std::endl;
    
    // 方向キー入力のシミュレーション
    char direction = 'w';  // w:上, s:下, a:左, d:右
    int playerX = 5, playerY = 5;
    
    std::cout << "\n現在位置: (" << playerX << ", " << playerY << ")" << std::endl;
    
    switch (direction) 
    {
        case 'w':
        case 'W':
            playerY--;
            std::cout << "北へ移動しました" << std::endl;
            break;
            
        case 's':
        case 'S':
            playerY++;
            std::cout << "南へ移動しました" << std::endl;
            break;
            
        case 'a':
        case 'A':
            playerX--;
            std::cout << "西へ移動しました" << std::endl;
            break;
            
        case 'd':
        case 'D':
            playerX++;
            std::cout << "東へ移動しました" << std::endl;
            break;
            
        default:
            std::cout << "無効な方向です" << std::endl;
    }
    
    std::cout << "新しい位置: (" << playerX << ", " << playerY << ")" << std::endl;
    
    return 0;
}

/*
    まとめ:
    - switch文は、一つの変数の値に基づいて多方向に分岐する構文
    - 整数型、文字型、列挙型で使用可能（文字列は不可）
    - 各caseの最後にはbreakが必要（フォールスルーを防ぐため）
    - defaultケースで、どのcaseにも該当しない場合の処理を記述
    - ゲーム開発では、メニュー選択、キャラクタータイプ、アイテム効果などで活用
    - if-else ifの連続より読みやすく、実行速度も速い場合がある
*/