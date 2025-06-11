/*
 * C++モダンコース - セクション9: アルゴリズムとイテレータ
 * レクチャー1: イテレータの基礎とカテゴリ
 * 
 * このレクチャーでは、イテレータの基本概念と各カテゴリについて学びます
 * ゲーム開発での実例を通じて、イテレータの活用方法を理解します
 */

#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <iterator>
#include <string>

// ゲームアイテムのデータ構造
struct GameItem
{
    std::string name;
    int value;
    int rarity;
    
    GameItem(const std::string& n, int v, int r) : name(n), value(v), rarity(r) {}
    
    // デバッグ用の表示関数
    void display() const
    {
        std::cout << "アイテム: " << name << " (価値: " << value << ", レア度: " << rarity << ")" << std::endl;
    }
};

void demonstrateInputIterator()
{
    std::cout << "\n=== Input Iterator（入力イテレータ）の例 ===" << std::endl;
    
    // プレイヤーのアイテムリスト
    std::vector<GameItem> inventory = 
    {
        {"魔法の剣", 1000, 5},
        {"回復ポーション", 50, 1},
        {"ドラゴンの鱗", 500, 4},
        {"魔力の指輪", 750, 3}
    };
    
    // Input Iteratorとして使用（読み取り専用）
    std::cout << "プレイヤーのインベントリ:" << std::endl;
    for (auto it = inventory.cbegin(); it != inventory.cend(); ++it)
    {
        it->display(); // 読み取り専用アクセス
    }
}

void demonstrateOutputIterator()
{
    std::cout << "\n=== Output Iterator（出力イテレータ）の例 ===" << std::endl;
    
    std::vector<int> playerScores = {8500, 7200, 9100, 6800, 8900};
    
    // back_insert_iteratorを使用してスコアをコピー
    std::vector<int> highScores;
    
    std::cout << "高得点のみを抽出中..." << std::endl;
    for (const auto& score : playerScores)
    {
        if (score >= 8000)
        {
            // Output Iteratorとして使用
            auto inserter = std::back_inserter(highScores);
            *inserter = score;
        }
    }
    
    std::cout << "高得点リスト: ";
    for (const auto& score : highScores)
    {
        std::cout << score << " ";
    }
    std::cout << std::endl;
}

void demonstrateForwardIterator()
{
    std::cout << "\n=== Forward Iterator（前進イテレータ）の例 ===" << std::endl;
    
    // ゲーム内のNPCリスト（forward_listを使用）
    std::list<std::string> npcList = {"商人", "村長", "鍛冶屋", "魔法使い", "守衛"};
    
    std::cout << "NPCとの会話順序:" << std::endl;
    
    // Forward Iteratorで一方向に進行
    for (auto it = npcList.begin(); it != npcList.end(); ++it)
    {
        std::cout << "-> " << *it << "と会話" << std::endl;
    }
}

void demonstrateBidirectionalIterator()
{
    std::cout << "\n=== Bidirectional Iterator（双方向イテレータ）の例 ===" << std::endl;
    
    // ゲームレベルのリスト
    std::list<std::string> gameLevels = {"草原", "森", "洞窟", "城", "魔王の塔"};
    
    std::cout << "レベル進行（前進）:" << std::endl;
    for (auto it = gameLevels.begin(); it != gameLevels.end(); ++it)
    {
        std::cout << "現在地: " << *it << std::endl;
    }
    
    std::cout << "\nレベル後退（後進）:" << std::endl;
    for (auto it = gameLevels.rbegin(); it != gameLevels.rend(); ++it)
    {
        std::cout << "戻り先: " << *it << std::endl;
    }
}

