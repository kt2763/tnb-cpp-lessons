// 第7講: 配列の基本
// 同じ型の複数の値をまとめて管理する

#include <iostream>

int main() 
{
    std::cout << "=== 配列の基本 ===" << std::endl;
    
    // 配列の宣言と初期化
    int itemIDs[5] = {101, 102, 103, 104, 105};  // アイテムID
    
    // 配列の要素にアクセス（インデックスは0から始まる）
    std::cout << "最初のアイテムID: " << itemIDs[0] << std::endl;
    std::cout << "3番目のアイテムID: " << itemIDs[2] << std::endl;
    
    // 配列の要素を変更
    itemIDs[1] = 200;  // 2番目の要素を変更
    std::cout << "変更後の2番目のアイテムID: " << itemIDs[1] << std::endl;
    
    // ゲームでの使用例：プレイヤーのステータス
    std::cout << "\n=== プレイヤーステータス配列 ===" << std::endl;
    
    // HP、MP、攻撃力、防御力、素早さ
    int playerStats[5] = {100, 50, 25, 20, 15};
    const char* statNames[5] = {"HP", "MP", "攻撃力", "防御力", "素早さ"};
    
    // 全ステータスを表示
    for (int i = 0; i < 5; ++i) 
    {
        std::cout << statNames[i] << ": " << playerStats[i] << std::endl;
    }
    
    // レベルアップでステータス上昇
    std::cout << "\nレベルアップ！" << std::endl;
    for (int i = 0; i < 5; ++i) 
    {
        playerStats[i] += 5;  // 全ステータス+5
        std::cout << statNames[i] << ": " << playerStats[i] << " (+5)" << std::endl;
    }
    
    // 部分的な初期化
    std::cout << "\n=== 部分的な初期化 ===" << std::endl;
    
    int enemyHP[10] = {50, 60, 70};  // 残りは0で初期化される
    std::cout << "敵のHP: ";
    for (int i = 0; i < 10; ++i) 
    {
        std::cout << enemyHP[i] << " ";
    }
    std::cout << std::endl;
    
    // 配列のサイズを求める
    std::cout << "\n=== 配列のサイズ ===" << std::endl;
    
    double itemPrices[] = {100.0, 250.0, 500.0, 1000.0, 2500.0};  // サイズを省略
    int arraySize = sizeof(itemPrices) / sizeof(itemPrices[0]);
    
    std::cout << "アイテム価格一覧:" << std::endl;
    for (int i = 0; i < arraySize; ++i) 
    {
        std::cout << "アイテム" << (i + 1) << ": " << itemPrices[i] << "G" << std::endl;
    }
    
    // 多次元配列
    std::cout << "\n=== 2次元配列（マップデータ） ===" << std::endl;
    
    // 5×5のマップ（0:通路、1:壁、2:宝箱）
    int gameMap[5][5] = {
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 2, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1}
    };
    
    std::cout << "ゲームマップ:" << std::endl;
    for (int y = 0; y < 5; ++y) 
    {
        for (int x = 0; x < 5; ++x) 
        {
            if (gameMap[y][x] == 0) 
            {
                std::cout << "  ";  // 通路
            }
            else if (gameMap[y][x] == 1) 
            {
                std::cout << "■ ";  // 壁
            }
            else if (gameMap[y][x] == 2) 
            {
                std::cout << "宝 ";  // 宝箱
            }
        }
        std::cout << std::endl;
    }
    
    // 配列を使った簡単なインベントリシステム
    std::cout << "\n=== インベントリシステム ===" << std::endl;
    
    int inventory[10] = {0};  // 全て0で初期化
    int itemCount = 0;
    
    // アイテムを追加
    inventory[itemCount++] = 101;  // ポーション
    inventory[itemCount++] = 102;  // エーテル
    inventory[itemCount++] = 201;  // 鉄の剣
    
    std::cout << "所持アイテム:" << std::endl;
    for (int i = 0; i < itemCount; ++i) 
    {
        std::cout << "スロット" << (i + 1) << ": アイテムID " << inventory[i] << std::endl;
    }
    std::cout << "空きスロット: " << (10 - itemCount) << std::endl;
    
    return 0;
}

/*
    配列の基本:
    
    宣言方法:
    - 型 配列名[要素数];
    - 型 配列名[要素数] = {値1, 値2, ...};
    - 型 配列名[] = {値1, 値2, ...};  // サイズ自動
    
    特徴:
    - インデックスは0から始まる
    - サイズは固定（実行時に変更不可）
    - 連続したメモリ領域に配置
    - 境界チェックなし（注意！）
    
    多次元配列:
    - int array[行][列];
    - ゲームマップ、座標データなどに使用
    
    ゲーム開発での使用例:
    - ステータス管理
    - インベントリ
    - マップデータ
    - 高得点ランキング
    - バフ/デバフ管理
*/