// 第1講: 関数の応用
// 関数の様々な形式と実践的な使い方を学ぶ

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

// === 様々な関数の宣言 ===
// 定数を返す関数
const double getPI();

// bool型を返す関数
bool isValidLevel(int level);
bool canEquipItem(int playerLevel, int itemRequiredLevel);

// 計算系の関数
double calculateDistance(double x1, double y1, double x2, double y2);
int calculateExpForNextLevel(int currentLevel);

// ゲームロジック関数
void processLevelUp(int& level, int& hp, int& mp, int& attack);
bool checkQuestCompletion(int monstersDefeated, int itemsCollected);
int calculateFinalScore(int enemiesDefeated, int timeBonus, int itemsCollected);

int main() 
{
    std::cout << "=== 関数の応用例 ===" << std::endl;
    
    // 定数を返す関数の使用
    double pi = getPI();
    std::cout << "円周率: " << pi << std::endl;
    
    // bool型関数での条件判定
    int playerLevel = 15;
    if (isValidLevel(playerLevel)) 
    {
        std::cout << "レベル" << playerLevel << "は有効な値です" << std::endl;
    }
    
    // アイテム装備判定
    int swordRequiredLevel = 10;
    if (canEquipItem(playerLevel, swordRequiredLevel)) 
    {
        std::cout << "伝説の剣を装備できます！" << std::endl;
    }
    
    std::cout << "\n=== ゲーム開発での実践例 ===" << std::endl;
    
    // 距離計算（敵との距離判定）
    double playerX = 10.0, playerY = 5.0;
    double enemyX = 15.0, enemyY = 8.0;
    double distance = calculateDistance(playerX, playerY, enemyX, enemyY);
    std::cout << "敵との距離: " << distance << std::endl;
    
    if (distance < 5.0) 
    {
        std::cout << "敵が攻撃範囲内にいます！" << std::endl;
    }
    
    // レベルアップ処理
    int level = 10;
    int hp = 100;
    int mp = 50;
    int attack = 30;
    
    std::cout << "\nレベルアップ前: Lv." << level 
              << " HP:" << hp << " MP:" << mp << " ATK:" << attack << std::endl;
    
    processLevelUp(level, hp, mp, attack);
    
    std::cout << "レベルアップ後: Lv." << level 
              << " HP:" << hp << " MP:" << mp << " ATK:" << attack << std::endl;
    
    // 次のレベルまでの必要経験値
    int expNeeded = calculateExpForNextLevel(level);
    std::cout << "次のレベルまで: " << expNeeded << " EXP必要" << std::endl;
    
    // クエスト完了判定
    int monstersDefeated = 5;
    int itemsCollected = 3;
    
    if (checkQuestCompletion(monstersDefeated, itemsCollected)) 
    {
        std::cout << "\nクエスト完了！報酬を獲得しました" << std::endl;
    }
    else 
    {
        std::cout << "\nクエスト進行中..." << std::endl;
        std::cout << "モンスター討伐: " << monstersDefeated << "/5" << std::endl;
        std::cout << "アイテム収集: " << itemsCollected << "/3" << std::endl;
    }
    
    // スコア計算
    int enemiesInStage = 20;
    int timeBonus = 500;
    int itemsInStage = 10;
    int finalScore = calculateFinalScore(enemiesInStage, timeBonus, itemsInStage);
    std::cout << "\nステージクリア！" << std::endl;
    std::cout << "最終スコア: " << finalScore << "点" << std::endl;
    
    // 関数の組み合わせ使用例
    std::cout << "\n=== 関数を組み合わせた複雑な処理 ===" << std::endl;
    
    // プレイヤーのステータス初期化
    struct Player 
    {
        int level = 1;
        int hp = 50;
        int mp = 20;
        int attack = 10;
        int exp = 0;
    };
    
    Player hero;
    
    // 経験値獲得とレベルアップチェック
    int expGained = 150;
    hero.exp += expGained;
    std::cout << "経験値を" << expGained << "獲得！" << std::endl;
    
    // レベルアップ可能かチェック
    while (hero.exp >= calculateExpForNextLevel(hero.level - 1)) 
    {
        hero.exp -= calculateExpForNextLevel(hero.level - 1);
        processLevelUp(hero.level, hero.hp, hero.mp, hero.attack);
        std::cout << "レベルアップ！ → Lv." << hero.level << std::endl;
    }
    
    std::cout << "現在の経験値: " << hero.exp << "/" 
              << calculateExpForNextLevel(hero.level - 1) << std::endl;
    
    return 0;
}

// === 関数の定義 ===

const double getPI() 
{
    return 3.14159265359;
}

bool isValidLevel(int level) 
{
    return level >= 1 && level <= 99;
}

bool canEquipItem(int playerLevel, int itemRequiredLevel) 
{
    return playerLevel >= itemRequiredLevel;
}

double calculateDistance(double x1, double y1, double x2, double y2) 
{
    double dx = x2 - x1;
    double dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

int calculateExpForNextLevel(int currentLevel) 
{
    // レベルが上がるごとに必要経験値が増加
    return 100 * currentLevel * currentLevel;
}

void processLevelUp(int& level, int& hp, int& mp, int& attack) 
{
    level++;
    hp += 20;      // HP +20
    mp += 10;      // MP +10
    attack += 5;   // 攻撃力 +5
    
    // レベルが5の倍数の時はボーナス
    if (level % 5 == 0) 
    {
        hp += 10;      // 追加HP +10
        mp += 5;       // 追加MP +5
        attack += 3;   // 追加攻撃力 +3
        std::cout << "★ レベル" << level << "到達ボーナス！" << std::endl;
    }
}

bool checkQuestCompletion(int monstersDefeated, int itemsCollected) 
{
    const int requiredMonsters = 5;
    const int requiredItems = 3;
    
    return (monstersDefeated >= requiredMonsters) && 
           (itemsCollected >= requiredItems);
}

int calculateFinalScore(int enemiesDefeated, int timeBonus, int itemsCollected) 
{
    int baseScore = 0;
    
    // 敵撃破スコア（1体100点）
    baseScore += enemiesDefeated * 100;
    
    // アイテム収集スコア（1個50点）
    baseScore += itemsCollected * 50;
    
    // タイムボーナス
    baseScore += timeBonus;
    
    // パーフェクトボーナス（全ての敵を倒し、全アイテム収集）
    if (enemiesDefeated >= 20 && itemsCollected >= 10) 
    {
        baseScore += 1000;
        std::cout << "パーフェクトボーナス +1000点！" << std::endl;
    }
    
    return baseScore;
}

/*
    まとめ:
    - 関数は様々な型の値を返すことができる（int, double, bool, string等）
    - const修飾子で定数を返すことも可能
    - bool型を返す関数は条件判定に便利
    - 参照渡し（&）で関数内で変数の値を変更できる
    - 関数を組み合わせることで、複雑な処理を分かりやすく実装
    - ゲーム開発では、レベルアップ、スコア計算、距離計算などで活用
    - 適切に関数を分割することで、コードの再利用性と保守性が向上
*/