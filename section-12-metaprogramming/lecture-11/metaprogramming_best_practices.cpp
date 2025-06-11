// メタプログラミングのベストプラクティス
// 保守性、可読性、パフォーマンスを考慮した実践的な指針

#include <iostream>
#include <type_traits>
#include <concepts>
#include <string>
#include <vector>
#include <chrono>

// 悪い例：過度に複雑なメタプログラミング
namespace bad_examples
{
    // 避けるべき：読みにくい名前と複雑すぎる条件
    template<typename T, typename U, typename V = void>
    struct ComplexChecker 
        : std::conditional_t<
            std::is_same_v<T, int> && 
            std::is_pointer_v<U> && 
            !std::is_const_v<std::remove_pointer_t<U>>,
            std::true_type,
            std::false_type
        > {};
    
    // 避けるべき：深すぎるテンプレート再帰
    template<int N>
    struct DeepRecursion
    {
        static constexpr int value = N + DeepRecursion<N-1>::value;
    };
    
    template<>
    struct DeepRecursion<0>
    {
        static constexpr int value = 0;
    };
}

// 良い例：明確で保守しやすいメタプログラミング
namespace good_examples
{
    // 1. 明確な命名と段階的な構築
    template<typename T>
    concept Arithmetic = std::is_arithmetic_v<T>;
    
    template<typename T>
    concept NonConstPointer = std::is_pointer_v<T> && 
                             !std::is_const_v<std::remove_pointer_t<T>>;
    
    template<typename T, typename U>
    concept CompatibleTypes = Arithmetic<T> && NonConstPointer<U>;
    
    // 2. 適切なエラーメッセージ
    template<Arithmetic T>
    T safe_divide(T a, T b)
    {
        static_assert(std::is_floating_point_v<T> || std::is_integral_v<T>,
                     "safe_divide requires arithmetic type");
        
        if constexpr (std::is_floating_point_v<T>)
        {
            return (b != T{0}) ? a / b : T{0};
        }
        else
        {
            return (b != 0) ? a / b : 0;
        }
    }
    
    // 3. 適度な抽象化レベル
    template<typename Container>
    concept HasSize = requires(Container c) {
        { c.size() } -> std::convertible_to<size_t>;
    };
    
    template<typename Container>
    concept HasPushBack = requires(Container c, typename Container::value_type v) {
        c.push_back(v);
    };
    
    template<HasSize Container>
    size_t get_container_size(const Container& c)
    {
        return c.size();
    }
}

// ゲーム開発でのベストプラクティス
namespace game_best_practices
{
    // 1. 型安全なIDシステム
    template<typename Tag>
    class TypedID
    {
    private:
        uint32_t id_;
        
    public:
        explicit constexpr TypedID(uint32_t id) : id_(id) {}
        constexpr uint32_t value() const { return id_; }
        
        constexpr bool operator==(const TypedID& other) const
        {
            return id_ == other.id_;
        }
        
        constexpr bool operator!=(const TypedID& other) const
        {
            return !(*this == other);
        }
    };
    
    // 異なるIDタイプを定義
    struct PlayerTag {};
    struct EnemyTag {};
    struct ItemTag {};
    
    using PlayerID = TypedID<PlayerTag>;
    using EnemyID = TypedID<EnemyTag>;
    using ItemID = TypedID<ItemTag>;
    
    // 2. コンパイル時設定システム
    template<typename GameMode>
    struct GameConfig;
    
    struct EasyMode {};
    struct NormalMode {};
    struct HardMode {};
    
    template<>
    struct GameConfig<EasyMode>
    {
        static constexpr float enemyHealthMultiplier = 0.7f;
        static constexpr float enemyDamageMultiplier = 0.8f;
        static constexpr int experienceBonus = 150;
        static constexpr const char* modeName = "イージー";
    };
    
    template<>
    struct GameConfig<NormalMode>
    {
        static constexpr float enemyHealthMultiplier = 1.0f;
        static constexpr float enemyDamageMultiplier = 1.0f;
        static constexpr int experienceBonus = 100;
        static constexpr const char* modeName = "ノーマル";
    };
    
