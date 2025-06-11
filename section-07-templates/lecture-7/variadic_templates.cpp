/*
 * テンプレート第7講: 可変長テンプレート (Variadic Templates)
 * 
 * 可変長テンプレートは任意の数の引数を受け取るテンプレートです。
 * C++11で導入され、型安全で効率的な可変引数処理が可能になりました。
 * ゲーム開発では、コンポーネントシステムやイベントシステムで活用されます。
 */

#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <memory>
#include <sstream>

// 基本的な可変長テンプレート関数
template<typename... Args>
void printGameInfo(Args&&... args)
{
    std::cout << "ゲーム情報: ";
    ((std::cout << std::forward<Args>(args) << " "), ...); // C++17 fold expression
    std::cout << std::endl;
}

// 再帰的な可変長テンプレート (C++11/14スタイル)
template<typename First>
void printGameInfoRecursive(First&& first)
{
    std::cout << first << std::endl;
}

template<typename First, typename... Rest>
void printGameInfoRecursive(First&& first, Rest&&... rest)
{
    std::cout << first << " ";
    printGameInfoRecursive(std::forward<Rest>(rest)...);
}

// 可変長テンプレートでの型カウント
template<typename... Types>
constexpr size_t countTypes()
{
    return sizeof...(Types);
}

// ゲームオブジェクトファクトリ
template<typename T, typename... Args>
std::unique_ptr<T> createGameObject(Args&&... args)
{
    std::cout << "ゲームオブジェクト作成: " << typeid(T).name() 
              << " (引数数: " << sizeof...(Args) << ")" << std::endl;
    return std::make_unique<T>(std::forward<Args>(args)...);
}

// ゲームプレイヤークラス
class GamePlayer
{
public:
    std::string name;
    int level;
    double health;
    
    GamePlayer(const std::string& n, int l, double h) : name(n), level(l), health(h) {}
    
    void display() const
    {
        std::cout << "プレイヤー: " << name << " (Lv." << level << ", HP:" << health << ")" << std::endl;
    }
};

// ゲームアイテムクラス
class GameItem
{
public:
    std::string itemName;
    int value;
    
    GameItem(const std::string& name, int val) : itemName(name), value(val) {}
    
    void display() const
    {
        std::cout << "アイテム: " << itemName << " (価値:" << value << ")" << std::endl;
    }
};

// 可変長テンプレートコンテナ
template<typename... Types>
class GameTuple
{
private:
    std::tuple<Types...> data;
    
public:
    GameTuple(Types... args) : data(args...) {}
    
    template<size_t Index>
    auto get() const -> decltype(std::get<Index>(data))
    {
        return std::get<Index>(data);
    }
    
    void displayAll() const
    {
        displayTuple(data, std::index_sequence_for<Types...>{});
    }
    
    constexpr size_t size() const
    {
        return sizeof...(Types);
    }
    
private:
    template<typename Tuple, size_t... Indices>
    void displayTuple(const Tuple& t, std::index_sequence<Indices...>) const
    {
        std::cout << "ゲームタプル: ";
        ((std::cout << "[" << Indices << "]=" << std::get<Indices>(t) << " "), ...);
        std::cout << std::endl;
    }
};

// C++17のfold expressionを使った数値計算
template<typename... Args>
auto calculateTotalDamage(Args... values)
{
    return (values + ...); // 左fold: (((v1 + v2) + v3) + ...)
}

template<typename... Args>
auto calculateMaxDamage(Args... values)
{
    return std::max({values...}); // 初期化リストでmax
}

// ゲームコンポーネントシステムの例
class Component
{
public:
    virtual ~Component() = default;
    virtual void update() = 0;
    virtual std::string getName() const = 0;
};

class PositionComponent : public Component
{
public:
    double x, y;
    PositionComponent(double px, double py) : x(px), y(py) {}
    
    void update() override
    {
        std::cout << "位置更新: (" << x << ", " << y << ")" << std::endl;
    }
    
    std::string getName() const override { return "Position"; }
};

class HealthComponent : public Component
{
public:
    int health, maxHealth;
    HealthComponent(int h, int maxH) : health(h), maxHealth(maxH) {}
    
    void update() override
    {
        std::cout << "体力更新: " << health << "/" << maxHealth << std::endl;
    }
    
    std::string getName() const override { return "Health"; }
};

class RenderComponent : public Component
{
public:
    std::string sprite;
    RenderComponent(const std::string& s) : sprite(s) {}
    