void demonstrateRandomAccessIterator()
{
    std::cout << "\n=== Random Access Iterator（ランダムアクセスイテレータ）の例 ===" << std::endl;
    
    // プレイヤーの戦闘履歴（vectorを使用）
    std::vector<std::string> battleHistory = 
    {
        "スライム戦", "ゴブリン戦", "オーク戦", "ドラゴン戦", "魔王戦"
    };
    
    std::cout << "戦闘履歴の任意アクセス:" << std::endl;
    
    // ランダムアクセス演算子を使用
    auto it = battleHistory.begin();
    
    std::cout << "最初の戦闘: " << *it << std::endl;
    std::cout << "3番目の戦闘: " << *(it + 2) << std::endl;
    std::cout << "最後の戦闘: " << *(it + 4) << std::endl;
    
    // イテレータ間の距離計算
    auto distance = battleHistory.end() - battleHistory.begin();
    std::cout << "総戦闘回数: " << distance << "回" << std::endl;
    
    // 範囲アクセス
    std::cout << "\n中間の戦闘（2-4番目）:" << std::endl;
    for (auto mid_it = it + 1; mid_it != it + 4; ++mid_it)
    {
        std::cout << "- " << *mid_it << std::endl;
    }
}

void demonstrateIteratorAdaptors()
{
    std::cout << "\n=== Iterator Adaptors（イテレータアダプタ）の例 ===" << std::endl;
    
    std::vector<int> playerLevels = {1, 5, 10, 15, 20, 25};
    
    // Reverse Iterator（逆順イテレータ）
    std::cout << "レベル降順表示:" << std::endl;
    for (auto rit = playerLevels.rbegin(); rit != playerLevels.rend(); ++rit)
    {
        std::cout << "レベル " << *rit << std::endl;
    }
    
    // Insert Iterator（挿入イテレータ）
    std::vector<int> bonusLevels;
    
    // back_inserterを使用
    auto back_ins = std::back_inserter(bonusLevels);
    *back_ins = 50; // ボーナスレベル追加
    *back_ins = 100;
    
    std::cout << "\nボーナスレベル追加後:" << std::endl;
    for (const auto& level : bonusLevels)
    {
        std::cout << "ボーナスレベル " << level << std::endl;
    }
}

void demonstrateIteratorTraits()
{
    std::cout << "\n=== Iterator Traits（イテレータ特性）の例 ===" << std::endl;
    
    std::vector<int> gameData = {100, 200, 300, 400, 500};
    
    // イテレータの型情報を取得
    using iter_type = std::vector<int>::iterator;
    using value_type = std::iterator_traits<iter_type>::value_type;
    using category = std::iterator_traits<iter_type>::iterator_category;
    
    std::cout << "イテレータの値型サイズ: " << sizeof(value_type) << " bytes" << std::endl;
    
    // カテゴリの確認（コンパイル時チェック）
    if constexpr (std::is_same_v<category, std::random_access_iterator_tag>)
    {
        std::cout << "このイテレータはランダムアクセス対応です" << std::endl;
        std::cout << "ゲームデータの中央値: " << gameData[gameData.size()/2] << std::endl;
    }
}

int main()
{
    std::cout << "C++アルゴリズムとイテレータ - レクチャー1: イテレータの基礎" << std::endl;
    std::cout << "======================================================" << std::endl;
    
    // 各イテレータカテゴリの実演
    demonstrateInputIterator();
    demonstrateOutputIterator();
    demonstrateForwardIterator();
    demonstrateBidirectionalIterator();
    demonstrateRandomAccessIterator();
    demonstrateIteratorAdaptors();
    demonstrateIteratorTraits();
    
    std::cout << "\n=== まとめ ===" << std::endl;
    std::cout << "1. Input Iterator: 読み取り専用、一方向進行" << std::endl;
    std::cout << "2. Output Iterator: 書き込み専用、一方向進行" << std::endl;
    std::cout << "3. Forward Iterator: 読み書き可能、一方向進行" << std::endl;
    std::cout << "4. Bidirectional Iterator: 読み書き可能、双方向進行" << std::endl;
    std::cout << "5. Random Access Iterator: 読み書き可能、任意位置アクセス" << std::endl;
    std::cout << "ゲーム開発では、データ構造に応じて適切なイテレータを選択することが重要です" << std::endl;
    
    return 0;
}