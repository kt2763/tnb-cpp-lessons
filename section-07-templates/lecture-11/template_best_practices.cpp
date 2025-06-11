/*
 * テンプレート第11講: テンプレートのベストプラクティス
 * 
 * テンプレートを効果的に使用するための設計パターンと
 * ベストプラクティスを学習します。
 * ゲーム開発での実践的な例を通じて、保守しやすく
 * パフォーマンスの良いテンプレートコードの書き方を習得します。
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>
#include <chrono>
#include <algorithm>
#include <functional>

// ベストプラクティス1: 明確な命名規則
namespace game
{
    // テンプレートパラメータには意味のある名前を使用
    template<typename EntityType, typename ComponentType>
    class EntityComponentSystem
    {
        // 実装...
    };
    
    // 略語よりも完全な名前を使用
    template<typename GameObjectType>
    class GameObjectManager  // "GameObjMgr" より良い
    {
        // 実装...
    };
}

// ベストプラクティス2: テンプレートパラメータの制約
template<typename T>
class NumericGameValue
{
    static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type");
    
private:
    T value;
    
public:
    explicit NumericGameValue(T v) : value(v) {}
    
    T getValue() const { return value; }
    
    NumericGameValue operator+(const NumericGameValue& other) const
    {
        return NumericGameValue(value + other.value);
    }
    
    // 型安全な比較
    template<typename U>
    bool operator<(const NumericGameValue<U>& other) const
    {
        static_assert(std::is_arithmetic_v<U>, "U must be arithmetic");
        return static_cast<double>(value) < static_cast<double>(other.getValue());
    }
};

// ベストプラクティス3: デフォルトテンプレート引数の効果的使用
template<
    typename T,
    typename Allocator = std::allocator<T>,
    size_t DefaultCapacity = 16
>
class GameContainer
{
private:
    std::vector<T, Allocator> data;
    
public:
    GameContainer()
    {
        data.reserve(DefaultCapacity);
    }
    
    void add(const T& item)
    {
        data.push_back(item);
    }
    
    size_t size() const { return data.size(); }
    size_t capacity() const { return data.capacity(); }
    
    const T& operator[](size_t index) const { return data[index]; }
    T& operator[](size_t index) { return data[index]; }
    
    // イテレータサポート
    auto begin() { return data.begin(); }
    auto end() { return data.end(); }
    auto begin() const { return data.begin(); }
    auto end() const { return data.end(); }
};

// ベストプラクティス4: 型推定の活用
namespace type_deduction
{
    // C++17のクラステンプレート引数推定を活用
    template<typename T>
    class GamePair
    {
    public:
        T first, second;
        
        GamePair(const T& f, const T& s) : first(f), second(s) {}
        
        void display() const
        {
            std::cout << "(" << first << ", " << second << ")" << std::endl;
        }
    };
    
    // 推定ガイド
    template<typename T>
    GamePair(T, T) -> GamePair<T>;
    
    // ファクトリ関数でauto戻り値型推定
    template<typename T>
    auto makeGameValue(T value)
    {
        return NumericGameValue<T>(value);
    }
}

// ベストプラクティス5: 完全転送の適切な使用
template<typename T>
class GameObjectFactory
{
public:
    template<typename ObjectType, typename... Args>
    static std::unique_ptr<ObjectType> create(Args&&... args)
    {
        static_assert(std::is_base_of_v<T, ObjectType>, 
                     "ObjectType must inherit from T");
        
        return std::make_unique<ObjectType>(std::forward<Args>(args)...);
    }
    
    template<typename ObjectType, typename... Args>
    static ObjectType createValue(Args&&... args)
    {
        static_assert(std::is_base_of_v<T, ObjectType>, 
                     "ObjectType must inherit from T");
        
        return ObjectType(std::forward<Args>(args)...);
    }
};

// ベストプラクティス6: テンプレートの適切な分離
// ヘッダファイル相当
namespace game_math
{
    // 宣言のみ
    template<typename T>
    class Vector3D;
    
    template<typename T>
    Vector3D<T> cross(const Vector3D<T>& a, const Vector3D<T>& b);
    
    // 実装 (.tpp ファイルや .hpp ファイルに分離することが多い)
    template<typename T>
    class Vector3D
    {
    private:
        T x, y, z;
        
    public:
        Vector3D(T x_val = T{}, T y_val = T{}, T z_val = T{}) 
            : x(x_val), y(y_val), z(z_val) {}
        
        T getX() const { return x; }
        T getY() const { return y; }
        T getZ() const { return z; }
        
        Vector3D operator+(const Vector3D& other) const
        {
            return Vector3D(x + other.x, y + other.y, z + other.z);
        }
        
        Vector3D operator*(T scalar) const
        {
            return Vector3D(x * scalar, y * scalar, z * scalar);
        }
        
        T dot(const Vector3D& other) const
        {
            return x * other.x + y * other.y + z * other.z;
        }
        
        void display() const
        {
            std::cout << "(" << x << ", " << y << ", " << z << ")" << std::endl;
        }
    };
    
    template<typename T>
    Vector3D<T> cross(const Vector3D<T>& a, const Vector3D<T>& b)
    {
        return Vector3D<T>(
            a.getY() * b.getZ() - a.getZ() * b.getY(),
            a.getZ() * b.getX() - a.getX() * b.getZ(),
            a.getX() * b.getY() - a.getY() * b.getX()
        );
    }
}

// ベストプラクティス7: エラーメッセージの改善
template<typename T>
class GameStateMachine
{
private:
    // 有用なエラーメッセージを提供
    static_assert(std::is_enum_v<T>, 
        "GameStateMachine requires an enum type for state representation. "
        "Please use an enum class for type safety.");
    
    T currentState;
    std::function<void(T, T)> transitionCallback;
    
public:
    explicit GameStateMachine(T initialState) : currentState(initialState) {}
    
    void setState(T newState)
    {
        T oldState = currentState;
        currentState = newState;
        
        if (transitionCallback)
        {
            transitionCallback(oldState, newState);
        }
    }
    
    T getState() const { return currentState; }
    
    void setTransitionCallback(std::function<void(T, T)> callback)
    {
        transitionCallback = std::move(callback);
    }
};

// ゲーム状態の例
enum class GameState
{
    Menu,
    Playing,
    Paused,
    GameOver
};

// ベストプラクティス8: パフォーマンス考慮事項
namespace performance
{
    // インライン化を意識した設計
    template<typename T>
    class FastGameVector
    {
    private:
        T data[3];
        
    public:
        // 小さな関数はインライン化される可能性が高い
        constexpr FastGameVector(T x, T y, T z) : data{x, y, z} {}
        
        constexpr T operator[](size_t index) const 
        { 
            return data[index]; 
        }
        
        constexpr T& operator[](size_t index) 
        { 
            return data[index]; 
        }
        
        // コンパイル時計算が可能
        constexpr T lengthSquared() const
        {
            return data[0] * data[0] + data[1] * data[1] + data[2] * data[2];
        }
    };
    
    // テンプレート特殊化でパフォーマンス最適化
    template<>
    class FastGameVector<float>
    {
    private:
        float data[4]; // SIMD用にアライメント調整
        
    public:
        FastGameVector(float x, float y, float z) : data{x, y, z, 0.0f} {}
        
        float operator[](size_t index) const { return data[index]; }
        float& operator[](size_t index) { return data[index]; }
        
        // SIMD命令を使用する可能性のある実装
        float lengthSquared() const
        {
            return data[0] * data[0] + data[1] * data[1] + data[2] * data[2];
        }
    };
}

// ベストプラクティス9: テンプレートのテストとデバッグ
namespace testing
{
    // テンプレートの動作確認用ヘルパー
    template<typename T>
    void debugTemplate()
    {
        std::cout << "=== テンプレート情報 ===" << std::endl;
        std::cout << "型名: " << typeid(T).name() << std::endl;
        std::cout << "サイズ: " << sizeof(T) << " バイト" << std::endl;
        std::cout << "算術型: " << std::is_arithmetic_v<T> << std::endl;
        std::cout << "ポインタ型: " << std::is_pointer_v<T> << std::endl;
        std::cout << "クラス型: " << std::is_class_v<T> << std::endl;
    }
    
    // コンパイル時テスト
    template<typename T>
    constexpr bool testNumericType()
    {
        return std::is_arithmetic_v<T>;
    }
    
    static_assert(testNumericType<int>(), "int should be numeric");
    static_assert(!testNumericType<std::string>(), "string should not be numeric");
}

// ベストプラクティス10: ドキュメント化とコメント
/**
 * @brief ゲーム用の汎用コンテナクラス
 * @tparam T 格納する要素の型（コピー構築可能である必要がある）
 * @tparam MaxSize 最大サイズ（デフォルト: 100）
 * 
 * このクラスは固定サイズのコンテナを提供し、
 * ゲームでの高速な要素アクセスを実現します。
 * 
 * 使用例:
 * @code
 * FixedGameContainer<int, 50> container;
 * container.add(42);
 * std::cout << container[0] << std::endl;
 * @endcode
 */
