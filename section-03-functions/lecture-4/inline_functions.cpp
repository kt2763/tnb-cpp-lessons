// 第4講: インライン関数
// 実行速度を向上させるインライン関数の使い方を学ぶ

#include <iostream>
#include <string>
#include <chrono>
#include <cmath>

// === インライン関数の基本 ===
// 通常の関数
int add(int a, int b) 
{
    return a + b;
}

// インライン関数
inline int addInline(int a, int b) 
{
    return a + b;
}

// より複雑なインライン関数
inline int getDamageWithCritical(int baseDamage, bool isCritical) 
{
    return isCritical ? baseDamage * 2 : baseDamage;
}

// === ゲーム開発でよく使うインライン関数 ===
// 最小値・最大値のクランプ
inline int clamp(int value, int min, int max) 
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

// HP/MPの安全な減算
inline int safeDamage(int currentHp, int damage) 
{
    return (currentHp - damage > 0) ? currentHp - damage : 0;
}

// 距離計算（2D）
inline double distance2D(double x1, double y1, double x2, double y2) 
{
    double dx = x2 - x1;
    double dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

// 範囲内判定
inline bool isInRange(double distance, double range) 
{
    return distance <= range;
}

// レベルからステータス計算
inline int calculateStatFromLevel(int baseValue, int level, int growthRate) 
{
    return baseValue + (level - 1) * growthRate;
}

// === クラス内のインライン関数 ===
class GameCharacter 
{
private:
    int hp;
    int maxHp;
    int mp;
    int maxMp;
    
public:
    GameCharacter(int health, int mana) : hp(health), maxHp(health), mp(mana), maxMp(mana) {}
    
    // クラス内で定義された関数は暗黙的にインライン
    int getHp() const { return hp; }
    int getMaxHp() const { return maxHp; }
    
    // 明示的なインライン関数
    inline double getHpPercentage() const 
    {
        return static_cast<double>(hp) / maxHp * 100.0;
    }
    
    inline bool isAlive() const 
    {
        return hp > 0;
    }
    
    inline void takeDamage(int damage) 
    {
        hp = safeDamage(hp, damage);
    }
    
    inline void heal(int amount) 
    {
        hp = clamp(hp + amount, 0, maxHp);
    }
    
    void displayStatus() const 
    {
        std::cout << "HP: " << hp << "/" << maxHp 
                  << " (" << getHpPercentage() << "%)" << std::endl;
        std::cout << "MP: " << mp << "/" << maxMp << std::endl;
    }
};

// === パフォーマンスが重要な計算関数 ===
// 攻撃判定（頻繁に呼ばれる）
inline bool checkHit(int accuracy, int evasion) 
{
    int hitChance = accuracy - evasion;
    hitChance = clamp(hitChance, 5, 95);  // 5%〜95%の範囲
    return (std::rand() % 100) < hitChance;
}

// ダメージ軽減計算
inline int applyDefense(int damage, int defense) 
{
    // 防御力による軽減（簡易式）
    double reduction = defense / (defense + 100.0);
    return damage * (1.0 - reduction);
}

// 経験値計算
inline int calculateExp(int enemyLevel, int playerLevel) 
{
    int baseExp = 100;
    int levelDiff = enemyLevel - playerLevel;
    
    // レベル差による経験値補正
    if (levelDiff > 0) 
    {
        return baseExp * (1 + levelDiff * 0.1);  // 高レベルの敵は経験値増加
    }
    else 
    {
        return baseExp * std::max(0.1, 1 + levelDiff * 0.1);  // 低レベルの敵は経験値減少
    }
}

// === インライン関数の適切な使用例と不適切な例 ===
// 適切：小さくて頻繁に呼ばれる関数
inline bool isEven(int n) 
{
    return n % 2 == 0;
}

// 不適切な例（コメントアウト）：大きすぎる関数
// inline void complexBattleSystem() {
//     // 100行以上の複雑な処理...
//     // インライン化するには大きすぎる
// }

// constexprを使った定数式（C++11以降）
constexpr int getMaxLevel() 
{
    return 99;
}

constexpr double getCriticalMultiplier() 
{
    return 1.5;
}

int main() 
{
    std::cout << "=== インライン関数の基本使用例 ===" << std::endl;
    
    // 通常の関数とインライン関数の使用
    int a = 10, b = 20;
    std::cout << "通常の加算: " << add(a, b) << std::endl;
    std::cout << "インライン加算: " << addInline(a, b) << std::endl;
    
    // ダメージ計算
    int damage = 50;
    std::cout << "通常ダメージ: " << getDamageWithCritical(damage, false) << std::endl;
    std::cout << "クリティカルダメージ: " << getDamageWithCritical(damage, true) << std::endl;
    
    std::cout << "\n=== ゲーム開発での実践例 ===" << std::endl;
    
    // HP管理
    int playerHp = 100;
    int enemyDamage = 30;
    
    std::cout << "プレイヤーHP: " << playerHp << std::endl;
    playerHp = safeDamage(playerHp, enemyDamage);
    std::cout << enemyDamage << "ダメージ → HP: " << playerHp << std::endl;
    
    // クランプ機能
    int healAmount = 50;
    playerHp = clamp(playerHp + healAmount, 0, 100);
    std::cout << healAmount << "回復 → HP: " << playerHp << " (最大100でクランプ)" << std::endl;
    
    // 距離と範囲判定
    double px = 10.0, py = 10.0;
    double ex = 15.0, ey = 13.0;
    double dist = distance2D(px, py, ex, ey);
    double attackRange = 6.0;
    
    std::cout << "\n敵との距離: " << dist << std::endl;
    if (isInRange(dist, attackRange)) 
    {
        std::cout << "攻撃範囲内です！" << std::endl;
    }
    else 
    {
        std::cout << "攻撃範囲外です（範囲: " << attackRange << "）" << std::endl;
    }
    
    // キャラクタークラスの使用
    std::cout << "\n=== キャラクターステータス ===" << std::endl;
    GameCharacter hero(150, 50);
    
    hero.displayStatus();
    
    hero.takeDamage(60);
    std::cout << "\n60ダメージを受けた！" << std::endl;
    hero.displayStatus();
    
    hero.heal(40);
    std::cout << "\n40回復！" << std::endl;
    hero.displayStatus();
    
    // 戦闘シミュレーション
    std::cout << "\n=== 戦闘シミュレーション ===" << std::endl;
    
    int playerAccuracy = 80;
    int enemyEvasion = 20;
    
    std::cout << "攻撃判定（命中率" << (playerAccuracy - enemyEvasion) << "%）:" << std::endl;
    for (int i = 0; i < 5; i++) 
    {
        if (checkHit(playerAccuracy, enemyEvasion)) 
        {
            int baseDamage = 100;
            int enemyDefense = 50;
            int finalDamage = applyDefense(baseDamage, enemyDefense);
            std::cout << "  攻撃" << (i + 1) << ": ヒット！ " 
                      << finalDamage << "ダメージ" << std::endl;
        }
        else 
        {
            std::cout << "  攻撃" << (i + 1) << ": ミス！" << std::endl;
        }
    }
    
    // 経験値計算
    std::cout << "\n=== 経験値計算 ===" << std::endl;
    int playerLevel = 10;
    
    std::cout << "プレイヤーレベル: " << playerLevel << std::endl;
    std::cout << "Lv5の敵を倒した: " << calculateExp(5, playerLevel) << " EXP" << std::endl;
    std::cout << "Lv10の敵を倒した: " << calculateExp(10, playerLevel) << " EXP" << std::endl;
    std::cout << "Lv15の敵を倒した: " << calculateExp(15, playerLevel) << " EXP" << std::endl;
    
    // constexprの使用
    std::cout << "\n=== ゲーム定数 ===" << std::endl;
    std::cout << "最大レベル: " << getMaxLevel() << std::endl;
    std::cout << "クリティカル倍率: " << getCriticalMultiplier() << "倍" << std::endl;
    
    // パフォーマンステスト（簡易版）
    std::cout << "\n=== パフォーマンス比較 ===" << std::endl;
    const int iterations = 10000000;
    
    auto start = std::chrono::high_resolution_clock::now();
    long long sum1 = 0;
    for (int i = 0; i < iterations; i++) 
    {
        sum1 += add(i, i);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    start = std::chrono::high_resolution_clock::now();
    long long sum2 = 0;
    for (int i = 0; i < iterations; i++) 
    {
        sum2 += addInline(i, i);
    }
    end = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "通常関数: " << duration1.count() << "ms" << std::endl;
    std::cout << "インライン関数: " << duration2.count() << "ms" << std::endl;
    
    return 0;
}

/*
    まとめ:
    - インライン関数は関数呼び出しのオーバーヘッドを削減
    - 関数の内容が呼び出し箇所に直接展開される
    - 小さくて頻繁に呼ばれる関数に適している
    - クラス内で定義したメンバ関数は暗黙的にインライン
    
    適切な使用場面：
    - 単純な計算（加減乗除、比較）
    - アクセサ（getter/setter）
    - 頻繁に呼ばれる小さな関数
    - ゲームループ内で使われる関数
    
    注意点：
    - 大きな関数には使わない（コードサイズが増大）
    - 再帰関数には使えない
    - inlineはコンパイラへの提案（強制ではない）
    - ヘッダファイルに定義を書く必要がある
    
    C++11以降：
    - constexprでコンパイル時定数を定義
    - より積極的な最適化が可能
*/