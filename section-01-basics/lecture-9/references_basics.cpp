// 第9講: 参照（リファレンス）
// 変数の別名を作る仕組み

#include <iostream>
#include <string>

// 値渡しの関数（元の値は変更されない）
void levelUpByValue(int level) 
{
    level += 1;  // コピーを変更するだけ
    std::cout << "関数内のレベル: " << level << std::endl;
}

// 参照渡しの関数（元の値が変更される）
void levelUpByReference(int& level) 
{
    level += 1;  // 元の変数を直接変更
    std::cout << "関数内のレベル: " << level << std::endl;
}

// 複数の値を返す例
void getPlayerStatus(int& hp, int& mp, std::string& status) 
{
    hp = 100;
    mp = 50;
    status = "正常";
}

// const参照（読み取り専用）
void displayItemInfo(const std::string& itemName, const int& price) 
{
    std::cout << "アイテム: " << itemName << " (価格: " << price << "G)" << std::endl;
    // itemName = "変更";  // エラー！const参照は変更不可
}

int main() 
{
    std::cout << "=== 参照の基本 ===" << std::endl;
    
    int playerLevel = 10;
    int& levelRef = playerLevel;  // playerLevelの参照（別名）
    
    std::cout << "元の変数: " << playerLevel << std::endl;
    std::cout << "参照: " << levelRef << std::endl;
    std::cout << "アドレス（元）: " << &playerLevel << std::endl;
    std::cout << "アドレス（参照）: " << &levelRef << std::endl;  // 同じアドレス！
    
    // 参照を通じて値を変更
    levelRef = 15;
    std::cout << "\n参照経由で変更後:" << std::endl;
    std::cout << "元の変数: " << playerLevel << std::endl;  // 15になる
    
    // 値渡しと参照渡しの違い
    std::cout << "\n=== 値渡し vs 参照渡し ===" << std::endl;
    
    int level1 = 20;
    int level2 = 20;
    
    std::cout << "値渡しの場合:" << std::endl;
    std::cout << "呼び出し前: " << level1 << std::endl;
    levelUpByValue(level1);
    std::cout << "呼び出し後: " << level1 << std::endl;  // 変わらない！
    
    std::cout << "\n参照渡しの場合:" << std::endl;
    std::cout << "呼び出し前: " << level2 << std::endl;
    levelUpByReference(level2);
    std::cout << "呼び出し後: " << level2 << std::endl;  // 変わる！
    
    // 複数の値を返す
    std::cout << "\n=== 複数の値を返す ===" << std::endl;
    
    int hp, mp;
    std::string status;
    getPlayerStatus(hp, mp, status);
    
    std::cout << "HP: " << hp << std::endl;
    std::cout << "MP: " << mp << std::endl;
    std::cout << "ステータス: " << status << std::endl;
    
    // const参照の使用
    std::cout << "\n=== const参照 ===" << std::endl;
    
    std::string swordName = "伝説の剣エクスカリバー";
    int swordPrice = 10000;
    
    displayItemInfo(swordName, swordPrice);  // コピーなしで効率的
    
    // 配列の参照
    std::cout << "\n=== 配列の参照 ===" << std::endl;
    
    int inventory[5] = {101, 102, 103, 104, 105};
    int (&invRef)[5] = inventory;  // 配列全体への参照
    
    std::cout << "インベントリ内容: ";
    for (int i = 0; i < 5; ++i) 
    {
        std::cout << invRef[i] << " ";
    }
    std::cout << std::endl;
    
    // ゲームでの実用例：ダメージ計算
    std::cout << "\n=== ゲームでの実用例 ===" << std::endl;
    
    struct Character 
    {
        std::string name;
        int hp;
        int maxHp;
        int attack;
        int defense;
    };
    
    Character hero = {"勇者", 100, 100, 30, 20};
    Character enemy = {"ゴブリン", 50, 50, 15, 10};
    
    // 攻撃処理（参照で効率的に）
    auto attackTarget = [](const Character& attacker, Character& target) 
    {
        int damage = attacker.attack - target.defense;
        if (damage < 1) damage = 1;  // 最低1ダメージ
        
        target.hp -= damage;
        if (target.hp < 0) target.hp = 0;
        
        std::cout << attacker.name << " の攻撃！" << std::endl;
        std::cout << target.name << " に " << damage << " のダメージ！" << std::endl;
        std::cout << target.name << " の残りHP: " << target.hp << "/" << target.maxHp << std::endl;
    };
    
    attackTarget(hero, enemy);
    
    // 参照を返す関数（要注意）
    std::cout << "\n=== 参照を返す関数 ===" << std::endl;
    
    int scores[3] = {100, 200, 300};
    
    // インデックスを指定して参照を返す
    auto getScore = [&scores](int index) -> int& 
    {
        return scores[index];
    };
    
    std::cout << "スコア[1]の値: " << getScore(1) << std::endl;
    getScore(1) = 250;  // 参照経由で変更
    std::cout << "変更後のスコア[1]: " << scores[1] << std::endl;
    
    return 0;
}

/*
    参照（リファレンス）のポイント:
    
    基本:
    - 型& 変数名 = 元の変数;
    - 変数の別名（エイリアス）
    - 必ず初期化が必要
    - 途中で別の変数を参照できない
    
    メリット:
    - コピーが発生しない（効率的）
    - 関数で複数の値を返せる
    - 大きなオブジェクトを効率的に渡せる
    
    const参照:
    - const 型& で読み取り専用
    - 一時オブジェクトも受け取れる
    - 関数の引数でよく使う
    
    注意点:
    - ローカル変数の参照を返さない
    - nullにできない（必ず何かを参照）
    - 参照の参照は作れない
    
    ゲーム開発での使用:
    - 大きな構造体の受け渡し
    - ステータスの更新
    - 配列やコンテナの要素アクセス
    - パフォーマンスが重要な場面
*/