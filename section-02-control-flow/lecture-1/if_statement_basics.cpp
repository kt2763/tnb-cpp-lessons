// 第1講: if文の基本
// 条件分岐の基礎を学び、ゲームロジックの実装方法を理解する

#include <iostream>
#include <string>

int main() 
{
    std::cout << "=== if文の基本構文 ===" << std::endl;
    
    // 基本的なif文
    int playerLevel = 5;
    
    if (playerLevel >= 5) 
    {
        std::cout << "プレイヤーはレベル5以上です！" << std::endl;
    }
    
    // if-else文
    int playerHP = 30;
    int maxHP = 100;
    
    if (playerHP > 0) 
    {
        std::cout << "プレイヤーは生存しています。HP: " << playerHP << std::endl;
    }
    else 
    {
        std::cout << "ゲームオーバー！" << std::endl;
    }
    
    // if-else if-else文
    double hpPercentage = static_cast<double>(playerHP) / maxHP * 100;
    
    if (hpPercentage >= 70) 
    {
        std::cout << "体力は十分です（" << hpPercentage << "%）" << std::endl;
    }
    else if (hpPercentage >= 30) 
    {
        std::cout << "体力が減っています（" << hpPercentage << "%）" << std::endl;
    }
    else 
    {
        std::cout << "危険！体力が少ないです（" << hpPercentage << "%）" << std::endl;
    }
    
    std::cout << "\n=== ゲーム開発での使用例 ===" << std::endl;
    
    // レベルアップ判定
    int currentExp = 150;
    int expForNextLevel = 100;
    
    if (currentExp >= expForNextLevel) 
    {
        playerLevel++;
        currentExp -= expForNextLevel;
        std::cout << "レベルアップ！ 新しいレベル: " << playerLevel << std::endl;
        std::cout << "次のレベルまでの経験値: " << currentExp << "/" << (expForNextLevel * 2) << std::endl;
    }
    
    // 装備可能判定
    int swordRequiredLevel = 10;
    std::string itemName = "炎の剣";
    
    if (playerLevel >= swordRequiredLevel) 
    {
        std::cout << itemName << "を装備できます！" << std::endl;
    }
    else 
    {
        std::cout << itemName << "を装備するにはレベル" << swordRequiredLevel << "が必要です。" << std::endl;
        std::cout << "あと" << (swordRequiredLevel - playerLevel) << "レベル必要です。" << std::endl;
    }
    
    // ネストしたif文（ダンジョン入場条件）
    bool hasKey = true;
    int requiredLevel = 5;
    
    if (playerLevel >= requiredLevel) 
    {
        if (hasKey) 
        {
            std::cout << "ダンジョンに入れます！" << std::endl;
        }
        else 
        {
            std::cout << "レベルは足りていますが、鍵が必要です。" << std::endl;
        }
    }
    else 
    {
        std::cout << "ダンジョンに入るにはレベル" << requiredLevel << "以上が必要です。" << std::endl;
    }
    
    return 0;
}

/*
    まとめ:
    - if文は条件に基づいて処理を分岐させる基本的な制御構造
    - if-else文で二択の分岐、if-else if-else文で多方向分岐が可能
    - 条件式が真（true）の場合のみ、対応するブロックが実行される
    - ゲーム開発では、レベル判定、HP判定、アイテム装備条件など様々な場面で使用
    - ネストしたif文で複雑な条件判定も可能だが、可読性に注意
*/