// 第1講: ヘッダファイルの基本
// プログラムを複数のファイルに分割し、ヘッダファイルで宣言を管理する方法を学ぶ

#include <iostream>
#include <string>

// このファイルは main.cpp として考える
// 実際のプロジェクトでは以下の内容を別ファイルに分ける

// === ヘッダファイル：player.h の内容 ===
/*
#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Player 
{
private:
    std::string name;
    int level;
    int hp;
    int maxHp;
    int exp;
    
public:
    Player(const std::string& playerName, int startLevel = 1);
    
    // ゲッター
    const std::string& getName() const;
    int getLevel() const;
    int getHp() const;
    int getMaxHp() const;
    int getExp() const;
    
    // ゲームプレイ関連
    void gainExp(int amount);
    void takeDamage(int damage);
    void heal(int amount);
    void displayStatus() const;
    
private:
    void levelUp();
    int calculateExpForNextLevel() const;
};

#endif // PLAYER_H
*/

// === 実装ファイル：player.cpp の内容 ===
// 今回はデモのため、同じファイル内にクラスを定義

class Player 
{
private:
    std::string name;
    int level;
    int hp;
    int maxHp;
    int exp;
    
public:
    Player(const std::string& playerName, int startLevel = 1)
        : name(playerName), level(startLevel), exp(0)
    {
        maxHp = 50 + (level - 1) * 20;
        hp = maxHp;
    }
    
    // ゲッター
    const std::string& getName() const { return name; }
    int getLevel() const { return level; }
    int getHp() const { return hp; }
    int getMaxHp() const { return maxHp; }
    int getExp() const { return exp; }
    
    void gainExp(int amount) 
    {
        exp += amount;
        std::cout << name << "は" << amount << "の経験値を獲得！" << std::endl;
        
        while (exp >= calculateExpForNextLevel()) 
        {
            levelUp();
        }
    }
    
    void takeDamage(int damage) 
    {
        hp -= damage;
        if (hp < 0) hp = 0;
        
        std::cout << name << "は" << damage << "のダメージを受けた！" << std::endl;
        if (hp == 0) 
        {
            std::cout << name << "は倒れた..." << std::endl;
        }
    }
    
    void heal(int amount) 
    {
        int oldHp = hp;
        hp = std::min(hp + amount, maxHp);
        int actualHeal = hp - oldHp;
        
        if (actualHeal > 0) 
        {
            std::cout << name << "は" << actualHeal << "のHPを回復！" << std::endl;
        }
    }
    
    void displayStatus() const 
    {
        std::cout << "=== " << name << "のステータス ===" << std::endl;
        std::cout << "レベル: " << level << std::endl;
        std::cout << "HP: " << hp << "/" << maxHp << std::endl;
        std::cout << "経験値: " << exp << "/" << calculateExpForNextLevel() << std::endl;
    }
    
private:
    void levelUp() 
    {
        exp -= calculateExpForNextLevel();
        level++;
        
        int hpIncrease = 20;
        maxHp += hpIncrease;
        hp += hpIncrease;  // レベルアップ時は追加分回復
        
        std::cout << "★ " << name << "はレベル" << level << "に上がった！" << std::endl;
        std::cout << "  最大HPが" << hpIncrease << "上昇！" << std::endl;
    }
    
    int calculateExpForNextLevel() const 
    {
        return level * 100;
    }
};

// === ヘッダファイル：enemy.h の内容 ===
/*
#ifndef ENEMY_H
#define ENEMY_H

#include <string>

class Enemy 
{
private:
    std::string name;
    int hp;
    int attack;
    int defense;
    
public:
    Enemy(const std::string& enemyName, int health, int attackPower, int defensePower);
    
    const std::string& getName() const;
    int getHp() const;
    int getAttack() const;
    int getDefense() const;
    
    void takeDamage(int damage);
    bool isAlive() const;
    void displayStatus() const;
};

#endif // ENEMY_H
*/

// === 実装：enemy.cpp の内容 ===
class Enemy 
{
private:
    std::string name;
    int hp;
    int attack;
    int defense;
    
public:
    Enemy(const std::string& enemyName, int health, int attackPower, int defensePower)
        : name(enemyName), hp(health), attack(attackPower), defense(defensePower) {}
    
