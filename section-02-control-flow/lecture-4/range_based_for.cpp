// 第4講: 範囲ベースfor文（C++11）
// コンテナの要素を簡潔に処理する範囲ベースfor文を学ぶ

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <map>

int main() 
{
    std::cout << "=== 範囲ベースfor文の基本 ===" << std::endl;
    
    // 配列での使用
    int numbers[] = {10, 20, 30, 40, 50};
    
    std::cout << "通常のfor文:" << std::endl;
    for (int i = 0; i < 5; i++) 
    {
        std::cout << numbers[i] << " ";
    }
    std::cout << std::endl;
    
    std::cout << "範囲ベースfor文:" << std::endl;
    for (int num : numbers) 
    {
        std::cout << num << " ";
    }
    std::cout << std::endl;
    
    // vectorでの使用
    std::vector<std::string> items = {"剣", "盾", "鎧", "兜"};
    
    std::cout << "\n装備品一覧:" << std::endl;
    for (const std::string& item : items) 
    {
        std::cout << "- " << item << std::endl;
    }
    
    // 値の変更（参照を使用）
    std::vector<int> damages = {10, 15, 20, 25};
    
    std::cout << "\nダメージ値を2倍に強化:" << std::endl;
    std::cout << "強化前: ";
    for (int dmg : damages) 
    {
        std::cout << dmg << " ";
    }
    std::cout << std::endl;
    
    // 参照を使って値を変更
    for (int& dmg : damages) 
    {
        dmg *= 2;
    }
    
    std::cout << "強化後: ";
    for (int dmg : damages) 
    {
        std::cout << dmg << " ";
    }
    std::cout << std::endl;
    
    std::cout << "\n=== ゲーム開発での使用例 ===" << std::endl;
    
    // キャラクター情報の構造体
    struct Character 
    {
        std::string name;
        int hp;
        int mp;
        int level;
    };
    
    std::vector<Character> party = {
        {"勇者", 100, 50, 15},
        {"戦士", 150, 20, 14},
        {"魔法使い", 70, 120, 16},
        {"僧侶", 80, 100, 13}
    };
    
    std::cout << "パーティメンバー情報:" << std::endl;
    for (const Character& member : party) 
    {
        std::cout << member.name << " (Lv." << member.level << ")"
                  << " HP:" << member.hp << " MP:" << member.mp << std::endl;
    }
    
    // 全員のレベルアップ
    std::cout << "\n全員レベルアップ！" << std::endl;
    for (Character& member : party) 
    {
        member.level++;
        member.hp += 10;
        member.mp += 5;
    }
    
    for (const Character& member : party) 
    {
        std::cout << member.name << " → Lv." << member.level << "!" << std::endl;
    }
    
    // スキルリスト
    std::array<std::string, 5> skills = {"火球", "雷撃", "回復", "防御", "会心の一撃"};
    
    std::cout << "\n使用可能スキル:" << std::endl;
    int skillNumber = 1;
    for (const std::string& skill : skills) 
    {
        std::cout << skillNumber++ << ". " << skill << std::endl;
    }
    
    // アイテムと個数の管理（map）
    std::map<std::string, int> inventory = {
        {"ポーション", 5},
        {"エーテル", 3},
        {"フェニックスの尾", 1},
        {"万能薬", 2}
    };
    
    std::cout << "\nアイテム所持数:" << std::endl;
    for (const auto& [item, count] : inventory)  // C++17の構造化束縛
    {
        std::cout << item << " × " << count << std::endl;
    }
    
    // 敵グループの処理
    struct Enemy 
    {
        std::string name;
        int hp;
        bool isAlive;
    };
    
    std::vector<Enemy> enemies = {
        {"ゴブリンA", 30, true},
        {"ゴブリンB", 25, true},
        {"オーク", 50, true}
    };
    
    // 全体攻撃魔法
    int magicDamage = 20;
    std::cout << "\n全体攻撃魔法発動！" << std::endl;
    
    for (Enemy& enemy : enemies) 
    {
        if (enemy.isAlive) 
        {
            enemy.hp -= magicDamage;
            std::cout << enemy.name << "に" << magicDamage << "のダメージ！";
            
            if (enemy.hp <= 0) 
            {
                enemy.isAlive = false;
                std::cout << " " << enemy.name << "を倒した！";
            }
            else 
            {
                std::cout << " (残りHP: " << enemy.hp << ")";
            }
            std::cout << std::endl;
        }
    }
    
    // 生存している敵のカウント
    int aliveCount = 0;
    for (const Enemy& enemy : enemies) 
    {
        if (enemy.isAlive) 
        {
            aliveCount++;
        }
    }
    std::cout << "残り敵数: " << aliveCount << std::endl;
    
    // autoキーワードの活用
    std::vector<double> multipliers = {1.0, 1.5, 2.0, 2.5, 3.0};
    
    std::cout << "\n難易度倍率:" << std::endl;
    for (auto mult : multipliers) 
    {
        std::cout << "×" << mult << " ";
    }
    std::cout << std::endl;
    
    return 0;
}

/*
    まとめ:
    - 範囲ベースfor文は、コンテナの全要素を簡潔に処理できる（C++11以降）
    - 構文: for (型 変数名 : コンテナ) { 処理 }
    - const参照（const 型&）で読み取り専用、参照（型&）で値の変更が可能
    - 配列、vector、array、map、setなど様々なコンテナで使用可能
    - インデックスが不要な場合は、通常のfor文より読みやすい
    - autoキーワードと組み合わせると、型を明示せずに使える
    - ゲーム開発では、パーティ管理、インベントリ処理、敵グループ処理などで便利
*/