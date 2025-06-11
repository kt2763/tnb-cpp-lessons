// 例外安全性とRAII
// 例外が発生してもリソースリークを防ぎ、プログラムの一貫性を保つ

#include <iostream>
#include <memory>
#include <vector>
#include <fstream>
#include <mutex>
#include <exception>

// ゲームリソースの管理
class TextureResource
{
private:
    std::string name;
    size_t size;
    bool loaded;
    
public:
    explicit TextureResource(const std::string& textureName)
        : name(textureName), size(1024 * 1024), loaded(false)
    {
        std::cout << "テクスチャ '" << name << "' を読み込み中..." << std::endl;
        // テクスチャの読み込みシミュレーション
        if (textureName == "corrupted.png")
        {
            throw std::runtime_error("テクスチャファイルが破損しています");
        }
        loaded = true;
        std::cout << "テクスチャ '" << name << "' 読み込み完了" << std::endl;
    }
    
    ~TextureResource()
    {
        if (loaded)
        {
            std::cout << "テクスチャ '" << name << "' を解放" << std::endl;
        }
    }
    
    size_t getSize() const { return size; }
    bool isLoaded() const { return loaded; }
};

// RAIIを使用したリソース管理
class GameSaveFile
{
private:
    std::string filename;
    std::ofstream file;
    std::mutex fileMutex;
    
public:
    explicit GameSaveFile(const std::string& name)
        : filename(name)
    {
        file.open(filename);
        if (!file.is_open())
        {
            throw std::runtime_error("セーブファイルを開けません: " + filename);
        }
        std::cout << "セーブファイル '" << filename << "' を開きました" << std::endl;
    }
    
    ~GameSaveFile()
    {
        if (file.is_open())
        {
            file.close();
            std::cout << "セーブファイル '" << filename << "' を閉じました" << std::endl;
        }
    }
    
    // コピーを禁止
    GameSaveFile(const GameSaveFile&) = delete;
    GameSaveFile& operator=(const GameSaveFile&) = delete;
    
    // ムーブを許可
    GameSaveFile(GameSaveFile&&) = default;
    GameSaveFile& operator=(GameSaveFile&&) = default;
    
    void write(const std::string& data)
    {
        std::lock_guard<std::mutex> lock(fileMutex);
        if (!file.is_open())
        {
            throw std::runtime_error("ファイルが開いていません");
        }
        file << data << std::endl;
    }
};

// 例外安全性のレベル
class PlayerInventory
{
private:
    std::vector<std::string> items;
    int gold;
    int maxItems;
    
public:
    PlayerInventory(int maxSize = 20) 
        : gold(100), maxItems(maxSize)
    {
        items.reserve(maxItems);
    }
    
    // 基本保証：例外が発生しても有効な状態を保つ
    void addItem(const std::string& item)
    {
        if (items.size() >= maxItems)
        {
            throw std::runtime_error("インベントリが満杯です");
        }
        
        // push_backは例外を投げる可能性がある
        items.push_back(item);
        std::cout << item << " を入手しました" << std::endl;
    }
    
    // 強い保証：例外が発生したら元の状態に戻る
    void buyItem(const std::string& item, int price)
    {
        if (gold < price)
        {
            throw std::runtime_error("所持金が不足しています");
        }
        
        // 一時的な状態を作成
        int tempGold = gold - price;
        std::vector<std::string> tempItems = items;
        
        try
        {
            tempItems.push_back(item);
            // すべて成功したら状態を更新
            gold = tempGold;
            items = std::move(tempItems);
            std::cout << item << " を " << price << " ゴールドで購入しました" << std::endl;
        }
        catch (...)
        {
            // 例外が発生した場合、元の状態のまま
            throw;
        }
    }
    
    // nothrow保証：例外を投げない
    int getGold() const noexcept
    {
        return gold;
    }
    
    size_t getItemCount() const noexcept
    {
        return items.size();
    }
    
    void displayInventory() const
    {
        std::cout << "\n=== インベントリ ===" << std::endl;
        std::cout << "所持金: " << gold << " ゴールド" << std::endl;
        std::cout << "アイテム数: " << items.size() << "/" << maxItems << std::endl;
        for (const auto& item : items)
        {
            std::cout << "- " << item << std::endl;
        }
    }
};

