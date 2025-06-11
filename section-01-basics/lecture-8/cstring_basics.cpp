// 第8講: C言語スタイルの文字列
// char配列を使った文字列処理（レガシーだが知っておく必要がある）

#include <iostream>
#include <cstring>  // C言語の文字列関数

int main() 
{
    std::cout << "=== C言語スタイルの文字列 ===" << std::endl;
    
    // char配列による文字列
    char playerName[20] = "Hero";  // 最大19文字+終端文字'\0'
    char enemyName[] = "Dragon";    // サイズは自動計算（7バイト）
    
    std::cout << "プレイヤー: " << playerName << std::endl;
    std::cout << "敵: " << enemyName << std::endl;
    
    // 文字列の長さ
    std::cout << "\n=== 文字列の長さ ===" << std::endl;
    
    int nameLength = std::strlen(playerName);
    std::cout << playerName << " の長さ: " << nameLength << " 文字" << std::endl;
    
    // 注意：配列のサイズと文字列の長さは違う
    std::cout << "配列のサイズ: " << sizeof(playerName) << " バイト" << std::endl;
    std::cout << "文字列の長さ: " << std::strlen(playerName) << " 文字" << std::endl;
    
    // 文字列のコピー
    std::cout << "\n=== 文字列のコピー ===" << std::endl;
    
    char backup[20];
    std::strcpy(backup, playerName);  // playerNameをbackupにコピー
    std::cout << "バックアップ: " << backup << std::endl;
    
    // より安全なコピー（サイズ指定）
    char safeCopy[10];
    std::strncpy(safeCopy, "VeryLongPlayerName", 9);
    safeCopy[9] = '\0';  // 終端文字を確実に追加
    std::cout << "安全なコピー: " << safeCopy << std::endl;
    
    // 文字列の連結
    std::cout << "\n=== 文字列の連結 ===" << std::endl;
    
    char title[50] = "勇者";
    std::strcat(title, "の");
    std::strcat(title, "冒険");
    std::cout << "タイトル: " << title << std::endl;
    
    // 文字列の比較
    std::cout << "\n=== 文字列の比較 ===" << std::endl;
    
    char password[] = "secret";
    char input[20];
    
    std::cout << "パスワードを入力: ";
    std::cin >> input;
    
    if (std::strcmp(password, input) == 0) 
    {
        std::cout << "パスワード正解！" << std::endl;
    }
    else 
    {
        std::cout << "パスワードが違います。" << std::endl;
    }
    
    // 文字配列の操作
    std::cout << "\n=== 文字配列の直接操作 ===" << std::endl;
    
    char message[] = "Level Up!";
    std::cout << "元のメッセージ: " << message << std::endl;
    
    // 小文字に変換
    for (int i = 0; message[i] != '\0'; ++i) 
    {
        if (message[i] >= 'A' && message[i] <= 'Z') 
        {
            message[i] = message[i] + 32;  // 小文字に変換
        }
    }
    std::cout << "小文字変換後: " << message << std::endl;
    
    // ゲームでの使用例：簡単なセーブデータ
    std::cout << "\n=== セーブデータの例 ===" << std::endl;
    
    char saveData[100];
    char playerClass[] = "Knight";
    int level = 15;
    int gold = 2500;
    
    // セーブデータを作成（sprintf使用）
    std::sprintf(saveData, "%s,%d,%d", playerClass, level, gold);
    std::cout << "セーブデータ: " << saveData << std::endl;
    
    // セーブデータを解析（簡易版）
    char loadedClass[20];
    int loadedLevel, loadedGold;
    std::sscanf(saveData, "%[^,],%d,%d", loadedClass, &loadedLevel, &loadedGold);
    
    std::cout << "\n読み込んだデータ:" << std::endl;
    std::cout << "職業: " << loadedClass << std::endl;
    std::cout << "レベル: " << loadedLevel << std::endl;
    std::cout << "所持金: " << loadedGold << "G" << std::endl;
    
    // 文字列の検索
    std::cout << "\n=== 文字列の検索 ===" << std::endl;
    
    char inventory[] = "Sword,Shield,Potion,Map";
    char* foundItem = std::strstr(inventory, "Potion");
    
    if (foundItem != nullptr) 
    {
        std::cout << "ポーションが見つかりました！" << std::endl;
        std::cout << "位置: " << (foundItem - inventory) << std::endl;
    }
    
    // C++のstring vs Cのchar配列
    std::cout << "\n=== std::string との比較 ===" << std::endl;
    
    // C言語スタイル（面倒で危険）
    char cstr1[50] = "Hello";
    char cstr2[50] = " World";
    char cstr3[100];
    std::strcpy(cstr3, cstr1);
    std::strcat(cstr3, cstr2);
    
    // C++スタイル（簡単で安全）
    std::string str1 = "Hello";
    std::string str2 = " World";
    std::string str3 = str1 + str2;
    
    std::cout << "Cスタイル: " << cstr3 << std::endl;
    std::cout << "C++スタイル: " << str3 << std::endl;
    
    return 0;
}

/*
    C言語スタイルの文字列:
    
    特徴:
    - char配列で表現
    - 終端文字 '\0' で終わる
    - 固定サイズ
    - 手動でメモリ管理が必要
    
    主な関数:
    - strlen(): 長さを取得
    - strcpy(): コピー
    - strcat(): 連結
    - strcmp(): 比較
    - strstr(): 検索
    
    注意点:
    - バッファオーバーフローの危険
    - 終端文字を忘れやすい
    - サイズ管理が面倒
    
    使用場面:
    - レガシーコードとの互換性
    - C言語のライブラリ使用時
    - 低レベルな処理
    - 組み込みシステム
    
    推奨:
    - 新しいコードではstd::stringを使う
    - C言語スタイルは必要な時のみ
*/