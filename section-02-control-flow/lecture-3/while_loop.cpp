// 第3講: while文
// 条件が真の間、処理を繰り返すwhile文の使い方を学ぶ

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

int main() 
{
    std::cout << "=== while文の基本構文 ===" << std::endl;
    
    // 基本的なwhile文
    int count = 0;
    while (count < 5) 
    {
        std::cout << "カウント: " << count << std::endl;
        count++;
    }
    
    // 条件式での計算
    int playerExp = 0;
    int expToNextLevel = 100;
    int expGained = 35;
    
    std::cout << "\n経験値獲得シミュレーション:" << std::endl;
    while (playerExp < expToNextLevel) 
    {
        playerExp += expGained;
        std::cout << "経験値 +" << expGained << " (合計: " << playerExp << "/" << expToNextLevel << ")" << std::endl;
        
        if (playerExp >= expToNextLevel) 
        {
            std::cout << "レベルアップ！" << std::endl;
        }
    }
    
    std::cout << "\n=== ゲーム開発での使用例 ===" << std::endl;
    
    // 戦闘システム
    int playerHP = 100;
    int enemyHP = 80;
    int playerAttack = 25;
    int enemyAttack = 15;
    int turn = 1;
    
    std::cout << "戦闘開始！" << std::endl;
    std::cout << "プレイヤーHP: " << playerHP << " vs ゴブリンHP: " << enemyHP << std::endl;
    
    while (playerHP > 0 && enemyHP > 0) 
    {
        std::cout << "\nターン " << turn << ":" << std::endl;
        
        // プレイヤーの攻撃
        enemyHP -= playerAttack;
        std::cout << "プレイヤーの攻撃！ ゴブリンに" << playerAttack << "のダメージ！" << std::endl;
        
        if (enemyHP <= 0) 
        {
            std::cout << "ゴブリンを倒した！" << std::endl;
            break;
        }
        
        // 敵の攻撃
        playerHP -= enemyAttack;
        std::cout << "ゴブリンの攻撃！ プレイヤーに" << enemyAttack << "のダメージ！" << std::endl;
        
        // 現在のHP表示
        std::cout << "プレイヤーHP: " << playerHP << " / ゴブリンHP: " << enemyHP << std::endl;
        
        turn++;
    }
    
    if (playerHP > 0) 
    {
        std::cout << "\n勝利！ 残りHP: " << playerHP << std::endl;
    }
    else 
    {
        std::cout << "\n敗北... ゲームオーバー" << std::endl;
    }
    
    // アイテム合成システム
    int materials = 15;
    int requiredMaterials = 3;
    int craftedItems = 0;
    
    std::cout << "\n素材を使ってポーションを作成します" << std::endl;
    std::cout << "所持素材: " << materials << "個" << std::endl;
    
    while (materials >= requiredMaterials) 
    {
        materials -= requiredMaterials;
        craftedItems++;
        std::cout << "ポーション作成！ (残り素材: " << materials << "個)" << std::endl;
    }
    
    std::cout << "合計" << craftedItems << "個のポーションを作成しました" << std::endl;
    if (materials > 0) 
    {
        std::cout << "素材が" << materials << "個余りました（必要数: " << requiredMaterials << "個）" << std::endl;
    }
    
    // 自動回復システム
    int currentHP = 30;
    int maxHP = 100;
    int regenAmount = 10;
    int regenTicks = 0;
    
    std::cout << "\n自動HP回復中..." << std::endl;
    while (currentHP < maxHP) 
    {
        int healAmount = std::min(regenAmount, maxHP - currentHP);
        currentHP += healAmount;
        regenTicks++;
        
        std::cout << "HP +" << healAmount << " → " << currentHP << "/" << maxHP << std::endl;
        
        // 最大5回まで回復
        if (regenTicks >= 5) 
        {
            std::cout << "自動回復終了（最大回数到達）" << std::endl;
            break;
        }
    }
    
    if (currentHP == maxHP) 
    {
        std::cout << "HPが完全に回復しました！" << std::endl;
    }
    
    // 無限ループの制御例（ゲームループの簡易版）
    bool gameRunning = true;
    int frameCount = 0;
    const int maxFrames = 10;  // デモ用に10フレームで終了
    
    std::cout << "\nゲームループのシミュレーション:" << std::endl;
    while (gameRunning) 
    {
        frameCount++;
        std::cout << "フレーム " << frameCount << ": ゲーム更新中..." << std::endl;
        
        // 終了条件（実際のゲームではユーザー入力など）
        if (frameCount >= maxFrames) 
        {
            gameRunning = false;
            std::cout << "ゲーム終了" << std::endl;
        }
    }
    
    return 0;
}

/*
    まとめ:
    - while文は条件が真である限り、処理を繰り返す
    - 条件式は毎回ループの開始時に評価される
    - break文でループを途中で抜けることができる
    - 無限ループ（while(true)）には必ず終了条件を設ける
    - ゲーム開発では、戦闘処理、自動回復、アイテム作成、ゲームループなどで使用
    - ループ内で条件を更新しないと無限ループになるので注意
*/