// 第9講: std::function（C++11）
// 汎用的な関数ラッパーstd::functionの使い方を学ぶ

#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include <map>

// 通常の関数
int add(int a, int b) { return a + b; }

// 関数オブジェクト
class Multiplier 
{
public:
    int operator()(int a, int b) const { return a * b; }
};

int main() 
{
    std::cout << "=== std::functionの基本 ===" << std::endl;
    
    // 様々な呼び出し可能オブジェクトを格納
    std::function<int(int, int)> operation;
    
    // 通常の関数
    operation = add;
    std::cout << "関数: " << operation(10, 5) << std::endl;
    
    // 関数オブジェクト
    operation = Multiplier();
    std::cout << "ファンクタ: " << operation(10, 5) << std::endl;
    
    // ラムダ式
    operation = [](int a, int b) { return a - b; };
    std::cout << "ラムダ: " << operation(10, 5) << std::endl;
    
    std::cout << "\n=== ゲーム開発での実践例 ===" << std::endl;
    
    // スキルシステム
    using SkillFunction = std::function<void(const std::string&)>;
    
    std::map<std::string, SkillFunction> skills;
    
    // 各種スキル
    skills["ファイア"] = [](const std::string& target) 
    {
        std::cout << target << "に火炎魔法を唱えた！" << std::endl;
    };
    
    skills["ヒール"] = [](const std::string& target) 
    {
        std::cout << target << "のHPを回復した！" << std::endl;
    };
    
    // スキル実行
    skills["ファイア"]("ゴブリン");
    skills["ヒール"]("勇者");
    
    // イベントハンドラシステム
    using EventHandler = std::function<void()>;
    std::vector<EventHandler> eventQueue;
    
    // イベントを登録
    eventQueue.push_back([]() { std::cout << "戦闘開始！" << std::endl; });
    eventQueue.push_back([]() { std::cout << "経験値獲得！" << std::endl; });
    
    // イベント実行
    std::cout << "\nイベント処理:" << std::endl;
    for (auto& event : eventQueue) 
    {
        event();
    }
    
    // コールバック機能
    auto processAction = [](const std::string& action, std::function<void()> callback) 
    {
        std::cout << action << "を実行中..." << std::endl;
        callback();
    };
    
    std::cout << "\nコールバック実行:" << std::endl;
    processAction("セーブ", []() { std::cout << "セーブ完了！" << std::endl; });
    
    return 0;
}

/*
    まとめ:
    - std::functionは任意の呼び出し可能オブジェクトを格納
    - 関数、ラムダ、ファンクタを統一的に扱える
    - ゲーム開発では、イベントシステム、コールバック、プラグインで活用
    - 型安全で柔軟なAPI設計が可能
*/