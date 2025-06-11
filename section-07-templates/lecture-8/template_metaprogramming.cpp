/*
 * テンプレート第8講: テンプレートメタプログラミングの基本
 * 
 * テンプレートメタプログラミング（TMP）は、コンパイル時に計算や型操作を行う技法です。
 * 実行時のオーバーヘッドなしで複雑な処理を実現でき、
 * ゲーム開発では高性能な数学計算やデータ構造最適化に活用されます。
 */

#include <iostream>
#include <string>
#include <type_traits>
#include <array>
#include <chrono>

// 1. 基本的なコンパイル時計算 - 階乗
template<int N>
struct Factorial
{
    static constexpr int value = N * Factorial<N - 1>::value;
};

template<>
struct Factorial<0>
{
    static constexpr int value = 1;
};

// C++14以降のconstexpr関数版
constexpr int factorial(int n)
{
    return (n <= 1) ? 1 : n * factorial(n - 1);
}

// 2. コンパイル時べき乗計算（ゲーム用）
template<int Base, int Exponent>
struct Power
{
    static constexpr int value = Base * Power<Base, Exponent - 1>::value;
};

template<int Base>
struct Power<Base, 0>
{
    static constexpr int value = 1;
};

// 3. 型リスト操作
template<typename... Types>
struct TypeList {};

// 型リストのサイズ
template<typename TypeList>
struct TypeListSize;

template<typename... Types>
struct TypeListSize<TypeList<Types...>>
{
    static constexpr size_t value = sizeof...(Types);
};

// 型リストの先頭を取得
template<typename TypeList>
struct TypeListHead;

template<typename Head, typename... Tail>
struct TypeListHead<TypeList<Head, Tail...>>
{
    using type = Head;
};

// N番目の型を取得
template<size_t Index, typename TypeList>
struct TypeAt;

template<size_t Index, typename Head, typename... Tail>
struct TypeAt<Index, TypeList<Head, Tail...>>
{
    using type = typename TypeAt<Index - 1, TypeList<Tail...>>::type;
};

template<typename Head, typename... Tail>
struct TypeAt<0, TypeList<Head, Tail...>>
{
    using type = Head;
};

// 4. ゲーム用のコンパイル時配列生成
template<size_t N>
constexpr std::array<int, N> generateLevelExperience()
{
    std::array<int, N> exp{};
    for (size_t i = 0; i < N; ++i)
    {
        exp[i] = (i + 1) * 100 + (i + 1) * (i + 1) * 10; // レベル式: 100*level + 10*level^2
    }
    return exp;
}

// 5. コンパイル時文字列操作
template<size_t N>
struct ConstString
{
    constexpr ConstString(const char (&str)[N])
    {
        for (size_t i = 0; i < N; ++i)
        {
            data[i] = str[i];
        }
    }
    
    constexpr char operator[](size_t i) const { return data[i]; }
    constexpr size_t size() const { return N - 1; } // null終端を除く
    
    char data[N];
};

template<ConstString str>
constexpr auto reverseString()
{
    constexpr size_t len = str.size();
    std::array<char, len + 1> result{};
    
    for (size_t i = 0; i < len; ++i)
    {
        result[i] = str[len - 1 - i];
    }
    result[len] = '\0';
    
    return result;
}

// 6. 型特性の活用 - ゲームオブジェクト判定
template<typename T>
struct IsGameNumeric
{
    static constexpr bool value = std::is_arithmetic_v<T>;
};

template<typename T>
constexpr bool IsGameNumeric_v = IsGameNumeric<T>::value;

// カスタム型特性
template<typename T>
struct IsGameString : std::false_type {};

template<>
struct IsGameString<std::string> : std::true_type {};

template<>
struct IsGameString<const char*> : std::true_type {};

template<size_t N>
struct IsGameString<char[N]> : std::true_type {};

template<typename T>
constexpr bool IsGameString_v = IsGameString<T>::value;

// 7. SFINAE + メタプログラミングでの条件付きメンバ
template<typename T, bool IsNumeric = IsGameNumeric_v<T>>
class GameValue
{
private:
    T value;
    
public:
    GameValue(const T& v) : value(v) {}
    
    T getValue() const { return value; }
    
    // 数値型の場合のみ利用可能なメソッド
    template<typename U = T>
    std::enable_if_t<IsGameNumeric_v<U>, U> multiply(U factor) const
    {
        return value * factor;
    }
    
    // 文字列型の場合のみ利用可能なメソッド
    template<typename U = T>
    std::enable_if_t<IsGameString_v<U>, std::string> toUpper() const
    {
        std::string result = value;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }
};

