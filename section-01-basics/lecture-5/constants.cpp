// 第5講: 定数
// 値が変更されない変数の定義方法

#include <iostream>

// #defineによるマクロ定数（古い方法、非推奨）
#define OLD_MAX_LEVEL 99

// constによる定数（推奨）
const int MAX_PLAYER_LEVEL = 100;
const double CRITICAL_RATE = 0.15;  // 15%
const int MAX_INVENTORY_SIZE = 50;

// C++11以降: constexpr（コンパイル時定数）
constexpr int MAX_PARTY_SIZE = 4;
constexpr double EXP_MULTIPLIER = 1.5;

int main() 
{
    std::cout << "=== 定数の使い方 ===" << std::endl;
    
    // ローカル定数
    const int startingHP = 100;
    const std::string gameName = "冒険者の旅";
    
    std::cout << "ゲーム名: " << gameName << std::endl;
    std::cout << "初期HP: " << startingHP << std::endl;
    std::cout << "最大レベル: " << MAX_PLAYER_LEVEL << std::endl;
    std::cout << "クリティカル率: " << (CRITICAL_RATE * 100) << "%" << std::endl;
    
    // 定数は変更できない
    // startingHP = 200;  // エラー！定数は変更不可
    
    // ゲーム設定の定数
    std::cout << "\n=== ゲーム設定 ===" << std::endl;
    
    // ダメージ計算用の定数
    const int BASE_ATTACK = 50;
    const double DEFENSE_REDUCTION = 0.7;  // 防御で30%軽減
    const double ELEMENTAL_BONUS = 1.2;    // 属性相性ボーナス20%
    
    int enemyDefense = 20;
    int damage = static_cast<int>(BASE_ATTACK * DEFENSE_REDUCTION);
    int elementalDamage = static_cast<int>(BASE_ATTACK * ELEMENTAL_BONUS);
    
    std::cout << "基本攻撃力: " << BASE_ATTACK << std::endl;
    std::cout << "通常ダメージ: " << damage << std::endl;
    std::cout << "属性ダメージ: " << elementalDamage << std::endl;
    
    // 配列のサイズに定数を使用
    const int SKILL_SLOTS = 4;
    int playerSkills[SKILL_SLOTS] = {101, 102, 103, 104};  // スキルID
    
    std::cout << "\n=== 装備可能スキル ===" << std::endl;
    for (int i = 0; i < SKILL_SLOTS; ++i) 
    {
        std::cout << "スロット" << (i + 1) << ": スキルID " << playerSkills[i] << std::endl;
    }
    
    // enumを使った定数グループ
    enum class ItemRarity 
    {
        COMMON = 1,
        UNCOMMON = 2,
        RARE = 3,
        EPIC = 4,
        LEGENDARY = 5
    };
    
    // 定数式の使用
    constexpr int MAX_PLAYERS = 100;
    constexpr int MAX_ENEMIES = 50;
    constexpr int MAX_ENTITIES = MAX_PLAYERS + MAX_ENEMIES;
    
    std::cout << "\n=== サーバー設定 ===" << std::endl;
    std::cout << "最大プレイヤー数: " << MAX_PLAYERS << std::endl;
    std::cout << "最大敵数: " << MAX_ENEMIES << std::endl;
    std::cout << "最大エンティティ数: " << MAX_ENTITIES << std::endl;
    
    // 定数を使った条件判定
    int playerLevel = 95;
    
    if (playerLevel >= MAX_PLAYER_LEVEL) 
    {
        std::cout << "\nレベルが最大に達しています！" << std::endl;
    }
    else 
    {
        std::cout << "\n次のレベルまで: " << (MAX_PLAYER_LEVEL - playerLevel) << std::endl;
    }
    
    return 0;
}

/*
    定数の種類と使い分け:
    
    1. #define（マクロ定数）:
       - 古い方法、型安全性がない
       - 現在は非推奨
    
    2. const:
       - 実行時に値が決まる定数
       - 型安全
       - 最も一般的
    
    3. constexpr:
       - コンパイル時に値が決まる定数
       - より効率的
       - C++11以降
    
    定数を使うメリット:
    - マジックナンバーを避ける
    - 値の変更が一箇所で済む
    - コードの可読性向上
    - 誤った値の変更を防ぐ
    
    ゲーム開発での定数の例:
    - ゲーム設定値（最大レベル、初期値など）
    - 物理定数（重力、摩擦係数など）
    - UI設定（画面サイズ、色など）
    - ゲームルール（制限時間、最大人数など）
*/