template<typename T, size_t MaxSize = 100>
class FixedGameContainer
{
    static_assert(std::is_copy_constructible_v<T>, 
        "T must be copy constructible");
    
private:
    T data[MaxSize];
    size_t currentSize = 0;
    
public:
    /**
     * @brief 要素を追加
     * @param item 追加する要素
     * @return 追加に成功した場合true
     */
    bool add(const T& item)
    {
        if (currentSize >= MaxSize)
        {
            return false;
        }
        
        data[currentSize++] = item;
        return true;
    }
    
    /**
     * @brief 要素にアクセス
     * @param index インデックス
     * @return 要素への参照
     * @pre index < size()
     */
    const T& operator[](size_t index) const
    {
        return data[index];
    }
    
    T& operator[](size_t index)
    {
        return data[index];
    }
    
    size_t size() const { return currentSize; }
    constexpr size_t maxSize() const { return MaxSize; }
    bool empty() const { return currentSize == 0; }
    bool full() const { return currentSize == MaxSize; }
};

// パフォーマンス測定用ヘルパー
template<typename Func>
auto measureExecutionTime(Func&& func, const std::string& description = "")
{
    auto start = std::chrono::high_resolution_clock::now();
    
    if constexpr (std::is_void_v<std::invoke_result_t<Func>>)
    {
        func();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        if (!description.empty())
        {
            std::cout << description << ": " << duration.count() << " μs" << std::endl;
        }
        
        return duration;
    }
    else
    {
        auto result = func();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        if (!description.empty())
        {
            std::cout << description << ": " << duration.count() << " μs" << std::endl;
        }
        
        return std::make_pair(result, duration);
    }
}

