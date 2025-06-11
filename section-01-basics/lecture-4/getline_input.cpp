// 第4講: getlineを使った入力
// 空白を含む文字列の入力方法

#include <iostream>
#include <string>

int main() 
{
    std::cout << "=== getlineを使った文字列入力 ===" << std::endl;
    
    // cin >> では空白までしか読めない問題
    std::string firstName, lastName;
    std::cout << "名前を入力してください（姓名を空白で区切る）: ";
    std::cin >> firstName >> lastName;
    std::cout << "姓: " << firstName << ", 名: " << lastName << std::endl;
    
    // 入力バッファのクリア
    std::cin.ignore();  // 改行文字を無視
    
    // getlineで行全体を読む
    std::string fullName;
    std::cout << "\nフルネームを入力してください: ";
    std::getline(std::cin, fullName);
    std::cout << "フルネーム: " << fullName << std::endl;
    
    // ゲームでの使用例
    std::cout << "\n=== ゲームメッセージ入力 ===" << std::endl;
    
    // プレイヤーの自己紹介
    std::string introduction;
    std::cout << "キャラクターの自己紹介を入力してください:\n";
    std::getline(std::cin, introduction);
    std::cout << "\n[" << fullName << "]: " << introduction << std::endl;
    
    // ギルド名の入力
    std::string guildName;
    std::cout << "\nギルド名を入力してください（空白も使用可能）: ";
    std::getline(std::cin, guildName);
    std::cout << "ギルド「" << guildName << "」を作成しました！" << std::endl;
    
    // 複数行の入力
    std::cout << "\n=== クエスト説明文の入力 ===" << std::endl;
    std::cout << "クエストの説明を3行で入力してください:" << std::endl;
    
    std::string line1, line2, line3;
    std::getline(std::cin, line1);
    std::getline(std::cin, line2);
    std::getline(std::cin, line3);
    
    std::cout << "\n【クエスト詳細】" << std::endl;
    std::cout << line1 << std::endl;
    std::cout << line2 << std::endl;
    std::cout << line3 << std::endl;
    
    // 数値入力後のgetlineの注意点
    int questLevel;
    std::cout << "\nクエストレベルを入力: ";
    std::cin >> questLevel;
    
    // 重要: 数値入力後は必ずignore()を使う
    std::cin.ignore();
    
    std::string questReward;
    std::cout << "報酬アイテム名を入力: ";
    std::getline(std::cin, questReward);
    
    std::cout << "\n=== クエスト情報 ===" << std::endl;
    std::cout << "レベル: " << questLevel << std::endl;
    std::cout << "報酬: " << questReward << std::endl;
    
    return 0;
}

/*
    getlineの使い方:
    
    std::getline(std::cin, 文字列変数):
    - 改行までの1行全体を読み込む
    - 空白も含めて読み込める
    - チャットメッセージやコマンド入力に最適
    
    cin >> と getline の混在時の注意:
    - cin >> の後には改行文字が残る
    - cin.ignore() で改行文字を除去する必要がある
    - これを忘れると getline が空文字を読んでしまう
    
    ゲーム開発での使用場面:
    - プレイヤー名（空白を含む）
    - チャットメッセージ
    - コマンド入力
    - セーブデータの名前
    - ギルド名やチーム名
*/