#include <iostream>
#include <deque>
#include <string>

int main()
{
    std::deque<std::string> party;

    // 末尾に追加
    party.push_back("勇者");
    party.push_back("魔法使い");
    party.push_back("僧侶");

    // 先頭に追加（vectorでは遅い操作もdequeなら高速）
    party.push_front("盗賊");

    std::cout << "=== パーティ ===" << std::endl;
    for (const auto& name : party)
    {
        std::cout << name << std::endl;
    }
    // 盗賊 / 勇者 / 魔法使い / 僧侶

    // ランダムアクセスも可能
    std::cout << "\n先頭: " << party.front() << std::endl;
    std::cout << "末尾: " << party.back() << std::endl;
    std::cout << "2番目: " << party[1] << std::endl;

    // 先頭・末尾から削除
    party.pop_front(); // 盗賊を外す
    party.pop_back();  // 僧侶を外す

    std::cout << "\n=== 変更後のパーティ ===" << std::endl;
    for (const auto& name : party)
    {
        std::cout << name << std::endl;
    }
    // 勇者 / 魔法使い

    return 0;
}