// ゲームオブジェクトの基底クラス
class GameObjectBase
{
public:
    virtual ~GameObjectBase() = default;
    virtual void update() = 0;
    virtual std::string getName() const = 0;
};

class TestGameObject : public GameObjectBase
{
private:
    std::string name;
    
public:
    TestGameObject(const std::string& n) : name(n) {}
    
    void update() override
    {
        // 更新処理
    }
    
    std::string getName() const override
    {
        return name;
    }
};

int main()
{
    std::cout << "=== テンプレートベストプラクティスのデモ ===" << std::endl;
    
    // 1. 制約付きテンプレートの使用
    std::cout << "\n1. 制約付きテンプレート:" << std::endl;
    NumericGameValue<int> intValue(42);
    NumericGameValue<double> doubleValue(3.14);
    
    auto sum = intValue + NumericGameValue<int>(8);
    std::cout << "合計: " << sum.getValue() << std::endl;
    std::cout << "比較: " << (intValue < doubleValue) << std::endl;
    
    // 2. デフォルト引数の活用
    std::cout << "\n2. デフォルト引数の活用:" << std::endl;
    GameContainer<std::string> defaultContainer; // デフォルト設定
    GameContainer<std::string, std::allocator<std::string>, 32> customContainer; // カスタム設定
    
    defaultContainer.add("アイテム1");
    defaultContainer.add("アイテム2");
    
    std::cout << "デフォルトコンテナ容量: " << defaultContainer.capacity() << std::endl;
    std::cout << "カスタムコンテナ容量: " << customContainer.capacity() << std::endl;
    
    // 3. 型推定の活用
    std::cout << "\n3. 型推定の活用:" << std::endl;
    type_deduction::GamePair pair{10, 20}; // 型推定
    pair.display();
    
    auto gameValue = type_deduction::makeGameValue(100);
    std::cout << "自動推定値: " << gameValue.getValue() << std::endl;
    
    // 4. ファクトリパターン
    std::cout << "\n4. ファクトリパターン:" << std::endl;
    auto gameObject = GameObjectFactory<GameObjectBase>::create<TestGameObject>("テストオブジェクト");
    std::cout << "作成されたオブジェクト: " << gameObject->getName() << std::endl;
    
    // 5. 数学ライブラリの使用
    std::cout << "\n5. 数学ライブラリ:" << std::endl;
    game_math::Vector3D<double> vec1(1.0, 2.0, 3.0);
    game_math::Vector3D<double> vec2(4.0, 5.0, 6.0);
    
    auto sum_vec = vec1 + vec2;
    auto scaled = vec1 * 2.0;
    auto dot_product = vec1.dot(vec2);
    auto cross_product = game_math::cross(vec1, vec2);
    
    std::cout << "ベクトル1: ";
    vec1.display();
    std::cout << "ベクトル2: ";
    vec2.display();
    std::cout << "合計: ";
    sum_vec.display();
    std::cout << "2倍: ";
    scaled.display();
    std::cout << "内積: " << dot_product << std::endl;
    std::cout << "外積: ";
    cross_product.display();
    
    // 6. 状態マシンの使用
    std::cout << "\n6. 状態マシンの使用:" << std::endl;
    GameStateMachine<GameState> stateMachine(GameState::Menu);
    
    stateMachine.setTransitionCallback([](GameState from, GameState to) {
        std::cout << "状態遷移: " << static_cast<int>(from) 
                  << " -> " << static_cast<int>(to) << std::endl;
    });
    
    stateMachine.setState(GameState::Playing);
    stateMachine.setState(GameState::Paused);
    stateMachine.setState(GameState::GameOver);
    
    // 7. 高速ベクトル
    std::cout << "\n7. 高速ベクトル:" << std::endl;
    constexpr performance::FastGameVector<int> fastVec(1, 2, 3);
    constexpr auto lengthSq = fastVec.lengthSquared();
    
    std::cout << "高速ベクトル長さの二乗: " << lengthSq << std::endl;
    
    // float特殊化版
    performance::FastGameVector<float> floatVec(1.5f, 2.5f, 3.5f);
    std::cout << "float版長さの二乗: " << floatVec.lengthSquared() << std::endl;
    
    // 8. テンプレート情報の表示
    std::cout << "\n8. テンプレート情報:" << std::endl;
    testing::debugTemplate<int>();
    testing::debugTemplate<std::string>();
    
    // 9. 固定サイズコンテナ
    std::cout << "\n9. 固定サイズコンテナ:" << std::endl;
    FixedGameContainer<std::string, 5> fixedContainer;
    
    fixedContainer.add("武器1");
    fixedContainer.add("武器2");
    fixedContainer.add("武器3");
    
    std::cout << "コンテナサイズ: " << fixedContainer.size() 
              << "/" << fixedContainer.maxSize() << std::endl;
    
    for (size_t i = 0; i < fixedContainer.size(); ++i)
    {
        std::cout << "アイテム " << i << ": " << fixedContainer[i] << std::endl;
    }
    
    // 10. パフォーマンス測定
    std::cout << "\n10. パフォーマンス測定:" << std::endl;
    
    measureExecutionTime([]() {
        volatile int sum = 0;
        for (int i = 0; i < 10000; ++i)
        {
            sum += i;
        }
    }, "ループ処理");
    
    auto [result, duration] = measureExecutionTime([]() {
        return std::string("テスト結果");
    }, "文字列作成");
    
    std::cout << "戻り値: " << result << std::endl;
    
    return 0;
}

/*
 * テンプレートベストプラクティス まとめ:
 * 
 * 1. 明確で意味のある命名を使用
 * 2. static_assertで制約を明確に
 * 3. デフォルト引数で使いやすさを向上
 * 4. 型推定を活用してコードを簡潔に
 * 5. 完全転送で効率的な転送を実現
 * 6. ヘッダと実装の適切な分離
 * 7. エラーメッセージを改善
 * 8. パフォーマンスを考慮した設計
 * 9. テストとデバッグを容易に
 * 10. 詳細なドキュメント化
 * 
 * これらのプラクティスにより、保守しやすく
 * 効率的なテンプレートコードが書けます。
 */