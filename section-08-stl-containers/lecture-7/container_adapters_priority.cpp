/*
 * Lecture 7: Container Adapters - Priority Queue と特殊コンテナ
 * 
 * コンテナアダプターは既存のコンテナに特殊なインターフェースを提供します。
 * priority_queueは特に重要で、ゲームのタスクスケジューリング、
 * AIの意思決定、イベント管理などで活用されます。
 */

#include <iostream>
#include <queue>
#include <vector>
#include <functional>
#include <string>
#include <chrono>

// ゲームタスクの定義
struct GameTask
{
    std::string name;
    int priority;
    float executionTime;
    
    GameTask(const std::string& n, int p, float time)
        : name(n), priority(p), executionTime(time)
    {
    }
    
    // priority_queue は最大ヒープなので、高い優先度が先に来るように
    bool operator<(const GameTask& other) const
    {
        return priority < other.priority;  // 優先度が高いほど先
    }
};

// AIエージェントの行動
struct AIAction
{
    std::string actionType;
    std::string target;
    float utility;  // 効用値（高いほど優先）
    int cost;
    
    AIAction(const std::string& type, const std::string& tgt, float util, int c)
        : actionType(type), target(tgt), utility(util), cost(c)
    {
    }
    
    bool operator<(const AIAction& other) const
    {
        return utility < other.utility;  // 効用値の高い順
    }
};

// ゲームイベント（時間ベース）
struct TimedEvent
{
    std::string eventName;
    float triggerTime;
    std::string description;
    
    TimedEvent(const std::string& name, float time, const std::string& desc)
        : eventName(name), triggerTime(time), description(desc)
    {
    }
    
    // 早い時間順にソート（最小ヒープ）
    bool operator>(const TimedEvent& other) const
    {
        return triggerTime > other.triggerTime;
    }
};

// ダメージイベント
struct DamageEvent
{
    std::string source;
    std::string target;
    int damage;
    float timestamp;
    
    DamageEvent(const std::string& src, const std::string& tgt, int dmg, float time)
        : source(src), target(tgt), damage(dmg), timestamp(time)
    {
    }
};

