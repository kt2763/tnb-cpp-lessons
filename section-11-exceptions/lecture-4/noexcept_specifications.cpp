// noexcept指定子
// 関数が例外を投げないことを明示し、最適化とコード品質を向上

#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <type_traits>
#include <utility>

// ゲームの基本的な構造体
struct Vector2D
{
    float x, y;
    
    // noexceptなコンストラクタ
    Vector2D() noexcept : x(0.0f), y(0.0f) {}
    
    Vector2D(float px, float py) noexcept : x(px), y(py) {}
    
    // noexceptな演算子
    Vector2D operator+(const Vector2D& other) const noexcept
    {
        return Vector2D(x + other.x, y + other.y);
    }
    
    // noexceptなアクセサ
    float length() const noexcept
    {
        return std::sqrt(x * x + y * y);
    }
};

// プレイヤーステータス
class PlayerStats
{
private:
    int health;
    int mana;
    int level;
    std::string name;
    
public:
    // ムーブコンストラクタはnoexcept
    PlayerStats(PlayerStats&& other) noexcept
        : health(other.health), mana(other.mana), 
          level(other.level), name(std::move(other.name))
    {
        other.health = 0;
        other.mana = 0;
        other.level = 1;
    }
    
    // ムーブ代入演算子もnoexcept
    PlayerStats& operator=(PlayerStats&& other) noexcept
    {
        if (this != &other)
        {
            health = other.health;
            mana = other.mana;
            level = other.level;
            name = std::move(other.name);
            
            other.health = 0;
            other.mana = 0;
            other.level = 1;
        }
        return *this;
    }
    
    // コピーコンストラクタは例外を投げる可能性
    PlayerStats(const PlayerStats& other)
        : health(other.health), mana(other.mana),
          level(other.level), name(other.name) // stringのコピーは例外を投げうる
    {
    }
    
    PlayerStats(const std::string& n, int h, int m, int l)
        : health(h), mana(m), level(l), name(n)
    {
    }
    
    // 単純なゲッターはnoexcept
    int getHealth() const noexcept { return health; }
    int getMana() const noexcept { return mana; }
    int getLevel() const noexcept { return level; }
    
    // stringを返す関数は例外を投げる可能性
    const std::string& getName() const noexcept { return name; }
    
    // 条件付きnoexcept
    void swap(PlayerStats& other) noexcept(
        std::is_nothrow_move_constructible<std::string>::value &&
        std::is_nothrow_move_assignable<std::string>::value)
    {
        std::swap(health, other.health);
        std::swap(mana, other.mana);
        std::swap(level, other.level);
        std::swap(name, other.name);
    }
};

// アイテムクラス
class GameItem
{
private:
    std::string name;
    int value;
    float weight;
    
public:
    GameItem(const std::string& n, int v, float w)
        : name(n), value(v), weight(w) {}
    
    // noexcept指定子の動的チェック
    GameItem(GameItem&& other) noexcept(
        noexcept(std::string(std::move(other.name))))
        : name(std::move(other.name)), value(other.value), weight(other.weight)
    {
        other.value = 0;
        other.weight = 0.0f;
    }
    
    // デストラクタは暗黙的にnoexcept
    ~GameItem() = default;
    
    // 比較演算子はnoexcept
    bool operator<(const GameItem& other) const noexcept
    {
        return value < other.value;
    }
    
    int getValue() const noexcept { return value; }
    float getWeight() const noexcept { return weight; }
};

// noexceptを活用したゲームシステム
class GameEngine
{
private:
    std::vector<PlayerStats> players;
    std::vector<GameItem> items;
    
public:
    // noexceptな初期化関数
    void initialize() noexcept
    {
        // vectorのclearはnoexcept
        players.clear();
        items.clear();
    }
    
    // 条件付きnoexcept - vectorのpush_backに依存
    template<typename T>
    void addElement(std::vector<T>& container, T&& element) 
        noexcept(noexcept(container.push_back(std::forward<T>(element))))
    {
        container.push_back(std::forward<T>(element));
    }
    
    // パフォーマンスクリティカルな関数
    float calculateTotalWeight() const noexcept
    {
        float total = 0.0f;
        for (const auto& item : items)
        {
            total += item.getWeight();
        }
        return total;
    }
    
    // ソート関数 - 比較関数がnoexceptならより効率的
    void sortItemsByValue() noexcept
    {
        std::sort(items.begin(), items.end(),
            [](const GameItem& a, const GameItem& b) noexcept {
                return a.getValue() > b.getValue();
            });
    }
};

// noexceptのテストユーティリティ
template<typename T>
void checkNoexcept()
{
    std::cout << "Type: " << typeid(T).name() << std::endl;
    std::cout << "- デフォルトコンストラクタ: " 
              << std::is_nothrow_default_constructible<T>::value << std::endl;
    std::cout << "- ムーブコンストラクタ: " 
              << std::is_nothrow_move_constructible<T>::value << std::endl;
    std::cout << "- ムーブ代入: " 
              << std::is_nothrow_move_assignable<T>::value << std::endl;
    std::cout << "- デストラクタ: " 
              << std::is_nothrow_destructible<T>::value << std::endl;
}