    void update() override
    {
        std::cout << "描画更新: " << sprite << std::endl;
    }
    
    std::string getName() const override { return "Render"; }
};

// 可変長テンプレートでゲームエンティティ作成
template<typename... Components>
class GameEntity
{
private:
    std::tuple<std::unique_ptr<Components>...> components;
    
public:
    template<typename... Args>
    GameEntity(Args&&... args) : components(std::make_unique<Components>(std::forward<Args>(args))...) {}
    
    template<typename T>
    T* getComponent()
    {
        return static_cast<T*>(std::get<std::unique_ptr<T>>(components).get());
    }
    
    void updateAll()
    {
        std::cout << "=== エンティティ更新 ===" << std::endl;
        updateComponents(std::index_sequence_for<Components...>{});
    }
    
    void displayComponents() const
    {
        std::cout << "エンティティコンポーネント: ";
        displayComponentNames(std::index_sequence_for<Components...>{});
        std::cout << std::endl;
    }
    
private:
    template<size_t... Indices>
    void updateComponents(std::index_sequence<Indices...>)
    {
        ((std::get<Indices>(components)->update()), ...);
    }
    
    template<size_t... Indices>
    void displayComponentNames(std::index_sequence<Indices...>) const
    {
        ((std::cout << std::get<Indices>(components)->getName() << " "), ...);
    }
};

// 可変長テンプレートでのイベントシステム
template<typename... EventTypes>
class GameEventManager
{
private:
    std::tuple<std::vector<EventTypes>...> eventQueues;
    
public:
    template<typename EventType>
    void addEvent(const EventType& event)
    {
        std::get<std::vector<EventType>>(eventQueues).push_back(event);
        std::cout << "イベント追加: " << typeid(EventType).name() << std::endl;
    }
    
    template<typename EventType>
    void processEvents()
    {
        auto& queue = std::get<std::vector<EventType>>(eventQueues);
        std::cout << "=== " << typeid(EventType).name() << " 処理中 ===" << std::endl;
        for (auto& event : queue)
        {
            event.process();
        }
        queue.clear();
    }
    
    void processAllEvents()
    {
        processAllEventsImpl(std::index_sequence_for<EventTypes...>{});
    }
    
private:
    template<size_t... Indices>
    void processAllEventsImpl(std::index_sequence<Indices...>)
    {
        (processEventsAtIndex<Indices>(), ...);
    }
    
    template<size_t Index>
    void processEventsAtIndex()
    {
        using EventType = std::tuple_element_t<Index, std::tuple<EventTypes...>>;
        processEvents<EventType>();
    }
};

// サンプルイベント
struct PlayerMoveEvent
{
    double x, y;
    PlayerMoveEvent(double px, double py) : x(px), y(py) {}
    void process() { std::cout << "プレイヤー移動: (" << x << ", " << y << ")" << std::endl; }
};

struct ItemPickupEvent
{
    std::string itemName;
    ItemPickupEvent(const std::string& name) : itemName(name) {}
    void process() { std::cout << "アイテム取得: " << itemName << std::endl; }
};

struct EnemyDefeatedEvent
{
    int exp;
    EnemyDefeatedEvent(int e) : exp(e) {}
    void process() { std::cout << "敵撃破: 経験値 " << exp << " 獲得" << std::endl; }
};

// 可変長テンプレートでの文字列フォーマット
template<typename... Args>
std::string formatGameMessage(const std::string& format, Args&&... args)
{
    std::ostringstream oss;
    formatImpl(oss, format, std::forward<Args>(args)...);
    return oss.str();
}

template<typename T>
void formatImpl(std::ostringstream& oss, const std::string& format, T&& value)
{
    size_t pos = format.find("{}");
    if (pos != std::string::npos)
    {
        oss << format.substr(0, pos) << std::forward<T>(value) << format.substr(pos + 2);
    }
    else
    {
        oss << format;
    }
}

template<typename T, typename... Rest>
void formatImpl(std::ostringstream& oss, const std::string& format, T&& value, Rest&&... rest)
{
    size_t pos = format.find("{}");
    if (pos != std::string::npos)
    {
        oss << format.substr(0, pos) << std::forward<T>(value);
        formatImpl(oss, format.substr(pos + 2), std::forward<Rest>(rest)...);
    }
    else
    {
        oss << format;
    }
}

