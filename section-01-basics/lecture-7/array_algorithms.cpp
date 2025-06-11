// 第7講: 配列を使ったアルゴリズム
// 配列に対する基本的な操作

#include <iostream>
#include <algorithm>  // std::sort, std::find
#include <numeric>    // std::accumulate

int main() 
{
    std::cout << "=== 配列の基本的な操作 ===" << std::endl;
    
    // 最大値・最小値を求める
    int playerScores[5] = {1200, 3500, 2800, 4100, 900};
    
    int maxScore = playerScores[0];
    int minScore = playerScores[0];
    int maxIndex = 0;
    int minIndex = 0;
    
    for (int i = 1; i < 5; ++i) 
    {
        if (playerScores[i] > maxScore) 
        {
            maxScore = playerScores[i];
            maxIndex = i;
        }
        if (playerScores[i] < minScore) 
        {
            minScore = playerScores[i];
            minIndex = i;
        }
    }
    
    std::cout << "スコア一覧: ";
    for (int i = 0; i < 5; ++i) 
    {
        std::cout << playerScores[i] << " ";
    }
    std::cout << std::endl;
    
    std::cout << "最高スコア: " << maxScore << " (プレイヤー" << (maxIndex + 1) << ")" << std::endl;
    std::cout << "最低スコア: " << minScore << " (プレイヤー" << (minIndex + 1) << ")" << std::endl;
    
    // 合計と平均を求める
    int totalScore = 0;
    for (int i = 0; i < 5; ++i) 
    {
        totalScore += playerScores[i];
    }
    double averageScore = static_cast<double>(totalScore) / 5;
    
    std::cout << "合計スコア: " << totalScore << std::endl;
    std::cout << "平均スコア: " << averageScore << std::endl;
    
    // 配列の検索
    std::cout << "\n=== アイテム検索 ===" << std::endl;
    
    int itemInventory[10] = {101, 102, 201, 103, 301, 104, 202, 105, 302, 203};
    int searchItem = 201;  // 探すアイテムID
    bool found = false;
    int foundIndex = -1;
    
    for (int i = 0; i < 10; ++i) 
    {
        if (itemInventory[i] == searchItem) 
        {
            found = true;
            foundIndex = i;
            break;  // 最初に見つかったら終了
        }
    }
    
    if (found) 
    {
        std::cout << "アイテムID " << searchItem << " はスロット" 
                  << (foundIndex + 1) << "にあります" << std::endl;
    }
    else 
    {
        std::cout << "アイテムID " << searchItem << " は見つかりませんでした" << std::endl;
    }
    
    // 配列のソート（バブルソート）
    std::cout << "\n=== ランキングのソート ===" << std::endl;
    
    int highScores[5] = {2500, 3200, 1800, 4500, 2100};
    std::string playerNames[5] = {"太郎", "花子", "次郎", "美咲", "健太"};
    
    std::cout << "ソート前: ";
    for (int i = 0; i < 5; ++i) 
    {
        std::cout << playerNames[i] << "(" << highScores[i] << ") ";
    }
    std::cout << std::endl;
    
    // バブルソート（降順）
    for (int i = 0; i < 4; ++i) 
    {
        for (int j = 0; j < 4 - i; ++j) 
        {
            if (highScores[j] < highScores[j + 1]) 
            {
                // スコアを交換
                int tempScore = highScores[j];
                highScores[j] = highScores[j + 1];
                highScores[j + 1] = tempScore;
                
                // 名前も一緒に交換
                std::string tempName = playerNames[j];
                playerNames[j] = playerNames[j + 1];
                playerNames[j + 1] = tempName;
            }
        }
    }
    
    std::cout << "ソート後（ランキング）:" << std::endl;
    for (int i = 0; i < 5; ++i) 
    {
        std::cout << (i + 1) << "位: " << playerNames[i] 
                  << " - " << highScores[i] << "点" << std::endl;
    }
    
    // 配列の要素カウント
    std::cout << "\n=== アイテム所持数カウント ===" << std::endl;
    
    int items[20] = {101, 102, 101, 103, 101, 102, 104, 101, 103, 102,
                     105, 101, 102, 103, 101, 104, 102, 101, 103, 102};
    
    // 各アイテムの個数を数える
    int itemCounts[6] = {0};  // ID 100-105用
    
    for (int i = 0; i < 20; ++i) 
    {
        if (items[i] >= 101 && items[i] <= 105) 
        {
            itemCounts[items[i] - 100]++;
        }
    }
    
    std::cout << "アイテム所持数:" << std::endl;
    const char* itemNames[] = {"", "ポーション", "エーテル", "万能薬", "フェニックスの尾", "エリクサー"};
    for (int i = 1; i <= 5; ++i) 
    {
        std::cout << itemNames[i] << "(ID:" << (100 + i) << "): " 
                  << itemCounts[i] << "個" << std::endl;
    }
    
    // 配列の反転
    std::cout << "\n=== 配列の反転 ===" << std::endl;
    
    char word[] = {'A', 'T', 'T', 'A', 'C', 'K'};
    int wordSize = 6;
    
    std::cout << "元の配列: ";
    for (int i = 0; i < wordSize; ++i) 
    {
        std::cout << word[i];
    }
    std::cout << std::endl;
    
    // 配列を反転
    for (int i = 0; i < wordSize / 2; ++i) 
    {
        char temp = word[i];
        word[i] = word[wordSize - 1 - i];
        word[wordSize - 1 - i] = temp;
    }
    
    std::cout << "反転後: ";
    for (int i = 0; i < wordSize; ++i) 
    {
        std::cout << word[i];
    }
    std::cout << std::endl;
    
    return 0;
}

/*
    配列の基本アルゴリズム:
    
    1. 最大値・最小値の探索
       - 全要素を比較
       - インデックスも記録すると便利
    
    2. 合計・平均の計算
       - ループで累積
       - 平均は型変換に注意
    
    3. 線形探索
       - 順番に比較
       - 見つかったらbreak
    
    4. ソート
       - バブルソート（簡単だが遅い）
       - 選択ソート
       - STLのstd::sort（高速）
    
    5. カウント
       - 配列を使った頻度集計
       - ハッシュテーブル的な使い方
    
    ゲーム開発での応用:
    - ランキング表示
    - インベントリ管理
    - ステータス集計
    - アイテムソート
*/