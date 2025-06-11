// 第1講: 関数の基本
// 関数の定義と呼び出し、基本的な使い方を学ぶ

#include <iostream>
#include <string>

// === 関数の宣言（プロトタイプ宣言）===
void greetPlayer();
int calculateDamage(int baseDamage, int attackPower);
std::string getCharacterClass(int classId);
void displayStats(std::string name, int hp, int mp);

// === main関数 ===
int main() 
{
    std::cout << "=== 関数の基本的な使い方 ===" << std::endl;
    
    // 戻り値なし、引数なしの関数
    greetPlayer();
    
    // 戻り値あり、引数ありの関数
    int baseDamage = 50;
    int attackPower = 30;
    int totalDamage = calculateDamage(baseDamage, attackPower);
    std::cout << "\n計算されたダメージ: " << totalDamage << std::endl;
    
    // 文字列を返す関数
    int selectedClass = 2;
    std::string className = getCharacterClass(selectedClass);
    std::cout << "選択したクラス: " << className << std::endl;
    
    // 複数の引数を持つ関数
    displayStats("勇者", 100, 50);
    
    std::cout << "\n=== ゲーム開発での実践例 ===" << std::endl;
    
    // 関数を組み合わせた処理
    std::string playerName = "アレックス";
    int playerClass = 1;
    int playerLevel = 5;
    
    std::cout << "プレイヤー名: " << playerName << std::endl;
    std::cout << "クラス: " << getCharacterClass(playerClass) << std::endl;
    
    // レベルに応じたダメージ計算
    int levelBonus = playerLevel * 10;
    int damage = calculateDamage(baseDamage, attackPower + levelBonus);
    std::cout << "レベル" << playerLevel << "の攻撃力: " << damage << std::endl;
    
    return 0;
}

// === 関数の定義 ===

// 戻り値なし（void）、引数なしの関数
void greetPlayer() 
{
    std::cout << "ようこそ、冒険者よ！" << std::endl;
    std::cout << "この世界には多くの試練が待ち受けている..." << std::endl;
}

// 戻り値あり（int）、引数ありの関数
int calculateDamage(int baseDamage, int attackPower) 
{
    // ダメージ計算式
    int damage = baseDamage + (attackPower * 2);
    
    // クリティカル判定（簡易版）
    bool isCritical = (attackPower > 40);
    if (isCritical) 
    {
        damage = damage * 1.5;
        std::cout << "クリティカルヒット！" << std::endl;
    }
    
    return damage;
}

// 文字列を返す関数
std::string getCharacterClass(int classId) 
{
    std::string className;
    
    switch (classId) 
    {
        case 1:
            className = "戦士";
            break;
        case 2:
            className = "魔法使い";
            break;
        case 3:
            className = "盗賊";
            break;
        case 4:
            className = "僧侶";
            break;
        default:
            className = "冒険者";
            break;
    }
    
    return className;
}

// 複数の引数を持つ関数
void displayStats(std::string name, int hp, int mp) 
{
    std::cout << "\n--- ステータス表示 ---" << std::endl;
    std::cout << "名前: " << name << std::endl;
    std::cout << "HP: " << hp << std::endl;
    std::cout << "MP: " << mp << std::endl;
    
    // HPの状態を判定
    if (hp > 80) 
    {
        std::cout << "状態: 健康" << std::endl;
    }
    else if (hp > 30) 
    {
        std::cout << "状態: 負傷" << std::endl;
    }
    else 
    {
        std::cout << "状態: 危険！" << std::endl;
    }
}

/*
    まとめ:
    - 関数は特定の処理をまとめて再利用可能にする
    - 関数の構成要素：戻り値の型、関数名、引数リスト、関数本体
    - void型は戻り値がない関数を表す
    - 関数は使用前に宣言（プロトタイプ宣言）または定義が必要
    - return文で値を返す（void型以外）
    - ゲーム開発では、ダメージ計算、ステータス表示など頻繁に使う処理を関数化
    - 適切な関数名をつけることで、コードの可読性が向上
*/