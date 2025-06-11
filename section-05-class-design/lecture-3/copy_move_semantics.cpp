// 第3講: コピーとムーブセマンティクス
// オブジェクトのコピー、ムーブの仕組みと効率的なリソース管理を学ぶ

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>

// === 基本的なコピーコンストラクタとコピー代入演算子 ===
class GameItem 
{
private:
    std::string name;
    int value;
    int* durability;  // 動的メモリ（ディープコピーが必要）

public:
    // 通常のコンストラクタ
    GameItem(const std::string& itemName, int itemValue, int dur = 100) 
        : name(itemName), value(itemValue), durability(new int(dur))
    {
        std::cout << "GameItem「" << name << "」が作成されました (アドレス: " << this << ")" << std::endl;
    }
    
    // === コピーコンストラクタ ===
    GameItem(const GameItem& other) 
        : name(other.name), value(other.value), durability(new int(*other.durability))
    {
        std::cout << "GameItem「" << name << "」がコピーされました (元: " << &other << " → 新: " << this << ")" << std::endl;
    }
    
    // === コピー代入演算子 ===
    GameItem& operator=(const GameItem& other) 
    {
        std::cout << "GameItem「" << name << "」にコピー代入します (元: " << &other << " → 先: " << this << ")" << std::endl;
        
        if (this != &other) 
        {  // 自己代入のチェック
            name = other.name;
            value = other.value;
            
            // 既存のリソースを解放
            delete durability;
            
            // 新しいリソースを確保してコピー
            durability = new int(*other.durability);
        }
        
        return *this;
    }
    
    // === ムーブコンストラクタ（C++11） ===
    GameItem(GameItem&& other) noexcept 
        : name(std::move(other.name)), value(other.value), durability(other.durability)
    {
        std::cout << "GameItem「" << name << "」がムーブされました (元: " << &other << " → 新: " << this << ")" << std::endl;
        
        // 元のオブジェクトのリソースを無効化
        other.durability = nullptr;
        other.value = 0;
    }
    
    // === ムーブ代入演算子（C++11） ===
    GameItem& operator=(GameItem&& other) noexcept 
    {
        std::cout << "GameItem「" << other.name << "」をムーブ代入します (元: " << &other << " → 先: " << this << ")" << std::endl;
        
        if (this != &other) 
        {
            // 既存のリソースを解放
            delete durability;
            
            // リソースを移動
            name = std::move(other.name);
            value = other.value;
            durability = other.durability;
            
            // 元のオブジェクトのリソースを無効化
            other.durability = nullptr;
            other.value = 0;
        }
        
        return *this;
    }
    
    // デストラクタ
    ~GameItem() 
    {
        if (durability) 
        {
            std::cout << "GameItem「" << name << "」が削除されました (アドレス: " << this << ")" << std::endl;
            delete durability;
        }
        else 
        {
            std::cout << "GameItem（ムーブ済み）が削除されました (アドレス: " << this << ")" << std::endl;
        }
    }
    
    // ゲッター
    const std::string& getName() const { return name; }
    int getValue() const { return value; }
    int getDurability() const { return durability ? *durability : 0; }
    
    void displayInfo() const 
    {
        std::cout << "アイテム: " << name << ", 価値: " << value 
                  << ", 耐久度: " << (durability ? *durability : 0) << std::endl;
    }
    
    bool isValid() const 
    {
        return durability != nullptr;
    }
};

// === プレイヤークラス（値渡し vs 参照渡しの比較） ===
class Player 
{
private:
    std::string name;
    std::vector<GameItem> inventory;

public:
    Player(const std::string& playerName) : name(playerName)
    {
        std::cout << "プレイヤー「" << name << "」が作成されました" << std::endl;
    }
    
    // 値渡し（非効率 - コピーが発生）
    void addItemByValue(GameItem item) 
    {
        std::cout << "\n[値渡し] アイテムを追加中..." << std::endl;
        inventory.push_back(item);
    }
    
    // const参照渡し（効率的 - コピーなし）
    void addItemByReference(const GameItem& item) 
    {
        std::cout << "\n[参照渡し] アイテムを追加中..." << std::endl;
        inventory.push_back(item);
    }
    
    // ムーブ渡し（最も効率的 - リソース移動）
    void addItemByMove(GameItem&& item) 
    {
        std::cout << "\n[ムーブ渡し] アイテムを追加中..." << std::endl;
        inventory.push_back(std::move(item));
    }
    
    void displayInventory() const 
    {
        std::cout << "\n=== " << name << "のインベントリ ===" << std::endl;
        for (size_t i = 0; i < inventory.size(); ++i) 
        {
            std::cout << i + 1 << ". ";
            inventory[i].displayInfo();
        }
        std::cout << "===============================" << std::endl;
    }
    
