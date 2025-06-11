// 第8講: 文字列の基本
// C++における文字列の扱い方

#include <iostream>
#include <string>
#include <cctype>  // 文字操作関数

int main() 
{
    std::cout << "=== std::string の基本 ===" << std::endl;
    
    // 文字列の宣言と初期化
    std::string playerName = "勇者";
    std::string guildName("冒険者ギルド");
    std::string emptyString;  // 空文字列
    
    std::cout << "プレイヤー名: " << playerName << std::endl;
    std::cout << "ギルド名: " << guildName << std::endl;
    std::cout << "空文字列の長さ: " << emptyString.length() << std::endl;
    
    // 文字列の連結
    std::cout << "\n=== 文字列の連結 ===" << std::endl;
    
    std::string title = "伝説の";
    std::string job = "騎士";
    std::string fullTitle = title + job;  // + 演算子で連結
    
    std::cout << "称号: " << fullTitle << std::endl;
    
    // 文字列への追加
    std::string message = "レベル";
    message += "アップ！";  // += で追加
    message = message + " Lv.10に到達！";
    
    std::cout << "メッセージ: " << message << std::endl;
    
    // 文字列の長さとアクセス
    std::cout << "\n=== 文字列の操作 ===" << std::endl;
    
    std::string itemName = "エクスカリバー";
    std::cout << "アイテム名: " << itemName << std::endl;
    std::cout << "文字数: " << itemName.length() << std::endl;
    std::cout << "最初の文字: " << itemName[0] << std::endl;
    std::cout << "最後の文字: " << itemName[itemName.length() - 1] << std::endl;
    
    // 文字列の比較
    std::cout << "\n=== 文字列の比較 ===" << std::endl;
    
    std::string password = "secret123";
    std::string userInput = "secret123";
    
    if (password == userInput) 
    {
        std::cout << "パスワードが一致しました！" << std::endl;
    }
    else 
    {
        std::cout << "パスワードが違います。" << std::endl;
    }
    
    // 辞書順での比較
    std::string item1 = "ポーション";
    std::string item2 = "エリクサー";
    
    if (item1 < item2) 
    {
        std::cout << item1 << " は " << item2 << " より辞書順で前です" << std::endl;
    }
    
    // 部分文字列の取得
    std::cout << "\n=== 部分文字列 ===" << std::endl;
    
    std::string questTitle = "魔王城への道：最終章";
    std::string mainTitle = questTitle.substr(0, 6);  // 開始位置, 長さ
    std::string chapter = questTitle.substr(7);  // 開始位置から最後まで
    
    std::cout << "クエスト: " << questTitle << std::endl;
    std::cout << "メインタイトル: " << mainTitle << std::endl;
    std::cout << "章: " << chapter << std::endl;
    
    // 文字列の検索
    std::cout << "\n=== 文字列の検索 ===" << std::endl;
    
    std::string chatMessage = "誰か一緒にダンジョンに行きませんか？";
    std::string keyword = "ダンジョン";
    
    size_t foundPos = chatMessage.find(keyword);
    if (foundPos != std::string::npos) 
    {
        std::cout << "'" << keyword << "' が位置 " << foundPos << " で見つかりました" << std::endl;
    }
    
    // 文字列の置換
    std::cout << "\n=== 文字列の置換 ===" << std::endl;
    
    std::string statusMessage = "HPが50回復しました";
    std::cout << "元のメッセージ: " << statusMessage << std::endl;
    
    // "50" を "100" に置換
    size_t pos = statusMessage.find("50");
    if (pos != std::string::npos) 
    {
        statusMessage.replace(pos, 2, "100");  // 位置, 長さ, 新しい文字列
    }
    std::cout << "置換後: " << statusMessage << std::endl;
    
    // 文字列と数値の変換
    std::cout << "\n=== 文字列と数値の相互変換 ===" << std::endl;
    
    // 数値を文字列に
    int playerLevel = 25;
    int playerExp = 1250;
    std::string levelText = "Lv." + std::to_string(playerLevel);
    std::string expText = "EXP: " + std::to_string(playerExp);
    
    std::cout << levelText << " " << expText << std::endl;
    
    // 文字列を数値に
    std::string damageText = "150";
    int damageValue = std::stoi(damageText);
    std::cout << "ダメージ値: " << damageValue << " (数値に変換)" << std::endl;
    
    // ゲームコマンドの解析例
    std::cout << "\n=== コマンド解析の例 ===" << std::endl;
    
    std::string command = "attack goblin";
    size_t spacePos = command.find(' ');
    
    if (spacePos != std::string::npos) 
    {
        std::string action = command.substr(0, spacePos);
        std::string target = command.substr(spacePos + 1);
        
        std::cout << "アクション: " << action << std::endl;
        std::cout << "ターゲット: " << target << std::endl;
        
        if (action == "attack") 
        {
            std::cout << target << " を攻撃します！" << std::endl;
        }
    }
    
    return 0;
}

/*
    std::string の主な機能:
    
    基本操作:
    - length() / size(): 文字列の長さ
    - empty(): 空文字列かチェック
    - clear(): 文字列をクリア
    - [index] / at(index): 文字へのアクセス
    
    連結・追加:
    - + 演算子: 文字列の連結
    - += 演算子: 文字列の追加
    - append(): 文字列の追加
    
    検索・置換:
    - find(): 部分文字列を検索
    - rfind(): 後ろから検索
    - replace(): 部分文字列を置換
    
    部分文字列:
    - substr(): 部分文字列を取得
    
    変換:
    - std::to_string(): 数値→文字列
    - std::stoi(): 文字列→int
    - std::stod(): 文字列→double
    
    ゲーム開発での使用:
    - プレイヤー名、アイテム名
    - チャットメッセージ
    - コマンド解析
    - セーブデータ
    - ローカライゼーション
*/