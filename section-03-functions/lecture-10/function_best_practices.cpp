// 第10講: 関数設計のベストプラクティス
// 読みやすく保守性の高い関数を書くための指針とテクニック

#include <iostream>
#include <string>
#include <vector>
#include <optional>

// === 悪い例と良い例の比較 ===

// 悪い例：長すぎる関数
// void badComplexFunction() { /* 100行以上の処理... */ }

// 良い例：責任を分割
bool isValidInput(int input) 
{
    return input >= 1 && input <= 100;
}

int calculateBonus(int score) 
{
    if (score >= 1000) return 500;
    if (score >= 500) return 200;
    return 50;
}

void displayResult(int score, int bonus) 
{
    std::cout << "スコア: " << score << ", ボーナス: " << bonus << std::endl;
}

// ゲームキャラクターの処理例
struct Character 
{
    std::string name;
    int hp;
    int maxHp;
    int mp;
    int maxMp;
    int level;
};

// 悪い例：副作用が多い
// void badHealFunction(Character& character) {
//     character.hp += 50;  // マジックナンバー
//     std::cout << "回復した！";  // 責任が混在
//     if (character.hp > character.maxHp) character.hp = character.maxHp;
// }

// 良い例：純粋関数的アプローチ
constexpr int POTION_HEAL_AMOUNT = 50;

int calculateHealedHp(int currentHp, int maxHp, int healAmount) 
{
    return std::min(currentHp + healAmount, maxHp);
}

bool healCharacter(Character& character, int healAmount = POTION_HEAL_AMOUNT) 
{
    if (character.hp >= character.maxHp) 
    {
        return false;  // 既に満タン
    }
    
    int newHp = calculateHealedHp(character.hp, character.maxHp, healAmount);
    character.hp = newHp;
    return true;
}

// エラーハンドリングの例
std::optional<Character> createCharacter(const std::string& name, int level) 
{
    if (name.empty()) 
    {
        return std::nullopt;  // エラー：名前が空
    }
    
    if (level < 1 || level > 99) 
    {
        return std::nullopt;  // エラー：無効なレベル
    }
    
    Character character;
    character.name = name;
    character.level = level;
    character.maxHp = 50 + (level - 1) * 10;
    character.hp = character.maxHp;
    character.maxMp = 20 + (level - 1) * 5;
    character.mp = character.maxMp;
    
    return character;
}

// RAII（Resource Acquisition Is Initialization）の例
class GameResource 
{
private:
    std::string resourceName;
    bool isLoaded;
    
public:
    explicit GameResource(const std::string& name) 
        : resourceName(name), isLoaded(false) 
    {
        // リソースの読み込み
        std::cout << resourceName << "を読み込み中..." << std::endl;
        isLoaded = true;
    }
    
    ~GameResource() 
    {
        if (isLoaded) 
        {
            std::cout << resourceName << "を解放しました" << std::endl;
        }
    }
    
    // コピー禁止
    GameResource(const GameResource&) = delete;
    GameResource& operator=(const GameResource&) = delete;
    
    bool isReady() const { return isLoaded; }
};

// const correctnessの例
class Player 
{
private:
    std::string name;
    int score;
    
public:
    Player(const std::string& playerName) : name(playerName), score(0) {}
    
    // const メンバ関数（状態を変更しない）
    const std::string& getName() const { return name; }
    int getScore() const { return score; }
    
    // 非const メンバ関数（状態を変更する）
    void addScore(int points) { score += points; }
    
    // const参照で効率的に情報を表示
    void displayInfo() const 
    {
        std::cout << "プレイヤー: " << name << ", スコア: " << score << std::endl;
    }
};

int main() 
{
    std::cout << "=== 関数設計のベストプラクティス ===" << std::endl;
    
    // 1. 単一責任の原則
    int userScore = 750;
    if (isValidInput(userScore)) 
    {
        int bonus = calculateBonus(userScore);
        displayResult(userScore, bonus);
    }
    
    // 2. 純粋関数の使用
    std::cout << "\n=== 回復処理の例 ===" << std::endl;
    Character hero = {"勇者", 80, 150, 40, 100, 10};
    
    std::cout << "回復前のHP: " << hero.hp << "/" << hero.maxHp << std::endl;
    
    if (healCharacter(hero)) 
    {
        std::cout << "回復しました！" << std::endl;
    }
    
    std::cout << "回復後のHP: " << hero.hp << "/" << hero.maxHp << std::endl;
    
    // 3. エラーハンドリング
    std::cout << "\n=== キャラクター作成 ===" << std::endl;
    
    auto validCharacter = createCharacter("魔法使い", 5);
    if (validCharacter) 
    {
        std::cout << "キャラクター作成成功: " << validCharacter->name << std::endl;
    }
    
    auto invalidCharacter = createCharacter("", 150);  // 無効な入力
    if (!invalidCharacter) 
    {
        std::cout << "キャラクター作成失敗: 無効な入力" << std::endl;
    }
    
    // 4. RAII
    std::cout << "\n=== リソース管理 ===" << std::endl;
    {
        GameResource texture("player_texture.png");
        GameResource sound("battle_music.wav");
        
        if (texture.isReady() && sound.isReady()) 
        {
            std::cout << "ゲーム開始準備完了" << std::endl;
        }
        
        // スコープを抜ける時に自動的にリソースが解放される
    }
    std::cout << "リソースが自動的に解放されました" << std::endl;
    
    // 5. const correctness
    std::cout << "\n=== const correctness ===" << std::endl;
    const Player player("テストプレイヤー");  // constオブジェクト
    
    // constメンバ関数のみ呼び出し可能
    player.displayInfo();
    std::cout << "プレイヤー名: " << player.getName() << std::endl;
    
    // player.addScore(100);  // エラー！constオブジェクトは変更できない
    
    return 0;
}

/*
    まとめ - 関数設計のベストプラクティス:
    
    1. 単一責任の原則
       - 一つの関数は一つの責任のみを持つ
       - 関数名から処理内容が明確に分かる
    
    2. 純粋関数を心がける
       - 副作用を最小限に抑える
       - 同じ入力に対して常に同じ出力
       - テストしやすく、バグが少ない
    
    3. 適切なエラーハンドリング
       - std::optionalやexceptionを活用
       - 失敗する可能性を明示
    
    4. const correctness
       - 変更しない引数はconst参照
       - 変更しないメンバ関数はconst
    
    5. RAII（Resource Acquisition Is Initialization）
       - リソースの取得と解放を自動化
       - メモリリークを防ぐ
    
    6. 適切な命名
       - 関数名で何をするかが分かる
       - 引数名で何を渡すかが分かる
    
    7. 関数のサイズ
       - 画面に収まる程度（20-30行程度）
       - 複雑になったら分割を検討
*/