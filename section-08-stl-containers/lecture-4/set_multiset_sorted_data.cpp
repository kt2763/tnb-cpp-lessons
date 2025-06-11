/*
 * Lecture 4: std::set/std::multiset - 順序付きコンテナ
 * 
 * setは重複を許さない順序付きコンテナ、multisetは重複を許します。
 * ゲームでは、ユニークなID管理、ソート済みデータ、ランキング
 * システムなどで活用されます。
 */

#include <iostream>
#include <set>
#include <unordered_set>
#include <string>
#include <algorithm>
#include <vector>

// プレイヤー情報を格納するクラス
class Player
{
public:
    std::string name;
    int score;
    int level;
    
    Player(const std::string& n, int s, int l)
        : name(n), score(s), level(l)
    {
    }
    
    // set用の比較演算子（スコア順でソート）
    bool operator<(const Player& other) const
    {
        if (score != other.score)
        {
            return score > other.score;  // 高スコア順
        }
        return name < other.name;  // スコアが同じなら名前順
    }
    
    void display() const
    {
        std::cout << name << " (スコア: " << score 
                  << ", レベル: " << level << ")";
    }
};

// アイテムの品質を表す列挙型
enum class ItemQuality
{
    Common = 1,
    Uncommon = 2,
    Rare = 3,
    Epic = 4,
    Legendary = 5
};

// ゲーム内アイテム
struct GameItem
{
    std::string name;
    ItemQuality quality;
    int value;
    
    GameItem(const std::string& n, ItemQuality q, int v)
        : name(n), quality(q), value(v)
    {
    }
    
    // 品質順でソート
    bool operator<(const GameItem& other) const
    {
        return static_cast<int>(quality) > static_cast<int>(other.quality);
    }
};

