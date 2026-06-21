#include <iostream>
#include <stack>
#include <string>

int main()
{
    // --- 基本操作 ---
    std::stack<std::string> action_history;

    // アクションを積む
    action_history.push("移動");
    action_history.push("攻撃");
    action_history.push("アイテム使用");

    std::cout << "=== undo シミュレーション ===" << std::endl;
    while (!action_history.empty())
    {
        // top() で確認してから pop() で取り出す
        std::cout << "取り消し: " << action_history.top() << std::endl;
        action_history.pop();
    }
    // 出力順：アイテム使用 → 攻撃 → 移動（逆順）

    std::cout << "\n--- スタックサイズ: " << action_history.size() << " ---" << std::endl;

    return 0;
}
