/*
 * C++モダンコース - セクション9: アルゴリズムとイテレータ
 * レクチャー6: ヒープアルゴリズム（make_heap, push_heap, pop_heap）
 * 
 * このレクチャーでは、ヒープデータ構造を操作するアルゴリズムを学びます
 * ゲーム開発での優先度キュー、AIの意思決定、リソース管理の実装を学習します
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>
#include <queue>

// ゲームタスクの優先度管理
struct GameTask
{
    int priority;
    std::string task_name;
    std::string category;
    int estimated_time;
    
    GameTask(int p, const std::string& name, const std::string& cat, int time)
        : priority(p), task_name(name), category(cat), estimated_time(time) {}
    
    void display() const
    {
        std::cout << "[優先度:" << std::setw(2) << priority << "] "
                  << std::setw(20) << task_name 
                  << " (" << std::setw(10) << category << ") "
                  << estimated_time << "分" << std::endl;
    }
    
    // ヒープでは大きい値が優先されるため、優先度の高いタスクが上位に
    bool operator<(const GameTask& other) const
    {
        return priority < other.priority; // 逆順比較で高優先度を上位に
    }
};

// プレイヤーのスコア管理
struct PlayerScore
{
    std::string player_name;
    int score;
    int level;
    
    PlayerScore(const std::string& name, int s, int lv)
        : player_name(name), score(s), level(lv) {}
    
    void display() const
    {
        std::cout << std::setw(12) << player_name 
                  << " | Score: " << std::setw(6) << score
                  << " | Level: " << std::setw(3) << level << std::endl;
    }
    
    bool operator<(const PlayerScore& other) const
    {
        return score < other.score; // スコアの大きい順
    }
};

void demonstrateMakeHeap()
{
    std::cout << "\n=== make_heap の実例 ===" << std::endl;
    
    // プレイヤーのスコアデータ
    std::vector<PlayerScore> players = 
    {
        {"アリス", 8750, 25},
        {"ボブ", 9200, 22},
        {"チャーリー", 7800, 28},
        {"ダイアナ", 9500, 30},
        {"イーサン", 8200, 24},
        {"フィオナ", 9100, 26}
    };
    
    std::cout << "元のプレイヤーデータ:" << std::endl;
    for (const auto& player : players)
    {
        player.display();
    }
    
    // 1. ヒープを作成（最大ヒープ）
    std::cout << "\n1. make_heap後のデータ（ヒープ構造）:" << std::endl;
    std::make_heap(players.begin(), players.end());
    
    for (const auto& player : players)
    {
        player.display();
    }
    
    std::cout << "最高スコアプレイヤー（ヒープの先頭）: ";
    players.front().display();
    
    // 2. ヒープかどうかの確認
    bool is_heap = std::is_heap(players.begin(), players.end());
    std::cout << "ヒープ構造になっている: " << (is_heap ? "はい" : "いいえ") << std::endl;
    
    // 3. ヒープの範囲確認
    auto heap_end = std::is_heap_until(players.begin(), players.end());
    std::cout << "ヒープ範囲の要素数: " << (heap_end - players.begin()) << std::endl;
}

void demonstratePushHeap()
{
    std::cout << "\n=== push_heap の実例 ===" << std::endl;
    
    // ゲームタスクの優先度管理
    std::vector<GameTask> task_queue = 
    {
        {5, "敵の攻撃処理", "戦闘", 2},
        {3, "アイテム収集", "探索", 5},
        {8, "ボス戦開始", "戦闘", 15},
        {2, "背景音楽再生", "システム", 1},
        {6, "レベルアップ処理", "成長", 3}
    };
    
    // ヒープを作成
    std::make_heap(task_queue.begin(), task_queue.end());
    
    std::cout << "初期タスクキュー（ヒープ構造）:" << std::endl;
    for (const auto& task : task_queue)
    {
        task.display();
    }
    
    std::cout << "最優先タスク: ";
    task_queue.front().display();
    
    // 1. 新しいタスクを追加
    std::cout << "\n1. 新しいタスクを追加:" << std::endl;
    std::vector<GameTask> new_tasks = 
    {
        {9, "緊急イベント処理", "イベント", 10},
        {1, "デバッグログ出力", "デバッグ", 1},
        {7, "セーブデータ保存", "システム", 4}
    };
    
    for (const auto& new_task : new_tasks)
    {
        std::cout << "追加タスク: ";
        new_task.display();
        
        // タスクを追加してヒープ性質を維持
        task_queue.push_back(new_task);
        std::push_heap(task_queue.begin(), task_queue.end());
        
        std::cout << "追加後の最優先タスク: ";
        task_queue.front().display();
        std::cout << std::endl;
    }
    
    std::cout << "最終的なタスクキュー:" << std::endl;
    for (const auto& task : task_queue)
    {
        task.display();
    }
}

void demonstratePopHeap()
{
    std::cout << "\n=== pop_heap の実例 ===" << std::endl;
    
    // AIの意思決定優先度
    std::vector<int> ai_decisions = {85, 72, 90, 65, 78, 95, 68, 82};
    
    std::cout << "AI意思決定の重要度: ";
    for (int decision : ai_decisions)
    {
        std::cout << decision << " ";
    }
    std::cout << std::endl;
    
    // ヒープを作成
    std::make_heap(ai_decisions.begin(), ai_decisions.end());
    
    std::cout << "\nヒープ作成後: ";
    for (int decision : ai_decisions)
    {
        std::cout << decision << " ";
    }
    std::cout << std::endl;
    
    // 1. 最優先の意思決定を順次処理
    std::cout << "\n1. 優先度順に意思決定を処理:" << std::endl;
    
    while (!ai_decisions.empty())
    {
        // 最高優先度の要素を取得
        int highest_priority = ai_decisions.front();
        
        // ヒープから削除
        std::pop_heap(ai_decisions.begin(), ai_decisions.end());
        ai_decisions.pop_back();
        
        std::cout << "処理中の意思決定 (優先度: " << highest_priority << ")";
        std::cout << " | 残り決定数: " << ai_decisions.size() << std::endl;
        
        // 残りの要素を表示（最初の3個まで）
        if (!ai_decisions.empty())
        {
            std::cout << "  次の候補: ";
            for (size_t i = 0; i < std::min(ai_decisions.size(), size_t(3)); ++i)
            {
                std::cout << ai_decisions[i] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

void demonstrateHeapSort()
{
    std::cout << "\n=== heap_sort の実例 ===" << std::endl;
    
    // ゲーム内の経験値データ
    std::vector<int> experience_points = {1250, 890, 2100, 675, 1800, 950, 1350, 2400, 780, 1650};
    
    std::cout << "元の経験値データ: ";
    for (int exp : experience_points)
    {
        std::cout << exp << " ";
    }
    std::cout << std::endl;
    
    // 1. ヒープソートによる昇順ソート
    std::cout << "\n1. ヒープソート実行:" << std::endl;
    
    auto heap_sort_data = experience_points;
    
    // ヒープ作成
    std::make_heap(heap_sort_data.begin(), heap_sort_data.end());
    std::cout << "ヒープ作成後: ";
    for (int exp : heap_sort_data)
    {
        std::cout << exp << " ";
    }
    std::cout << std::endl;
    
    // ヒープソート実行
    std::sort_heap(heap_sort_data.begin(), heap_sort_data.end());
    std::cout << "ソート後（昇順）: ";
    for (int exp : heap_sort_data)
    {
        std::cout << exp << " ";
    }
    std::cout << std::endl;
    
    // 2. 通常のソートとの比較
    auto normal_sort_data = experience_points;
    std::sort(normal_sort_data.begin(), normal_sort_data.end());
    
    std::cout << "通常ソート結果: ";
    for (int exp : normal_sort_data)
    {
        std::cout << exp << " ";
    }
    std::cout << std::endl;
    
    bool same_result = (heap_sort_data == normal_sort_data);
    std::cout << "ヒープソートと通常ソートの結果が同じ: " << (same_result ? "はい" : "いいえ") << std::endl;
}

void demonstrateGameEventQueue()
{
    std::cout << "\n=== ゲームイベントキューシステム ===" << std::endl;
    
    // ゲームイベントの管理
    struct GameEvent
    {
        int timestamp;
        int priority;
        std::string event_type;
        std::string description;
        
        GameEvent(int time, int prio, const std::string& type, const std::string& desc)
            : timestamp(time), priority(prio), event_type(type), description(desc) {}
        
        void display() const
        {
            std::cout << "[T:" << std::setw(3) << timestamp 
                      << " P:" << priority << "] "
                      << std::setw(12) << event_type 
                      << " | " << description << std::endl;
        }
        
        // 優先度が高く、時間が早いものを優先
        bool operator<(const GameEvent& other) const
        {
            if (priority != other.priority)
                return priority < other.priority;
            return timestamp > other.timestamp; // 時間は早い順
        }
    };
    
    std::vector<GameEvent> event_queue = 
    {
        {100, 5, "戦闘", "スライムとの戦闘開始"},
        {120, 3, "アイテム", "ポーション使用"},
        {150, 8, "ボス", "ドラゴン出現"},
        {200, 2, "システム", "自動セーブ"},
        {180, 6, "スキル", "ファイアボール詠唱"}
    };
    
    std::cout << "初期イベントリスト:" << std::endl;
    for (const auto& event : event_queue)
    {
        event.display();
    }
    
    // ヒープ作成
    std::make_heap(event_queue.begin(), event_queue.end());
    
    std::cout << "\nヒープ構造での管理:" << std::endl;
    std::cout << "最優先イベント: ";
    event_queue.front().display();
    
    // 新しいイベントの追加と処理
    std::cout << "\n新しい緊急イベントを追加:" << std::endl;
    GameEvent emergency_event(110, 9, "緊急", "プレイヤーHP危険");
    emergency_event.display();
    
    event_queue.push_back(emergency_event);
    std::push_heap(event_queue.begin(), event_queue.end());
    
    std::cout << "追加後の最優先イベント: ";
    event_queue.front().display();
    
    // イベントを順次処理
    std::cout << "\nイベント処理順序:" << std::endl;
    int process_count = 0;
    while (!event_queue.empty() && process_count < 4)
    {
        auto current_event = event_queue.front();
        std::pop_heap(event_queue.begin(), event_queue.end());
        event_queue.pop_back();
        
        std::cout << ++process_count << ". 処理中: ";
        current_event.display();
    }
}

void demonstrateResourceManagement()
{
    std::cout << "\n=== リソース管理での優先度キュー ===" << std::endl;
    
    // ゲームリソースの管理
    struct GameResource
    {
        std::string resource_name;
        int priority;
        int memory_usage;
        bool is_critical;
        
        GameResource(const std::string& name, int prio, int memory, bool critical)
            : resource_name(name), priority(prio), memory_usage(memory), is_critical(critical) {}
        
        void display() const
        {
            std::cout << std::setw(15) << resource_name 
                      << " | P:" << priority
                      << " | " << std::setw(4) << memory_usage << "MB"
                      << " | " << (is_critical ? "重要" : "通常") << std::endl;
        }
        
        bool operator<(const GameResource& other) const
        {
            // 重要度、優先度、メモリ使用量の順で比較
            if (is_critical != other.is_critical)
                return !is_critical; // 重要なものが優先
            if (priority != other.priority)
                return priority < other.priority;
            return memory_usage > other.memory_usage; // メモリ使用量少ない順
        }
    };
    
    std::vector<GameResource> resources = 
    {
        {"プレイヤーテクスチャ", 8, 50, true},
        {"背景音楽", 4, 20, false},
        {"エフェクト音", 6, 10, false},
        {"UI画像", 7, 30, true},
        {"モンスターモデル", 5, 80, false},
        {"地形データ", 9, 120, true}
    };
    
    std::cout << "ゲームリソース一覧:" << std::endl;
    for (const auto& resource : resources)
    {
        resource.display();
    }
    
    // 優先度キューとしてヒープを作成
    std::make_heap(resources.begin(), resources.end());
    
    std::cout << "\n優先度順でのリソース読み込み:" << std::endl;
    int load_order = 1;
    auto resource_heap = resources;
    
    while (!resource_heap.empty() && load_order <= 4)
    {
        auto current_resource = resource_heap.front();
        std::pop_heap(resource_heap.begin(), resource_heap.end());
        resource_heap.pop_back();
        
        std::cout << load_order++ << ". 読み込み中: ";
        current_resource.display();
    }
    
    // メモリ不足シミュレーション
    std::cout << "\nメモリ不足時の緊急リソース解放:" << std::endl;
    
    // メモリ使用量で逆順ソート（大きいものから解放）
    std::make_heap(resources.begin(), resources.end(),
        [](const GameResource& a, const GameResource& b) 
        {
            if (a.is_critical != b.is_critical)
                return a.is_critical; // 重要でないものから解放
            return a.memory_usage < b.memory_usage; // メモリ使用量大きい順
        });
    
    std::cout << "解放候補（重要でないリソース優先）: ";
    resources.front().display();
}

int main()
{
    std::cout << "C++アルゴリズムとイテレータ - レクチャー6: ヒープアルゴリズム" << std::endl;
    std::cout << "========================================================" << std::endl;
    
    // 各ヒープアルゴリズムの実演
    demonstrateMakeHeap();
    demonstratePushHeap();
    demonstratePopHeap();
    demonstrateHeapSort();
    demonstrateGameEventQueue();
    demonstrateResourceManagement();
    
    std::cout << "\n=== まとめ ===" << std::endl;
    std::cout << "1. make_heap: 配列をヒープ構造に変換" << std::endl;
    std::cout << "2. push_heap: 新要素を追加してヒープ性質を維持" << std::endl;
    std::cout << "3. pop_heap: 最大要素を削除してヒープ性質を維持" << std::endl;
    std::cout << "4. sort_heap: ヒープをソート済み配列に変換" << std::endl;
    std::cout << "5. is_heap: ヒープかどうかの確認" << std::endl;
    std::cout << "ゲーム開発では、優先度キュー、イベント管理、リソース管理で重要" << std::endl;
    std::cout << "priority_queueの内部実装でも使用されています" << std::endl;
    
    return 0;
}