// Lecture 8: 仮想デストラクタ
// Virtual Destructors in C++

#include <iostream>
#include <string>
#include <vector>
#include <memory>

// 基底クラス：仮想デストラクタがない場合の問題点
class BadBaseClass 
{
protected:
    std::string name;
    
public:
    BadBaseClass(const std::string& n) : name(n) 
    {
        std::cout << "BadBaseClass " << name << " が作成されました\n";
    }
    
    // 非仮想デストラクタ（問題あり）
    ~BadBaseClass() 
    {
        std::cout << "BadBaseClass " << name << " が破棄されました\n";
    }
    
    virtual void action() 
    {
        std::cout << name << " が基本アクションを実行\n";
    }
};

// 派生クラス：リソースを持つ
class BadDerivedClass : public BadBaseClass 
{
private:
    int* dynamicData;  // 動的に割り当てられたメモリ
    
public:
    BadDerivedClass(const std::string& n) : BadBaseClass(n) 
    {
        dynamicData = new int[100];
        std::cout << "BadDerivedClass " << name << " が作成され、メモリを割り当てました\n";
    }
    
    // このデストラクタが呼ばれない場合がある！
    ~BadDerivedClass() 
    {
        delete[] dynamicData;
        std::cout << "BadDerivedClass " << name << " が破棄され、メモリを解放しました\n";
    }
    
    void action() override 
    {
        std::cout << name << " が特別なアクションを実行\n";
    }
};

// 正しい基底クラス：仮想デストラクタあり
class GameEntity 
{
protected:
    std::string name;
    int entityId;
    
public:
    GameEntity(const std::string& n, int id) : name(n), entityId(id) 
    {
        std::cout << "GameEntity " << name << " (ID: " << entityId << ") が作成されました\n";
    }
    
    // 仮想デストラクタ（重要！）
    virtual ~GameEntity() 
    {
        std::cout << "GameEntity " << name << " が破棄されました\n";
    }
    
    virtual void update() = 0;
    virtual void render() const = 0;
    
    std::string getName() const { return name; }
    int getId() const { return entityId; }
};

// プレイヤークラス：複雑なリソース管理
class Player : public GameEntity 
{
private:
    int* stats;        // 動的配列：ステータス
    std::string* inventory;  // 動的配列：インベントリ
    int inventorySize;
    
public:
    Player(const std::string& name, int id) 
        : GameEntity(name, id), inventorySize(50) 
    {
        stats = new int[6]{100, 20, 15, 10, 5, 1};  // HP, MP, ATK, DEF, SPD, LV
        inventory = new std::string[inventorySize];
        
        // 初期アイテム
        inventory[0] = "木の剣";
        inventory[1] = "布の服";
        inventory[2] = "体力ポーション";
        
        std::cout << "Player " << name << " のリソースを初期化しました\n";
    }
    
    // 仮想デストラクタで適切にクリーンアップ
    ~Player() override 
    {
        delete[] stats;
        delete[] inventory;
        std::cout << "Player " << name << " のリソースを解放しました\n";
    }
    
    void update() override 
    {
        std::cout << "Player " << name << " を更新中 (HP: " << stats[0] << ")\n";
    }
    
    void render() const override 
    {
        std::cout << "Player " << name << " をレベル" << stats[5] << "で描画\n";
    }
    
    void addItem(const std::string& item) 
    {
        for (int i = 0; i < inventorySize; ++i) {
            if (inventory[i].empty()) {
                inventory[i] = item;
                std::cout << item << " をインベントリに追加しました\n";
                break;
            }
        }
    }
};

// 敵クラス：AI用のメモリ管理
class Enemy : public GameEntity 
{
private:
    int* aiStates;     // AI状態管理用
    int stateCount;
    
public:
    Enemy(const std::string& name, int id, int states) 
        : GameEntity(name, id), stateCount(states) 
    {
        aiStates = new int[stateCount]();
        std::cout << "Enemy " << name << " のAI状態を初期化しました\n";
    }
    
    ~Enemy() override 
    {
        delete[] aiStates;
        std::cout << "Enemy " << name << " のAI状態を解放しました\n";
    }
    
    void update() override 
    {
        std::cout << "Enemy " << name << " のAIを更新中\n";
    }
    
    void render() const override 
    {
        std::cout << "Enemy " << name << " を描画\n";
    }
};

