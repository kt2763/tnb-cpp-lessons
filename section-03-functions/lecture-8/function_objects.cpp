// 第8講: 関数オブジェクト（ファンクタ）
// operator()を定義したクラスによる関数オブジェクトの作成と活用

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

// === 基本的な関数オブジェクト ===
class Adder 
{
private:
    int value;
    
public:
    Adder(int v) : value(v) {}
    
    int operator()(int x) const 
    {
        return x + value;
    }
};

// === ゲーム用の関数オブジェクト ===
class DamageCalculator 
{
private:
    int baseDamage;
    double criticalRate;
    double criticalMultiplier;
    
public:
    DamageCalculator(int base, double critRate = 0.1, double critMult = 1.5)
        : baseDamage(base), criticalRate(critRate), criticalMultiplier(critMult) {}
    
    int operator()(int defense) const 
    {
        int damage = baseDamage - defense;
        if (damage < 1) damage = 1;
        
        // クリティカル判定
        bool isCritical = (std::rand() % 100) < (criticalRate * 100);
        if (isCritical) 
        {
            damage *= criticalMultiplier;
            std::cout << "[クリティカル！] ";
        }
        
        return damage;
    }
};

// プレイヤー比較用ファンクタ
struct Player 
{
    std::string name;
    int level;
    int score;
};

class CompareByLevel 
{
public:
    bool operator()(const Player& a, const Player& b) const 
    {
        return a.level > b.level;
    }
};

class CompareByScore 
{
public:
    bool operator()(const Player& a, const Player& b) const 
    {
        return a.score > b.score;
    }
};

// フィルタリング用ファンクタ
class HighLevelFilter 
{
private:
    int minLevel;
    
public:
    HighLevelFilter(int level) : minLevel(level) {}
    
    bool operator()(const Player& player) const 
    {
        return player.level >= minLevel;
    }
};

// カウンタ機能付きファンクタ
class CallCounter 
{
private:
    mutable int count;
    std::string name;
    
public:
    CallCounter(const std::string& n) : count(0), name(n) {}
    
    void operator()() const 
    {
        count++;
        std::cout << name << "が" << count << "回目の呼び出しです" << std::endl;
    }
    
    int getCount() const { return count; }
};

int main() 
{
    std::cout << "=== 関数オブジェクトの基本 ===" << std::endl;
    
    // 基本的な使用
    Adder add10(10);
    std::cout << "5 + 10 = " << add10(5) << std::endl;
    std::cout << "15 + 10 = " << add10(15) << std::endl;
    
    std::cout << "\n=== ゲーム開発での実践例 ===" << std::endl;
    
    // ダメージ計算システム
    DamageCalculator normalAttack(100);
    DamageCalculator criticalAttack(100, 0.3, 2.0);
    
    int enemyDefense = 20;
    
    std::cout << "通常攻撃: " << normalAttack(enemyDefense) << " ダメージ" << std::endl;
    std::cout << "必殺技: " << criticalAttack(enemyDefense) << " ダメージ" << std::endl;
    
    // プレイヤーデータのソート
    std::vector<Player> players = {
        {"勇者", 15, 2500},
        {"魔法使い", 12, 3200},
        {"戦士", 18, 1800},
        {"盗賊", 10, 2800}
    };
    
    std::cout << "\n元のデータ:" << std::endl;
    for (const auto& p : players) 
    {
        std::cout << "  " << p.name << " Lv." << p.level << " Score:" << p.score << std::endl;
    }
    
    // レベル順でソート
    std::sort(players.begin(), players.end(), CompareByLevel());
    std::cout << "\nレベル順ソート:" << std::endl;
    for (const auto& p : players) 
    {
        std::cout << "  " << p.name << " Lv." << p.level << std::endl;
    }
    
    // スコア順でソート
    std::sort(players.begin(), players.end(), CompareByScore());
    std::cout << "\nスコア順ソート:" << std::endl;
    for (const auto& p : players) 
    {
        std::cout << "  " << p.name << " Score:" << p.score << std::endl;
    }
    
    // フィルタリング
    HighLevelFilter highLevel(15);
    std::cout << "\n高レベルプレイヤー（Lv.15以上）:" << std::endl;
    for (const auto& p : players) 
    {
        if (highLevel(p)) 
        {
            std::cout << "  " << p.name << " Lv." << p.level << std::endl;
        }
    }
    
    // カウンタ機能
    std::cout << "\n呼び出し回数カウント:" << std::endl;
    CallCounter battleCounter("戦闘処理");
    
    for (int i = 0; i < 3; i++) 
    {
        battleCounter();
    }
    
    std::cout << "総呼び出し回数: " << battleCounter.getCount() << std::endl;
    
    // STLアルゴリズムとの組み合わせ
    std::vector<int> damages = {10, 25, 30, 15, 40};
    
    std::cout << "\n元のダメージ値: ";
    for (int d : damages) std::cout << d << " ";
    std::cout << std::endl;
    
    // 全てに15を加算
    std::transform(damages.begin(), damages.end(), damages.begin(), Adder(15));
    
    std::cout << "強化後（+15）: ";
    for (int d : damages) std::cout << d << " ";
    std::cout << std::endl;
    
    return 0;
}

/*
    まとめ:
    - 関数オブジェクト（ファンクタ）はoperator()を定義したクラス
    - 状態を持てるため、通常の関数より柔軟
    - STLアルゴリズムとの相性が良い
    - ゲーム開発では、ダメージ計算、ソート条件、フィルタリングで活用
    - C++11以降はラムダ式で同様の機能をより簡潔に実現可能
    - パフォーマンスが重要な場面では関数オブジェクトが有効
*/