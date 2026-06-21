#include <iostream>
#include <queue>
#include <string>

struct Task
{
    std::string name;
    int         priority;
};

int main()
{
    // --- タスクキューのシミュレーション ---
    std::queue<Task> task_queue;

    task_queue.push({ "敵A の攻撃",    1 });
    task_queue.push({ "敵B の移動",    2 });
    task_queue.push({ "プレイヤーの防御", 3 });

    std::cout << "=== タスク処理順 ===" << std::endl;
    while (!task_queue.empty())
    {
        const Task& current = task_queue.front();
        std::cout << "処理中: " << current.name << std::endl;
        task_queue.pop();
    }
    // 出力順：敵Aの攻撃 → 敵Bの移動 → プレイヤーの防御（入れた順）

    // --- BFS（幅優先探索）の骨格 ---
    // ゲームマップのノードを順番に探索するイメージ
    std::queue<int> bfs_queue;
    bfs_queue.push(0); // スタートノード

    std::cout << "\n=== BFS 探索順 ===" << std::endl;
    while (!bfs_queue.empty())
    {
        int node = bfs_queue.front();
        bfs_queue.pop();
        std::cout << "ノード " << node << " を訪問" << std::endl;

        // 隣接ノードを追加（仮の例）
        if (node < 3)
        {
            bfs_queue.push(node + 1);
        }
    }

    return 0;
}
