/*
 * Lecture 2: std::list/std::deque - 連結リストと両端キュー
 * 
 * listは双方向連結リスト、dequeは両端キューです。
 * ゲームでは、順序の変更や前後への挿入が頻繁なシステムで使用します。
 */

#include <iostream>
#include <list>
#include <deque>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>

// ゲーム内のターン管理システム用キャラクター
class Character
{
public:
    std::string name;
    int speed;
    int initiative;
    
    Character(const std::string& n, int s, int i)
        : name(n), speed(s), initiative(i)
    {
    }
    
    bool operator<(const Character& other) const
    {
        return initiative > other.initiative;  // イニシアチブの高い順
    }
};

// コマンドパターン用コマンド
struct Command
{
    std::string type;
    std::string target;
    int priority;
    
    Command(const std::string& t, const std::string& tgt, int p)
        : type(t), target(tgt), priority(p)
    {
    }
};

int main()
{
    // 1. std::list - 連結リストの基本
    std::cout << "=== std::list - 連結リスト ===\n";
    
    std::list<Character> turnOrder;
    
    // キャラクターを追加
    turnOrder.push_back(Character("戦士", 10, 5));
    turnOrder.push_back(Character("魔法使い", 8, 7));
    turnOrder.push_back(Character("盗賊", 15, 8));
    turnOrder.push_front(Character("僧侶", 6, 3));  // 前に挿入
    
    std::cout << "ターン順序 (list):\n";
    for (const auto& character : turnOrder)
    {
        std::cout << "  " << character.name 
                  << " (イニシアチブ: " << character.initiative << ")\n";
    }
    
    // リストのソート（イニシアチブ順）
    turnOrder.sort();
    
    std::cout << "\nイニシアチブ順にソート後:\n";
    for (const auto& character : turnOrder)
    {
        std::cout << "  " << character.name 
                  << " (イニシアチブ: " << character.initiative << ")\n";
    }
    
    // 2. 中間への挿入 - リストの強み
    std::cout << "\n=== 中間への挿入 ===\n";
    
    // 特定の位置を見つけて新しいキャラクターを挿入
    auto it = std::find_if(turnOrder.begin(), turnOrder.end(),
        [](const Character& c) { return c.name == "魔法使い"; });
    
    if (it != turnOrder.end())
    {
        turnOrder.insert(it, Character("弓手", 12, 6));
        std::cout << "魔法使いの前に弓手を挿入\n";
    }
    
    // 3. std::deque - 両端キュー
    std::cout << "\n=== std::deque - 両端キュー ===\n";
    
    std::deque<Command> commandQueue;
    
    // 通常のコマンドを後ろに追加
    commandQueue.push_back(Command("攻撃", "ゴブリン", 1));
    commandQueue.push_back(Command("回復", "戦士", 2));
    commandQueue.push_back(Command("魔法", "スライム", 1));
    
    // 緊急コマンドを前に挿入
    commandQueue.push_front(Command("緊急回復", "魔法使い", 5));
    
    std::cout << "コマンドキュー:\n";
    for (const auto& cmd : commandQueue)
    {
        std::cout << "  " << cmd.type 
                  << " -> " << cmd.target 
                  << " (優先度: " << cmd.priority << ")\n";
    }
    
    // dequeはランダムアクセスも可能
    if (!commandQueue.empty())
    {
        std::cout << "\n最初のコマンド: " 
                  << commandQueue[0].type << "\n";
        std::cout << "最後のコマンド: " 
                  << commandQueue.back().type << "\n";
    }
    
    // 4. パフォーマンス比較 - 異なるコンテナの特性
    std::cout << "\n=== パフォーマンス比較 ===\n";
    
    const int testSize = 100000;
    
    // vectorでの前方挿入テスト
    std::vector<int> testVector;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i)
    {
        testVector.insert(testVector.begin(), i);  // 非効率的
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto vectorTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // dequeでの前方挿入テスト
    std::deque<int> testDeque;
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i)
    {
        testDeque.push_front(i);  // 効率的
    }
    
    end = std::chrono::high_resolution_clock::now();
    auto dequeTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "1000回の前方挿入:\n";
    std::cout << "  vector: " << vectorTime.count() << " マイクロ秒\n";
    std::cout << "  deque:  " << dequeTime.count() << " マイクロ秒\n";
    
    // 5. ゲーム内での実用例 - イベントシステム
    std::cout << "\n=== イベントシステムの例 ===\n";
    
    struct GameEvent
    {
        std::string type;
        float triggerTime;
        std::string data;
        
        GameEvent(const std::string& t, float time, const std::string& d)
            : type(t), triggerTime(time), data(d)
        {
        }
    };
    
    std::list<GameEvent> eventList;
    
    // イベントを時間順に追加
    eventList.push_back(GameEvent("敵出現", 5.0f, "ゴブリン群"));
    eventList.push_back(GameEvent("アイテム出現", 10.0f, "ポーション"));
    eventList.push_back(GameEvent("ボス出現", 15.0f, "ドラゴン"));
    
    // 途中で緊急イベントを挿入
    auto eventIt = std::find_if(eventList.begin(), eventList.end(),
        [](const GameEvent& e) { return e.triggerTime >= 12.0f; });
    
    if (eventIt != eventList.end())
    {
        eventList.insert(eventIt, GameEvent("特殊イベント", 12.0f, "宝箱発見"));
    }
    
    std::cout << "ゲームイベントスケジュール:\n";
    for (const auto& event : eventList)
    {
        std::cout << "  " << event.triggerTime << "s: " 
                  << event.type << " (" << event.data << ")\n";
    }
    
    // 6. リストの特殊操作
    std::cout << "\n=== リストの特殊操作 ===\n";
    
    std::list<int> scoreList1 = {100, 200, 150, 200, 300};
    std::list<int> scoreList2 = {250, 180, 200};
    
    std::cout << "リスト1: ";
    for (int score : scoreList1)
    {
        std::cout << score << " ";
    }
    std::cout << "\n";
    
    // 重複を削除
    scoreList1.sort();
    scoreList1.unique();
    
    std::cout << "重複削除後: ";
    for (int score : scoreList1)
    {
        std::cout << score << " ";
    }
    std::cout << "\n";
    
    // リストを結合（両方がソート済みの場合）
    scoreList2.sort();
    scoreList1.merge(scoreList2);
    
    std::cout 
              << "リスト結合後: ";
    for (int score : scoreList1)
    {
        std::cout << score << " ";
    }
    std::cout << "\n";
    
    return 0;
}

/*
 * まとめ：
 * - listは中間への挿入/削除がO(1)で効率的
 * - dequeは両端への操作がO(1)でランダムアクセスも可能
 * - ゲームのターン管理やイベントシステムでの活用
 * - vectorと異なりメモリが連続していないためキャッシュ効率は低い
 * - コンテナの特性を理解して適切な選択を行うことが重要
 */