    size_t getInventorySize() const 
    {
        return inventory.size();
    }
};

// === リソース管理クラス（RAII + ムーブセマンティクス） ===
class GameTexture 
{
private:
    std::string filename;
    int* textureData;  // 仮の画像データ
    size_t dataSize;

public:
    GameTexture(const std::string& file, size_t size = 1024) 
        : filename(file), dataSize(size)
    {
        textureData = new int[dataSize];
        std::cout << "テクスチャ「" << filename << "」を読み込みました (" 
                  << dataSize << " bytes)" << std::endl;
    }
    
    // コピーコンストラクタ（重い処理）
    GameTexture(const GameTexture& other) 
        : filename(other.filename + "_copy"), dataSize(other.dataSize)
    {
        std::cout << "テクスチャ「" << other.filename << "」をコピー中..." << std::endl;
        textureData = new int[dataSize];
        
        // データをコピー（重い処理をシミュレート）
        for (size_t i = 0; i < dataSize; ++i) 
        {
            textureData[i] = other.textureData[i];
        }
        
        std::cout << "テクスチャコピー完了 → 「" << filename << "」" << std::endl;
    }
    
    // ムーブコンストラクタ（高速）
    GameTexture(GameTexture&& other) noexcept 
        : filename(std::move(other.filename)), textureData(other.textureData), dataSize(other.dataSize)
    {
        std::cout << "テクスチャ「" << filename << "」をムーブしました（高速）" << std::endl;
        
        other.textureData = nullptr;
        other.dataSize = 0;
    }
    
    // コピー代入演算子
    GameTexture& operator=(const GameTexture& other) 
    {
        std::cout << "テクスチャ「" << other.filename << "」をコピー代入中..." << std::endl;
        
        if (this != &other) 
        {
            delete[] textureData;
            
            filename = other.filename + "_assigned";
            dataSize = other.dataSize;
            textureData = new int[dataSize];
            
            for (size_t i = 0; i < dataSize; ++i) 
            {
                textureData[i] = other.textureData[i];
            }
        }
        
        return *this;
    }
    
    // ムーブ代入演算子
    GameTexture& operator=(GameTexture&& other) noexcept 
    {
        std::cout << "テクスチャ「" << other.filename << "」をムーブ代入しました" << std::endl;
        
        if (this != &other) 
        {
            delete[] textureData;
            
            filename = std::move(other.filename);
            textureData = other.textureData;
            dataSize = other.dataSize;
            
            other.textureData = nullptr;
            other.dataSize = 0;
        }
        
        return *this;
    }
    
    ~GameTexture() 
    {
        if (textureData) 
        {
            std::cout << "テクスチャ「" << filename << "」を解放しました" << std::endl;
            delete[] textureData;
        }
        else 
        {
            std::cout << "テクスチャ（ムーブ済み）を解放しました" << std::endl;
        }
    }
    
    const std::string& getFilename() const { return filename; }
    size_t getDataSize() const { return dataSize; }
    bool isValid() const { return textureData != nullptr; }
    
    void use() const 
    {
        if (isValid()) 
        {
            std::cout << "テクスチャ「" << filename << "」を使用中..." << std::endl;
        }
        else 
        {
            std::cout << "無効なテクスチャです" << std::endl;
        }
    }
};

// === ファクトリ関数（RVO - Return Value Optimization）===
GameItem createRandomItem() 
{
    static int counter = 0;
    counter++;
    
    std::cout << "\n[ファクトリ] アイテム作成中..." << std::endl;
    return GameItem("ランダムアイテム" + std::to_string(counter), counter * 10, 50 + counter * 5);
}

std::vector<GameTexture> loadTextures() 
{
    std::cout << "\n[ファクトリ] テクスチャ群を読み込み中..." << std::endl;
    
    std::vector<GameTexture> textures;
    textures.emplace_back("player.png", 512);    // emplace_back でムーブ構築
    textures.emplace_back("enemy.png", 256);
    textures.emplace_back("background.png", 2048);
    
    return textures;  // RVO が適用される
}