// スマートポインタを使った例外安全なリソース管理
class GameWorld
{
private:
    std::vector<std::unique_ptr<TextureResource>> textures;
    std::string worldName;
    
public:
    explicit GameWorld(const std::string& name) : worldName(name)
    {
        std::cout << "ゲームワールド '" << worldName << "' を作成中..." << std::endl;
    }
    
    // 例外安全なテクスチャ追加
    void addTexture(const std::string& textureName)
    {
        // make_uniqueは例外安全
        auto texture = std::make_unique<TextureResource>(textureName);
        
        // ここで例外が発生してもtextureは自動的に解放される
        textures.push_back(std::move(texture));
    }
    
    void loadWorldAssets()
    {
        try
        {
            addTexture("grass.png");
            addTexture("stone.png");
            addTexture("corrupted.png"); // これは例外を投げる
            addTexture("water.png");     // これは実行されない
        }
        catch (const std::exception& e)
        {
            std::cerr << "アセット読み込みエラー: " << e.what() << std::endl;
            // すでに読み込まれたテクスチャは自動的に解放される
        }
    }
    
    size_t getTextureCount() const noexcept
    {
        return textures.size();
    }
};

// スコープガードパターン
class ScopeGuard
{
private:
    std::function<void()> cleanup;
    bool dismissed;
    
public:
    explicit ScopeGuard(std::function<void()> f)
        : cleanup(f), dismissed(false) {}
    
    ~ScopeGuard()
    {
        if (!dismissed)
        {
            cleanup();
        }
    }
    
    void dismiss()
    {
        dismissed = true;
    }
    
    // コピー禁止
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
};

// トランザクション的な処理
void transferGold(PlayerInventory& from, PlayerInventory& to, int amount)
{
    // 現在の状態を保存
    int fromGold = from.getGold();
    int toGold = to.getGold();
    
    // クリーンアップ処理を設定
    ScopeGuard guard([&]() {
        std::cout << "トランザクションをロールバック" << std::endl;
        // 実際のゲームでは元の値に戻す処理をここに書く
    });
    
    try
    {
        // ゴールドの移動処理
        if (fromGold < amount)
        {
            throw std::runtime_error("送金元の所持金が不足しています");
        }
        
        std::cout << amount << " ゴールドを送金中..." << std::endl;
        
        // 成功したらガードを解除
        guard.dismiss();
        std::cout << "送金完了！" << std::endl;
    }
    catch (...)
    {
        // guardのデストラクタが自動的にロールバック処理を実行
        throw;
    }
}

int main()
{
    std::cout << "=== 例外安全性とRAII ===" << std::endl;
    
    // 1. RAIIによるリソース管理
    try
    {
        GameSaveFile save("player_save.dat");
        save.write("Level: 25");
        save.write("Gold: 1500");
        // 例外が発生してもファイルは自動的に閉じられる
        throw std::runtime_error("テスト例外");
    }
    catch (const std::exception& e)
    {
        std::cout << "エラーをキャッチ: " << e.what() << std::endl;
    }
    
    // 2. スマートポインタによる例外安全性
    std::cout << "\n=== ゲームワールドの読み込み ===" << std::endl;
    GameWorld world("草原エリア");
    world.loadWorldAssets();
    std::cout << "読み込まれたテクスチャ数: " << world.getTextureCount() << std::endl;
    
    // 3. 例外安全性の保証レベル
    std::cout << "\n=== インベントリ操作 ===" << std::endl;
    PlayerInventory inventory(5);
    
    try
    {
        inventory.addItem("ポーション");
        inventory.addItem("エリクサー");
        inventory.buyItem("伝説の剣", 50);
        inventory.displayInventory();
        
        // インベントリを満杯にする
        inventory.addItem("薬草");
        inventory.addItem("解毒剤");
        inventory.addItem("これは入らない"); // 例外が発生
    }
    catch (const std::exception& e)
    {
        std::cerr << "インベントリエラー: " << e.what() << std::endl;
        inventory.displayInventory(); // 有効な状態が保たれている
    }
    
    // 4. nothrow保証の関数
    std::cout << "\n現在の所持金: " << inventory.getGold() << " ゴールド" << std::endl;
    
    return 0;
}

// まとめ：
// - RAIIパターンで自動的なリソース管理を実現
// - 例外安全性には3つのレベル（基本保証、強い保証、nothrow保証）
// - スマートポインタで動的メモリを安全に管理
// - スコープガードでクリーンアップ処理を保証
// - noexceptで例外を投げない関数を明示