// 第10講: ポインタの基本
// メモリアドレスを扱う仕組み

#include <iostream>
#include <string>

int main() 
{
    std::cout << "=== ポインタの基本概念 ===" << std::endl;
    
    // 通常の変数
    int playerLevel = 25;
    std::cout << "プレイヤーレベル: " << playerLevel << std::endl;
    std::cout << "メモリアドレス: " << &playerLevel << std::endl;
    
    // ポインタの宣言と初期化
    int* levelPtr = &playerLevel;  // playerLevelのアドレスを格納
    
    std::cout << "\nポインタが保存している値（アドレス）: " << levelPtr << std::endl;
    std::cout << "ポインタが指す値: " << *levelPtr << std::endl;  // 間接参照
    std::cout << "ポインタ自体のアドレス: " << &levelPtr << std::endl;
    
    // ポインタを通じた値の変更
    *levelPtr = 30;  // プレイヤーのレベルを30に変更
    std::cout << "\nポインタ経由で変更後: " << playerLevel << std::endl;
    
    // nullptrの使用
    std::cout << "\n=== null ポインタ ===" << std::endl;
    
    int* weaponPtr = nullptr;  // 何も指していない
    
    if (weaponPtr == nullptr) 
    {
        std::cout << "武器を装備していません" << std::endl;
    }
    
    // 安全にポインタを使用
    if (weaponPtr != nullptr) 
    {
        std::cout << "武器ID: " << *weaponPtr << std::endl;
    }
    else 
    {
        std::cout << "武器ポインタがnullです" << std::endl;
    }
    
    // 配列とポインタ
    std::cout << "\n=== 配列とポインタ ===" << std::endl;
    
    int inventory[5] = {101, 102, 103, 104, 105};
    int* invPtr = inventory;  // 配列の最初の要素を指す
    
    std::cout << "配列の最初の要素: " << *invPtr << std::endl;
    std::cout << "2番目の要素: " << *(invPtr + 1) << std::endl;  // ポインタ演算
    std::cout << "3番目の要素: " << invPtr[2] << std::endl;  // 配列記法も使える
    
    // ポインタを使った配列の巡回
    std::cout << "\nインベントリ内容: ";
    for (int i = 0; i < 5; ++i) 
    {
        std::cout << *(invPtr + i) << " ";
    }
    std::cout << std::endl;
    
    // 文字列とポインタ
    std::cout << "\n=== 文字列とポインタ ===" << std::endl;
    
    const char* gameTitle = "冒険者の旅";  // 文字列リテラルへのポインタ
    std::cout << "ゲームタイトル: " << gameTitle << std::endl;
    
    char playerName[] = "勇者";
    char* namePtr = playerName;
    
    std::cout << "文字ごとの表示: ";
    while (*namePtr != '\0') 
    {
        std::cout << *namePtr << " ";
        namePtr++;  // 次の文字へ
    }
    std::cout << std::endl;
    
    // ポインタを返す関数
    std::cout << "\n=== ポインタを返す関数 ===" << std::endl;
    
    int scores[3] = {100, 200, 300};
    
    // 最大値を持つ要素のポインタを返す
    auto findMaxScore = [](int* arr, int size) -> int* 
    {
        int* maxPtr = arr;
        for (int i = 1; i < size; ++i) 
        {
            if (arr[i] > *maxPtr) 
            {
                maxPtr = &arr[i];
            }
        }
        return maxPtr;
    };
    
    int* maxScorePtr = findMaxScore(scores, 3);
    std::cout << "最高スコア: " << *maxScorePtr << std::endl;
    *maxScorePtr = 350;  // 最高スコアを更新
    std::cout << "更新後の配列: ";
    for (int i = 0; i < 3; ++i) 
    {
        std::cout << scores[i] << " ";
    }
    std::cout << std::endl;
    
    // ゲームでの実用例：リンクリスト風データ構造
    std::cout << "\n=== ゲームでの応用例 ===" << std::endl;
    
    struct Enemy 
    {
        std::string name;
        int hp;
        Enemy* next;  // 次の敵へのポインタ
    };
    
    // 敵を作成
    Enemy goblin = {"ゴブリン", 30, nullptr};
    Enemy orc = {"オーク", 50, nullptr};
    Enemy dragon = {"ドラゴン", 200, nullptr};
    
    // 敵をつなげる
    goblin.next = &orc;
    orc.next = &dragon;
    
    // 敵リストを巡回
    std::cout << "敵リスト:" << std::endl;
    Enemy* currentEnemy = &goblin;
    int enemyCount = 1;
    
    while (currentEnemy != nullptr) 
    {
        std::cout << enemyCount << ". " << currentEnemy->name 
                  << " (HP: " << currentEnemy->hp << ")" << std::endl;
        currentEnemy = currentEnemy->next;
        enemyCount++;
    }
    
    // ポインタの演算
    std::cout << "\n=== ポインタ演算 ===" << std::endl;
    
    int stats[4] = {100, 50, 30, 20};  // HP, MP, 攻撃, 防御
    int* statPtr = stats;
    
    std::cout << "HP: " << *statPtr << std::endl;
    statPtr++;  // 次の要素へ
    std::cout << "MP: " << *statPtr << std::endl;
    statPtr += 2;  // 2つ先へ
    std::cout << "防御: " << *statPtr << std::endl;
    
    // ポインタ同士の差
    int* hpPtr = &stats[0];
    int* defensePtr = &stats[3];
    std::cout << "ポインタ間の距離: " << (defensePtr - hpPtr) << " 要素" << std::endl;
    
    return 0;
}

/*
    ポインタの基本:
    
    宣言と初期化:
    - 型* ポインタ名;
    - 型* ポインタ名 = &変数;
    - 型* ポインタ名 = nullptr;
    
    演算子:
    - & : アドレス演算子（変数のアドレスを取得）
    - * : 間接参照演算子（ポインタが指す値にアクセス）
    
    ポインタ演算:
    - ptr++ : 次の要素へ
    - ptr-- : 前の要素へ
    - ptr + n : n個先の要素へ
    - ptr1 - ptr2 : ポインタ間の距離
    
    配列との関係:
    - 配列名は最初の要素へのポインタ
    - arr[i] は *(arr + i) と同じ
    
    注意点:
    - nullポインタの参照はクラッシュの原因
    - 未初期化ポインタは危険
    - メモリリークに注意
    - ダングリングポインタ（無効なメモリを指す）
    
    ゲーム開発での使用:
    - 動的メモリ管理
    - データ構造（リスト、ツリー）
    - オブジェクト間の参照
    - コールバック関数
*/