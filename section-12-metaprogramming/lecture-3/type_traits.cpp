// 型特性（Type Traits）
// 型の属性を調べ、型に基づいた条件分岐やコード生成を行う

#include <iostream>
#include <type_traits>
#include <string>
#include <vector>
#include <memory>
#include <concepts>  // C++20

// 基本的な型特性の使用
template<typename T>
void printTypeInfo()
{
    std::cout << "型: " << typeid(T).name() << std::endl;
    std::cout << "  - 整数型: " << std::is_integral<T>::value << std::endl;
    std::cout << "  - 浮動小数点型: " << std::is_floating_point<T>::value << std::endl;
    std::cout << "  - ポインタ型: " << std::is_pointer<T>::value << std::endl;
    std::cout << "  - クラス型: " << std::is_class<T>::value << std::endl;
    std::cout << "  - POD型: " << std::is_trivial<T>::value << std::endl;
}

// ゲームオブジェクトの型判定
class GameObject
{
public:
    virtual ~GameObject() = default;
    virtual void update() = 0;
};

class Player : public GameObject
{
public:
    void update() override {}
    void attack() {}
};

class Enemy : public GameObject
{
public:
    void update() override {}
    void patrol() {}
};

// カスタム型特性
template<typename T>
struct is_game_object : std::is_base_of<GameObject, T> {};

template<typename T>
constexpr bool is_game_object_v = is_game_object<T>::value;

// 条件付きテンプレート（SFINAE）
template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value, T>::type
calculateDamage(T baseDamage, T multiplier)
{
    return baseDamage * multiplier;
}

template<typename T>
typename std::enable_if<std::is_class<T>::value && is_game_object_v<T>, int>::type
calculateDamage(const T& attacker, int baseDamage)
{
    // クラス型の場合は別の計算
    return baseDamage * 2;  // 仮の実装
}

// C++17: if constexprを使った型による分岐
template<typename T>
auto processData(T value)
{
    if constexpr (std::is_integral_v<T>)
    {
        std::cout << "整数処理: " << value * 2 << std::endl;
        return value * 2;
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        std::cout << "浮動小数点処理: " << value * 1.5 << std::endl;
        return value * 1.5;
    }
    else if constexpr (std::is_pointer_v<T>)
    {
        std::cout << "ポインタ処理" << std::endl;
        return value;
    }
    else
    {
        std::cout << "その他の型" << std::endl;
        return value;
    }
}

// メンバー検出イディオム
template<typename T, typename = void>
struct has_health : std::false_type {};

template<typename T>
struct has_health<T, std::void_t<decltype(std::declval<T>().health)>> 
    : std::true_type {};

template<typename T>
constexpr bool has_health_v = has_health<T>::value;

// より高度なメンバー検出
template<typename T, typename = void>
struct has_attack_method : std::false_type {};

template<typename T>
struct has_attack_method<T, std::void_t<decltype(std::declval<T>().attack())>>
    : std::true_type {};

// テスト用構造体
struct Character
{
    int health = 100;
    void attack() {}
};

struct Item
{
    int value = 50;
    // healthもattackメソッドもない
};

// 型変換と修飾子の操作
template<typename T>
class SmartBuffer
{
public:
    using value_type = std::remove_cv_t<T>;  // const/volatileを除去
    using pointer = std::add_pointer_t<value_type>;  // ポインタ型に変換
    using const_pointer = std::add_const_t<pointer>;  // constを追加
    
private:
    std::vector<value_type> buffer;
    
public:
    void add(const T& value)
    {
        buffer.push_back(value);
    }
    
    pointer data() { return buffer.data(); }
    const_pointer data() const { return buffer.data(); }
};

// 条件付き型選択
template<bool UseFloat>
struct CoordinateType
{
    using type = std::conditional_t<UseFloat, float, double>;
};

// 共通型の推論
template<typename T, typename U>
auto add(T a, U b) -> std::common_type_t<T, U>
{
    return a + b;
}

// 完全転送とstd::forward
template<typename T>
void perfectForward(T&& value)
{
    using DecayedType = std::decay_t<T>;
    
    std::cout << "元の型: " << typeid(T).name() << std::endl;
    std::cout << "Decay後: " << typeid(DecayedType).name() << std::endl;
    
    if constexpr (std::is_lvalue_reference_v<T>)
    {
        std::cout << "左辺値参照として受け取りました" << std::endl;
    }
    else
    {
        std::cout << "右辺値参照として受け取りました" << std::endl;
    }
}

// C++20: コンセプトを使った制約
template<typename T>
concept Numeric = std::is_arithmetic_v<T>;

template<typename T>
concept GameEntity = std::is_base_of_v<GameObject, T> && 
                     requires(T t) {
                         { t.update() } -> std::same_as<void>;
                     };

template<Numeric T>
T multiply(T a, T b)
{
    return a * b;
}

template<GameEntity T>
void updateEntity(T& entity)
{
    entity.update();
}

