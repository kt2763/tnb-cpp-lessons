// 第6講: goto文
// goto文の使い方と適切な使用場面を学ぶ（使用は最小限に）

#include <iostream>
#include <string>
#include <vector>

int main() 
{
    std::cout << "=== goto文の基本構文 ===" << std::endl;
    
    // 基本的なgoto文
    int count = 0;
    
    start:  // ラベル
    count++;
    std::cout << "カウント: " << count << std::endl;
    
    if (count < 3) 
    {
        goto start;  // startラベルへジャンプ
    }
    
    std::cout << "処理完了" << std::endl;
    
    // goto文を使った簡単なエラー処理
    int playerGold = 50;
    int itemPrice = 100;
    
    std::cout << "\nアイテム購入処理:" << std::endl;
    std::cout << "所持金: " << playerGold << "G" << std::endl;
    std::cout << "アイテム価格: " << itemPrice << "G" << std::endl;
    
    if (playerGold < itemPrice) 
    {
        goto purchase_error;
    }
    
    playerGold -= itemPrice;
    std::cout << "購入成功！残金: " << playerGold << "G" << std::endl;
    goto purchase_end;
    
    purchase_error:
    std::cout << "エラー: 所持金が不足しています" << std::endl;
    
    purchase_end:
    std::cout << "購入処理終了" << std::endl;
    
    std::cout << "\n=== ゲーム開発での使用例 ===" << std::endl;
    
    // 多重ループからの脱出（推奨される使用例）
    const int dungeonFloors = 3;
    const int roomsPerFloor = 3;
    bool treasureFound = false;
    
    std::cout << "ダンジョン探索開始:" << std::endl;
    
    for (int floor = 1; floor <= dungeonFloors; floor++) 
    {
        std::cout << "\n階層" << floor << ":" << std::endl;
        
        for (int room = 1; room <= roomsPerFloor; room++) 
        {
            std::cout << "  部屋" << room << "を探索... ";
            
            // 2階の2番目の部屋に宝がある
            if (floor == 2 && room == 2) 
            {
                std::cout << "宝を発見！" << std::endl;
                treasureFound = true;
                goto exploration_complete;  // 多重ループを一度に抜ける
            }
            else 
            {
                std::cout << "何もない" << std::endl;
            }
        }
    }
    
    exploration_complete:
    if (treasureFound) 
    {
        std::cout << "探索完了: 宝を手に入れた！" << std::endl;
    }
    else 
    {
        std::cout << "探索完了: 宝は見つからなかった" << std::endl;
    }
    
    // リソースの初期化とクリーンアップ
    std::cout << "\nゲームリソースの初期化:" << std::endl;
    
    bool graphicsLoaded = false;
    bool soundLoaded = false;
    bool dataLoaded = false;
    
    // グラフィックスの初期化
    std::cout << "グラフィックスを初期化中... ";
    graphicsLoaded = true;  // 成功と仮定
    if (!graphicsLoaded) 
    {
        goto cleanup;
    }
    std::cout << "成功" << std::endl;
    
    // サウンドの初期化
    std::cout << "サウンドを初期化中... ";
    soundLoaded = true;  // 成功と仮定
    if (!soundLoaded) 
    {
        goto cleanup;
    }
    std::cout << "成功" << std::endl;
    
    // ゲームデータの読み込み
    std::cout << "ゲームデータを読み込み中... ";
    dataLoaded = false;  // 失敗と仮定
    if (!dataLoaded) 
    {
        goto cleanup;
    }
    std::cout << "成功" << std::endl;
    
    std::cout << "すべての初期化が完了しました" << std::endl;
    goto init_success;
    
    cleanup:
    std::cout << "失敗" << std::endl;
    std::cout << "エラー: 初期化に失敗しました" << std::endl;
    
    // クリーンアップ処理
    if (soundLoaded) 
    {
        std::cout << "サウンドリソースを解放" << std::endl;
    }
    if (graphicsLoaded) 
    {
        std::cout << "グラフィックスリソースを解放" << std::endl;
    }
    
    init_success:
    
    // goto文を使わない書き方の比較
    std::cout << "\n=== goto文を使わない推奨される書き方 ===" << std::endl;
    
    // フラグを使った多重ループの脱出
    bool exitLoops = false;
    
    std::cout << "別のダンジョン探索（goto不使用）:" << std::endl;
    
    for (int floor = 1; floor <= dungeonFloors && !exitLoops; floor++) 
    {
        std::cout << "\n階層" << floor << ":" << std::endl;
        
        for (int room = 1; room <= roomsPerFloor && !exitLoops; room++) 
        {
            std::cout << "  部屋" << room << "を探索... ";
            
            if (floor == 1 && room == 3) 
            {
                std::cout << "ボスを発見！" << std::endl;
                exitLoops = true;  // フラグを使って脱出
            }
            else 
            {
                std::cout << "何もない" << std::endl;
            }
        }
    }
    
    // 関数を使ったエラー処理（推奨）
    auto processItem = [](int gold, int price) -> bool 
    {
        if (gold < price) 
        {
            std::cout << "購入失敗: 資金不足" << std::endl;
            return false;
        }
        
        std::cout << "購入成功！" << std::endl;
        return true;
    };
    
    std::cout << "\n関数を使った購入処理:" << std::endl;
    processItem(200, 150);  // 成功
    processItem(50, 100);   // 失敗
    
    return 0;
}

/*
    まとめ:
    - goto文は指定したラベルへ無条件にジャンプする
    - 多重ループからの脱出やエラー処理での使用が考えられる
    - しかし、コードの流れが分かりにくくなるため、使用は最小限に
    - 代替手段：
      - 多重ループ脱出 → フラグ変数や関数の分割
      - エラー処理 → 例外処理や早期return
      - リソース管理 → RAII（スマートポインタなど）
    - ゲーム開発でも、より構造化された方法が推奨される
    - C++では例外処理、関数、クラスなどを活用すべき
*/