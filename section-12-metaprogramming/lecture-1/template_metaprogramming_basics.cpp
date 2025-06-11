// テンプレートメタプログラミングの基礎
// コンパイル時に計算やコード生成を行う強力な技法

#include <iostream>
#include <type_traits>
#include <array>
#include <cmath>

// コンパイル時定数の計算
template<int N>
struct Factorial
{
    static constexpr int value = N * Factorial<N - 1>::value;
};

// 再帰の終了条件（特殊化）
template<>
struct Factorial<0>
{
    static constexpr int value = 1;
};

// C++14以降はconstexpr関数で簡潔に
constexpr int factorial(int n)
{
    return (n == 0) ? 1 : n * factorial(n - 1);
}

// フィボナッチ数列のコンパイル時計算
template<int N>
struct Fibonacci
{
    static constexpr int value = Fibonacci<N-1>::value + Fibonacci<N-2>::value;
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

// ゲームでの応用：経験値テーブルの生成
template<int Level>
struct ExperienceTable
{
    static constexpr int value = Level * Level * 100 + ExperienceTable<Level-1>::value;
};

template<>
struct ExperienceTable<1>
{
    static constexpr int value = 100;
};

// コンパイル時の型選択
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

// ゲームでの使用例：プラットフォーム依存の型選択
template<int PlatformBits>
struct GameCoordinate
{
    using type = typename ConditionalType<
        (PlatformBits >= 64),
        double,
        float
    >::type;
};

// コンパイル時のループ展開
template<int N>
struct DamageCalculator
{
    template<typename Func>
    static int calculate(int baseDamage, Func modifier)
    {
        return modifier(baseDamage) + DamageCalculator<N-1>::calculate(baseDamage, modifier);
    }
};

template<>
struct DamageCalculator<0>
{
    template<typename Func>
    static int calculate(int, Func)
    {
        return 0;
    }
};

// タイプリストの実装
template<typename... Types>
struct TypeList {};

// タイプリストの操作
template<typename List>
struct Length;

template<typename... Types>
struct Length<TypeList<Types...>>
{
    static constexpr size_t value = sizeof...(Types);
};

// 最初の型を取得
template<typename List>
struct Head;

template<typename First, typename... Rest>
struct Head<TypeList<First, Rest...>>
{
    using type = First;
};

// ゲームエンティティの型リスト
using GameEntities = TypeList<
    struct Player,
    struct Enemy,
    struct NPC,
    struct Item,
    struct Projectile
>;

// コンパイル時の条件分岐
template<int Level>
struct SkillUnlock
{
    static constexpr const char* getUnlockedSkill()
    {
        if constexpr (Level >= 30)
        {
            return "究極魔法：メテオストーム";
        }
        else if constexpr (Level >= 20)
        {
            return "上級魔法：サンダーボルト";
        }
        else if constexpr (Level >= 10)
        {
            return "中級魔法：ファイアボール";
        }
        else
        {
            return "初級魔法：マジックミサイル";
        }
    }
};

// SFINAE（Substitution Failure Is Not An Error）の基礎
template<typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
calculateDamage(T attack, T defense)
{
    return attack - defense / 2;
}

template<typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type
calculateDamage(T attack, T defense)
{
    return attack * 1.5f - defense * 0.5f;
}

// コンパイル時の配列生成
template<size_t N>
struct PowerOfTwoArray
{
    static constexpr std::array<int, N> generate()
    {
        std::array<int, N> arr{};
        for (size_t i = 0; i < N; ++i)
        {
            arr[i] = 1 << i;  // 2のi乗
        }
        return arr;
    }
};

// 再帰的なテンプレート構造
template<int HP>
struct Boss
{
    static constexpr int health = HP;
    static constexpr int phase = (HP > 5000) ? 3 : (HP > 2000) ? 2 : 1;
    
    static void displayInfo()
    {
        std::cout << "ボスHP: " << health << " (フェーズ" << phase << ")" << std::endl;
    }
};

// 型特性の検出
template<typename T>
struct has_level_member
{
private:
    template<typename U>
    static auto test(int) -> decltype(std::declval<U>().level, std::true_type{});
    
    template<typename>
    static std::false_type test(...);
    
public:
    static constexpr bool value = decltype(test<T>(0))::value;
};

// テスト用のクラス
struct Hero
{
    int level = 25;
    int health = 100;
};

struct Monster
{
    int health = 50;
    // levelメンバーなし
};

// 実践的な例：ゲームの設定値管理
template<int Difficulty>
struct GameSettings
{
    static constexpr float enemyHealthMultiplier = 1.0f + (Difficulty * 0.5f);
    static constexpr float enemyDamageMultiplier = 1.0f + (Difficulty * 0.3f);
    static constexpr int experienceBonus = Difficulty * 20;
    
