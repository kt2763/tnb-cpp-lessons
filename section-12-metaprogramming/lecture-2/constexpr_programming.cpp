// constexprプログラミング
// コンパイル時計算を現代的な方法で実現

#include <iostream>
#include <array>
#include <string_view>
#include <algorithm>
#include <numeric>
#include <cmath>

// constexpr関数の基本
constexpr int square(int x)
{
    return x * x;
}

// より複雑なconstexpr関数
constexpr int fibonacci(int n)
{
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// C++14以降：ループも使用可能
constexpr int factorial(int n)
{
    int result = 1;
    for (int i = 2; i <= n; ++i)
    {
        result *= i;
    }
    return result;
}

// ゲームの経験値計算
class ExperienceCalculator
{
public:
    static constexpr int baseExp = 100;
    static constexpr float growthRate = 1.5f;
    
    // レベルアップに必要な経験値
    static constexpr int requiredExp(int level)
    {
        if (level <= 1) return baseExp;
        
        float exp = baseExp;
        for (int i = 2; i <= level; ++i)
        {
            exp *= growthRate;
        }
        return static_cast<int>(exp);
    }
    
    // 累積経験値
    static constexpr int totalExp(int level)
    {
        int total = 0;
        for (int i = 1; i <= level; ++i)
        {
            total += requiredExp(i);
        }
        return total;
    }
};

// constexprクラス
class Point2D
{
private:
    float x_, y_;
    
public:
    constexpr Point2D(float x = 0, float y = 0) : x_(x), y_(y) {}
    
    constexpr float x() const { return x_; }
    constexpr float y() const { return y_; }
    
    constexpr Point2D operator+(const Point2D& other) const
    {
        return Point2D(x_ + other.x_, y_ + other.y_);
    }
    
    constexpr Point2D operator*(float scalar) const
    {
        return Point2D(x_ * scalar, y_ * scalar);
    }
    
    constexpr float distance() const
    {
        // C++26でstd::sqrtがconstexprになる予定
        // 現在は独自実装が必要
        return sqrt_impl(x_ * x_ + y_ * y_);
    }
    
private:
    // 簡易的な平方根計算（Newton法）
    static constexpr float sqrt_impl(float x)
    {
        if (x < 0) return 0;
        if (x == 0) return 0;
        
        float guess = x;
        float epsilon = 0.00001f;
        
        while (true)
        {
            float next = (guess + x / guess) / 2;
            if (abs_impl(next - guess) < epsilon)
                return next;
            guess = next;
        }
    }
    
    static constexpr float abs_impl(float x)
    {
        return x < 0 ? -x : x;
    }
};

// constexprコンテナ操作
template<typename T, size_t N>
class ConstexprArray
{
private:
    T data_[N];
    
public:
    constexpr ConstexprArray() : data_{} {}
    
    constexpr T& operator[](size_t i) { return data_[i]; }
    constexpr const T& operator[](size_t i) const { return data_[i]; }
    
    constexpr size_t size() const { return N; }
    
    // ソート（バブルソート実装）
    constexpr void sort()
    {
        for (size_t i = 0; i < N - 1; ++i)
        {
            for (size_t j = 0; j < N - i - 1; ++j)
            {
                if (data_[j] > data_[j + 1])
                {
                    T temp = data_[j];
                    data_[j] = data_[j + 1];
                    data_[j + 1] = temp;
                }
            }
        }
    }
    
    // 合計値
    constexpr T sum() const
    {
        T result = T{};
        for (size_t i = 0; i < N; ++i)
        {
            result += data_[i];
        }
        return result;
    }
};

// ゲームのステータス計算
class GameStats
{
public:
    struct CharacterStats
    {
        int strength;
        int agility;
        int intelligence;
        
        constexpr int getAttackPower() const
        {
            return strength * 2 + agility;
        }
        
        constexpr int getDefense() const
        {
            return strength + agility / 2;
        }
        
        constexpr int getMagicPower() const
        {
            return intelligence * 3;
        }
    };
    
    // 装備によるステータス補正
    static constexpr CharacterStats applyEquipment(
        const CharacterStats& base,
        const CharacterStats& equipment)
    {
        return {
            base.strength + equipment.strength,
            base.agility + equipment.agility,
            base.intelligence + equipment.intelligence
        };
    }
    
    // レベルによるステータス成長
    static constexpr CharacterStats calculateStats(int level)
    {
        return {
            10 + level * 3,      // strength
            8 + level * 2,       // agility  
            5 + level * 4        // intelligence
        };
    }
};

// constexpr文字列操作
class ConstexprString
{
private:
    static constexpr size_t MAX_SIZE = 256;
    char data_[MAX_SIZE];
    size_t length_;
    
public:
    constexpr ConstexprString() : data_{}, length_(0) {}
    
    constexpr ConstexprString(const char* str) : data_{}, length_(0)
    {
        while (str[length_] && length_ < MAX_SIZE - 1)
        {
            data_[length_] = str[length_];
            ++length_;
        }
        data_[length_] = '\0';
    }
    
    constexpr size_t length() const { return length_; }
    constexpr const char* c_str() const { return data_; }
    
    // 文字列連結
    constexpr ConstexprString operator+(const ConstexprString& other) const
    {
        ConstexprString result;
        
        // 最初の文字列をコピー
        for (size_t i = 0; i < length_ && result.length_ < MAX_SIZE - 1; ++i)
        {
            result.data_[result.length_++] = data_[i];
        }
        
        // 二番目の文字列を追加
        for (size_t i = 0; i < other.length_ && result.length_ < MAX_SIZE - 1; ++i)
        {
            result.data_[result.length_++] = other.data_[i];
        }
        
        result.data_[result.length_] = '\0';
        return result;
    }
};

// コンパイル時のルックアップテーブル生成
template<size_t N>
struct SineLookupTable
{
    static constexpr float PI = 3.14159265f;
    
    static constexpr std::array<float, N> generate()
    {
        std::array<float, N> table{};
        for (size_t i = 0; i < N; ++i)
        {
            float angle = (2.0f * PI * i) / N;
            table[i] = sin_approx(angle);
        }
        return table;
    }
    
private:
    // Taylor級数による近似
    static constexpr float sin_approx(float x)
    {
        float x3 = x * x * x;
        float x5 = x3 * x * x;
        float x7 = x5 * x * x;
        
        return x - x3 / 6.0f + x5 / 120.0f - x7 / 5040.0f;
    }
};

// if constexprを使った条件分岐
template<typename T>
constexpr auto processValue(T value)
{
    if constexpr (std::is_integral_v<T>)
    {
        return value * 2;  // 整数なら2倍
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        return value * 1.5f;  // 浮動小数点なら1.5倍
    }
    else
    {
        return value;  // その他はそのまま
    }
}

// コンパイル時の検証
template<int MinLevel = 1, int MaxLevel = 99>
class LevelValidator
{
public:
    static constexpr bool isValid(int level)
    {
        return level >= MinLevel && level <= MaxLevel;
    }
    
    static constexpr int clamp(int level)
    {
        if (level < MinLevel) return MinLevel;
        if (level > MaxLevel) return MaxLevel;
        return level;
    }
};

// 実行時とコンパイル時の両方で使える関数
constexpr int damageFormula(int attack, int defense, int level)
{
    int baseDamage = attack - defense / 2;
    int levelBonus = level * 5;
    int totalDamage = baseDamage + levelBonus;
    
    return totalDamage > 0 ? totalDamage : 1;  // 最小1ダメージ
}

int main()
{
    std::cout << "=== constexprプログラミング ===" << std::endl;
    
    // 1. 基本的なconstexpr
    std::cout << "\n--- 基本的なconstexpr ---" << std::endl;
    constexpr int sq = square(5);
    constexpr int fib = fibonacci(10);
    constexpr int fact = factorial(5);
    
    std::cout << "5の二乗: " << sq << std::endl;
    std::cout << "フィボナッチ(10): " << fib << std::endl;
    std::cout << "5!: " << fact << std::endl;
    
    // 2. 経験値計算
    std::cout << "\n--- 経験値計算 ---" << std::endl;
    constexpr int expLv10 = ExperienceCalculator::requiredExp(10);
    constexpr int totalExpLv10 = ExperienceCalculator::totalExp(10);
    
    std::cout << "レベル10に必要な経験値: " << expLv10 << std::endl;
    std::cout << "レベル10までの累積経験値: " << totalExpLv10 << std::endl;
    
    // 3. constexprクラス
    std::cout << "\n--- constexpr座標計算 ---" << std::endl;
    constexpr Point2D p1(3, 4);
    constexpr Point2D p2(1, 2);
    constexpr Point2D p3 = p1 + p2;
    constexpr float dist = p1.distance();
    
    std::cout << "p1 + p2 = (" << p3.x() << ", " << p3.y() << ")" << std::endl;
    std::cout << "p1の距離: " << dist << std::endl;
    
    // 4. constexpr配列
    std::cout << "\n--- constexpr配列操作 ---" << std::endl;
    constexpr auto makeArray = []() {
        ConstexprArray<int, 5> arr;
        arr[0] = 5; arr[1] = 2; arr[2] = 8; arr[3] = 1; arr[4] = 3;
        arr.sort();
        return arr;
    };
    
    constexpr auto sortedArray = makeArray();
    std::cout << "ソート済み配列: ";
    for (size_t i = 0; i < sortedArray.size(); ++i)
    {
        std::cout << sortedArray[i] << " ";
    }
    std::cout << std::endl;
    
    // 5. ゲームステータス
    std::cout << "\n--- ステータス計算 ---" << std::endl;
    constexpr auto heroStats = GameStats::calculateStats(25);
    constexpr GameStats::CharacterStats equipment{10, 5, 8};
    constexpr auto totalStats = GameStats::applyEquipment(heroStats, equipment);
    
    std::cout << "レベル25のステータス:" << std::endl;
    std::cout << "  攻撃力: " << totalStats.getAttackPower() << std::endl;
    std::cout << "  防御力: " << totalStats.getDefense() << std::endl;
    std::cout << "  魔法力: " << totalStats.getMagicPower() << std::endl;
    
    // 6. constexpr文字列
    std::cout << "\n--- constexpr文字列 ---" << std::endl;
    constexpr ConstexprString name("勇者");
    constexpr ConstexprString title("伝説の");
    constexpr auto fullName = title + name;
    
    std::cout << "フルネーム: " << fullName.c_str() << std::endl;
    
    // 7. ルックアップテーブル
    std::cout << "\n--- サイン値テーブル ---" << std::endl;
    constexpr auto sineTable = SineLookupTable<8>::generate();
    std::cout << "コンパイル時生成のサイン値:" << std::endl;
    for (size_t i = 0; i < sineTable.size(); ++i)
    {
        std::cout << "  [" << i * 45 << "°]: " << sineTable[i] << std::endl;
    }
    
    // 8. if constexpr
    std::cout << "\n--- if constexpr ---" << std::endl;
    constexpr auto intResult = processValue(10);
    constexpr auto floatResult = processValue(10.0f);
    
    std::cout << "整数処理: " << intResult << std::endl;
    std::cout << "浮動小数点処理: " << floatResult << std::endl;
    
    // 9. コンパイル時検証
    std::cout << "\n--- レベル検証 ---" << std::endl;
    constexpr int testLevel = 150;
    constexpr bool isValidLevel = LevelValidator<>::isValid(testLevel);
    constexpr int clampedLevel = LevelValidator<>::clamp(testLevel);
    
    std::cout << "レベル" << testLevel << "は有効？: " << isValidLevel << std::endl;
    std::cout << "クランプ後: " << clampedLevel << std::endl;
    
    // 10. 実行時とコンパイル時の両用
    std::cout << "\n--- ダメージ計算 ---" << std::endl;
    constexpr int compileDamage = damageFormula(100, 30, 20);
    std::cout << "コンパイル時計算: " << compileDamage << std::endl;
    
    int runtimeAttack = 120;
    int runtimeDamage = damageFormula(runtimeAttack, 40, 25);
    std::cout << "実行時計算: " << runtimeDamage << std::endl;
    
    return 0;
}

// まとめ：
// - constexprで関数やクラスをコンパイル時評価可能に
// - C++14以降はループや条件分岐も使用可能
// - コンパイル時にテーブルや設定値を生成
// - if constexprで型に応じた処理を効率的に実装
// - 実行時とコンパイル時の両方で使える柔軟な設計