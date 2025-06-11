// 第2講: デフォルト引数
// 関数の引数にデフォルト値を設定する方法を学ぶ

#include <iostream>
#include <string>
#include <vector>

// === デフォルト引数の基本 ===
// デフォルト引数は右側から順に設定する必要がある
void createCharacter(std::string name, int hp = 100, int mp = 50, int level = 1) 
{
    std::cout << "キャラクター作成: " << name << std::endl;
    std::cout << "  HP: " << hp << ", MP: " << mp << ", レベル: " << level << std::endl;
}

// ダメージ計算（クリティカル率のデフォルト値）
int calculateDamage(int baseDamage, double criticalRate = 0.1, double criticalMultiplier = 1.5) 
{
    // クリティカル判定（簡易版）
    bool isCritical = (std::rand() % 100) < (criticalRate * 100);
    
    if (isCritical) 
    {
        std::cout << "クリティカルヒット！ ";
        return baseDamage * criticalMultiplier;
    }
    
    return baseDamage;
}

// アイテムドロップ（レアリティによるドロップ率）
bool dropItem(std::string itemName, double dropRate = 0.5, bool bossKill = false) 
{
    // ボス撃破時はドロップ率2倍
    if (bossKill) 
    {
        dropRate = std::min(dropRate * 2, 1.0);
    }
    
    bool dropped = (std::rand() % 100) < (dropRate * 100);
    
    if (dropped) 
    {
        std::cout << itemName << " をドロップ！";
        if (bossKill) std::cout << "（ボスボーナス）";
        std::cout << std::endl;
    }
    
    return dropped;
}

// メッセージ表示（色とスタイルのデフォルト値）
enum class MessageType 
{
    Normal,
    Warning,
    Error,
    Success
};

void displayMessage(std::string message, MessageType type = MessageType::Normal, bool showBorder = false) 
{
    if (showBorder) 
    {
        std::cout << "========================================" << std::endl;
    }
    
    switch (type) 
    {
        case MessageType::Warning:
            std::cout << "[警告] ";
            break;
        case MessageType::Error:
            std::cout << "[エラー] ";
            break;
        case MessageType::Success:
            std::cout << "[成功] ";
            break;
        default:
            break;
    }
    
    std::cout << message << std::endl;
    
    if (showBorder) 
    {
        std::cout << "========================================" << std::endl;
    }
}

// スキル発動（詠唱時間とMP消費のデフォルト値）
struct SkillEffect 
{
    std::string name;
    int damage;
    int mpCost;
    double castTime;
};

SkillEffect useSkill(std::string skillName, int baseDamage = 50, int mpCost = 10, double castTime = 1.0) 
{
    SkillEffect effect;
    effect.name = skillName;
    effect.damage = baseDamage;
    effect.mpCost = mpCost;
    effect.castTime = castTime;
    
    std::cout << "スキル発動: " << skillName << std::endl;
    std::cout << "  威力: " << baseDamage << ", MP消費: " << mpCost 
              << ", 詠唱時間: " << castTime << "秒" << std::endl;
    
    return effect;
}

// セーブデータの作成
void saveGame(int slot, std::string playerName, int chapter = 1, 
              std::string location = "始まりの村", bool autoSave = false) 
{
    std::cout << "\n--- セーブデータ " << slot << " ---" << std::endl;
    std::cout << "プレイヤー: " << playerName << std::endl;
    std::cout << "チャプター: " << chapter << std::endl;
    std::cout << "現在地: " << location << std::endl;
    
    if (autoSave) 
    {
        std::cout << "[オートセーブ]" << std::endl;
    }
    else 
    {
        std::cout << "[手動セーブ]" << std::endl;
    }
    
    std::cout << "セーブ完了！" << std::endl;
}

// 戦闘開始（難易度設定）
enum class Difficulty 
{
    Easy,
    Normal,
    Hard
};