    const std::string& getName() const { return name; }
    int getHp() const { return hp; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    
    void takeDamage(int damage) 
    {
        hp -= damage;
        if (hp < 0) hp = 0;
        
        std::cout << name << "は" << damage << "のダメージを受けた！" << std::endl;
        if (hp == 0) 
        {
            std::cout << name << "を倒した！" << std::endl;
        }
    }
    
    bool isAlive() const 
    {
        return hp > 0;
    }
    
    void displayStatus() const 
    {
        std::cout << name << " - HP:" << hp << " ATK:" << attack << " DEF:" << defense << std::endl;
    }
};

// === ヘッダファイル：battle.h の内容 ===
/*
#ifndef BATTLE_H
#define BATTLE_H

#include "player.h"
#include "enemy.h"

class BattleSystem 
{
public:
    static void startBattle(Player& player, Enemy& enemy);
    
private:
    static int calculateDamage(int attack, int defense);
    static void playerTurn(Player& player, Enemy& enemy);
    static void enemyTurn(Player& player, Enemy& enemy);
};

#endif // BATTLE_H
*/

// === 実装：battle.cpp の内容 ===
class BattleSystem 
{
public:
    static void startBattle(Player& player, Enemy& enemy) 
    {
        std::cout << "\n=== 戦闘開始！ ===" << std::endl;
        std::cout << player.getName() << " vs " << enemy.getName() << std::endl;
        
        int turn = 1;
        while (player.getHp() > 0 && enemy.isAlive()) 
        {
            std::cout << "\n--- ターン " << turn << " ---" << std::endl;
            
            // プレイヤーターン
            playerTurn(player, enemy);
            if (!enemy.isAlive()) break;
            
            // 敵ターン
            enemyTurn(player, enemy);
            
            // ステータス表示
            std::cout << player.getName() << " HP:" << player.getHp() << "/" << player.getMaxHp() << std::endl;
            std::cout << enemy.getName() << " HP:" << enemy.getHp() << std::endl;
            
            turn++;
        }
        
        // 戦闘結果
        if (player.getHp() > 0) 
        {
            std::cout << "\n勝利！" << std::endl;
            player.gainExp(100);  // 勝利報酬
        }
        else 
        {
            std::cout << "\n敗北..." << std::endl;
        }
    }
    
private:
    static int calculateDamage(int attack, int defense) 
    {
        int damage = attack - defense;
        return damage > 0 ? damage : 1;  // 最低1ダメージ
    }
    
    static void playerTurn(Player& player, Enemy& enemy) 
    {
        int damage = calculateDamage(50, enemy.getDefense());  // プレイヤーの攻撃力を50と仮定
        std::cout << player.getName() << "の攻撃！" << std::endl;
        enemy.takeDamage(damage);
    }
    
    static void enemyTurn(Player& player, Enemy& enemy) 
    {
        int damage = calculateDamage(enemy.getAttack(), 10);  // プレイヤーの防御力を10と仮定
        std::cout << enemy.getName() << "の攻撃！" << std::endl;
        player.takeDamage(damage);
    }
};

int main() 
{
    std::cout << "=== ヘッダファイルを使ったモジュール化の例 ===" << std::endl;
    
    // プレイヤー作成
    Player hero("勇者", 3);
    hero.displayStatus();
    
    // 敵作成
    Enemy goblin("ゴブリン", 60, 25, 5);
    std::cout << "\n敵が現れた！" << std::endl;
    goblin.displayStatus();
    
    // 戦闘開始
    BattleSystem::startBattle(hero, goblin);
    
    // 戦闘後のステータス
    std::cout << "\n戦闘後のステータス:" << std::endl;
    hero.displayStatus();
    
    return 0;
}

/*
    まとめ:
    - ヘッダファイル（.h）に宣言、実装ファイル（.cpp）に定義を分ける
    - #ifndef, #define, #endifでインクルードガードを設定
    - 複数のファイルで同じヘッダを安全にインクルード可能
    - クラスごと、機能ごとにファイルを分割
    - ゲーム開発では、Player, Enemy, BattleSystemなどを分離
    
    ファイル構成例:
    - player.h / player.cpp: プレイヤークラス
    - enemy.h / enemy.cpp: 敵クラス  
    - battle.h / battle.cpp: 戦闘システム
    - main.cpp: メイン処理
    
    利点:
    - コードの可読性向上
    - 再利用性の向上
    - チーム開発での分担が容易
    - コンパイル時間の短縮
    - 保守性の向上
*/