// 関数ポインタとnoexcept
void safeFunction() noexcept
{
    std::cout << "例外を投げない関数" << std::endl;
}

void unsafeFunction()
{
    std::cout << "例外を投げる可能性がある関数" << std::endl;
}

// noexcept演算子
template<typename F>
void callFunction(F func)
{
    if constexpr (noexcept(func()))
    {
        std::cout << "この関数は例外を投げません" << std::endl;
        func();
    }
    else
    {
        std::cout << "この関数は例外を投げる可能性があります" << std::endl;
        try
        {
            func();
        }
        catch (...)
        {
            std::cerr << "例外がキャッチされました" << std::endl;
        }
    }
}

// 実践的な例：バッファ管理
class GameBuffer
{
private:
    std::unique_ptr<char[]> buffer;
    size_t size;
    
public:
    // noexceptなムーブコンストラクタで効率的な転送
    GameBuffer(GameBuffer&& other) noexcept
        : buffer(std::move(other.buffer)), size(other.size)
    {
        other.size = 0;
    }
    
    GameBuffer& operator=(GameBuffer&& other) noexcept
    {
        if (this != &other)
        {
            buffer = std::move(other.buffer);
            size = other.size;
            other.size = 0;
        }
        return *this;
    }
    
    explicit GameBuffer(size_t s) : buffer(std::make_unique<char[]>(s)), size(s) {}
    
    // noexceptなアクセサ
    size_t getSize() const noexcept { return size; }
    
    // データアクセスもnoexcept
    char* getData() noexcept { return buffer.get(); }
    const char* getData() const noexcept { return buffer.get(); }
};

int main()
{
    std::cout << "=== noexcept指定子 ===" << std::endl;
    
    // 1. 基本的な使用例
    std::cout << "\n--- 基本的なnoexcept ---" << std::endl;
    Vector2D pos1(10.0f, 20.0f);
    Vector2D pos2(5.0f, 15.0f);
    Vector2D result = pos1 + pos2;  // noexceptな演算
    std::cout << "座標計算: (" << result.x << ", " << result.y << ")" << std::endl;
    
    // 2. 型特性のチェック
    std::cout << "\n--- 型のnoexcept特性 ---" << std::endl;
    std::cout << "Vector2D:" << std::endl;
    checkNoexcept<Vector2D>();
    std::cout << "\nPlayerStats:" << std::endl;
    checkNoexcept<PlayerStats>();
    
    // 3. ムーブ操作の効率性
    std::cout << "\n--- ムーブ操作 ---" << std::endl;
    std::vector<PlayerStats> playerList;
    playerList.reserve(10);  // 再配置を避ける
    
    // emplace_backはムーブコンストラクタがnoexceptなら効率的
    playerList.emplace_back("勇者", 100, 50, 25);
    playerList.emplace_back("魔法使い", 70, 120, 23);
    
    // 4. noexcept演算子の使用
    std::cout << "\n--- noexcept演算子 ---" << std::endl;
    std::cout << "safeFunction is noexcept: " 
              << noexcept(safeFunction()) << std::endl;
    std::cout << "unsafeFunction is noexcept: " 
              << noexcept(unsafeFunction()) << std::endl;
    
    // 5. 条件付きnoexcept
    std::cout << "\n--- 条件付きnoexcept ---" << std::endl;
    callFunction(safeFunction);
    callFunction(unsafeFunction);
    
    // 6. STLコンテナでの利点
    std::cout << "\n--- STLコンテナでの効率性 ---" << std::endl;
    std::vector<GameBuffer> buffers;
    buffers.reserve(5);
    
    // GameBufferのムーブがnoexceptなので、vectorの再配置が効率的
    for (int i = 0; i < 3; ++i)
    {
        buffers.emplace_back(1024 * (i + 1));
    }
    
    std::cout << "バッファサイズ:" << std::endl;
    for (const auto& buf : buffers)
    {
        std::cout << "- " << buf.getSize() << " bytes" << std::endl;
    }
    
    // 7. パフォーマンスクリティカルなコード
    std::cout << "\n--- パフォーマンス最適化 ---" << std::endl;
    GameEngine engine;
    engine.initialize();  // noexceptで最適化可能
    
    return 0;
}

// まとめ：
// - noexceptは関数が例外を投げないことを保証
// - ムーブ操作をnoexceptにすることでSTLコンテナが最適化される
// - 条件付きnoexceptで柔軟な指定が可能
// - noexcept演算子でコンパイル時に例外安全性をチェック
// - デストラクタは暗黙的にnoexcept(true)