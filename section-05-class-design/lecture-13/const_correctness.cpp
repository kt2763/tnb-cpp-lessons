// Lecture 13: const の正しい使い方
// Const Correctness in C++

#include <iostream>
#include <string>
#include <vector>
#include <memory>

// const の基本：ゲーム内のアイテムクラス
class GameItem 
{
private:
    std::string name;
    int value;
    bool isEquipped;
    mutable int accessCount;  // mutable: const関数内でも変更可能
    
public:
    // コンストラクタ
    GameItem(const std::string& itemName, int itemValue) 
        : name(itemName), value(itemValue), isEquipped(false), accessCount(0) 
    {
        std::cout << "アイテム '" << name << "' (価値: " << value << ") を作成\n";
    }
    
    // const メンバー関数：オブジェクトを変更しない
    std::string getName() const 
    {
        accessCount++;  // mutableなので変更可能
        return name;
    }
    
    int getValue() const 
    {
        accessCount++;
        return value;
    }
    
    bool getEquipStatus() const 
    {
        accessCount++;
        return isEquipped;
    }
    
    int getAccessCount() const 
    {
        return accessCount;
    }
    
    // 非const メンバー関数：オブジェクトを変更する可能性
    void equip() 
    {
        if (!isEquipped) {
            isEquipped = true;
            std::cout << name << " を装備しました\n";
        } else {
            std::cout << name << " は既に装備されています\n";
        }
    }
    
    void unequip() 
    {
        if (isEquipped) {
            isEquipped = false;
            std::cout << name << " を外しました\n";
        } else {
            std::cout << name << " は装備されていません\n";
        }
    }
    
    // const版と非const版のオーバーロード
    const std::string& getNameRef() const 
    {
        std::cout << "[const版] getNameRef() が呼ばれました\n";
        return name;
    }
    
    std::string& getNameRef() 
    {
        std::cout << "[非const版] getNameRef() が呼ばれました\n";
        return name;
    }
    
    // const正しさの例：アイテム情報表示
    void showInfo() const 
    {
        std::cout << "=== アイテム情報 ===\n";
        std::cout << "名前: " << getName() << "\n";  // const関数を呼び出し
        std::cout << "価値: " << getValue() << "\n";
        std::cout << "装備状態: " << (getEquipStatus() ? "装備中" : "未装備") << "\n";
        std::cout << "アクセス回数: " << getAccessCount() << "\n";
    }
};

// const引数とconst戻り値の例
class PlayerInventory 
{
private:
    std::vector<GameItem> items;
    std::string playerName;
    
public:
    PlayerInventory(const std::string& name) : playerName(name) 
    {
        std::cout << "プレイヤー '" << playerName << "' のインベントリを作成\n";
    }
    
    // const参照で引数を受け取る（コピーを避ける）
    void addItem(const GameItem& item) 
    {
        items.push_back(item);
        std::cout << "'" << item.getName() << "' をインベントリに追加\n";
    }
    
