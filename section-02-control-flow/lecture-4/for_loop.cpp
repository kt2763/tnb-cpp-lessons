// 第4講: for文
// 回数が決まっている繰り返し処理に適したfor文の使い方を学ぶ

#include <iostream>
#include <string>
#include <vector>

int main() 
{
    std::cout << "=== for文の基本構文 ===" << std::endl;
    
    // 基本的なfor文
    for (int i = 0; i < 5; i++) 
    {
        std::cout << "カウント: " << i << std::endl;
    }
    
    // 逆順のfor文
    std::cout << "\nカウントダウン:" << std::endl;
    for (int i = 5; i > 0; i--) 
    {
        std::cout << i << "..." << std::endl;
    }
    std::cout << "スタート！" << std::endl;
    
    // 増分を変更
    std::cout << "\n2ずつ増加:" << std::endl;
    for (int i = 0; i <= 10; i += 2) 
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    
    std::cout << "\n=== ゲーム開発での使用例 ===" << std::endl;
    
    // インベントリの表示
    std::vector<std::string> inventory = {"ポーション", "エリクサー", "薬草", "解毒薬", "テント"};
    
    std::cout << "所持アイテム一覧:" << std::endl;
    for (int i = 0; i < inventory.size(); i++) 
    {
        std::cout << i + 1 << ". " << inventory[i] << std::endl;
    }
    
    // パーティメンバーのステータス表示
    std::vector<std::string> partyNames = {"勇者", "戦士", "魔法使い", "僧侶"};
    std::vector<int> partyHP = {100, 150, 70, 80};
    std::vector<int> partyMaxHP = {100, 150, 70, 80};
    
    std::cout << "\nパーティステータス:" << std::endl;
    for (int i = 0; i < partyNames.size(); i++) 
    {
        std::cout << partyNames[i] << ": HP " << partyHP[i] << "/" << partyMaxHP[i];
        
        // HPバーの表示
        int barLength = 20;
        int filledLength = (partyHP[i] * barLength) / partyMaxHP[i];
        std::cout << " [";
        
        for (int j = 0; j < barLength; j++) 
        {
            if (j < filledLength) 
            {
                std::cout << "■";
            }
            else 
            {
                std::cout << "□";
            }
        }
        std::cout << "]" << std::endl;
    }
    
    // マップの生成（2次元配列）
    const int mapWidth = 10;
    const int mapHeight = 5;
    char map[mapHeight][mapWidth];
    
    // マップを草原で初期化
    for (int y = 0; y < mapHeight; y++) 
    {
        for (int x = 0; x < mapWidth; x++) 
        {
            map[y][x] = '.';  // . は草原
        }
    }
    
    // 特定の位置に要素を配置
    map[2][5] = 'P';  // プレイヤー
    map[1][3] = 'E';  // 敵
    map[3][7] = 'T';  // 宝箱
    map[0][9] = 'S';  // ショップ
    
    std::cout << "\nゲームマップ:" << std::endl;
    for (int y = 0; y < mapHeight; y++) 
    {
        for (int x = 0; x < mapWidth; x++) 
        {
            std::cout << map[y][x] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "P:プレイヤー E:敵 T:宝箱 S:ショップ" << std::endl;
    
    // 経験値テーブルの生成
    const int maxLevel = 10;
    int expTable[maxLevel];
    
    // レベルごとの必要経験値を計算
    for (int level = 0; level < maxLevel; level++) 
    {
        expTable[level] = (level + 1) * (level + 1) * 100;
    }
    
    std::cout << "\nレベルアップ必要経験値:" << std::endl;
    for (int i = 0; i < maxLevel; i++) 
    {
        std::cout << "Lv" << i + 1 << " → Lv" << i + 2 << ": " << expTable[i] << " EXP" << std::endl;
    }
    
    // 連続攻撃の実装
    int hitCount = 3;
    int baseDamage = 20;
    int totalDamage = 0;
    
    std::cout << "\n3連撃発動！" << std::endl;
    for (int hit = 1; hit <= hitCount; hit++) 
    {
        int damage = baseDamage + (hit * 5);  // 連撃ごとにダメージ増加
        totalDamage += damage;
        std::cout << "攻撃" << hit << ": " << damage << "ダメージ！" << std::endl;
    }
    std::cout << "合計ダメージ: " << totalDamage << std::endl;
    
    // スコア集計
    std::vector<int> stageScores = {1000, 1500, 800, 2000, 1200};
    int totalScore = 0;
    int bonusScore = 0;
    
    std::cout << "\nステージクリアスコア:" << std::endl;
    for (int i = 0; i < stageScores.size(); i++) 
    {
        std::cout << "ステージ" << i + 1 << ": " << stageScores[i] << "点" << std::endl;
        totalScore += stageScores[i];
        
        // 1000点以上でボーナス
        if (stageScores[i] >= 1000) 
        {
            bonusScore += 500;
            std::cout << "  ボーナス +500点！" << std::endl;
        }
    }
    
    std::cout << "\n基本スコア: " << totalScore << "点" << std::endl;
    std::cout << "ボーナススコア: " << bonusScore << "点" << std::endl;
    std::cout << "総合スコア: " << totalScore + bonusScore << "点" << std::endl;
    
    return 0;
}

/*
    まとめ:
    - for文は（初期化; 条件式; 更新式）の3つの要素で構成される
    - 回数が決まっている繰り返し処理に最適
    - 配列やvectorの要素を順番に処理する際によく使用
    - ネストしたfor文で2次元配列やマップの処理が可能
    - ゲーム開発では、インベントリ表示、マップ生成、スコア集計などで活用
    - カウンタ変数はfor文のスコープ内でのみ有効
*/