void startBattle(std::string enemyName, int enemyLevel = 1, 
                Difficulty difficulty = Difficulty::Normal, bool isBoss = false) 
{
    std::cout << "\n戦闘開始！" << std::endl;
    
    if (isBoss) 
    {
        std::cout << "【ボス戦】 ";
    }
    
    std::cout << enemyName << " (Lv." << enemyLevel << ") が現れた！" << std::endl;
    
    // 難易度による敵の強化
    double statMultiplier = 1.0;
    switch (difficulty) 
    {
        case Difficulty::Easy:
            statMultiplier = 0.7;
            std::cout << "難易度: イージー" << std::endl;
            break;
        case Difficulty::Hard:
            statMultiplier = 1.5;
            std::cout << "難易度: ハード" << std::endl;
            break;
        default:
            std::cout << "難易度: ノーマル" << std::endl;
    }
    
    int enemyHp = 100 * enemyLevel * statMultiplier;
    std::cout << "敵のHP: " << enemyHp << std::endl;
}

int main() 
{
    std::cout << "=== デフォルト引数の基本使用例 ===" << std::endl;
    
    // 全ての引数を指定
    createCharacter("勇者", 150, 80, 5);
    
    // 一部の引数を省略（デフォルト値が使われる）
    createCharacter("魔法使い", 80, 120);  // レベルは1（デフォルト）
    createCharacter("戦士", 200);          // MPは50、レベルは1（デフォルト）
    createCharacter("盗賊");               // HP100、MP50、レベル1（全てデフォルト）
    
    std::cout << "\n=== ゲーム開発での実践例 ===" << std::endl;
    
    std::srand(std::time(nullptr));
    
    // ダメージ計算
    int damage1 = calculateDamage(100);                    // デフォルトのクリティカル率
    std::cout << "通常攻撃: " << damage1 << " ダメージ" << std::endl;
    
    int damage2 = calculateDamage(100, 0.3);              // クリティカル率30%
    std::cout << "必殺技: " << damage2 << " ダメージ" << std::endl;
    
    int damage3 = calculateDamage(100, 0.5, 2.0);         // クリティカル率50%、倍率2.0
    std::cout << "超必殺技: " << damage3 << " ダメージ" << std::endl;
    
    // アイテムドロップ
    std::cout << "\n敵を倒した！" << std::endl;
    dropItem("ポーション");                    // 50%の確率
    dropItem("レアソード", 0.1);              // 10%の確率
    dropItem("伝説の装備", 0.05, true);       // ボス撃破で10%（5%×2）
    
    // メッセージ表示
    std::cout << std::endl;
    displayMessage("ゲームを開始します");
    displayMessage("HPが残り少ないです！", MessageType::Warning);
    displayMessage("クエストクリア！", MessageType::Success, true);
    displayMessage("セーブデータが見つかりません", MessageType::Error);
    
    // スキル使用
    std::cout << std::endl;
    useSkill("ファイアボール");                        // 全てデフォルト
    useSkill("メテオ", 200, 50, 3.0);                 // 全て指定
    useSkill("ヒール", 0, 20);                        // ダメージ0、MP20、詠唱1秒
    
    // セーブ機能
    saveGame(1, "アレックス", 3, "魔王城前");
    saveGame(2, "ミア");                               // チャプター1、始まりの村
    saveGame(99, "オートセーブ", 2, "森の迷宮", true);
    
    // 戦闘開始
    startBattle("スライム");                          // 全てデフォルト
    startBattle("オーガ", 15);                       // レベル15、他はデフォルト
    startBattle("ドラゴン", 50, Difficulty::Hard);   // ハード難易度
    startBattle("魔王", 99, Difficulty::Hard, true);  // ボス戦
    
    return 0;
}

/*
    まとめ:
    - デフォルト引数を使うと、関数呼び出し時に一部の引数を省略できる
    - デフォルト値は関数宣言時に指定する（右側の引数から順に）
    - 同じ関数で異なる使い方ができるため、オーバーロードの必要が減る
    - よく使われる値をデフォルトにすることで、コードが簡潔になる
    
    ゲーム開発での活用：
    - キャラクター作成時の初期値設定
    - ダメージ計算のオプションパラメータ
    - メッセージ表示のスタイル設定
    - セーブ機能の詳細オプション
    - 難易度やゲームモードの設定
    
    注意点：
    - デフォルト引数は宣言時のみ指定（定義時には書かない）
    - 左側の引数にデフォルト値がある場合、右側全てにも必要
    - オーバーロードと組み合わせる場合は曖昧さに注意
*/