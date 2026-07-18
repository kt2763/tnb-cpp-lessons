#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// 引数チェックをして例外を投げる関数
int divide(int a, int b)
{
    if (b == 0)
    {
        throw std::invalid_argument("ゼロ除算はできません");
    }
    return a / b;
}

std::string get_enemy_name(int index)
{
    const std::vector<std::string> names = { "ゴブリン", "オーク", "ドラゴン" };
    if (index < 0 || index >= static_cast<int>(names.size()))
    {
        throw std::out_of_range("インデックスが範囲外です: " + std::to_string(index));
    }
    return names[index];
}

int main()
{
    // --- 基本的な try / catch ---
    try
    {
        int result = divide(10, 0); // ← ここで throw される
        std::cout << "結果: " << result << std::endl;
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "[invalid_argument] " << e.what() << std::endl;
    }

    // --- 複数の catch ---
    try
    {
        std::cout << get_enemy_name(1) << std::endl; // OK
        std::cout << get_enemy_name(99) << std::endl; // ← ここで throw
    }
    catch (const std::out_of_range& e)
    {
        std::cerr << "[out_of_range] " << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        // その他の std::exception 系をまとめて捕まえる
        std::cerr << "[exception] " << e.what() << std::endl;
    }
    catch (...)
    {
        // 型不明の例外（最後の砦）
        std::cerr << "[unknown exception]" << std::endl;
    }

    std::cout << "main続行中..." << std::endl;

    return 0;
}
