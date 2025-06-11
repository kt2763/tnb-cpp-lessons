/*
 * Lecture 5: std::map/std::multimap - キー・バリューペア
 * 
 * mapはキーと値のペアを格納する連想配列です。
 * ゲーム開発では、設定管理、プレイヤーデータベース、
 * アイテム辞書、ゲーム状態の管理などで非常に重要です。
 */

#include <iostream>
#include <map>
#include <string>
#include <vector>

// プレイヤーの統計情報
struct PlayerStats
{
    int level;
    int experience;
    int health;
    int mana;
    
    PlayerStats(int l = 1, int exp = 0, int hp = 100, int mp = 50)
        : level(l), experience(exp), health(hp), mana(mp)
    {
    }
    
    void display() const
    {
        std::cout << "レベル:" << level 
                  << " 経験値:" << experience
                  << " HP:" << health 
                  << " MP:" << mana;
    }
};

// ゲーム内アイテム
struct Item
{
    std::string name;
    int price;
    std::string description;
    
    Item(const std::string& n = "", int p = 0, const std::string& desc = "")
        : name(n), price(p), description(desc)
    {
    }
};

// クエスト情報
struct Quest
{
    std::string title;
    std::string description;
    int reward;
    bool completed;
    
    Quest(const std::string& t = "", const std::string& desc = "", 
          int r = 0, bool c = false)
        : title(t), description(desc), reward(r), completed(c)
    {
    }
};