// 特殊なNPCクラス：複数のリソースタイプ
class SpecialNPC : public GameEntity 
{
private:
    std::vector<std::string>* dialogues;  // 会話データ
    int* questData;                       // クエストデータ
    
public:
    SpecialNPC(const std::string& name, int id) : GameEntity(name, id) 
    {
        dialogues = new std::vector<std::string>{
            "こんにちは、冒険者さん！",
            "何かお手伝いできることはありますか？",
            "またいつでも声をかけてくださいね。"
        };
        
        questData = new int[10]();  // クエスト進行状況
        
        std::cout << "SpecialNPC " << name << " の会話システムを初期化しました\n";
    }
    
    ~SpecialNPC() override 
    {
        delete dialogues;
        delete[] questData;
        std::cout << "SpecialNPC " << name << " の会話システムを解放しました\n";
    }
    
    void update() override 
    {
        std::cout << "SpecialNPC " << name << " のクエスト状態を更新中\n";
    }
    
    void render() const override 
    {
        std::cout << "SpecialNPC " << name << " を描画（会話可能）\n";
    }
    
    void talk() 
    {
        if (!dialogues->empty()) {
            std::cout << name << ": \"" << (*dialogues)[0] << "\"\n";
        }
    }
};

// 問題のあるケースをデモンストレーション
void demonstrateProblematicCase() 
{
    std::cout << "\n=== 問題のあるケース（非仮想デストラクタ）===\n";
    
    // 基底クラスポインタで派生オブジェクトを削除
    BadBaseClass* badPtr = new BadDerivedClass("問題のあるオブジェクト");
    delete badPtr;  // BadDerivedClassのデストラクタが呼ばれない！
    
    std::cout << "↑ メモリリークが発生している可能性があります\n";
}

// 正しいケースをデモンストレーション
void demonstrateCorrectCase() 
{
    std::cout << "\n=== 正しいケース（仮想デストラクタ）===\n";
    
    // ゲームエンティティの配列
    std::vector<std::unique_ptr<GameEntity>> entities;
    
    entities.push_back(std::make_unique<Player>("勇者", 1));
    entities.push_back(std::make_unique<Enemy>("スライム", 2, 5));
    entities.push_back(std::make_unique<SpecialNPC>("村長", 3));
    
    // 全エンティティを更新・描画
    for (auto& entity : entities) {
        entity->update();
        entity->render();
        std::cout << "---\n";
    }
    
    std::cout << "\n全エンティティを破棄中...\n";
    // unique_ptrが自動的に適切なデストラクタを呼び出す
    entities.clear();
}

// RAII（Resource Acquisition Is Initialization）の例
class GameResource 
{
private:
    std::string resourceName;
    int* data;
    size_t size;
    
public:
    GameResource(const std::string& name, size_t s) 
        : resourceName(name), size(s) 
    {
        data = new int[size];
        std::cout << "Resource " << resourceName << " (" << size << "要素) を取得\n";
    }
    
    // 仮想デストラクタでRAII確保
    virtual ~GameResource() 
    {
        delete[] data;
        std::cout << "Resource " << resourceName << " を解放\n";
    }
    
    // コピー禁止（Rule of 3/5/0）
    GameResource(const GameResource&) = delete;
    GameResource& operator=(const GameResource&) = delete;
    
    // ムーブコンストラクタ・代入演算子
    GameResource(GameResource&& other) noexcept
        : resourceName(std::move(other.resourceName)), 
          data(other.data), size(other.size) 
    {
        other.data = nullptr;
        other.size = 0;
    }
    
    GameResource& operator=(GameResource&& other) noexcept 
    {
        if (this != &other) {
            delete[] data;
            resourceName = std::move(other.resourceName);
            data = other.data;
            size = other.size;
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }
};

int main() 
{
    std::cout << "=== 仮想デストラクタのデモ ===\n";
    
    // 問題のあるケースを実演
    demonstrateProblematicCase();
    
    // 正しいケースを実演
    demonstrateCorrectCase();
    
    // RAII の例
    std::cout << "\n=== RAII の例 ===\n";
    {
        GameResource resource1("テクスチャデータ", 1000);
        GameResource resource2("サウンドデータ", 2000);
        
        std::cout << "スコープ内でリソースを使用中...\n";
    } // ここで自動的にデストラクタが呼ばれる
    
    std::cout << "\nスコープを抜けました - リソースは自動解放されました\n";
    
    return 0;
}

/*
学習ポイント：
1. 基底クラスのデストラクタは必ず仮想関数にする
2. 非仮想デストラクタだと派生クラスのデストラクタが呼ばれない
3. メモリリークやリソースリークの原因になる
4. ポリモーフィズムを使う場合は仮想デストラクタが必須
5. RAII パターンでリソース管理を確実にする
6. Rule of 3/5/0 に従ってリソース管理クラスを作成
7. スマートポインタ（unique_ptr, shared_ptr）も仮想デストラクタと組み合わせて使う
*/