#include <iostream>
#include <vector>
#include <stdexcept>

// 汎用スタック（どんな型でも積み重ねられる）
template<typename T>
class Stack
{
public:
    void push(const T& value)
    {
        items_.push_back(value);
    }

    void pop()
    {
        if (items_.empty())
        {
            throw std::underflow_error("スタックが空です");
        }
        items_.pop_back();
    }

    const T& top() const
    {
        if (items_.empty())
        {
            throw std::underflow_error("スタックが空です");
        }
        return items_.back();
    }

    bool empty() const { return items_.empty(); }
    int  size()  const { return static_cast<int>(items_.size()); }

private:
    std::vector<T> items_;
};

int main()
{
    // int 版スタック
    Stack<int> damage_log;
    damage_log.push(30);
    damage_log.push(50);
    damage_log.push(20);

    std::cout << "最新ダメージ: " << damage_log.top() << std::endl; // 20
    damage_log.pop();
    std::cout << "次のダメージ: " << damage_log.top() << std::endl; // 50

    // string 版スタック：undoヒストリーのイメージ
    Stack<std::string> action_history;
    action_history.push("移動");
    action_history.push("攻撃");
    action_history.push("アイテム使用");

    std::cout << "\n--- アクション履歴（undoシミュレーション）---" << std::endl;
    while (!action_history.empty())
    {
        std::cout << "取り消し: " << action_history.top() << std::endl;
        action_history.pop();
    }

    return 0;
}