    template<>
    struct GameConfig<HardMode>
    {
        static constexpr float enemyHealthMultiplier = 1.5f;
        static constexpr float enemyDamageMultiplier = 1.3f;
        static constexpr int experienceBonus = 120;
        static constexpr const char* modeName = "ハード";
    };
    
    // 3. 型安全なイベントシステム
    template<typename EventType>
    class Event
    {
    private:
        EventType data_;
        
    public:
        template<typename... Args>
        explicit Event(Args&&... args) : data_(std::forward<Args>(args)...) {}
        
        const EventType& getData() const { return data_; }
    };
    
    struct PlayerLevelUpEvent
    {
        PlayerID playerId;
        int newLevel;
        int experienceGained;
    };
    
    struct EnemyDefeatedEvent
    {
        EnemyID enemyId;
        PlayerID defeatedBy;
        int experienceAwarded;
    };
    
    // 4. コンパイル時バリデーション
    template<typename T>
    concept ValidGameObject = requires {
        typename T::IDType;
        requires std::is_base_of_v<TypedID<typename T::IDType::tag_type>, typename T::IDType>;
    };
    
    // 5. パフォーマンスを意識した設計
    template<size_t MaxSize>
    class StaticVector
    {
    private:
        alignas(64) std::array<int, MaxSize> data_;
        size_t size_;
        
    public:
        constexpr StaticVector() : size_(0) {}
        
        constexpr void push_back(int value)
        {
            if (size_ < MaxSize)
            {
                data_[size_++] = value;
            }
        }
        
        constexpr size_t size() const { return size_; }
        constexpr int operator[](size_t index) const { return data_[index]; }
    };
}

// デバッグとテストのためのメタプログラミング
namespace debug_helpers
{
    // 1. コンパイル時情報出力
    template<typename T>
    void print_type_info()
    {
        std::cout << "型: " << typeid(T).name() << std::endl;
        std::cout << "サイズ: " << sizeof(T) << " bytes" << std::endl;
        std::cout << "アライメント: " << alignof(T) << " bytes" << std::endl;
        std::cout << "トリビアル: " << std::is_trivial_v<T> << std::endl;
        std::cout << "POD: " << std::is_pod_v<T> << std::endl;
    }
    
    // 2. コンパイル時アサーション
    template<typename T>
    constexpr bool validate_game_object()
    {
        static_assert(sizeof(T) <= 256, "ゲームオブジェクトのサイズが大きすぎます");
        static_assert(std::is_move_constructible_v<T>, "ムーブ構築可能である必要があります");
        static_assert(!std::is_trivially_destructible_v<T> || 
                     std::is_trivially_copyable_v<T>, "一貫したトリビアル特性が必要です");
        return true;
    }
    
    // 3. テンプレート特殊化の検証
    template<typename T, typename = void>
    struct has_debug_method : std::false_type {};
    
    template<typename T>
    struct has_debug_method<T, std::void_t<decltype(std::declval<T>().debug())>>
        : std::true_type {};
    
    template<typename T>
    void conditional_debug(T& obj)
    {
        if constexpr (has_debug_method<T>::value)
        {
            obj.debug();
        }
        else
        {
            std::cout << "デバッグメソッドが存在しません" << std::endl;
        }
    }
}

// パフォーマンス測定
class PerformanceMeasurement
{
public:
    template<typename Func>
    static auto measure_time(Func&& func)
    {
        auto start = std::chrono::high_resolution_clock::now();
        
        if constexpr (std::is_void_v<std::invoke_result_t<Func>>)
        {
            func();
            auto end = std::chrono::high_resolution_clock::now();
            return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        }
        else
        {
            auto result = func();
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            return std::make_pair(result, duration);
        }
    }
};