// 8. コンパイル時ゲームマップ生成
template<size_t Width, size_t Height>
constexpr std::array<std::array<int, Width>, Height> generateGameMap()
{
    std::array<std::array<int, Width>, Height> map{};
    
    for (size_t y = 0; y < Height; ++y)
    {
        for (size_t x = 0; x < Width; ++x)
        {
            // 境界は壁（1）、内部は通路（0）
            if (x == 0 || x == Width - 1 || y == 0 || y == Height - 1)
            {
                map[y][x] = 1; // 壁
            }
            else
            {
                map[y][x] = 0; // 通路
            }
        }
    }
    
    return map;
}

// 9. 再帰的テンプレートでのフィボナッチ数列
template<int N>
struct Fibonacci
{
    static constexpr int value = Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
};

template<>
struct Fibonacci<0>
{
    static constexpr int value = 0;
};

template<>
struct Fibonacci<1>
{
    static constexpr int value = 1;
};

// 10. 可変長テンプレートでの型変換チェック
template<typename Target, typename... Sources>
struct AllConvertibleTo
{
    static constexpr bool value = (std::is_convertible_v<Sources, Target> && ...);
};

template<typename Target, typename... Sources>
constexpr bool AllConvertibleTo_v = AllConvertibleTo<Target, Sources...>::value;

// 11. コンパイル時最大値/最小値
template<int... Values>
struct MaxValue
{
    static constexpr int value = std::max({Values...});
};

template<int... Values>
struct MinValue
{
    static constexpr int value = std::min({Values...});
};

// 12. ゲーム用のコンパイル時バイト操作
template<uint32_t Value>
struct ByteSwap
{
    static constexpr uint32_t value = 
        ((Value & 0xFF000000) >> 24) |
        ((Value & 0x00FF0000) >> 8)  |
        ((Value & 0x0000FF00) << 8)  |
        ((Value & 0x000000FF) << 24);
};

// 13. 条件付きコンパイル時処理
template<bool Condition, typename TrueType, typename FalseType>
struct ConditionalType
{
    using type = TrueType;
};

template<typename TrueType, typename FalseType>
struct ConditionalType<false, TrueType, FalseType>
{
    using type = FalseType;
};

template<bool Condition, typename TrueType, typename FalseType>
using ConditionalType_t = typename ConditionalType<Condition, TrueType, FalseType>::type;

