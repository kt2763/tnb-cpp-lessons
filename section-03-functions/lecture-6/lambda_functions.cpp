// 第6講: ラムダ関数（C++11）
// 無名関数（ラムダ式）の使い方とゲーム開発での活用を学ぶ

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <string>

int main() 
{
    std::cout << "=== ラムダ関数の基本 ===" << std::endl;
    
    // 基本的なラムダ式
    auto simpleAdd = [](int a, int b) { return a + b; };
    std::cout << "5 + 3 = " << simpleAdd(5, 3) << std::endl;
    
    // キャプチャを使用したラムダ
    int baseValue = 100;
    auto addBase = [baseValue](int x) { return x + baseValue; };
    std::cout << "10 + " << baseValue << " = " << addBase(10) << std::endl;
    
    std::cout << "\n=== ゲーム開発での実践例 ===" << std::endl;
    
    // プレイヤーデータ
    struct Player 
    {
        std::string name;
        int level;
        int score;
        bool isAlive;
    };
    
    std::vector<Player> players = {
        {"勇者", 15, 2500, true},
        {"魔法使い", 12, 1800, false},
        {"戦士", 18, 3200, true},
        {"盗賊", 10, 1200, true}
    };
    
    // 生存者のみをフィルタリング
    std::cout << "生存者一覧:" << std::endl;
    std::for_each(players.begin(), players.end(), 
        [](const Player& p) 
        {
            if (p.isAlive) 
            {
                std::cout << "  " << p.name << " (Lv." << p.level << ")" << std::endl;
            }
        });
    
    // ソート機能
    std::cout << "\nレベル順でソート:" << std::endl;
    std::sort(players.begin(), players.end(), 
        [](const Player& a, const Player& b) 
        {
            return a.level > b.level;
        });
    
    for (const auto& p : players) 
    {
        std::cout << "  " << p.name << " Lv." << p.level << std::endl;
    }
    
    // 条件に一致するプレイヤーを検索
    int minLevel = 15;
    auto highLevelPlayer = std::find_if(players.begin(), players.end(),
        [minLevel](const Player& p) 
        {
            return p.level >= minLevel && p.isAlive;
        });
    
    if (highLevelPlayer != players.end()) 
    {
        std::cout << "\n高レベルプレイヤー発見: " << highLevelPlayer->name << std::endl;
    }
    
    // ダメージ計算システム
    auto calculateDamage = [](int baseDamage, double multiplier = 1.0) 
    {
        return [baseDamage, multiplier](int targetDefense) 
        {
            int damage = baseDamage * multiplier - targetDefense;
            return damage > 0 ? damage : 1;
        };
    };
    
    auto normalAttack = calculateDamage(100);
    auto criticalAttack = calculateDamage(100, 2.0);
    
    int enemyDefense = 30;
    std::cout << "\n通常攻撃: " << normalAttack(enemyDefense) << " ダメージ" << std::endl;
    std::cout << "クリティカル: " << criticalAttack(enemyDefense) << " ダメージ" << std::endl;
    
    return 0;
}

/*
    まとめ:
    - ラムダ式は無名関数を作成する機能（C++11以降）
    - [キャプチャ](引数) { 処理 } の形式
    - キャプチャで外部変数を取り込める
    - STLアルゴリズムと組み合わせて強力
    - ゲーム開発では、ソート、フィルタリング、イベント処理で活用
    - 関数オブジェクトより簡潔に書ける
*/