int main()
{
    // 1. 基本的なpriority_queue
    std::cout << \"=== 基本的なpriority_queue ===\\n\";\n    \n    std::priority_queue<int> basicPQ;\n    \n    // 要素を追加（内部で自動ソート）\n    basicPQ.push(30);\n    basicPQ.push(10);\n    basicPQ.push(50);\n    basicPQ.push(20);\n    basicPQ.push(40);\n    \n    std::cout << \"デフォルト（最大ヒープ）の順序: \";\n    while (!basicPQ.empty())\n    {\n        std::cout << basicPQ.top() << \" \";\n        basicPQ.pop();\n    }\n    std::cout << \"\\n\";\n    \n    // 最小ヒープ（昇順）\n    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;\n    \n    minHeap.push(30);\n    minHeap.push(10);\n    minHeap.push(50);\n    minHeap.push(20);\n    \n    std::cout << \"最小ヒープの順序: \";\n    while (!minHeap.empty())\n    {\n        std::cout << minHeap.top() << \" \";\n        minHeap.pop();\n    }\n    std::cout << \"\\n\";\n    \n    // 2. ゲームタスクスケジューラ\n    std::cout << \"\\n=== ゲームタスクスケジューラ ===\\n\";\n    \n    std::priority_queue<GameTask> taskScheduler;\n    \n    // タスクを優先度付きで追加\n    taskScheduler.push(GameTask(\"レンダリング\", 1, 16.67f));  // 60FPS\n    taskScheduler.push(GameTask(\"物理シミュレーション\", 5, 2.0f));\n    taskScheduler.push(GameTask(\"AI処理\", 3, 5.0f));\n    taskScheduler.push(GameTask(\"サウンド処理\", 2, 1.0f));\n    taskScheduler.push(GameTask(\"ネットワーク同期\", 4, 10.0f));\n    taskScheduler.push(GameTask(\"緊急システム処理\", 10, 0.5f));\n    \n    std::cout << \"タスク実行順序（優先度順）:\\n\";\n    float totalTime = 0.0f;\n    \n    while (!taskScheduler.empty())\n    {\n        const auto& task = taskScheduler.top();\n        totalTime += task.executionTime;\n        \n        std::cout << \"  優先度\" << task.priority << \": \" \n                  << task.name << \" (\" << task.executionTime << \"ms)\\n\";\n        taskScheduler.pop();\n    }\n    \n    std::cout << \"総実行時間: \" << totalTime << \"ms\\n\";\n    \n    // 3. AI行動選択システム\n    std::cout << \"\\n=== AI行動選択システム ===\\n\";\n    \n    std::priority_queue<AIAction> actionQueue;\n    \n    // AIの可能な行動を評価して追加\n    actionQueue.push(AIAction(\"攻撃\", \"プレイヤー\", 0.8f, 10));\n    actionQueue.push(AIAction(\"回復\", \"自分\", 0.9f, 15));\n    actionQueue.push(AIAction(\"逃走\", \"安全地帯\", 0.6f, 5));\n    actionQueue.push(AIAction(\"アイテム使用\", \"ポーション\", 0.7f, 8));\n    actionQueue.push(AIAction(\"魔法攻撃\", \"プレイヤー\", 0.95f, 20));\n    \n    std::cout << \"AI行動決定（効用値順）:\\n\";\n    int actionCount = 0;\n    \n    while (!actionQueue.empty() && actionCount < 3)  // 上位3つの行動\n    {\n        const auto& action = actionQueue.top();\n        std::cout << \"  \" << (actionCount + 1) << \". \" << action.actionType \n                  << \" -> \" << action.target \n                  << \" (効用値: \" << action.utility \n                  << \", コスト: \" << action.cost << \")\\n\";\n        \n        actionQueue.pop();\n        ++actionCount;\n    }\n    \n    // 4. イベントタイムライン管理\n    std::cout << \"\\n=== イベントタイムライン管理 ===\\n\";\n    \n    // 最小ヒープで時間順にイベントを管理\n    std::priority_queue<TimedEvent, std::vector<TimedEvent>, std::greater<TimedEvent>> eventTimeline;\n    \n    eventTimeline.push(TimedEvent(\"ゲーム開始\", 0.0f, \"ゲームが開始されました\"));\n    eventTimeline.push(TimedEvent(\"敵出現\", 5.0f, \"ゴブリンが現れた\"));\n    eventTimeline.push(TimedEvent(\"アイテム出現\", 3.0f, \"ポーションが出現\"));\n    eventTimeline.push(TimedEvent(\"ボス警告\", 15.0f, \"ボスが近づいている\"));\n    eventTimeline.push(TimedEvent(\"宝箱発見\", 8.0f, \"宝箱を発見した\"));\n    eventTimeline.push(TimedEvent(\"ボス出現\", 20.0f, \"ドラゴンが現れた\"));\n    \n    std::cout << \"時系列イベント:\\n\";\n    while (!eventTimeline.empty())\n    {\n        const auto& event = eventTimeline.top();\n        std::cout << \"  [\" << event.triggerTime << \"s] \" \n                  << event.eventName << \": \" << event.description << \"\\n\";\n        eventTimeline.pop();\n    }\n    \n    // 5. ダメージ処理システム\n    std::cout << \"\\n=== ダメージ処理システム ===\\n\";\n    \n    // カスタム比較関数を使用したpriority_queue\n    auto damageComparator = [](const DamageEvent& a, const DamageEvent& b)\n    {\n        // ダメージ量の多い順、同じなら時間の早い順\n        if (a.damage != b.damage)\n        {\n            return a.damage < b.damage;\n        }\n        return a.timestamp > b.timestamp;\n    };\n    \n    std::priority_queue<DamageEvent, std::vector<DamageEvent>, decltype(damageComparator)> \n        damageQueue(damageComparator);\n    \n    // ダメージイベントを追加\n    damageQueue.push(DamageEvent(\"ゴブリン\", \"プレイヤー\", 15, 1.0f));\n    damageQueue.push(DamageEvent(\"プレイヤー\", \"ゴブリン\", 25, 1.2f));\n    damageQueue.push(DamageEvent(\"オーク\", \"プレイヤー\", 30, 2.0f));\n    damageQueue.push(DamageEvent(\"プレイヤー\", \"オーク\", 30, 2.1f));\n    damageQueue.push(DamageEvent(\"ドラゴン\", \"プレイヤー\", 50, 3.0f));\n    \n    std::cout << \"ダメージイベント処理順序（ダメージ量順）:\\n\";\n    while (!damageQueue.empty())\n    {\n        const auto& event = damageQueue.top();\n        std::cout << \"  [\" << event.timestamp << \"s] \" \n                  << event.source << \" -> \" << event.target \n                  << \" ダメージ: \" << event.damage << \"\\n\";\n        damageQueue.pop();\n    }\n    \n    // 6. パフォーマンス監視システム\n    std::cout << \"\\n=== パフォーマンス監視システム ===\\n\";\n    \n    struct PerformanceData\n    {\n        std::string systemName;\n        float processingTime;\n        int priority;\n        \n        PerformanceData(const std::string& name, float time, int prio)\n            : systemName(name), processingTime(time), priority(prio)\n        {\n        }\n        \n        // 処理時間の長い順（問題のあるシステムを優先）\n        bool operator<(const PerformanceData& other) const\n        {\n            return processingTime < other.processingTime;\n        }\n    };\n    \n    std::priority_queue<PerformanceData> performanceMonitor;\n    \n    // システムの処理時間を監視\n    performanceMonitor.push(PerformanceData(\"レンダリング\", 12.3f, 1));\n    performanceMonitor.push(PerformanceData(\"物理エンジン\", 8.7f, 2));\n    performanceMonitor.push(PerformanceData(\"AI処理\", 25.1f, 3));  // 問題あり\n    performanceMonitor.push(PerformanceData(\"サウンド\", 3.2f, 4));\n    performanceMonitor.push(PerformanceData(\"ネットワーク\", 15.6f, 5));\n    \n    std::cout << \"パフォーマンス問題（処理時間順）:\\n\";\n    while (!performanceMonitor.empty())\n    {\n        const auto& data = performanceMonitor.top();\n        std::cout << \"  \" << data.systemName \n                  << \": \" << data.processingTime << \"ms\";\n        \n        if (data.processingTime > 16.67f)  // 60FPS基準\n        {\n            std::cout << \" [警告: フレーム落ち]\";\n        }\n        std::cout << \"\\n\";\n        \n        performanceMonitor.pop();\n    }\n    \n    // 7. 動的優先度調整システム\n    std::cout << \"\\n=== 動的優先度調整システム ===\\n\";\n    \n    struct DynamicTask\n    {\n        std::string name;\n        int basePriority;\n        float urgencyMultiplier;\n        \n        DynamicTask(const std::string& n, int base, float mult)\n            : name(n), basePriority(base), urgencyMultiplier(mult)\n        {\n        }\n        \n        float getEffectivePriority() const\n        {\n            return basePriority * urgencyMultiplier;\n        }\n        \n        bool operator<(const DynamicTask& other) const\n        {\n            return getEffectivePriority() < other.getEffectivePriority();\n        }\n    };\n    \n    std::priority_queue<DynamicTask> dynamicQueue;\n    \n    dynamicQueue.push(DynamicTask(\"通常攻撃\", 3, 1.0f));\n    dynamicQueue.push(DynamicTask(\"回復\", 2, 2.5f));  // 緊急度高\n    dynamicQueue.push(DynamicTask(\"防御\", 4, 0.8f));\n    dynamicQueue.push(DynamicTask(\"特殊攻撃\", 5, 1.2f));\n    \n    std::cout << \"動的優先度調整後のタスク実行順序:\\n\";\n    while (!dynamicQueue.empty())\n    {\n        const auto& task = dynamicQueue.top();\n        std::cout << \"  \" << task.name \n                  << \" (基本優先度: \" << task.basePriority\n                  << \", 緊急度: \" << task.urgencyMultiplier\n                  << \", 実効優先度: \" << task.getEffectivePriority() << \")\\n\";\n        dynamicQueue.pop();\n    }\n    \n    // 8. メモリ効率的な実装\n    std::cout << \"\\n=== メモリ効率的な実装 ===\\n\";\n    \n    // deque をベースにしたpriority_queue\n    std::priority_queue<int, std::deque<int>> dequePQ;\n    \n    // 事前に容量を確保した vector を使用\n    std::vector<int> reservedVector;\n    reservedVector.reserve(1000);\n    std::priority_queue<int, std::vector<int>> reservedPQ(std::less<int>(), std::move(reservedVector));\n    \n    for (int i = 0; i < 10; ++i)\n    {\n        dequePQ.push(i * 3 + 1);\n        reservedPQ.push(i * 2 + 5);\n    }\n    \n    std::cout << \"dequeベースのpriority_queue: \";\n    while (!dequePQ.empty())\n    {\n        std::cout << dequePQ.top() << \" \";\n        dequePQ.pop();\n    }\n    std::cout << \"\\n\";\n    \n    std::cout << \"容量予約vectorベースのpriority_queue: \";\n    int count = 0;\n    while (!reservedPQ.empty() && count < 5)  // 最初の5つだけ表示\n    {\n        std::cout << reservedPQ.top() << \" \";\n        reservedPQ.pop();\n        ++count;\n    }\n    std::cout << \"...\\n\";\n    \n    return 0;\n}\n\n/*\n * まとめ：\n * - priority_queueは自動的に優先度順に要素を管理するコンテナアダプター\n * - デフォルトは最大ヒープ、比較関数で最小ヒープも作成可能\n * - ゲームのタスクスケジューリング、AI行動選択、イベント管理に最適\n * - カスタム比較関数で複雑な優先度ロジックを実装可能\n * - 内部的にはヒープ構造で、挿入・削除がO(log n)\n * - リアルタイムゲームでの効率的な優先度処理を実現\n * - 動的優先度調整やパフォーマンス監視システムでも活用可能\n */"}]