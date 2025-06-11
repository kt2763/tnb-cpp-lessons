/**
 * @file capture_modes.cpp
 * @brief ラムダ式のキャプチャモードとメカニズム
 * 
 * キャプチャはラムダ式が外部の変数にアクセスする方法を制御します。
 * ゲーム開発では、AIの状態管理やゲームオブジェクトの情報を扱う際に重要です。
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <string>
#include <memory>

int main()
{
    std::cout << "=== キャプチャモードの基礎 ===\n";
    
    // 1. 値キャプチャ [=] - コピーでキャプチャ
    int playerScore = 1000;
    int enemyCount = 5;
    
    auto calculateBonus = [=]() mutable // mutableキーワードでコピーを変更可能
    {
        playerScore += 500; // コピーのみ変更
        return playerScore * enemyCount;
    };
    
    int bonus = calculateBonus();
    std::cout << "ボーナススコア: " << bonus << "\n";
    std::cout << "元のプレイヤースコア: " << playerScore << "\n\n"; // 変更されていない
    
    // 2. 参照キャプチャ [&] - 参照でキャプチャ
    std::cout << "=== 参照キャプチャの例 ===\n";
    
    int playerHealth = 100;
    int playerMana = 50;
    
    auto castSpell = [&](const std::string& spellName, int manaCost, int damage)
    {
        if (playerMana >= manaCost)
        {
            playerMana -= manaCost;
            std::cout << spellName << "を発動！ダメージ: " << damage << "\n";
            std::cout << "残りMP: " << playerMana << "\n";
            return true;
        }
        else
        {
            std::cout << "MP不足！" << spellName << "を使用できません\n";
            return false;
        }
    };
    
    castSpell("ファイアボール", 20, 80);
    castSpell("ライトニング", 35, 120);
    castSpell("メテオ", 50, 200); // MP不足
    
    // 3. 混合キャプチャ - 一部は値、一部は参照
    std::cout << "\n=== 混合キャプチャの例 ===\n";
    
    const int baseExperience = 100;
    int currentLevel = 5;
    float experienceMultiplier = 1.2f;
    
    // baseExperienceは値、currentLevelとexperienceMultiplierは参照
    auto gainExperience = [baseExperience, &currentLevel, &experienceMultiplier](int enemyLevel)
    {
        float exp = baseExperience * experienceMultiplier;
        if (enemyLevel > currentLevel)
        {
            exp *= 1.5f; // 高レベルの敵ボーナス
        }
        
        std::cout << "EXP " << static_cast<int>(exp) << "を獲得！\n";
        
        // レベルアップチェック
        if (exp >= 200)
        {
            currentLevel++;
            experienceMultiplier += 0.1f;
            std::cout << "レベルアップ！新しいレベル: " << currentLevel << "\n";
        }
    };
    
    gainExperience(3); // 低レベルの敵
    gainExperience(8); // 高レベルの敵
    
    // 4. ゲームAIの状態管理例
    std::cout << "\n=== AI状態管理 ===\n";
    
    enum class AIState
    {
        Idle,
        Patrolling,
        Chasing,
        Attacking
    };
    
    struct Enemy
    {
        std::string name;
        AIState state;
        int health;
        int alertLevel;
        float distanceToPlayer;
    };
    
    Enemy goblin = {"ゴブリン", AIState::Idle, 80, 0, 15.0f};
    
    // AIの状態遷移ロジック
    auto updateAI = [&goblin](float deltaTime)
    {
        // 警戒レベルの更新
        if (goblin.distanceToPlayer < 10.0f)
        {
            goblin.alertLevel = std::min(100, goblin.alertLevel + static_cast<int>(deltaTime * 20));
        }
        else
        {
            goblin.alertLevel = std::max(0, goblin.alertLevel - static_cast<int>(deltaTime * 10));
        }
        
        // 状態遷移のロジック
        switch (goblin.state)
        {
            case AIState::Idle:
                if (goblin.alertLevel > 30)
                {
                    goblin.state = AIState::Patrolling;
                    std::cout << goblin.name << "が警戒状態に入った\n";
                }
                break;
                
            case AIState::Patrolling:
                if (goblin.alertLevel > 70)
                {
                    goblin.state = AIState::Chasing;
                    std::cout << goblin.name << "が追跡を開始！\n";
                }
                else if (goblin.alertLevel < 10)
                {
                    goblin.state = AIState::Idle;
                    std::cout << goblin.name << "が通常状態に戻った\n";
                }
                break;
                
            case AIState::Chasing:
                if (goblin.distanceToPlayer < 3.0f)
                {
                    goblin.state = AIState::Attacking;
                    std::cout << goblin.name << "が攻撃姿勢！\n";
                }
                else if (goblin.distanceToPlayer > 20.0f)
                {
                    goblin.state = AIState::Patrolling;
                    std::cout << goblin.name << "が追跡を請めた\n";
                }
                break;
                
            case AIState::Attacking:
                if (goblin.distanceToPlayer > 5.0f)
                {
                    goblin.state = AIState::Chasing;
                    std::cout << goblin.name << "が再び追跡を開始\n";
                }
                break;
        }
        
        std::cout << "警戒レベル: " << goblin.alertLevel 
                  << ", 距離: " << goblin.distanceToPlayer << "m\n";
    };
    
    // AI更新のシミュレーション
    std::vector<float> playerDistances = {15.0f, 12.0f, 8.0f, 5.0f, 2.0f, 1.0f, 3.0f, 8.0f, 15.0f};
    
    for (size_t i = 0; i < playerDistances.size(); ++i)
    {
        goblin.distanceToPlayer = playerDistances[i];
        std::cout << "\nターン " << (i + 1) << ":\n";
        updateAI(1.0f); // 1秒経過
    }
    
    // 5. イベントシステムでのキャプチャ使用例
    std::cout << "\n=== イベントシステム ===\n";
    
    struct GameStats
    {
        int totalScore = 0;
        int itemsCollected = 0;
        int enemiesDefeated = 0;
    };
    
    GameStats stats;
    
    // スコアシステムのコールバック
    auto onItemCollected = [&stats](const std::string& itemName, int value)
    {
        stats.itemsCollected++;
        stats.totalScore += value;
        
        // 特別ボーナスチェック
        if (stats.itemsCollected % 10 == 0)
        {
            int bonus = stats.itemsCollected * 50;
            stats.totalScore += bonus;
            std::cout << "コンボボーナス！+" << bonus << "ポイント\n";
        }
        
        std::cout << itemName << "を収集！+" << value << "ポイント "
                  << "(総スコア: " << stats.totalScore << ")\n";
    };
    
    auto onEnemyDefeated = [&stats](const std::string& enemyName, int expReward)
    {
        stats.enemiesDefeated++;
        stats.totalScore += expReward * 2; // EXPの2倍をスコアに加算
        
        std::cout << enemyName << "を撃破！EXP +" << expReward 
                  << ", スコア +" << (expReward * 2) 
                  << " (総スコア: " << stats.totalScore << ")\n";
    };
    
    // イベント発生シミュレーション
    onItemCollected("コイン", 10);
    onItemCollected("ジェム", 50);
    onEnemyDefeated("スライム", 20);
    onItemCollected("トレジャーチェスト", 200);
    onEnemyDefeated("オーク", 50);
    
    // 最終結果
    std::cout << "\n=== 最終結果 ===\n";
    std::cout << "総スコア: " << stats.totalScore << "\n";
    std::cout << "アイテム収集数: " << stats.itemsCollected << "\n";
    std::cout << "敵撃破数: " << stats.enemiesDefeated << "\n";
    
    return 0;
}

/**
 * まとめ：
 * - [=] 値キャプチャ: 外部変数をコピーでキャプチャ
 * - [&] 参照キャプチャ: 外部変数を参照でキャプチャ
 * - 混合キャプチャ: [var1, &var2] のように個別指定可能
 * - mutableキーワード: 値キャプチャしたコピーを変更可能に
 * - ゲームAIやイベントシステムでキャプチャを活用
 */