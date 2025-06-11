// 第7講: 関数ポインタ
// 関数のアドレスを格納し、動的に関数を呼び出す方法を学ぶ

#include <iostream>
#include <string>
#include <vector>
#include <map>

// === 基本的な関数群 ===
int add(int a, int b) { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }
int divide(int a, int b) { return b != 0 ? a / b : 0; }

// === ゲーム用の関数群 ===
void fireAttack() { std::cout << "火炎攻撃！" << std::endl; }
void iceAttack() { std::cout << "氷結攻撃！" << std::endl; }
void thunderAttack() { std::cout << "雷撃攻撃！" << std::endl; }

int calculatePhysicalDamage(int attack, int defense) 
{
    return std::max(1, attack - defense);
}

int calculateMagicalDamage(int magic, int resistance) 
{
    return std::max(1, magic - resistance / 2);
}

// === 関数ポインタの使用例 ===
void executeOperation(int (*func)(int, int), int a, int b) 
{
    int result = func(a, b);
    std::cout << "結果: " << result << std::endl;
}

// ゲームイベントシステム
typedef void (*EventHandler)();

struct GameEvent 
{
    std::string name;
    EventHandler handler;
};

int main() 
{
    std::cout << "=== 関数ポインタの基本 ===" << std::endl;
    
    // 関数ポインタの宣言と使用
    int (*operation)(int, int);
    
    operation = add;
    std::cout << "加算: " << operation(10, 5) << std::endl;
    
    operation = multiply;
    std::cout << "乗算: " << operation(10, 5) << std::endl;
    
    // 関数ポインタ配列
    int (*operations[])(int, int) = {add, subtract, multiply, divide};
    std::string opNames[] = {"加算", "減算", "乗算", "除算"};
    
    std::cout << "\n演算テーブル:" << std::endl;
    for (int i = 0; i < 4; i++) 
    {
        std::cout << opNames[i] << ": " << operations[i](20, 4) << std::endl;
    }
    
    std::cout << "\n=== ゲーム開発での実践例 ===" << std::endl;
    
    // スキルシステム
    void (*skills[])(void) = {fireAttack, iceAttack, thunderAttack};
    std::string skillNames[] = {"ファイア", "ブリザード", "サンダー"};
    
    int selectedSkill = 1;  // ブリザード
    std::cout << skillNames[selectedSkill] << "を発動！" << std::endl;
    skills[selectedSkill]();
    
    // ダメージ計算システム
    int (*damageCalculators[])(int, int) = {calculatePhysicalDamage, calculateMagicalDamage};
    
    int attack = 100, defense = 30;
    int magic = 80, resistance = 20;
    
    std::cout << "\n物理ダメージ: " << damageCalculators[0](attack, defense) << std::endl;
    std::cout << "魔法ダメージ: " << damageCalculators[1](magic, resistance) << std::endl;
    
    // イベントシステム
    std::vector<GameEvent> events = {
        {"戦闘開始", []() { std::cout << "戦闘が始まった！" << std::endl; }},
        {"レベルアップ", []() { std::cout << "レベルが上がった！" << std::endl; }},
        {"アイテム取得", []() { std::cout << "アイテムを手に入れた！" << std::endl; }}
    };
    
    std::cout << "\nイベント実行:" << std::endl;
    for (const auto& event : events) 
    {
        std::cout << event.name << " -> ";
        event.handler();
    }
    
    return 0;
}

/*
    まとめ:
    - 関数ポインタは関数のアドレスを格納する変数
    - 戻り値の型 (*ポインタ名)(引数の型) で宣言
    - 動的に呼び出す関数を切り替えられる
    - ゲーム開発では、スキルシステム、イベントハンドラ、AI行動選択で活用
    - C++11以降はstd::functionやラムダ式がより推奨される
*/