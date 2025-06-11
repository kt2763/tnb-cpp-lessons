/*
 * Lecture 3: std::stack/std::queue - LIFO/FIFOコンテナ
 * 
 * stackはLIFO（Last In, First Out）、queueはFIFO（First In, First Out）です。
 * ゲームでは、状態管理、コマンドバッファ、Undo/Redo機能などで
 * 非常に有用です。
 */

#include <iostream>
#include <stack>
#include <queue>
#include <string>
#include <vector>
#include <memory>

// ゲームの状態を表すクラス
class GameState
{
public:
    std::string stateName;
    int level;
    int playerHP;
    
    GameState(const std::string& name, int lvl, int hp)
        : stateName(name), level(lvl), playerHP(hp)
    {
    }
    
    void display() const
    {
        std::cout << "[状態: " << stateName 
                  << ", レベル: " << level 
                  << ", HP: " << playerHP << "]";
    }
};

// コマンドパターン用のコマンド
class Command
{
public:
    std::string action;
    std::string target;
    int damage;
    
    Command(const std::string& a, const std::string& t, int d)
        : action(a), target(t), damage(d)
    {
    }
    
    void execute() const
    {
        std::cout << action << " -> " << target 
                  << " (ダメージ: " << damage << ")\n";
    }
};

// イベントシステム用のイベント
struct GameEvent
{
    std::string type;
    std::string message;
    float timestamp;
    
    GameEvent(const std::string& t, const std::string& msg, float time)
        : type(t), message(msg), timestamp(time)
    {
    }
};

