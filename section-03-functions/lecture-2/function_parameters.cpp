// 第2講: 関数の引数
// 値渡し、参照渡し、const参照渡しの違いと使い分けを学ぶ

#include <iostream>
#include <string>
#include <vector>

// === 値渡し ===
void healByValue(int hp) 
{
    hp += 50;  // コピーを変更（元の値は変わらない）
    std::cout << "関数内のHP: " << hp << std::endl;
}

// === 参照渡し ===
void healByReference(int& hp) 
{
    hp += 50;  // 元の値を直接変更
    std::cout << "関数内のHP: " << hp << std::endl;
}

// === const参照渡し ===
void displayPlayerInfo(const std::string& name, const int& level) 
{
    // const参照なので値を変更できない（読み取り専用）
    // name = "変更";  // エラー！
    std::cout << "プレイヤー: " << name << " (Lv." << level << ")" << std::endl;
}

// === ポインタ渡し ===
void healByPointer(int* hp) 
{
    if (hp != nullptr) 
    {
        *hp += 50;  // ポインタが指す値を変更
        std::cout << "関数内のHP: " << *hp << std::endl;
    }
}

// === 実践的な例 ===
// 複数の値を変更する関数
void takeDamage(int& hp, int& mp, int damage, bool isMagicDamage) 
{
    if (isMagicDamage) 
    {
        mp -= damage / 2;  // 魔法ダメージはMPも減少
        if (mp < 0) mp = 0;
    }
    
    hp -= damage;
    if (hp < 0) hp = 0;
}

// 大きなオブジェクトはconst参照で受け取る
double calculateAverageLevel(const std::vector<int>& levels) 
{
    if (levels.empty()) return 0.0;
    
    int sum = 0;
    for (int level : levels) 
    {
        sum += level;
    }
    
    return static_cast<double>(sum) / levels.size();
}

// 構造体の引数
struct Character 
{
    std::string name;
    int hp;
    int maxHp;
    int mp;
    int maxMp;
};

// 値渡し（コピーが作られる）
void displayCharacterByValue(Character character) 
{
    std::cout << character.name << " HP:" << character.hp << "/" << character.maxHp << std::endl;
    character.hp = 0;  // コピーなので元のデータは変わらない
}

// 参照渡し（元のデータを変更可能）
void fullRestore(Character& character) 
{
    character.hp = character.maxHp;
    character.mp = character.maxMp;
    std::cout << character.name << "のHPとMPが完全回復した！" << std::endl;
}

// const参照渡し（読み取り専用、効率的）
void printDetailedStats(const Character& character) 
{
    std::cout << "\n=== " << character.name << " の詳細ステータス ===" << std::endl;
    std::cout << "HP: " << character.hp << "/" << character.maxHp;
    
    // HPバー表示
    int barLength = 20;
    int filledLength = (character.hp * barLength) / character.maxHp;
    std::cout << " [";
    for (int i = 0; i < barLength; i++) 
    {
        std::cout << (i < filledLength ? "■" : "□");
    }
    std::cout << "]" << std::endl;
    
    std::cout << "MP: " << character.mp << "/" << character.maxMp;
    
    // MPバー表示
    filledLength = (character.mp * barLength) / character.maxMp;
    std::cout << " [";
    for (int i = 0; i < barLength; i++) 
    {
        std::cout << (i < filledLength ? "▰" : "▱");
    }
    std::cout << "]" << std::endl;
}

int main() 
{
    std::cout << "=== 値渡しと参照渡しの違い ===" << std::endl;
    
    // 値渡しの例
    int playerHp = 50;
    std::cout << "値渡しの例:" << std::endl;
    std::cout << "元のHP: " << playerHp << std::endl;
    healByValue(playerHp);
    std::cout << "関数呼び出し後のHP: " << playerHp << std::endl;  // 変わらない
    
    // 参照渡しの例
    std::cout << "\n参照渡しの例:" << std::endl;
    std::cout << "元のHP: " << playerHp << std::endl;
    healByReference(playerHp);
    std::cout << "関数呼び出し後のHP: " << playerHp << std::endl;  // 変わる
    
    // ポインタ渡しの例
    std::cout << "\nポインタ渡しの例:" << std::endl;
    std::cout << "元のHP: " << playerHp << std::endl;
    healByPointer(&playerHp);
    std::cout << "関数呼び出し後のHP: " << playerHp << std::endl;  // 変わる
    
    std::cout << "\n=== ゲーム開発での実践例 ===" << std::endl;
    
    // ダメージ処理
    int hp = 100;
    int mp = 50;
    
    std::cout << "戦闘開始 - HP:" << hp << " MP:" << mp << std::endl;
    
    takeDamage(hp, mp, 30, false);  // 物理ダメージ
    std::cout << "物理攻撃を受けた - HP:" << hp << " MP:" << mp << std::endl;
    
    takeDamage(hp, mp, 40, true);   // 魔法ダメージ
    std::cout << "魔法攻撃を受けた - HP:" << hp << " MP:" << mp << std::endl;
    
    // パーティの平均レベル計算
    std::vector<int> partyLevels = {15, 18, 14, 16};
    double avgLevel = calculateAverageLevel(partyLevels);
    std::cout << "\nパーティの平均レベル: " << avgLevel << std::endl;
    
    // 構造体の引数渡し
    Character hero = {"勇者", 80, 150, 30, 80};
    
    std::cout << "\n構造体の値渡し:" << std::endl;
    displayCharacterByValue(hero);  // コピーが渡される
    std::cout << "関数呼び出し後のHP: " << hero.hp << std::endl;  // 変わらない
    
    std::cout << "\n構造体の参照渡し:" << std::endl;
    printDetailedStats(hero);
    fullRestore(hero);
    printDetailedStats(hero);
    
    // 複数キャラクターの処理
    std::vector<Character> party = {
        {"戦士", 50, 200, 10, 30},
        {"魔法使い", 30, 80, 5, 150},
        {"僧侶", 40, 100, 60, 120}
    };
    
    std::cout << "\n=== パーティ全員を回復 ===" << std::endl;
    for (Character& member : party)  // 参照で受け取る
    {
        fullRestore(member);
    }
    
    // const参照の利点
    std::string playerName = "アレックス";
    int playerLevel = 25;
    displayPlayerInfo(playerName, playerLevel);  // 効率的にデータを渡す
    
    return 0;
}

/*
    まとめ:
    - 値渡し：引数のコピーが作られる。元の値は変更されない
    - 参照渡し（&）：元の値を直接操作。値を変更したい場合に使用
    - const参照渡し：読み取り専用で効率的。大きなオブジェクトに推奨
    - ポインタ渡し：nullチェックが必要だが、より柔軟な操作が可能
    
    使い分けの指針：
    - 基本型の読み取り → 値渡し
    - 基本型の変更 → 参照渡し
    - 大きなオブジェクトの読み取り → const参照渡し
    - 大きなオブジェクトの変更 → 参照渡し
    - オプショナルな引数 → ポインタ渡し（nullptrチェック）
    
    ゲーム開発では、キャラクターデータや配列など大きなデータを
    扱うことが多いため、適切な引数の渡し方を選ぶことが重要
*/