    // const版とnonconst版のオーバーロード
    const GameItem& getItem(size_t index) const 
    {
        if (index < items.size()) {
            std::cout << "[const版] アイテム取得\n";
            return items[index];
        }
        throw std::out_of_range(\"インデックスが範囲外です\");
    }
    
    GameItem& getItem(size_t index) 
    {
        if (index < items.size()) {
            std::cout << "[非const版] アイテム取得\n";
            return items[index];
        }
        throw std::out_of_range(\"インデックスが範囲外です\");
    }
    
    // const関数：インベントリの状態を変更しない
    size_t getItemCount() const 
    {
        return items.size();
    }
    
    void showInventory() const 
    {
        std::cout << "\n=== " << playerName << " のインベントリ ===\n";
        std::cout << "アイテム数: " << getItemCount() << "\n";
        
        for (size_t i = 0; i < items.size(); ++i) {
            std::cout << "[" << i << "] ";
            items[i].showInfo();  // const関数を呼び出し
            std::cout << "---\n";
        }
    }
    
    // constポインタ・参照の例
    const GameItem* findItemByName(const std::string& name) const 
    {
        for (const auto& item : items) {
            if (item.getName() == name) {
                return &item;  // constポインタを返す
            }
        }
        return nullptr;
    }
    
    // 非const版
    GameItem* findItemByName(const std::string& name) 
    {
        for (auto& item : items) {
            if (item.getName() == name) {
                return &item;  // 非constポインタを返す
            }
        }
        return nullptr;
    }
};

// const関数内でのmutableの使用例
class GameStatistics 
{
private:
    int totalGames;
    int wins;
    int losses;
    mutable std::string cachedWinRate;  // キャッシュ用
    mutable bool cacheValid;            // キャッシュ有効フラグ
    
public:
    GameStatistics() : totalGames(0), wins(0), losses(0), cacheValid(false) {}
    
    void addWin() 
    {
        wins++;
        totalGames++;
        cacheValid = false;  // キャッシュを無効化
        std::cout << "勝利を記録しました\n";
    }
    
    void addLoss() 
    {
        losses++;
        totalGames++;
        cacheValid = false;  // キャッシュを無効化
        std::cout << "敗北を記録しました\n";
    }
    
    // const関数だが、mutableメンバーは変更可能
    std::string getWinRateString() const 
    {
        if (!cacheValid) {
            // 重い計算をシミュレート
            if (totalGames > 0) {
                double rate = (double)wins / totalGames * 100.0;
                cachedWinRate = std::to_string(rate) + "%";
            } else {
                cachedWinRate = "0%";
            }
            cacheValid = true;
            std::cout << "[計算] 勝率を計算してキャッシュに保存\n";
        } else {
            std::cout << "[キャッシュ] 保存済みの勝率を使用\n";
        }
        return cachedWinRate;
    }
    
    void showStats() const 
    {
        std::cout << "\n=== ゲーム統計 ===\n";
        std::cout << "総ゲーム数: " << totalGames << "\n";
        std::cout << "勝利: " << wins << ", 敗北: " << losses << "\n";
        std::cout << "勝率: " << getWinRateString() << "\n";
    }
};

// const correctnessの実践例：ゲーム設定
class GameConfig 
{
private:
    const int MAX_PLAYERS;     // const メンバー変数
    const std::string GAME_NAME;
    int currentPlayers;
    bool isRunning;
    
public:
    // const メンバー変数はコンストラクタ初期化リストで初期化
    GameConfig(int maxPlayers, const std::string& gameName)
        : MAX_PLAYERS(maxPlayers), GAME_NAME(gameName), 
          currentPlayers(0), isRunning(false) 
    {
        std::cout << "ゲーム設定を初期化: " << GAME_NAME 
                  << " (最大" << MAX_PLAYERS << "人)\n";
    }
    
    // const メンバー変数のアクセサー
    int getMaxPlayers() const { return MAX_PLAYERS; }
    const std::string& getGameName() const { return GAME_NAME; }
    
    // 通常のアクセサー
    int getCurrentPlayers() const { return currentPlayers; }
    bool getIsRunning() const { return isRunning; }
    
    // ゲーム状態変更
    bool addPlayer() 
    {
        if (currentPlayers < MAX_PLAYERS && !isRunning) {
            currentPlayers++;
            std::cout << "プレイヤーが参加 (" << currentPlayers << "/" << MAX_PLAYERS << ")\n";
            return true;
        }
        return false;
    }
    
    void startGame() 
    {
        if (currentPlayers > 0 && !isRunning) {
            isRunning = true;
            std::cout << GAME_NAME << " を開始しました\n";
        }
    }
    
    void showConfig() const 
    {
        std::cout << "\n=== ゲーム設定 ===\n";
        std::cout << "ゲーム名: " << GAME_NAME << " (変更不可)\n";
        std::cout << "最大プレイヤー数: " << MAX_PLAYERS << " (変更不可)\n";
        std::cout << "現在のプレイヤー数: " << currentPlayers << "\n";
        std::cout << "実行状態: " << (isRunning ? "実行中" : "停止中") << "\n";
    }
};

// const correctnessとポインタ
void demonstrateConstPointers() 
{
    std::cout << "\n=== constポインタのデモ ===\n";
    
    int value1 = 100;
    int value2 = 200;
    
    // 1. ポインタが指すデータがconst
    const int* ptr1 = &value1;
    std::cout << "const int* ptr1 = " << *ptr1 << "\n";
    // *ptr1 = 300;  // エラー：const データは変更不可
    ptr1 = &value2;  // OK：ポインタ自体は変更可能
    std::cout << "ptr1を変更後 = " << *ptr1 << "\n";
    
    // 2. ポインタ自体がconst
    int* const ptr2 = &value1;
    std::cout << "int* const ptr2 = " << *ptr2 << "\n";
    *ptr2 = 300;     // OK：データは変更可能
    // ptr2 = &value2;  // エラー：constポインタは変更不可
    std::cout << "ptr2のデータ変更後 = " << *ptr2 << "\n";
    
    // 3. 両方ともconst
    const int* const ptr3 = &value2;
    std::cout << "const int* const ptr3 = " << *ptr3 << "\n";
    // *ptr3 = 400;     // エラー：データ変更不可\n";
    // ptr3 = &value1;  // エラー：ポインタ変更不可\n";
}\n\n// const関数でのオーバーロード解決のデモ\nvoid demonstrateConstOverloads() \n{\n    std::cout << \"\\n=== constオーバーロードのデモ ===\\n\";\n    \n    GameItem item(\"ファイアソード\", 500);\n    const GameItem constItem(\"アイスシールド\", 300);\n    \n    // 非constオブジェクト → 非const版が呼ばれる\n    std::string& nameRef1 = item.getNameRef();\n    nameRef1 = \"フレイムソード\";  // 変更可能\n    \n    // constオブジェクト → const版が呼ばれる\n    const std::string& nameRef2 = constItem.getNameRef();\n    // nameRef2 = \"変更\";  // エラー：const参照なので変更不可\n    \n    std::cout << \"変更後のアイテム名: \" << item.getName() << \"\\n\";\n    std::cout << \"constアイテム名: \" << constItem.getName() << \"\\n\";\n}\n\n// constexprの例（C++11以降）\nclass MathUtils \n{\npublic:\n    // コンパイル時定数関数\n    static constexpr int square(int x) \n    {\n        return x * x;\n    }\n    \n    static constexpr double pi() \n    {\n        return 3.14159265359;\n    }\n    \n    // constexpr コンストラクタ\n    constexpr MathUtils() = default;\n};\n\nint main() \n{\n    std::cout << \"=== const の正しい使い方のデモ ===\\n\\n\";\n    \n    // 基本的なconst使用法\n    std::cout << \"1. 基本的なconst使用法\\n\";\n    GameItem sword(\"エクスカリバー\", 1000);\n    const GameItem shield(\"ドラゴンシールド\", 800);\n    \n    // const関数の呼び出し\n    sword.showInfo();\n    shield.showInfo();\n    \n    // 非const関数はconstオブジェクトで呼び出し不可\n    sword.equip();   // OK\n    // shield.equip();  // エラー：constオブジェクトで非const関数は呼び出せない\n    \n    std::cout << \"\\n---\\n\";\n    \n    // インベントリでのconst使用\n    std::cout << \"\\n2. インベントリでのconst使用\\n\";\n    PlayerInventory inventory(\"勇者\");\n    \n    inventory.addItem(GameItem(\"ポーション\", 50));\n    inventory.addItem(GameItem(\"魔法の杖\", 300));\n    \n    // const参照で安全にアクセス\n    const PlayerInventory& constInventory = inventory;\n    constInventory.showInventory();\n    \n    // 非constアクセスで変更\n    GameItem& item = inventory.getItem(0);\n    item.equip();\n    \n    std::cout << \"\\n---\\n\";\n    \n    // mutableの使用例\n    std::cout << \"\\n3. mutableの使用例\\n\";\n    GameStatistics stats;\n    stats.addWin();\n    stats.addWin();\n    stats.addLoss();\n    \n    // const関数内でmutableメンバーが変更される\n    stats.showStats();\n    stats.showStats();  // 2回目はキャッシュが使われる\n    \n    std::cout << \"\\n---\\n\";\n    \n    // const メンバー変数\n    std::cout << \"\\n4. const メンバー変数\\n\";\n    GameConfig config(4, \"チームバトル\");\n    config.addPlayer();\n    config.addPlayer();\n    config.startGame();\n    config.showConfig();\n    \n    std::cout << \"\\n---\\n\";\n    \n    // constポインタ\n    demonstrateConstPointers();\n    \n    std::cout << \"\\n---\\n\";\n    \n    // constオーバーロード\n    demonstrateConstOverloads();\n    \n    std::cout << \"\\n---\\n\";\n    \n    // constexprの使用\n    std::cout << \"\\n5. constexprの使用\\n\";\n    constexpr int result = MathUtils::square(5);  // コンパイル時計算\n    constexpr double piValue = MathUtils::pi();\n    \n    std::cout << \"square(5) = \" << result << \" (コンパイル時計算)\\n\";\n    std::cout << \"pi = \" << piValue << \" (コンパイル時定数)\\n\";\n    \n    return 0;\n}\n\n/*\n学習ポイント：\n\nconst correctnessの重要性：\n1. バグの予防：意図しない変更を防ぐ\n2. インターフェースの明確化：関数の意図を明示\n3. 最適化の促進：コンパイラがより良いコードを生成\n4. 設計の改善：不変性を明確にする\n\nconstの種類：\n1. const変数：値を変更できない\n2. constメンバー関数：オブジェクトの状態を変更しない\n3. constメンバー変数：オブジェクトの生存期間中変更不可\n4. const引数：関数内で引数を変更しない\n5. const戻り値：戻り値を変更できない\n\nポインタとconst：\n1. const T* : 指すデータがconst\n2. T* const : ポインタ自体がconst\n3. const T* const : 両方ともconst\n\nbestプラクティス：\n1. できるだけconstを使用する\n2. constメンバー関数を適切に設計する\n3. const参照で引数を渡す\n4. mutableは慎重に使用する\n5. constexprでコンパイル時計算を活用\n\n注意点：\n1. const_castによるconst外しは危険\n2. mutableの乱用は避ける\n3. constメンバー変数は初期化リストで初期化\n4. constオーバーロードは適切に設計する\n\nconstを正しく使うことで、より安全で保守性の高いコードが書ける\n*/