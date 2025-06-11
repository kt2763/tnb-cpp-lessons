/*
 * テンプレート第5講: テンプレート引数推定
 * 
 * テンプレート引数推定は、コンパイラが自動的にテンプレートパラメータの型を
 * 推定する機能です。C++17のCTAD（クラステンプレート引数推定）により
 * さらに強力になり、ゲーム開発でのコードの簡潔性が向上しました。
 */

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <map>

// 基本的な関数テンプレートの引数推定
template<typename T>
void displayGameValue(const T& value)
{
    std::cout << "ゲーム値: " << value << " (型推定完了)" << std::endl;
}

template<typename T, typename U>
auto calculateGameStat(const T& base, const U& modifier) -> decltype(base + modifier)
{
    auto result = base + modifier;
    std::cout << "計算結果: " << result << " (型: " << typeid(result).name() << ")" << std::endl;
    return result;
}

// C++14のauto戻り値型推定
template<typename T, typename U>
auto multiplyStats(const T& stat1, const U& stat2)
{
    return stat1 * stat2;
}

// 引数推定を活用したゲームペア
template<typename T, typename U>
class GamePair
{
public:
    T first;
    U second;
    
    GamePair(const T& f, const U& s) : first(f), second(s) {}
    
    void display() const
    {
        std::cout << "ペア: (" << first << ", " << second << ")" << std::endl;
    }
};

// C++17 CTAD用の推定ガイド
template<typename T, typename U>
GamePair(T, U) -> GamePair<T, U>;

// より複雑な例: ゲームコンテナ
template<typename T>
class GameContainer
{
private:
    std::vector<T> items;
    
public:
    GameContainer() = default;
    
    // イニシャライザリストコンストラクタ
    GameContainer(std::initializer_list<T> init) : items(init) {}
    
    // 単一要素コンストラクタ
    explicit GameContainer(const T& item) : items{item} {}
    
    void add(const T& item)
    {
        items.push_back(item);
    }
    
    void display() const
    {
        std::cout << "=== ゲームコンテナ ===" << std::endl;
        for (size_t i = 0; i < items.size(); ++i)
        {
            std::cout << "[" << i << "] " << items[i] << std::endl;
        }
    }
    
    size_t size() const { return items.size(); }
};

// CTAD用の推定ガイド
template<typename T>
GameContainer(std::initializer_list<T>) -> GameContainer<T>;

template<typename T>
GameContainer(T) -> GameContainer<T>;

// 可変長テンプレートでの引数推定
template<typename... Args>
auto createGameTuple(Args&&... args)
{
    return std::make_tuple(std::forward<Args>(args)...);
}

// 完全転送を使った関数
template<typename T, typename... Args>
std::unique_ptr<T> makeGameObject(Args&&... args)
{
    std::cout << "ゲームオブジェクト作成中..." << std::endl;
    return std::make_unique<T>(std::forward<Args>(args)...);
}

// ゲームプレイヤークラス
class GamePlayer
{
private:
    std::string name;
    int level;
    double health;
    
public:
    GamePlayer(const std::string& n, int l, double h) : name(n), level(l), health(h) {}
    
    void display() const
    {
        std::cout << "プレイヤー: " << name << " (Lv." << level << ", HP:" << health << ")" << std::endl;
    }
};

// 型推定を活用したファクトリ関数
template<typename T>
auto createGameItem(const T& item)
{
    return GameContainer{item}; // CTAD活用
}

// 複雑な型推定の例
template<typename Func, typename... Args>
auto invokeGameFunction(Func&& func, Args&&... args) -> decltype(func(args...))
{
    std::cout << "ゲーム関数を実行中..." << std::endl;
    return func(std::forward<Args>(args)...);
}

// ラムダとの組み合わせ
auto createGameCalculator()
{
    return [](auto x, auto y) {
        std::cout << "計算: " << x << " + " << y << " = ";
        auto result = x + y;
        std::cout << result << std::endl;
        return result;
    };
}

// C++20のrequires句との組み合わせ（C++20対応コンパイラで有効）
#if __cplusplus >= 202002L
template<typename T>
concept Numeric = std::is_arithmetic_v<T>;

template<Numeric T>
auto calculateDamage(const T& base, const T& multiplier)
{
    return base * multiplier;
}
#endif