    static constexpr const char* getDifficultyName()
    {
        if constexpr (Difficulty == 0) return "イージー";
        else if constexpr (Difficulty == 1) return "ノーマル";
        else if constexpr (Difficulty == 2) return "ハード";
        else if constexpr (Difficulty == 3) return "ナイトメア";
        else return "カスタム";
    }
};

int main()
{
    std::cout << "=== テンプレートメタプログラミングの基礎 ===" << std::endl;
    
    // 1. コンパイル時の計算
    std::cout << "\n--- コンパイル時計算 ---" << std::endl;
    std::cout << "5! = " << Factorial<5>::value << std::endl;
    std::cout << "10番目のフィボナッチ数: " << Fibonacci<10>::value << std::endl;
    std::cout << "レベル10の累積経験値: " << ExperienceTable<10>::value << std::endl;
    
    // 2. 型の選択
    std::cout << "\n--- 型の選択 ---" << std::endl;
    using Coord32 = GameCoordinate<32>::type;
    using Coord64 = GameCoordinate<64>::type;
    std::cout << "32ビット座標型: " << typeid(Coord32).name() << std::endl;
    std::cout << "64ビット座標型: " << typeid(Coord64).name() << std::endl;
    
    // 3. ループ展開
    std::cout << "\n--- コンパイル時ループ展開 ---" << std::endl;
    auto criticalModifier = [](int damage) { return damage * 2; };
    int totalDamage = DamageCalculator<3>::calculate(50, criticalModifier);
    std::cout << "3ヒットの総ダメージ: " << totalDamage << std::endl;
    
    // 4. タイプリスト
    std::cout << "\n--- タイプリスト ---" << std::endl;
    std::cout << "ゲームエンティティ数: " << Length<GameEntities>::value << std::endl;
    
    // 5. 条件付きコンパイル
    std::cout << "\n--- スキルアンロック ---" << std::endl;
    std::cout << "Lv5: " << SkillUnlock<5>::getUnlockedSkill() << std::endl;
    std::cout << "Lv15: " << SkillUnlock<15>::getUnlockedSkill() << std::endl;
    std::cout << "Lv25: " << SkillUnlock<25>::getUnlockedSkill() << std::endl;
    std::cout << "Lv35: " << SkillUnlock<35>::getUnlockedSkill() << std::endl;
    
    // 6. SFINAE
    std::cout << "\n--- SFINAE によるオーバーロード ---" << std::endl;
    std::cout << "整数ダメージ計算: " << calculateDamage(100, 30) << std::endl;
    std::cout << "浮動小数点ダメージ計算: " << calculateDamage(100.0f, 30.0f) << std::endl;
    
    // 7. コンパイル時配列
    std::cout << "\n--- コンパイル時配列生成 ---" << std::endl;
    constexpr auto powers = PowerOfTwoArray<8>::generate();
    std::cout << "2の累乗: ";
    for (auto p : powers)
    {
        std::cout << p << " ";
    }
    std::cout << std::endl;
    
    // 8. ボス情報
    std::cout << "\n--- ボス情報 ---" << std::endl;
    Boss<1000>::displayInfo();
    Boss<3000>::displayInfo();
    Boss<6000>::displayInfo();
    
    // 9. 型特性の検出
    std::cout << "\n--- メンバー検出 ---" << std::endl;
    std::cout << "Hero has level: " << has_level_member<Hero>::value << std::endl;
    std::cout << "Monster has level: " << has_level_member<Monster>::value << std::endl;
    
    // 10. ゲーム設定
    std::cout << "\n--- 難易度設定 ---" << std::endl;
    using NormalMode = GameSettings<1>;
    using HardMode = GameSettings<2>;
    
    std::cout << "ノーマルモード:" << std::endl;
    std::cout << "  難易度名: " << NormalMode::getDifficultyName() << std::endl;
    std::cout << "  敵HP倍率: " << NormalMode::enemyHealthMultiplier << std::endl;
    std::cout << "  経験値ボーナス: " << NormalMode::experienceBonus << std::endl;
    
    std::cout << "ハードモード:" << std::endl;
    std::cout << "  難易度名: " << HardMode::getDifficultyName() << std::endl;
    std::cout << "  敵HP倍率: " << HardMode::enemyHealthMultiplier << std::endl;
    std::cout << "  経験値ボーナス: " << HardMode::experienceBonus << std::endl;
    
    return 0;
}

// まとめ：
// - テンプレートメタプログラミングでコンパイル時に計算を実行
// - 再帰的なテンプレートで複雑な処理を実現
// - 型の選択や条件分岐もコンパイル時に決定
// - SFINAEで型に応じた関数選択
// - ゲーム開発では設定値の管理や最適化に活用