void demonstrateCopyVsMove() 
{
    std::cout << "\n=== コピー vs ムーブの比較 ===" << std::endl;
    
    Player player("テストプレイヤー");
    
    // 1. 値渡し（コピーが発生）
    {
        std::cout << "\n--- 1. 値渡しテスト ---" << std::endl;
        GameItem item("テストソード", 100, 80);
        player.addItemByValue(item);  // コピーコンストラクタが呼ばれる
        std::cout << "元のアイテムの状態:";
        item.displayInfo();  // 元のアイテムはそのまま
    }
    
    // 2. 参照渡し（コピーなし）
    {
        std::cout << "\n--- 2. 参照渡しテスト ---" << std::endl;
        GameItem item("テストシールド", 80, 90);
        player.addItemByReference(item);  // vectorでのpush_backでコピーが発生
        std::cout << "元のアイテムの状態:";
        item.displayInfo();
    }
    
    // 3. ムーブ渡し（最も効率的）
    {
        std::cout << "\n--- 3. ムーブ渡しテスト ---" << std::endl;
        GameItem item("テストマジックロッド", 150, 60);
        player.addItemByMove(std::move(item));  // ムーブコンストラクタが呼ばれる
        std::cout << "元のアイテムの状態（ムーブ後）:";
        item.displayInfo();  // ムーブ後は無効状態
    }
    
    // 4. RVO（Return Value Optimization）
    {
        std::cout << "\n--- 4. RVOテスト ---" << std::endl;
        GameItem item = createRandomItem();  // RVOでコピー/ムーブが最適化される可能性
        player.addItemByMove(std::move(item));
    }
    
    player.displayInventory();
}

void demonstrateTextureManagement() 
{
    std::cout << "\n=== テクスチャ管理での効率化 ===" << std::endl;
    
    // 1. コピーによる非効率な方法
    {
        std::cout << "\n--- コピーでの処理 ---" << std::endl;
        GameTexture original("original.png", 1024);
        GameTexture copy = original;  // 重いコピー処理
        copy.use();
    }
    
    // 2. ムーブによる効率的な方法
    {
        std::cout << "\n--- ムーブでの処理 ---" << std::endl;
        GameTexture original("moveable.png", 1024);
        GameTexture moved = std::move(original);  // 高速なムーブ
        moved.use();
        
        std::cout << "元のテクスチャの状態: ";
        if (!original.isValid()) 
        {
            std::cout << "無効（ムーブ済み）" << std::endl;
        }
    }
    
    // 3. vectorでの効率的な管理
    {
        std::cout << "\n--- vectorでの効率的な管理 ---" << std::endl;
        std::vector<GameTexture> textures = loadTextures();  // RVO
        
        std::cout << "読み込まれたテクスチャ:" << std::endl;
        for (const auto& texture : textures) 
        {
            std::cout << "- " << texture.getFilename() << " (" << texture.getDataSize() << " bytes)" << std::endl;
        }
    }
}

int main() 
{
    std::cout << "=== コピーとムーブセマンティクス ===" << std::endl;
    
    // 基本的なコピー・ムーブの動作確認
    demonstrateCopyVsMove();
    
    // リソース管理でのコピー・ムーブの重要性
    demonstrateTextureManagement();
    
    std::cout << "\n=== perfect forwarding の簡単な例 ===" << std::endl;
    
    // emplace_back vs push_back
    {
        std::vector<GameItem> items;
        
        std::cout << "\npush_back使用:" << std::endl;
        items.push_back(GameItem("PushBackItem", 50));  // 一時オブジェクト作成 → ムーブ
        
        std::cout << "\nemplace_back使用:" << std::endl;
        items.emplace_back("EmplaceBackItem", 50);      // 直接構築（より効率的）
    }
    
    return 0;
}

/*
    まとめ:
    
    1. コピーセマンティクス:
       - コピーコンストラクタ: T(const T& other)
       - コピー代入演算子: T& operator=(const T& other)
       - ディープコピーで独立したオブジェクトを作成
       - リソースの複製が発生（重い処理）
    
    2. ムーブセマンティクス（C++11）:
       - ムーブコンストラクタ: T(T&& other) noexcept
       - ムーブ代入演算子: T& operator=(T&& other) noexcept
       - リソースの所有権を移動（軽い処理）
       - 一時オブジェクトや不要になったオブジェクトから効率的に移動
    
    3. RVO（Return Value Optimization）:
       - コンパイラによる最適化
       - 戻り値でのコピー/ムーブを省略
       - 関数から直接構築される
    
    4. Perfect Forwarding:
       - std::forward を使用
       - テンプレート関数での効率的な引数転送
       - emplace系関数での活用
    
    5. ベストプラクティス:
       - 大きなオブジェクトはムーブを活用
       - std::move で明示的にムーブ
       - emplace系関数を使用
       - noexcept 指定でパフォーマンス向上
       - 自己代入のチェック
    
    ゲーム開発での活用:
    - 大きなリソース（テクスチャ、音声）の効率的な管理
    - ゲームオブジェクトの生成・移動
    - STLコンテナでの効率的な要素追加
    - 一時的なエフェクトオブジェクトの処理
    - レベルデータの読み込み・切り替え
*/