// 実践的な例：シリアライゼーション
class Serializer
{
public:
    template<typename T>
    void serialize(const T& value)
    {
        if constexpr (std::is_trivially_copyable_v<T>)
        {
            // POD型は直接バイナリ書き込み
            std::cout << "バイナリシリアライズ: " << sizeof(T) << " bytes" << std::endl;
        }
        else if constexpr (std::is_class_v<T>)
        {
            // クラス型はカスタムシリアライズ
            std::cout << "カスタムシリアライズが必要" << std::endl;
        }
        else
        {
            std::cout << "シリアライズ不可" << std::endl;
        }
    }
};

// アロケータの選択
template<typename T>
class GameAllocator
{
public:
    using value_type = T;
    
    T* allocate(size_t n)
    {
        if constexpr (std::is_trivially_destructible_v<T>)
        {
            std::cout << "単純なメモリプールから割り当て" << std::endl;
            return static_cast<T*>(::operator new(n * sizeof(T)));
        }
        else
        {
            std::cout << "複雑なオブジェクト用アロケータを使用" << std::endl;
            return static_cast<T*>(::operator new(n * sizeof(T)));
        }
    }
    
    void deallocate(T* p, size_t)
    {
        ::operator delete(p);
    }
};

// 型特性を使った最適化
template<typename Container>
void clearContainer(Container& container)
{
    using ValueType = typename Container::value_type;
    
    if constexpr (std::is_trivially_destructible_v<ValueType>)
    {
        // 単純な型は高速クリア
        container.clear();
        std::cout << "高速クリア実行" << std::endl;
    }
    else
    {
        // 複雑な型は個別に破棄
        while (!container.empty())
        {
            container.pop_back();
        }
        std::cout << "要素ごとに破棄" << std::endl;
    }
}

int main()
{
    std::cout << "=== 型特性（Type Traits）===" << std::endl;
    
    // 1. 基本的な型情報
    std::cout << "\n--- 型情報の表示 ---" << std::endl;
    printTypeInfo<int>();
    std::cout << std::endl;
    printTypeInfo<Player>();
    
    // 2. カスタム型特性
    std::cout << "\n--- ゲームオブジェクト判定 ---" << std::endl;
    std::cout << "Player is GameObject: " << is_game_object_v<Player> << std::endl;
    std::cout << "int is GameObject: " << is_game_object_v<int> << std::endl;
    
    // 3. SFINAEによる関数選択
    std::cout << "\n--- ダメージ計算 ---" << std::endl;
    std::cout << "数値版: " << calculateDamage(100, 1.5f) << std::endl;
    Player player;
    std::cout << "オブジェクト版: " << calculateDamage(player, 50) << std::endl;
    
    // 4. if constexprによる処理
    std::cout << "\n--- 型別処理 ---" << std::endl;
    processData(42);
    processData(3.14);
    int* ptr = nullptr;
    processData(ptr);
    
    // 5. メンバー検出
    std::cout << "\n--- メンバー検出 ---" << std::endl;
    std::cout << "Character has health: " << has_health_v<Character> << std::endl;
    std::cout << "Item has health: " << has_health_v<Item> << std::endl;
    std::cout << "Character has attack(): " << has_attack_method<Character>::value << std::endl;
    std::cout << "Item has attack(): " << has_attack_method<Item>::value << std::endl;
    
    // 6. 型変換
    std::cout << "\n--- 型変換 ---" << std::endl;
    SmartBuffer<const int> buffer;
    buffer.add(10);
    
    // 7. 条件付き型
    std::cout << "\n--- 条件付き型選択 ---" << std::endl;
    using FloatCoord = CoordinateType<true>::type;
    using DoubleCoord = CoordinateType<false>::type;
    std::cout << "FloatCoord size: " << sizeof(FloatCoord) << std::endl;
    std::cout << "DoubleCoord size: " << sizeof(DoubleCoord) << std::endl;
    
    // 8. 共通型
    std::cout << "\n--- 共通型の推論 ---" << std::endl;
    auto result1 = add(10, 20.5);  // doubleになる
    std::cout << "10 + 20.5 = " << result1 << " (型: " << typeid(result1).name() << ")" << std::endl;
    
    // 9. 完全転送
    std::cout << "\n--- 完全転送 ---" << std::endl;
    int x = 42;
    perfectForward(x);        // 左辺値
    perfectForward(100);      // 右辺値
    
    // 10. コンセプト（C++20）
    std::cout << "\n--- コンセプト ---" << std::endl;
    std::cout << "10 * 20 = " << multiply(10, 20) << std::endl;
    updateEntity(player);
    
    // 11. シリアライゼーション
    std::cout << "\n--- シリアライゼーション ---" << std::endl;
    Serializer serializer;
    serializer.serialize(42);
    serializer.serialize(player);
    
    // 12. 最適化
    std::cout << "\n--- コンテナクリア ---" << std::endl;
    std::vector<int> intVec{1, 2, 3, 4, 5};
    std::vector<std::string> strVec{"A", "B", "C"};
    
    clearContainer(intVec);
    clearContainer(strVec);
    
    return 0;
}

// まとめ：
// - std::type_traitsで型の属性を調査
// - SFINAEやif constexprで型に応じた処理を実装
// - カスタム型特性で独自の型判定を作成
// - C++20のコンセプトでより直感的な制約を記述
// - 型特性を活用して最適なコード生成を実現