// パフォーマンス測定用のヘルパー
template<typename Func>
auto measureTime(Func&& func)
{
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

int main()
{
    std::cout << "=== テンプレートメタプログラミングのデモ ===" << std::endl;
    
    // 1. コンパイル時計算
    std::cout << "\n1. コンパイル時計算:" << std::endl;
    constexpr int fact5 = Factorial<5>::value;
    constexpr int fact10 = factorial(10);
    constexpr int power23 = Power<2, 3>::value;
    
    std::cout << "5! = " << fact5 << std::endl;
    std::cout << "10! = " << fact10 << std::endl;
    std::cout << "2^3 = " << power23 << std::endl;
    
    // 2. 型リスト操作
    std::cout << "\n2. 型リスト操作:" << std::endl;
    using GameTypes = TypeList<int, double, std::string, bool>;
    
    constexpr size_t typeCount = TypeListSize<GameTypes>::value;
    std::cout << "型リストサイズ: " << typeCount << std::endl;
    
    using FirstType = TypeListHead<GameTypes>::type;
    using SecondType = TypeAt<1, GameTypes>::type;
    
    std::cout << "最初の型はint: " << std::is_same_v<FirstType, int> << std::endl;
    std::cout << "2番目の型はdouble: " << std::is_same_v<SecondType, double> << std::endl;
    
    // 3. コンパイル時配列生成
    std::cout << "\n3. コンパイル時レベル経験値テーブル:" << std::endl;
    constexpr auto expTable = generateLevelExperience<10>();
    
    for (size_t i = 0; i < 5; ++i)
    {
        std::cout << "レベル " << (i + 1) << ": " << expTable[i] << " EXP" << std::endl;
    }
    
    // 4. コンパイル時文字列操作
    std::cout << "\n4. コンパイル時文字列操作:" << std::endl;
    constexpr ConstString gameTitle{"ADVENTURE"};
    constexpr auto reversed = reverseString<gameTitle>();
    
    std::cout << "元の文字列: ";
    for (size_t i = 0; i < gameTitle.size(); ++i)
    {
        std::cout << gameTitle[i];
    }
    std::cout << std::endl;
    
    std::cout << "逆順: " << reversed.data() << std::endl;
    
    // 5. カスタム型特性
    std::cout << "\n5. カスタム型特性:" << std::endl;
    std::cout << "intは数値型: " << IsGameNumeric_v<int> << std::endl;
    std::cout << "stringは数値型: " << IsGameNumeric_v<std::string> << std::endl;
    std::cout << "stringは文字列型: " << IsGameString_v<std::string> << std::endl;
    std::cout << "intは文字列型: " << IsGameString_v<int> << std::endl;
    
    // 6. 条件付きメンバ関数
    std::cout << "\n6. 条件付きメンバ関数:" << std::endl;
    GameValue<int> numValue(42);
    GameValue<std::string> strValue("hello");
    
    std::cout << "数値の2倍: " << numValue.multiply(2) << std::endl;
    std::cout << "文字列を大文字に: " << strValue.toUpper() << std::endl;
    
    // 7. コンパイル時マップ生成
    std::cout << "\n7. コンパイル時ゲームマップ:" << std::endl;
    constexpr auto gameMap = generateGameMap<5, 4>();
    
    for (size_t y = 0; y < 4; ++y)
    {
        for (size_t x = 0; x < 5; ++x)
        {
            std::cout << (gameMap[y][x] ? '#' : '.') << ' ';
        }
        std::cout << std::endl;
    }
    
    // 8. フィボナッチ数列
    std::cout << "\n8. フィボナッチ数列（コンパイル時計算）:" << std::endl;
    constexpr int fib10 = Fibonacci<10>::value;
    std::cout << "フィボナッチ(10) = " << fib10 << std::endl;
    
    // 9. 型変換チェック
    std::cout << "\n9. 型変換チェック:" << std::endl;
    constexpr bool allToDouble = AllConvertibleTo_v<double, int, float, char>;
    constexpr bool allToString = AllConvertibleTo_v<std::string, int, float>;
    
    std::cout << "int,float,charはdoubleに変換可能: " << allToDouble << std::endl;
    std::cout << "int,floatはstringに変換可能: " << allToString << std::endl;
    
    // 10. 最大値/最小値
    std::cout << "\n10. コンパイル時最大値/最小値:" << std::endl;
    constexpr int maxVal = MaxValue<10, 25, 5, 30, 15>::value;
    constexpr int minVal = MinValue<10, 25, 5, 30, 15>::value;
    
    std::cout << "最大値: " << maxVal << std::endl;
    std::cout << "最小値: " << minVal << std::endl;
    
    // 11. バイト操作
    std::cout << "\n11. コンパイル時バイト操作:" << std::endl;
    constexpr uint32_t original = 0x12345678;
    constexpr uint32_t swapped = ByteSwap<original>::value;
    
    std::cout << "元の値: 0x" << std::hex << original << std::endl;
    std::cout << "バイトスワップ後: 0x" << swapped << std::dec << std::endl;
    
    // 12. 条件付き型選択
    std::cout << "\n12. 条件付き型選択:" << std::endl;
    using FastType = ConditionalType_t<true, int, double>;
    using SlowType = ConditionalType_t<false, int, double>;
    
    std::cout << "FastTypeはint: " << std::is_same_v<FastType, int> << std::endl;
    std::cout << "SlowTypeはdouble: " << std::is_same_v<SlowType, double> << std::endl;
    
    // 13. パフォーマンス比較
    std::cout << "\n13. パフォーマンス比較（コンパイル時 vs 実行時）:" << std::endl;
    
    // 実行時計算
    auto runtimeTime = measureTime([]() {
        volatile int result = 1;
        for (int i = 1; i <= 10; ++i)
        {
            result *= i;
        }
    });
    
    // コンパイル時計算（実際は実行時に値を参照するだけ）
    auto compileTime = measureTime([]() {
        volatile int result = Factorial<10>::value;
    });
    
    std::cout << "実行時計算: " << runtimeTime << " マイクロ秒" << std::endl;
    std::cout << "コンパイル時計算結果の参照: " << compileTime << " マイクロ秒" << std::endl;
    
    return 0;
}

/*
 * 学習ポイント:
 * 1. constexprでコンパイル時計算が可能
 * 2. テンプレート特殊化で再帰的な計算を実現
 * 3. 型リストで型レベルでのデータ構造操作
 * 4. SFINAEと組み合わせて条件付きメンバ関数
 * 5. コンパイル時にデータ構造を生成可能
 * 6. 実行時オーバーヘッドゼロで複雑な処理
 * 7. ゲーム開発では設定値やテーブルの事前計算に活用
 * 8. メタプログラミングでパフォーマンス最適化を実現
 */