// 実践例：ゲームエンティティシステム
class GameObject
{
public:
    using IDType = game_best_practices::PlayerID;
    
private:
    IDType id_;
    std::string name_;
    
public:
    GameObject(IDType id, const std::string& name) : id_(id), name_(name) {}
    
    IDType getId() const { return id_; }
    const std::string& getName() const { return name_; }
    
    void debug() const
    {
        std::cout << "GameObject Debug: ID=" << id_.value() 
                  << ", Name=" << name_ << std::endl;
    }
};

int main()
{
    std::cout << "=== メタプログラミングのベストプラクティス ===" << std::endl;
    
    // 1. 型安全なIDシステム
    std::cout << "\n--- 型安全なIDシステム ---" << std::endl;
    using namespace game_best_practices;
    
    PlayerID playerId(1001);
    EnemyID enemyId(2001);
    
    std::cout << "プレイヤーID: " << playerId.value() << std::endl;
    std::cout << "敵ID: " << enemyId.value() << std::endl;
    
    // コンパイルエラー: 異なる型のIDは比較できない
    // bool same = (playerId == enemyId);  
    
    // 2. コンパイル時設定
    std::cout << "\n--- 難易度設定 ---" << std::endl;
    std::cout << "イージーモード:" << std::endl;
    std::cout << "  敵HP倍率: " << GameConfig<EasyMode>::enemyHealthMultiplier << std::endl;
    std::cout << "  経験値ボーナス: " << GameConfig<EasyMode>::experienceBonus << "%" << std::endl;
    
    std::cout << "ハードモード:" << std::endl;
    std::cout << "  敵HP倍率: " << GameConfig<HardMode>::enemyHealthMultiplier << std::endl;
    std::cout << "  経験値ボーナス: " << GameConfig<HardMode>::experienceBonus << "%" << std::endl;
    
    // 3. 型安全なイベント
    std::cout << "\n--- イベントシステム ---" << std::endl;
    Event<PlayerLevelUpEvent> levelUpEvent({playerId, 25, 1500});
    std::cout << "プレイヤー " << levelUpEvent.getData().playerId.value() 
              << " がレベル " << levelUpEvent.getData().newLevel << " に！" << std::endl;
    
    // 4. 静的ベクトル
    std::cout << "\n--- 静的ベクトル ---" << std::endl;
    StaticVector<10> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    
    std::cout << "静的ベクトルサイズ: " << vec.size() << std::endl;
    for (size_t i = 0; i < vec.size(); ++i)
    {
        std::cout << vec[i] << " ";
    }
    std::cout << std::endl;
    
    // 5. デバッグヘルパー
    std::cout << "\n--- 型情報出力 ---" << std::endl;
    debug_helpers::print_type_info<GameObject>();
    
    // 6. バリデーション
    std::cout << "\n--- バリデーション ---" << std::endl;
    constexpr bool isValid = debug_helpers::validate_game_object<GameObject>();
    std::cout << "GameObject バリデーション: " << (isValid ? "成功" : "失敗") << std::endl;
    
    // 7. 条件付きデバッグ
    std::cout << "\n--- 条件付きデバッグ ---" << std::endl;
    GameObject obj(playerId, "テストオブジェクト");
    debug_helpers::conditional_debug(obj);
    
    int plainInt = 42;
    debug_helpers::conditional_debug(plainInt);
    
    // 8. パフォーマンス測定
    std::cout << "\n--- パフォーマンス測定 ---" << std::endl;
    auto duration = PerformanceMeasurement::measure_time([]() {
        volatile int sum = 0;
        for (int i = 0; i < 10000; ++i)
        {
            sum += i;
        }
    });
    
    std::cout << "計算時間: " << duration.count() << " マイクロ秒" << std::endl;
    
    return 0;
}

// まとめ：
// - 明確な命名と段階的な構築
// - 適切なエラーメッセージとアサーション
// - コンパイル時設定で実行時オーバーヘッドを削減
// - 型安全性を重視した設計
// - デバッグとテストを考慮した実装
// - パフォーマンスを意識したメタプログラミング