int main()
{
    // 1. std::set - 基本的な使用法
    std::cout << "=== std::set - 基本操作 ===\n";
    
    std::set<int> uniqueIDs;
    
    // 要素の挿入
    uniqueIDs.insert(1001);
    uniqueIDs.insert(1003);
    uniqueIDs.insert(1002);
    uniqueIDs.insert(1001);  // 重複は無視される
    uniqueIDs.insert(1005);
    
    std::cout << "ユニークID一覧（自動ソート）: ";
    for (const auto& id : uniqueIDs)
    {
        std::cout << id << " ";
    }
    std::cout << "\n";
    
    // 要素の検索
    int searchID = 1003;
    if (uniqueIDs.find(searchID) != uniqueIDs.end())
    {
        std::cout << "ID " << searchID << " が見つかりました\n";
    }
    
    // count()での存在確認（set では 0 または 1）
    std::cout << "ID 1002 の個数: " << uniqueIDs.count(1002) << "\n";
    
    // 2. std::multiset - 重複を許可
    std::cout << "\n=== std::multiset - 重複を許可 ===\n";
    
    std::multiset<int> scores;
    
    // 同じスコアを複数回挿入
    scores.insert(100);
    scores.insert(250);
    scores.insert(150);
    scores.insert(250);  // 重複OK
    scores.insert(200);
    scores.insert(250);  // さらに重複OK
    
    std::cout << "スコア一覧（重複あり、ソート済み）: ";
    for (const auto& score : scores)
    {
        std::cout << score << " ";
    }
    std::cout << "\n";
    
    std::cout << "スコア250の個数: " << scores.count(250) << "\n";
    
    // 3. カスタム比較関数 - プレイヤーランキング
    std::cout << "\n=== プレイヤーランキングシステム ===\n";
    
    std::set<Player> leaderboard;
    
    leaderboard.insert(Player("勇者アキラ", 2500, 25));
    leaderboard.insert(Player("魔法使いユミ", 3000, 22));
    leaderboard.insert(Player("戦士タロウ", 1800, 20));
    leaderboard.insert(Player("盗賊サトシ", 2200, 18));
    leaderboard.insert(Player("僧侶ハナ", 2500, 24));  // 同じスコア
    
    std::cout << "ハイスコアランキング:\n";
    int rank = 1;
    for (const auto& player : leaderboard)
    {
        std::cout << "  " << rank << "位: ";
        player.display();
        std::cout << "\n";
        ++rank;
    }
    
    // 4. アイテム品質管理システム
    std::cout << "\n=== アイテム品質管理 ===\n";
    
    std::multiset<GameItem> inventory;
    
    inventory.insert(GameItem("鉄の剣", ItemQuality::Common, 100));
    inventory.insert(GameItem("魔法の杖", ItemQuality::Rare, 500));
    inventory.insert(GameItem("ポーション", ItemQuality::Common, 50));
    inventory.insert(GameItem("ドラゴンの鱗", ItemQuality::Legendary, 2000));
    inventory.insert(GameItem("エンチャント防具", ItemQuality::Epic, 1200));
    inventory.insert(GameItem("回復薬", ItemQuality::Uncommon, 150));
    
    std::cout << "インベントリ（品質順）:\n";
    for (const auto& item : inventory)
    {
        std::cout << "  " << item.name 
                  << " (品質: " << static_cast<int>(item.quality)
                  << ", 価値: " << item.value << ")\n";
    }
    
    // 5. set操作 - 集合演算
    std::cout << "\n=== 集合演算 ===\n";
    
    std::set<std::string> partyA = {"勇者", "戦士", "魔法使い", "僧侶"};
    std::set<std::string> partyB = {"盗賊", "戦士", "弓手", "僧侶"};
    
    // 和集合（union）
    std::set<std::string> allMembers;
    std::set_union(partyA.begin(), partyA.end(),
                   partyB.begin(), partyB.end(),
                   std::inserter(allMembers, allMembers.begin()));
    
    std::cout << "全メンバー（和集合）: ";
    for (const auto& member : allMembers)
    {
        std::cout << member << " ";
    }
    std::cout << "\n";
    
    // 積集合（intersection）
    std::set<std::string> commonMembers;
    std::set_intersection(partyA.begin(), partyA.end(),
                         partyB.begin(), partyB.end(),
                         std::inserter(commonMembers, commonMembers.begin()));
    
    std::cout << "共通メンバー（積集合）: ";
    for (const auto& member : commonMembers)
    {
        std::cout << member << " ";
    }
    std::cout << "\n";
    
    // 差集合（difference）
    std::set<std::string> uniqueToA;
    std::set_difference(partyA.begin(), partyA.end(),
                       partyB.begin(), partyB.end(),
                       std::inserter(uniqueToA, uniqueToA.begin()));
    
    std::cout << "パーティAのみのメンバー（差集合）: ";
    for (const auto& member : uniqueToA)
    {
        std::cout << member << " ";
    }
    std::cout << "\n";
    
    // 6. 範囲検索 - lower_bound/upper_bound
    std::cout << "\n=== 範囲検索 ===\n";
    
    std::multiset<int> levelRanges = {5, 12, 8, 23, 15, 3, 18, 25, 12, 7};
    
    std::cout << "全レベル: ";
    for (int level : levelRanges)
    {
        std::cout << level << " ";
    }
    std::cout << "\n";
    
    // レベル10以上20以下のプレイヤーを検索
    auto lower = levelRanges.lower_bound(10);
    auto upper = levelRanges.upper_bound(20);
    
    std::cout << "レベル10-20のプレイヤー: ";
    for (auto it = lower; it != upper; ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << "\n";
    
    // equal_range を使用した同じ結果
    auto range = levelRanges.equal_range(12);
    std::cout << "レベル12のプレイヤー数: " 
              << std::distance(range.first, range.second) << "\n";
    
    // 7. パフォーマンス比較
    std::cout << "\n=== パフォーマンス特性 ===\n";
    
    std::set<int> sortedSet;
    std::vector<int> unsortedVector;
    
    // データを挿入
    for (int i = 1000; i >= 1; --i)
    {
        sortedSet.insert(i);
        unsortedVector.push_back(i);
    }
    
    std::cout << "setのサイズ: " << sortedSet.size() << " (自動ソート)\n";
    std::cout << "vectorのサイズ: " << unsortedVector.size() << " (非ソート)\n";
    
    // vectorをソート
    std::sort(unsortedVector.begin(), unsortedVector.end());
    
    // 検索テスト
    int target = 500;
    
    // setでの検索 (O(log n))
    auto setResult = sortedSet.find(target);
    if (setResult != sortedSet.end())
    {
        std::cout << "setで" << target << "を発見\n";
    }
    
    // ソート済みvectorでの二分探索 (O(log n))
    bool vectorResult = std::binary_search(unsortedVector.begin(), 
                                          unsortedVector.end(), target);
    if (vectorResult)
    {
        std::cout << "ソート済みvectorで" << target << "を発見\n";
    }
    
    // 8. 実用例：ユニークなアイテムID管理
    std::cout << "\n=== ユニークアイテムID管理 ===\n";
    
    std::set<int> usedItemIDs;
    int nextItemID = 1000;
    
    auto generateUniqueID = [&]() -> int
    {
        while (usedItemIDs.count(nextItemID) > 0)
        {
            ++nextItemID;
        }
        usedItemIDs.insert(nextItemID);
        return nextItemID++;
    };
    
    // IDを使用済みにマーク
    usedItemIDs.insert(1001);
    usedItemIDs.insert(1003);
    usedItemIDs.insert(1005);
    
    std::cout << "新しいユニークID: " << generateUniqueID() << "\n";
    std::cout << "新しいユニークID: " << generateUniqueID() << "\n";
    std::cout << "新しいユニークID: " << generateUniqueID() << "\n";
    
    std::cout << "使用済みID一覧: ";
    for (int id : usedItemIDs)
    {
        std::cout << id << " ";
    }
    std::cout << "\n";
    
    return 0;
}

/*
 * まとめ：
 * - setは重複を許さず自動ソートされる順序付きコンテナ
 * - multisetは重複を許可する順序付きコンテナ
 * - カスタム比較関数でソート順をカスタマイズ可能
 * - 集合演算（和集合、積集合、差集合）が利用可能
 * - 検索・挿入・削除がO(log n)で効率的
 * - ゲームでのランキング、ユニークID管理、アイテム品質管理に最適
 * - 内部的には赤黒木で実装され、常にバランスが保たれる
 */