int main()
{
    std::cout << "=== テンプレート引数推定のデモ ===" << std::endl;
    
    // 1. 基本的な関数テンプレート引数推定
    std::cout << "\n1. 基本的な引数推定:" << std::endl;
    displayGameValue(42);           // int型と推定
    displayGameValue(3.14);         // double型と推定
    displayGameValue("Hello");      // const char*型と推定
    displayGameValue(std::string("World")); // std::string型と推定
    
    // 2. 複数の型パラメータの推定
    std::cout << "\n2. 複数型パラメータの推定:" << std::endl;
    calculateGameStat(100, 25);     // int, int
    calculateGameStat(100.5, 25);   // double, int -> double
    calculateGameStat(100, 25.5);   // int, double -> double
    
    auto result1 = multiplyStats(5, 2.5);
    std::cout << "乗算結果: " << result1 << std::endl;
    
    // 3. C++17 CTAD（クラステンプレート引数推定）
    std::cout << "\n3. クラステンプレート引数推定 (CTAD):" << std::endl;
    GamePair pair1{10, 20};                    // GamePair<int, int>
    GamePair pair2{"攻撃力", 150};             // GamePair<const char*, int>
    GamePair pair3{3.14, std::string("円周率")}; // GamePair<double, std::string>
    
    pair1.display();
    pair2.display();
    pair3.display();
    
    // 4. コンテナでのCTAD
    std::cout << "\n4. コンテナでのCTAD:" << std::endl;
    GameContainer weapons{"剣", "斧", "弓"};    // GameContainer<const char*>
    GameContainer levels{1, 5, 10, 15};        // GameContainer<int>
    GameContainer singleWeapon{"特別な剣"};     // GameContainer<const char*>
    
    weapons.display();
    levels.display();
    singleWeapon.display();
    
    // 5. 標準ライブラリのCTAD
    std::cout << "\n5. 標準ライブラリのCTAD:" << std::endl;
    std::vector playerStats{100, 75, 50, 25};  // std::vector<int>
    std::pair playerInfo{"アリス", 25};         // std::pair<const char*, int>
    
    std::cout << "プレイヤー情報: " << playerInfo.first << ", レベル " << playerInfo.second << std::endl;
    std::cout << "ステータス数: " << playerStats.size() << std::endl;
    
    // 6. 可変長テンプレートでの推定
    std::cout << "\n6. 可変長テンプレートでの推定:" << std::endl;
    auto gameTuple = createGameTuple("プレイヤー", 42, 100.5, true);
    // std::tuple<const char*, int, double, bool>
    
    std::cout << "タプル要素: " 
              << std::get<0>(gameTuple) << ", "
              << std::get<1>(gameTuple) << ", "
              << std::get<2>(gameTuple) << ", "
              << std::get<3>(gameTuple) << std::endl;
    
    // 7. 完全転送での型推定
    std::cout << "\n7. 完全転送での型推定:" << std::endl;
    auto player = makeGameObject<GamePlayer>("ボブ", 30, 95.5);
    player->display();
    
    // 8. ファクトリ関数での推定
    std::cout << "\n8. ファクトリ関数での推定:" << std::endl;
    auto itemContainer = createGameItem(std::string("魔法の薬"));
    itemContainer.display();
    
    // 9. 関数オブジェクトでの推定
    std::cout << "\n9. 関数オブジェクトでの推定:" << std::endl;
    auto calculator = createGameCalculator();
    auto calcResult = calculator(10, 20);
    auto calcResult2 = calculator(5.5, 2.3);
    
    // 10. ラムダでの型推定
    std::cout << "\n10. ラムダでの型推定:" << std::endl;
    auto gameProcessor = [](auto&& container) {
        std::cout << "コンテナ処理中... サイズ: " << container.size() << std::endl;
        return container.size();
    };
    
    gameProcessor(weapons);
    gameProcessor(levels);
    gameProcessor(playerStats);
    
    // 11. 複雑な関数での推定
    std::cout << "\n11. 複雑な関数での推定:" << std::endl;
    auto damageCalc = [](int base, double multiplier) {
        return static_cast<int>(base * multiplier);
    };
    
    auto damage = invokeGameFunction(damageCalc, 50, 1.5);
    std::cout << "計算されたダメージ: " << damage << std::endl;
    
#if __cplusplus >= 202002L
    // C++20のコンセプトでの推定
    std::cout << "\n12. C++20 コンセプトでの推定:" << std::endl;
    auto dmg1 = calculateDamage(100, 1.5);
    auto dmg2 = calculateDamage(75, 2);
    std::cout << "ダメージ1: " << dmg1 << ", ダメージ2: " << dmg2 << std::endl;
#endif
    
    return 0;
}

/*
 * 学習ポイント:
 * 1. 関数テンプレートは引数から自動的に型推定される
 * 2. C++17のCTADでクラステンプレートも型推定可能
 * 3. 推定ガイドで推定ルールをカスタマイズ可能
 * 4. autoキーワードで戻り値型も推定可能
 * 5. 可変長テンプレートでも推定が働く
 * 6. 完全転送で型を保持したまま推定
 * 7. ゲーム開発では型推定でコードの簡潔性向上
 * 8. 型推定により保守性とパフォーマンスを両立
 */