// 第5講: break文とcontinue文
// ループの流れを制御するbreak文とcontinue文の使い方を学ぶ

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

int main() 
{
    std::cout << "=== break文の基本 ===" << std::endl;
    
    // break文でループを抜ける
    for (int i = 1; i <= 10; i++) 
    {
        std::cout << i << " ";
        if (i == 5) 
        {
            std::cout << "\n5に到達したので終了" << std::endl;
            break;
        }
    }
    
    std::cout << "\n=== continue文の基本 ===" << std::endl;
    
    // continue文で次の繰り返しへ
    for (int i = 1; i <= 10; i++) 
    {
        if (i % 2 == 0) 
        {
            continue;  // 偶数はスキップ
        }
        std::cout << i << " ";
    }
    std::cout << "\n奇数のみ表示されました" << std::endl;
    
    std::cout << "\n=== ゲーム開発での使用例 ===" << std::endl;
    
    // 宝探しゲーム
    std::srand(std::time(nullptr));
    const int maxBoxes = 10;
    int treasureBox = std::rand() % maxBoxes + 1;  // 1-10のランダム
    
    std::cout << "宝探しゲーム（1-10の箱から宝を見つけよう）" << std::endl;
    
    for (int box = 1; box <= maxBoxes; box++) 
    {
        std::cout << "箱" << box << "を開ける... ";
        
        if (box == treasureBox) 
        {
            std::cout << "宝を発見！" << std::endl;
            break;  // 宝を見つけたら探索終了
        }
        else 
        {
            std::cout << "空っぽ" << std::endl;
        }
    }
    
    // アイテム使用システム
    struct Item 
    {
        std::string name;
        int quantity;
        bool isUsable;
    };
    
    std::vector<Item> items = {
        {"ポーション", 5, true},
        {"壊れた剣", 1, false},
        {"エリクサー", 2, true},
        {"古い地図", 1, false},
        {"テント", 3, true}
    };
    
    std::cout << "\n使用可能なアイテム一覧:" << std::endl;
    
    for (const Item& item : items) 
    {
        if (!item.isUsable) 
        {
            continue;  // 使用不可アイテムはスキップ
        }
        
        std::cout << "- " << item.name << " (×" << item.quantity << ")" << std::endl;
    }
    
    // 敵のターゲット選択
    struct Character 
    {
        std::string name;
        int hp;
        bool isDefending;
    };
    
    std::vector<Character> party = {
        {"勇者", 100, false},
        {"戦士", 0, false},      // 戦闘不能
        {"魔法使い", 70, true},  // 防御中
        {"僧侶", 80, false}
    };
    
    std::cout << "\n敵の攻撃ターゲット選択:" << std::endl;
    Character* target = nullptr;
    
    for (Character& member : party) 
    {
        std::cout << member.name << " をチェック... ";
        
        if (member.hp <= 0) 
        {
            std::cout << "戦闘不能のためスキップ" << std::endl;
            continue;
        }
        
        if (member.isDefending) 
        {
            std::cout << "防御中（ターゲット優先度低）" << std::endl;
            continue;
        }
        
        std::cout << "ターゲットに選択！" << std::endl;
        target = &member;
        break;  // 最初の有効なターゲットで決定
    }
    
    if (target) 
    {
        std::cout << "敵は" << target->name << "を攻撃！" << std::endl;
    }
    
    // スキル習得チェック
    int playerLevel = 8;
    
    struct Skill 
    {
        std::string name;
        int requiredLevel;
    };
    
    std::vector<Skill> skillTree = {
        {"ファイア", 1},
        {"ファイラ", 5},
        {"ファイガ", 10},
        {"メテオ", 15},
        {"アルテマ", 20}
    };
    
    std::cout << "\n習得済みスキル:" << std::endl;
    int learnedCount = 0;
    
    for (const Skill& skill : skillTree) 
    {
        if (skill.requiredLevel > playerLevel) 
        {
            break;  // 必要レベルを超えたら終了（後のスキルも習得不可）
        }
        
        std::cout << "- " << skill.name << " (Lv." << skill.requiredLevel << ")" << std::endl;
        learnedCount++;
    }
    
    std::cout << "合計" << learnedCount << "個のスキルを習得しています" << std::endl;
    
    // ネストしたループでのbreak
    const int mapSize = 5;
    char gameMap[mapSize][mapSize] = {
        {'.', '.', '.', '.', '.'},
        {'.', '.', 'E', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', 'T', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };
    
    std::cout << "\nマップ上で敵(E)を探索:" << std::endl;
    bool enemyFound = false;
    int enemyX = -1, enemyY = -1;
    
    for (int y = 0; y < mapSize; y++) 
    {
        for (int x = 0; x < mapSize; x++) 
        {
            if (gameMap[y][x] == 'E') 
            {
                enemyX = x;
                enemyY = y;
                enemyFound = true;
                break;  // 内側のループを抜ける
            }
        }
        
        if (enemyFound) 
        {
            break;  // 外側のループも抜ける
        }
    }
    
    if (enemyFound) 
    {
        std::cout << "敵を座標(" << enemyX << ", " << enemyY << ")で発見！" << std::endl;
    }
    
    // 無限ループでのbreak使用
    int actionPoints = 3;
    int actionCount = 0;
    
    std::cout << "\n行動ポイントシステム（AP: " << actionPoints << "）" << std::endl;
    
    while (true) 
    {
        actionCount++;
        std::cout << "行動" << actionCount << ": ";
        
        // シミュレーション用のランダム行動
        int actionCost = std::rand() % 2 + 1;  // 1-2のコスト
        
        if (actionPoints < actionCost) 
        {
            std::cout << "AP不足で行動終了" << std::endl;
            break;
        }
        
        actionPoints -= actionCost;
        std::cout << "実行（コスト" << actionCost << "）残りAP: " << actionPoints << std::endl;
        
        if (actionPoints == 0) 
        {
            std::cout << "APを使い切りました" << std::endl;
            break;
        }
    }
    
    return 0;
}

/*
    まとめ:
    - break文は、ループを即座に終了して次の処理へ移る
    - continue文は、現在の繰り返しをスキップして次の繰り返しへ移る
    - switch文でもbreak文を使用（フォールスルー防止）
    - ネストしたループでは、breakは最も内側のループのみを抜ける
    - ゲーム開発では、探索処理、ターゲット選択、条件チェックなどで活用
    - 無限ループと組み合わせて、特定条件での終了処理に使用
    - 適切に使用することで、効率的で読みやすいコードになる
*/