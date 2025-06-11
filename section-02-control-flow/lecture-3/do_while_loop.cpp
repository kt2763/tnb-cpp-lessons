// 第3講: do-while文
// 最低1回は実行されるdo-while文の特徴と使い方を学ぶ

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

int main() 
{
    std::cout << "=== do-while文の基本構文 ===" << std::endl;
    
    // 基本的なdo-while文
    int counter = 0;
    do 
    {
        std::cout << "実行回数: " << counter + 1 << std::endl;
        counter++;
    } while (counter < 3);
    
    // whileとdo-whileの違い
    std::cout << "\nwhile vs do-whileの比較:" << std::endl;
    
    int value = 10;
    
    // while文（条件が偽なので実行されない）
    std::cout << "while文の場合:" << std::endl;
    while (value < 5) 
    {
        std::cout << "  これは表示されません" << std::endl;
    }
    
    // do-while文（最低1回は実行される）
    std::cout << "do-while文の場合:" << std::endl;
    do 
    {
        std::cout << "  最低1回は実行されます（value = " << value << "）" << std::endl;
    } while (value < 5);
    
    std::cout << "\n=== ゲーム開発での使用例 ===" << std::endl;
    
    // メニュー選択システム
    int menuChoice;
    bool validChoice;
    
    std::cout << "メインメニュー:" << std::endl;
    do 
    {
        std::cout << "1. ニューゲーム" << std::endl;
        std::cout << "2. コンティニュー" << std::endl;
        std::cout << "3. オプション" << std::endl;
        std::cout << "4. 終了" << std::endl;
        std::cout << "選択してください (1-4): ";
        
        // シミュレーション用の入力
        menuChoice = 2;  // 実際はstd::cinで入力
        std::cout << menuChoice << std::endl;
        
        validChoice = (menuChoice >= 1 && menuChoice <= 4);
        
        if (!validChoice) 
        {
            std::cout << "無効な選択です。もう一度入力してください。" << std::endl;
        }
        else 
        {
            std::cout << "選択: " << menuChoice << " が選ばれました" << std::endl;
        }
        
        break;  // デモ用（実際は正しい入力まで繰り返す）
        
    } while (!validChoice);
    
    // ダイスロールシステム
    std::srand(std::time(nullptr));
    int targetNumber = 15;
    int totalRoll = 0;
    int rollCount = 0;
    
    std::cout << "\nダイスロール（目標値: " << targetNumber << "以上）" << std::endl;
    
    do 
    {
        int die1 = (std::rand() % 6) + 1;
        int die2 = (std::rand() % 6) + 1;
        int currentRoll = die1 + die2;
        totalRoll = currentRoll;
        rollCount++;
        
        std::cout << "ロール" << rollCount << ": " << die1 << " + " << die2 
                  << " = " << currentRoll << std::endl;
        
        if (totalRoll < targetNumber) 
        {
            std::cout << "失敗！もう一度ロールします..." << std::endl;
        }
        
    } while (totalRoll < targetNumber && rollCount < 5);  // 最大5回まで
    
    if (totalRoll >= targetNumber) 
    {
        std::cout << "成功！クリティカルヒット！" << std::endl;
    }
    else 
    {
        std::cout << "失敗...通常攻撃になります" << std::endl;
    }
    
    // ショップでの購入システム
    int playerGold = 500;
    int itemPrice = 150;
    int purchaseCount = 0;
    char continueShopping;
    
    std::cout << "\n武器屋へようこそ！" << std::endl;
    std::cout << "所持金: " << playerGold << "G" << std::endl;
    
    do 
    {
        std::cout << "\nポーション（" << itemPrice << "G）を購入しますか？" << std::endl;
        
        if (playerGold >= itemPrice) 
        {
            playerGold -= itemPrice;
            purchaseCount++;
            std::cout << "ポーションを購入しました！（残金: " << playerGold << "G）" << std::endl;
            
            if (playerGold >= itemPrice) 
            {
                continueShopping = 'y';  // デモ用（実際は入力を受け取る）
                std::cout << "続けて購入しますか？ (y/n): " << continueShopping << std::endl;
            }
            else 
            {
                std::cout << "所持金が足りません。" << std::endl;
                continueShopping = 'n';
            }
        }
        else 
        {
            std::cout << "所持金が足りません！" << std::endl;
            continueShopping = 'n';
        }
        
        // デモ用：3個買ったら終了
        if (purchaseCount >= 3) continueShopping = 'n';
        
    } while (continueShopping == 'y' || continueShopping == 'Y');
    
    std::cout << "\n合計" << purchaseCount << "個のポーションを購入しました" << std::endl;
    std::cout << "残金: " << playerGold << "G" << std::endl;
    
    // レベルアップ時のステータス振り分け
    int statPoints = 5;
    int strength = 10;
    int agility = 10;
    int intelligence = 10;
    
    std::cout << "\nレベルアップ！ステータスポイントを振り分けます" << std::endl;
    
    do 
    {
        std::cout << "\n残りポイント: " << statPoints << std::endl;
        std::cout << "現在のステータス - STR:" << strength 
                  << " AGI:" << agility << " INT:" << intelligence << std::endl;
        
        // デモ用の自動振り分け
        if (statPoints >= 2) 
        {
            strength += 2;
            statPoints -= 2;
            std::cout << "STRに2ポイント振り分けました" << std::endl;
        }
        else if (statPoints >= 1) 
        {
            agility += 1;
            statPoints -= 1;
            std::cout << "AGIに1ポイント振り分けました" << std::endl;
        }
        
    } while (statPoints > 0);
    
    std::cout << "\n最終ステータス - STR:" << strength 
              << " AGI:" << agility << " INT:" << intelligence << std::endl;
    
    return 0;
}

/*
    まとめ:
    - do-while文は、最低1回は処理を実行してから条件を判定する
    - 条件判定が最後にあるため、必ず1回は実行される
    - メニュー選択、入力検証など、最低1回は処理が必要な場面で有効
    - ゲーム開発では、ユーザー入力の検証、ショップシステム、ダイスロールなどで使用
    - while文との使い分け：初回実行が必須かどうかで判断
    - 無限ループを避けるため、必ず終了条件を適切に設定する
*/