int main()
{
    std::cout << "=== 可変長テンプレートのデモ ===" << std::endl;
    
    // 1. 基本的な可変長テンプレート
    std::cout << "\n1. 基本的な可変長テンプレート:" << std::endl;
    printGameInfo("プレイヤー:", "アリス", "レベル:", 25, "HP:", 100.5);
    
    std::cout << "\n再帰版:" << std::endl;
    printGameInfoRecursive("敵:", "ドラゴン", "攻撃力:", 150);
    
    // 2. 型カウント
    std::cout << "\n2. 型カウント:" << std::endl;
    std::cout << "型数: " << countTypes<int, double, std::string>() << std::endl;
    std::cout << "型数: " << countTypes<>() << std::endl;
    
    // 3. オブジェクトファクトリ
    std::cout << "\n3. オブジェクトファクトリ:" << std::endl;
    auto player = createGameObject<GamePlayer>("ボブ", 30, 85.0);
    auto item = createGameObject<GameItem>("魔法の剣", 500);
    
    player->display();
    item->display();
    
    // 4. ゲームタプル
    std::cout << "\n4. ゲームタプル:" << std::endl;
    GameTuple<std::string, int, double, bool> gameData("クエスト完了", 100, 12.5, true);
    gameData.displayAll();
    
    std::cout << "要素アクセス: " << gameData.get<0>() << ", " << gameData.get<1>() << std::endl;
    std::cout << "タプルサイズ: " << gameData.size() << std::endl;
    
    // 5. fold expressionでの計算
    std::cout << "\n5. fold expressionでの計算:" << std::endl;
    auto totalDamage = calculateTotalDamage(50, 25, 30, 15);
    auto maxDamage = calculateMaxDamage(50, 25, 30, 15);
    
    std::cout << "総ダメージ: " << totalDamage << std::endl;
    std::cout << "最大ダメージ: " << maxDamage << std::endl;
    
    // 6. コンポーネントシステム
    std::cout << "\n6. コンポーネントシステム:" << std::endl;
    GameEntity<PositionComponent, HealthComponent, RenderComponent> entity(
        PositionComponent(10.0, 20.0),
        HealthComponent(100, 100),
        RenderComponent("player_sprite.png")
    );
    
    entity.displayComponents();
    entity.updateAll();
    
    // コンポーネントへのアクセス
    auto* pos = entity.getComponent<PositionComponent>();
    pos->x = 15.0;
    std::cout << "位置更新後: ";
    pos->update();
    
    // 7. イベントシステム
    std::cout << "\n7. イベントシステム:" << std::endl;
    GameEventManager<PlayerMoveEvent, ItemPickupEvent, EnemyDefeatedEvent> eventManager;
    
    eventManager.addEvent(PlayerMoveEvent(5.0, 3.0));
    eventManager.addEvent(ItemPickupEvent("体力ポーション"));
    eventManager.addEvent(EnemyDefeatedEvent(150));
    eventManager.addEvent(PlayerMoveEvent(8.0, 7.0));
    
    eventManager.processAllEvents();
    
    // 8. 文字列フォーマット
    std::cout << "\n8. 文字列フォーマット:" << std::endl;
    auto message1 = formatGameMessage("プレイヤー {} がレベル {} に到達しました！", "アリス", 26);
    auto message2 = formatGameMessage("ダメージ: {} (クリティカル: {})", 150, true);
    
    std::cout << message1 << std::endl;
    std::cout << message2 << std::endl;
    
    // 9. 複雑な可変長テンプレート使用例
    std::cout << "\n9. 複雑な使用例:" << std::endl;
    
    // 複数のゲームオブジェクトを一度に作成
    auto createMultipleObjects = [](auto&&... args) {
        std::cout << "複数オブジェクト作成:" << std::endl;
        ((std::cout << "  - " << args << std::endl), ...);
    };
    
    createMultipleObjects("プレイヤー", "敵", "アイテム", "NPC");
    
    return 0;
}

/*
 * 学習ポイント:
 * 1. template<typename... Args> で可変引数テンプレートを定義
 * 2. sizeof...(Args) で引数数を取得
 * 3. C++17のfold expressionで簡潔な処理が可能
 * 4. std::forward<Args>(args)... で完全転送
 * 5. index_sequenceでタプル要素への個別アクセス
 * 6. 再帰テンプレートで段階的処理も可能
 * 7. ゲーム開発ではコンポーネントシステムで威力発揮
 * 8. 型安全で効率的な可変引数処理を実現
 */