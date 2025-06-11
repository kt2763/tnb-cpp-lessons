// 第4講: 基本的な入出力
// ユーザーからの入力を受け取り、結果を出力する

#include <iostream>
#include <string>

int main() 
{
    std::cout << "=== C++の基本的な入出力 ===" << std::endl;
    
    // 整数の入力
    int playerAge;
    std::cout << "あなたの年齢を入力してください: ";
    std::cin >> playerAge;
    std::cout << "あなたは " << playerAge << " 歳ですね。" << std::endl;
    
    // 文字列の入力（単語）
    std::string playerName;
    std::cout << "\nキャラクター名を入力してください: ";
    std::cin >> playerName;
    std::cout << "ようこそ、" << playerName << "さん！" << std::endl;
    
    // 複数の値を一度に入力
    int playerLevel, playerHP;
    std::cout << "\nレベルとHPを空白で区切って入力してください: ";
    std::cin >> playerLevel >> playerHP;
    std::cout << "レベル: " << playerLevel << ", HP: " << playerHP << std::endl;
    
    // ゲーム設定の入力例
    std::cout << "\n=== ゲーム設定 ===" << std::endl;
    
    char difficulty;
    std::cout << "難易度を選択してください (E:Easy, N:Normal, H:Hard): ";
    std::cin >> difficulty;
    
    switch(difficulty) 
    {
        case 'E':
        case 'e':
            std::cout << "イージーモードを選択しました。" << std::endl;
            break;
        case 'N':
        case 'n':
            std::cout << "ノーマルモードを選択しました。" << std::endl;
            break;
        case 'H':
        case 'h':
            std::cout << "ハードモードを選択しました。" << std::endl;
            break;
        default:
            std::cout << "無効な選択です。ノーマルモードに設定します。" << std::endl;
    }
    
    // 浮動小数点数の入力
    double attackMultiplier;
    std::cout << "\n攻撃力倍率を入力してください (1.0-2.0): ";
    std::cin >> attackMultiplier;
    std::cout << "攻撃力倍率: " << attackMultiplier << "倍" << std::endl;
    
    // 出力のフォーマット
    std::cout << "\n=== キャラクターステータス ===" << std::endl;
    std::cout << "名前: " << playerName << std::endl;
    std::cout << "年齢: " << playerAge << " 歳" << std::endl;
    std::cout << "レベル: " << playerLevel << std::endl;
    std::cout << "HP: " << playerHP << "/" << playerHP << std::endl;
    std::cout << "攻撃力倍率: " << attackMultiplier << "x" << std::endl;
    
    // エスケープシーケンスの使用
    std::cout << "\n=== エスケープシーケンスの例 ===" << std::endl;
    std::cout << "改行: 1行目\n2行目" << std::endl;
    std::cout << "タブ: 項目1\t項目2\t項目3" << std::endl;
    std::cout << "引用符: \"Hello, World!\"" << std::endl;
    std::cout << "バックスラッシュ: C:\\Users\\Player" << std::endl;
    
    return 0;
}

/*
    入出力の基本:
    
    std::cout (出力):
    - << 演算子で値を出力
    - std::endl で改行とバッファフラッシュ
    - \n だけなら改行のみ（高速）
    
    std::cin (入力):
    - >> 演算子で値を読み込む
    - 空白文字（スペース、タブ、改行）で区切られる
    - 文字列は最初の空白まで
    
    エスケープシーケンス:
    - \n : 改行
    - \t : タブ
    - \" : ダブルクォート
    - \\ : バックスラッシュ
    
    注意点:
    - std::cin は入力待ちでプログラムが停止する
    - 不正な入力（数値を期待して文字が入力された等）でエラーになる可能性
    - バッファに残った入力が次の読み込みに影響することがある
*/