int main()
{
    // 1. std::stack - LIFOコンテナ
    std::cout << "=== std::stack - LIFOコンテナ ===\n";
    
    std::stack<GameState> stateStack;
    
    // ゲーム状態をスタックにプッシュ
    stateStack.push(GameState("メインメニュー", 1, 100));
    stateStack.push(GameState("ゲームプレイ", 5, 80));
    stateStack.push(GameState("バトル", 5, 65));
    stateStack.push(GameState("アイテム選択", 5, 65));
    
    std::cout << "現在のスタックサイズ: " << stateStack.size() << "\n";
    
    // スタックから状態を取り出し（ポップ）
    std::cout << "\nスタックから状態を取り出し:\n";
    while (!stateStack.empty())
    {
        std::cout << "現在の状態: ";
        stateStack.top().display();
        std::cout << "\n";
        stateStack.pop();
    }
    
    // 2. Undo/Redoシステムの実装
    std::cout << "\n=== Undo/Redoシステム ===\n";
    
    std::stack<std::string> undoStack;
    std::stack<std::string> redoStack;
    
    auto performAction = [&](const std::string& action)
    {
        undoStack.push(action);
        // Redoスタックをクリア（新しいアクション後はRedo不可）
        while (!redoStack.empty())
        {
            redoStack.pop();
        }
        std::cout << "実行: " << action << "\n";
    };
    
    auto undoAction = [&]()
    {
        if (!undoStack.empty())
        {
            std::string lastAction = undoStack.top();
            undoStack.pop();
            redoStack.push(lastAction);
            std::cout << "Undo: " << lastAction << "\n";
        }
        else
        {
            std::cout << "Undoできるアクションがありません\n";
        }
    };
    
    auto redoAction = [&]()
    {
        if (!redoStack.empty())
        {
            std::string action = redoStack.top();
            redoStack.pop();
            undoStack.push(action);
            std::cout << "Redo: " << action << "\n";
        }
        else
        {
            std::cout << "Redoできるアクションがありません\n";
        }
    };
    
    // アクションの実行
    performAction("アイテムを拾う");
    performAction("敵を攻撃");
    performAction("ポーションを使用");
    
    undoAction();
    undoAction();
    redoAction();
    
    // 3. std::queue - FIFOコンテナ
    std::cout << "\n=== std::queue - FIFOコンテナ ===\n";
    
    std::queue<Command> commandQueue;
    
    // コマンドをキューに追加
    commandQueue.push(Command("魔法攻撃", "ゴブリン", 15));
    commandQueue.push(Command("ヒール", "プレイヤー", 20));
    commandQueue.push(Command("ファイアボール", "オーク", 25));
    commandQueue.push(Command("シールド", "プレイヤー", 0));
    
    std::cout << "コマンドキューの実行:\n";
    while (!commandQueue.empty())
    {
        commandQueue.front().execute();
        commandQueue.pop();
    }
    
    // 4. イベントシステム - メッセージキュー
    std::cout << "\n=== イベントメッセージシステム ===\n";
    
    std::queue<GameEvent> eventQueue;
    
    // ゲームイベントをキューに追加
    eventQueue.push(GameEvent("ダメージ", "プレイヤーが10ダメージを受けた", 1.0f));
    eventQueue.push(GameEvent("レベルアップ", "プレイヤーのレベルが上がった", 1.5f));
    eventQueue.push(GameEvent("アイテム獲得", "ポーションを獲得した", 2.0f));
    eventQueue.push(GameEvent("敵遭遇", "ゴブリンが現れた", 2.5f));
    
    std::cout << "イベントの処理：（時系列順）\n";
    while (!eventQueue.empty())
    {
        const auto& event = eventQueue.front();
        std::cout << "[" << event.timestamp << "s] " 
                  << event.type << ": " << event.message << "\n";
        eventQueue.pop();
    }
    
    // 5. priority_queue - 優先度付きキュー
    std::cout << "\n=== std::priority_queue - 優先度付きキュー ===\n";
    
    struct Task
    {
        std::string name;
        int priority;
        
        Task(const std::string& n, int p) : name(n), priority(p) {}
        
        // priority_queueはデフォルトで最大ヒープ（値が大きいほど優先）
        bool operator<(const Task& other) const
        {
            return priority < other.priority;
        }
    };
    
    std::priority_queue<Task> taskQueue;
    
    // タスクを優先度付きで追加
    taskQueue.push(Task("アイテム管理", 1));
    taskQueue.push(Task("敵AI処理", 5));
    taskQueue.push(Task("物理シミュレーション", 3));
    taskQueue.push(Task("レンダリング", 2));
    taskQueue.push(Task("緊急処理", 10));
    
    std::cout << "タスクの優先度順実行:\n";
    while (!taskQueue.empty())
    {
        const auto& task = taskQueue.top();
        std::cout << "  優先度 " << task.priority 
                  << ": " << task.name << "\n";
        taskQueue.pop();
    }
    
    // 6. コンテナアダプターのカスタマイズ
    std::cout << "\n=== カスタマイズされたコンテナ ===\n";
    
    // dequeをベースとしたstack
    std::stack<int, std::deque<int>> customStack;
    
    // vectorをベースとしたqueue
    std::queue<int, std::vector<int>> customQueue;
    
    for (int i = 1; i <= 5; ++i)
    {
        customStack.push(i * 10);
        customQueue.push(i * 20);
    }
    
    std::cout << "dequeベースのstack: ";
    while (!customStack.empty())
    {
        std::cout << customStack.top() << " ";
        customStack.pop();
    }
    std::cout << "\n";
    
    std::cout << "vectorベースのqueue: ";
    while (!customQueue.empty())
    {
        std::cout << customQueue.front() << " ";
        customQueue.pop();
    }
    std::cout << "\n";
    
    // 7. ゲームループでの実用例
    std::cout << "\n=== ゲームループでの実用例 ===\n";
    
    // コマンドバッファシステム
    std::queue<std::string> inputBuffer;
    
    // ユーザー入力をシミュレート
    inputBuffer.push("上");
    inputBuffer.push("上");
    inputBuffer.push("下");
    inputBuffer.push("下");
    inputBuffer.push("左");
    inputBuffer.push("右");
    inputBuffer.push("左");
    inputBuffer.push("右");
    
    std::cout << "コマンド入力シーケンス: ";
    while (!inputBuffer.empty())
    {
        std::cout << inputBuffer.front() << " ";
        inputBuffer.pop();
    }
    std::cout << "\n";
    
    return 0;
}

/*
 * まとめ：
 * - stackはLIFOで、状態管理やUndo/Redoシステムに最適
 * - queueはFIFOで、コマンドバッファやイベントシステムに最適
 * - priority_queueは優先度付き処理でタスクスケジューリングに便利
 * - コンテナアダプターは内部コンテナをカスタマイズ可能
 * - ゲムロジックの構造化と整理に非常に有用
 */