int main()
{
    // 1. std::map - 基本的な使用法
    std::cout << "=== std::map - 基本操作 ===\n";
    
    std::map<std::string, int> playerLevels;
    
    // 要素の挿入（複数の方法）
    playerLevels["勇者アキラ"] = 25;
    playerLevels.insert(std::make_pair("魔法使いユミ", 22));
    playerLevels.insert({"戦士タロウ", 20});
    playerLevels.emplace("盗賊サトシ", 18);
    
    std::cout << "プレイヤーレベル一覧:\n";
    for (const auto& pair : playerLevels)
    {
        std::cout << "  " << pair.first << ": レベル " << pair.second << "\n";
    }
    
    // 要素のアクセスと検索
    std::string playerName = "勇者アキラ";
    if (playerLevels.find(playerName) != playerLevels.end())
    {
        std::cout << playerName << "のレベル: " << playerLevels[playerName] << "\n";
    }
    
    // at()メソッド（安全なアクセス）
    try
    {
        std::cout << "魔法使いユミのレベル: " << playerLevels.at("魔法使いユミ") << "\n";
    }
    catch (const std::out_of_range& e)
    {
        std::cout << "プレイヤーが見つかりません: " << e.what() << "\n";
    }
    
    // 2. プレイヤーデータベース
    std::cout << "\n=== プレイヤーデータベース ===\n";
    
    std::map<std::string, PlayerStats> playerDB;
    
    playerDB["Hero001"] = PlayerStats(25, 12500, 200, 150);
    playerDB["Mage002"] = PlayerStats(22, 9800, 120, 250);
    playerDB["Warrior003"] = PlayerStats(20, 8000, 300, 80);
    
    // プレイヤー情報の更新
    playerDB["Hero001"].experience += 500;
    playerDB["Hero001"].level = 26;
    
    std::cout << "プレイヤーデータベース:\n";
    for (const auto& [playerID, stats] : playerDB)  // C++17 構造化束縛
    {
        std::cout << "  " << playerID << ": ";
        stats.display();
        std::cout << "\n";
    }
    
    // 3. アイテム辞書システム
    std::cout << "\n=== アイテム辞書システム ===\n";
    
    std::map<int, Item> itemDatabase;\n    \n    // アイテムIDをキーとして使用\n    itemDatabase[1001] = Item(\"ポーション\", 50, \"HPを50回復\");\n    itemDatabase[1002] = Item(\"鉄の剣\", 300, \"攻撃力+10\");\n    itemDatabase[1003] = Item(\"魔法の杖\", 500, \"魔力+15\");\n    itemDatabase[1004] = Item(\"革の鎧\", 200, \"防御力+5\");\n    \n    std::cout << \"アイテムカタログ:\\n\";\n    for (const auto& [itemID, item] : itemDatabase)\n    {\n        std::cout << \"  ID\" << itemID << \": \" << item.name \n                  << \" (\" << item.price << \"G) - \" << item.description << \"\\n\";\n    }\n    \n    // アイテム検索機能\n    int searchID = 1002;\n    auto itemIt = itemDatabase.find(searchID);\n    if (itemIt != itemDatabase.end())\n    {\n        std::cout << \"\\nアイテム検索結果 (ID\" << searchID << \"): \" \n                  << itemIt->second.name << \"\\n\";\n    }\n    \n    // 4. ゲーム設定管理\n    std::cout << \"\\n=== ゲーム設定管理 ===\\n\";\n    \n    std::map<std::string, std::string> gameSettings;\n    \n    gameSettings[\"resolution\"] = \"1920x1080\";\n    gameSettings[\"difficulty\"] = \"Normal\";\n    gameSettings[\"sound_volume\"] = \"75\";\n    gameSettings[\"music_volume\"] = \"60\";\n    gameSettings[\"language\"] = \"Japanese\";\n    \n    std::cout << \"現在のゲーム設定:\\n\";\n    for (const auto& [key, value] : gameSettings)\n    {\n        std::cout << \"  \" << key << \" = \" << value << \"\\n\";\n    }\n    \n    // 設定の更新\n    gameSettings[\"difficulty\"] = \"Hard\";\n    gameSettings[\"sound_volume\"] = \"80\";\n    \n    std::cout << \"\\n更新後の設定:\\n\";\n    std::cout << \"  difficulty = \" << gameSettings[\"difficulty\"] << \"\\n\";\n    std::cout << \"  sound_volume = \" << gameSettings[\"sound_volume\"] << \"\\n\";\n    \n    // 5. std::multimap - 複数の値を持つキー\n    std::cout << \"\\n=== std::multimap - 複数値キー ===\\n\";\n    \n    std::multimap<std::string, Quest> playerQuests;\n    \n    // 同じプレイヤーに複数のクエストを割り当て\n    playerQuests.insert({\"勇者アキラ\", Quest(\"ゴブリン退治\", \"森のゴブリンを10匹倒せ\", 100, false)});\n    playerQuests.insert({\"勇者アキラ\", Quest(\"薬草採集\", \"薬草を5個集めろ\", 50, true)});\n    playerQuests.insert({\"勇者アキラ\", Quest(\"ドラゴン討伐\", \"古代ドラゴンを倒せ\", 1000, false)});\n    \n    playerQuests.insert({\"魔法使いユミ\", Quest(\"魔法書探し\", \"失われた魔法書を見つけろ\", 200, false)});\n    playerQuests.insert({\"魔法使いユミ\", Quest(\"元素の調和\", \"四大元素を集めろ\", 300, true)});\n    \n    std::cout << \"プレイヤークエスト一覧:\\n\";\n    for (const auto& [player, quest] : playerQuests)\n    {\n        std::cout << \"  \" << player << \": \" << quest.title \n                  << \" (報酬: \" << quest.reward << \"G) \"\n                  << (quest.completed ? \"[完了]\" : \"[進行中]\") << \"\\n\";\n    }\n    \n    // 特定のプレイヤーのクエストを検索\n    std::string targetPlayer = \"勇者アキラ\";\n    auto range = playerQuests.equal_range(targetPlayer);\n    \n    std::cout << \"\\n\" << targetPlayer << \"のクエスト:\\n\";\n    for (auto it = range.first; it != range.second; ++it)\n    {\n        std::cout << \"  - \" << it->second.title << \"\\n\";\n    }\n    \n    // 6. 応用例：インベントリシステム\n    std::cout << \"\\n=== インベントリシステム ===\\n\";\n    \n    std::map<int, int> inventory;  // アイテムID -> 個数\n    \n    // アイテムを追加する関数\n    auto addItem = [&](int itemID, int count)\n    {\n        inventory[itemID] += count;\n        std::cout << \"アイテムID\" << itemID << \"を\" << count << \"個追加\\n\";\n    };\n    \n    // アイテムを使用する関数\n    auto useItem = [&](int itemID, int count) -> bool\n    {\n        if (inventory[itemID] >= count)\n        {\n            inventory[itemID] -= count;\n            if (inventory[itemID] == 0)\n            {\n                inventory.erase(itemID);\n            }\n            std::cout << \"アイテムID\" << itemID << \"を\" << count << \"個使用\\n\";\n            return true;\n        }\n        else\n        {\n            std::cout << \"アイテムID\" << itemID << \"が不足しています\\n\";\n            return false;\n        }\n    };\n    \n    // インベントリの使用例\n    addItem(1001, 5);  // ポーション5個\n    addItem(1002, 1);  // 鉄の剣1本\n    addItem(1001, 3);  // ポーション追加3個\n    \n    std::cout << \"\\n現在のインベントリ:\\n\";\n    for (const auto& [itemID, count] : inventory)\n    {\n        auto itemIt = itemDatabase.find(itemID);\n        if (itemIt != itemDatabase.end())\n        {\n            std::cout << \"  \" << itemIt->second.name << \": \" << count << \"個\\n\";\n        }\n    }\n    \n    useItem(1001, 2);  // ポーション2個使用\n    useItem(1003, 1);  // 存在しないアイテム\n    \n    // 7. パフォーマンス考慮：カウンターシステム\n    std::cout << \"\\n=== カウンターシステム ===\\n\";\n    \n    std::map<std::string, int> eventCounters;\n    \n    // イベントカウンターの実装\n    auto incrementCounter = [&](const std::string& eventName)\n    {\n        ++eventCounters[eventName];  // 存在しなければ0で初期化後に+1\n    };\n    \n    // ゲームイベントのシミュレーション\n    std::vector<std::string> gameEvents = {\n        \"敵撃破\", \"アイテム取得\", \"レベルアップ\", \"敵撃破\", \"死亡\",\n        \"アイテム取得\", \"敵撃破\", \"宝箱発見\", \"敵撃破\", \"レベルアップ\"\n    };\n    \n    for (const auto& event : gameEvents)\n    {\n        incrementCounter(event);\n    }\n    \n    std::cout << \"ゲームイベント統計:\\n\";\n    for (const auto& [event, count] : eventCounters)\n    {\n        std::cout << \"  \" << event << \": \" << count << \"回\\n\";\n    }\n    \n    // 8. map の高度な操作\n    std::cout << \"\\n=== 高度なmap操作 ===\\n\";\n    \n    std::map<int, std::string> levelNames;\n    levelNames[1] = \"初心者の森\";\n    levelNames[5] = \"暗黒の洞窟\";\n    levelNames[10] = \"魔王の城\";\n    levelNames[15] = \"天空の塔\";\n    levelNames[20] = \"異次元の迷宮\";\n    \n    // 下限値検索（lower_bound）\n    int playerLevel = 7;\n    auto it = levelNames.lower_bound(playerLevel);\n    if (it != levelNames.end())\n    {\n        std::cout << \"プレイヤーレベル\" << playerLevel \n                  << \"で挑戦できるエリア: \" << it->second << \"\\n\";\n    }\n    \n    // 範囲削除の例\n    std::map<int, std::string> tempMap = levelNames;\n    tempMap.erase(tempMap.lower_bound(5), tempMap.upper_bound(10));\n    \n    std::cout << \"レベル5-10のエリア削除後:\\n\";\n    for (const auto& [level, name] : tempMap)\n    {\n        std::cout << \"  レベル\" << level << \": \" << name << \"\\n\";\n    }\n    \n    return 0;\n}\n\n/*\n * まとめ：\n * - mapはキー・バリューペアを格納する連想配列\n * - キーで自動ソートされ、検索・挿入・削除がO(log n)\n * - multimapは同じキーに複数の値を格納可能\n * - operator[]は要素が存在しない場合に新しい要素を作成\n * - at()は安全なアクセスで、存在しない場合は例外を投げる\n * - ゲーム開発でのデータベース管理、設定管理、統計管理に最適\n * - 構造化束縛（C++17）により、コードが読みやすくなる\n */"}]