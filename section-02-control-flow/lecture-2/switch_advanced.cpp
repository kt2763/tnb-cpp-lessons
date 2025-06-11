// 第2講: switch文の応用
// switch文の高度な使い方とゲーム開発での実践例

#include <iostream>
#include <string>
#include <vector>

int main() 
{
    std::cout << "=== switch文の応用テクニック ===" << std::endl;
    
    // 複数のcaseをまとめる
    int dayOfWeek = 3;  // 1:月曜 ... 7:日曜
    std::string shopStatus;
    
    switch (dayOfWeek) 
    {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            shopStatus = "武器屋：営業中（平日料金）";
            break;
            
        case 6:
        case 7:
            shopStatus = "武器屋：営業中（週末セール20%OFF）";
            break;
            
        default:
            shopStatus = "エラー：無効な曜日";
    }
    
    std::cout << shopStatus << std::endl;
    
    // switch文内での変数宣言（スコープに注意）
    int questID = 2;
    
    switch (questID) 
    {
        case 1:
        {
            // ブロックスコープを使用
            int goblinsToDefeat = 5;
            std::cout << "クエスト：ゴブリンを" << goblinsToDefeat << "体倒せ！" << std::endl;
            break;
        }
        
        case 2:
        {
            int itemsToCollect = 10;
            std::string itemName = "薬草";
            std::cout << "クエスト：" << itemName << "を" << itemsToCollect << "個集めろ！" << std::endl;
            break;
        }
        
        case 3:
        {
            std::string bossName = "ドラゴン";
            std::cout << "クエスト：" << bossName << "を討伐せよ！" << std::endl;
            break;
        }
    }
    
    std::cout << "\n=== ゲーム開発での実践例 ===" << std::endl;
    
    // コンボシステム
    std::vector<char> comboInput = {'A', 'A', 'B', 'A'};
    int comboLength = comboInput.size();
    std::string skillName;
    int damage = 0;
    
    // 最後の4つの入力でコンボ判定
    if (comboLength >= 4) 
    {
        // 簡易的なコンボパターン判定
        if (comboInput[comboLength-4] == 'A' && 
            comboInput[comboLength-3] == 'A' && 
            comboInput[comboLength-2] == 'B' && 
            comboInput[comboLength-1] == 'A') 
        {
            skillName = "火炎斬り";
            damage = 150;
        }
        else if (comboInput[comboLength-3] == 'B' && 
                 comboInput[comboLength-2] == 'B' && 
                 comboInput[comboLength-1] == 'A') 
        {
            skillName = "疾風突き";
            damage = 120;
        }
        else 
        {
            skillName = "通常攻撃";
            damage = 50;
        }
    }
    
    std::cout << "発動スキル: " << skillName << " (ダメージ: " << damage << ")" << std::endl;
    
    // ゲーム状態管理
    enum class GameState 
    {
        Title,
        Playing,
        Paused,
        GameOver,
        Victory
    };
    
    GameState currentState = GameState::Playing;
    bool canSave = false;
    bool canMove = false;
    bool showMenu = false;
    
    switch (currentState) 
    {
        case GameState::Title:
            showMenu = true;
            std::cout << "\nタイトル画面" << std::endl;
            std::cout << "- ニューゲーム" << std::endl;
            std::cout << "- コンティニュー" << std::endl;
            std::cout << "- オプション" << std::endl;
            break;
            
        case GameState::Playing:
            canSave = true;
            canMove = true;
            std::cout << "\nゲームプレイ中" << std::endl;
            std::cout << "移動: " << (canMove ? "可能" : "不可") << std::endl;
            std::cout << "セーブ: " << (canSave ? "可能" : "不可") << std::endl;
            break;
            
        case GameState::Paused:
            canSave = true;
            showMenu = true;
            std::cout << "\nポーズ中" << std::endl;
            std::cout << "- ゲームに戻る" << std::endl;
            std::cout << "- セーブ" << std::endl;
            std::cout << "- タイトルに戻る" << std::endl;
            break;
            
        case GameState::GameOver:
            showMenu = true;
            std::cout << "\nゲームオーバー" << std::endl;
            std::cout << "- リトライ" << std::endl;
            std::cout << "- タイトルに戻る" << std::endl;
            break;
            
        case GameState::Victory:
            canSave = true;
            std::cout << "\nクリア！" << std::endl;
            std::cout << "- セーブして続ける" << std::endl;
            std::cout << "- タイトルに戻る" << std::endl;
            break;
    }
    
    // 属性相性システム
    enum Element 
    {
        Fire = 1,
        Water = 2,
        Grass = 3,
        Electric = 4
    };
    
    Element attackElement = Fire;
    Element defenseElement = Grass;
    double damageMultiplier = 1.0;
    
    std::cout << "\n属性相性判定: ";
    
    switch (attackElement) 
    {
        case Fire:
            switch (defenseElement) 
            {
                case Water:
                    damageMultiplier = 0.5;
                    std::cout << "火→水: 効果は今ひとつ（0.5倍）" << std::endl;
                    break;
                case Grass:
                    damageMultiplier = 2.0;
                    std::cout << "火→草: 効果は抜群（2.0倍）" << std::endl;
                    break;
                default:
                    std::cout << "火→その他: 通常ダメージ（1.0倍）" << std::endl;
            }
            break;
            
        case Water:
            switch (defenseElement) 
            {
                case Fire:
                    damageMultiplier = 2.0;
                    std::cout << "水→火: 効果は抜群（2.0倍）" << std::endl;
                    break;
                case Grass:
                    damageMultiplier = 0.5;
                    std::cout << "水→草: 効果は今ひとつ（0.5倍）" << std::endl;
                    break;
                default:
                    std::cout << "水→その他: 通常ダメージ（1.0倍）" << std::endl;
            }
            break;
            
        default:
            std::cout << "通常ダメージ（1.0倍）" << std::endl;
    }
    
    return 0;
}

/*
    まとめ:
    - 複数のcaseを同じ処理にまとめることができる（フォールスルーを活用）
    - case内で変数を宣言する場合は、ブロックスコープ{}を使用する
    - switch文はネストして使用することも可能（属性相性など）
    - enumやenum classと組み合わせると、可読性の高いコードになる
    - ゲーム状態管理、コンボシステム、属性相性など複雑な分岐に有効
    - 各caseの処理が複雑になる場合は、関数に分割することを検討
*/