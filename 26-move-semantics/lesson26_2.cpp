#include <iostream>
#include <vector>
#include <string>

int main()
{
    // --- lvalue と rvalue の基本 ---
    int a = 10;    // a は lvalue（名前がある）
    int b = a + 5; // (a + 5) は rvalue（一時的な計算結果）

    // --- vector のムーブ ---
    std::vector<int> source(10000, 3); // 大きなvector
    std::cout << "sourceサイズ: " << source.size() << std::endl; // 10000

    // コピー：source のデータを全部複製
    std::vector<int> copied = source;
    std::cout << "copiedサイズ:  " << copied.size() << std::endl; // 10000
    std::cout << "sourceはまだ: " << source.size() << std::endl; // 10000 (そのまま)

    // ムーブ：source の内部ポインタを destination に「譲渡」する
    std::vector<int> moved = std::move(source);
    std::cout << "moved サイズ:   " << moved.size() << std::endl; // 10000
    std::cout << "sourceは今:    " << source.size() << std::endl; // 0（空になる）

    // ⚠️ std::move 後の source は使ってはいけない（未規定状態）
    // source.size() が 0 なのは実装依存であり保証ではないことに注意

    // --- std::string のムーブ ---
    std::string hero = "勇者";
    std::string moved_hero = std::move(hero);
    std::cout << "moved_hero: " << moved_hero << std::endl; // 勇者
    std::cout << "heroは今:  " << hero << std::endl; // "" (空)

    return 0;
}
