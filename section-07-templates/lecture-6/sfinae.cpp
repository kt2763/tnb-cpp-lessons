/*
 * テンプレート第6講: SFINAE (Substitution Failure Is Not An Error)
 * 
 * SFINAEは「置換の失敗はエラーではない」という原則で、
 * テンプレートの置換に失敗した場合にコンパイルエラーにならず、
 * 候補から除外される仕組みです。ゲーム開発では型に応じた
 * 最適化された処理の選択に活用されます。
 */

#include <iostream>
#include <string>
#include <vector>
#include <type_traits>
#include <memory>

// 基本的なSFINAEの例: enable_ifを使用
template<typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
processGameValue(T value)
{
    std::cout << "整数値処理: " << value << " (2倍にします)" << std::endl;
    return value * 2;
}

template<typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type
processGameValue(T value)
{
    std::cout << "浮動小数点値処理: " << value << " (平方根を求めます)" << std::endl;
    return std::sqrt(value);
}

template<typename T>
typename std::enable_if<std::is_same<T, std::string>::value, T>::type
processGameValue(T value)
{
    std::cout << "文字列処理: " << value << " (大文字に変換)" << std::endl;
    std::string result = value;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

// C++14のenable_if_t簡略記法
template<typename T>
std::enable_if_t<std::is_arithmetic<T>::value, void>
displayGameStat(const T& stat)
{
    std::cout << "数値ステータス: " << stat << std::endl;
}

template<typename T>
std::enable_if_t<!std::is_arithmetic<T>::value, void>
displayGameStat(const T& stat)
{
    std::cout << "非数値ステータス: " << stat << std::endl;
}

// SFINAEを使ったメンバ関数の検出
template<typename T>
class GameObjectChecker
{
private:
    // メンバ関数の存在チェック用のテンプレート
    template<typename U>
    static auto checkDisplay(int) -> decltype(std::declval<U>().display(), std::true_type{});
    
    template<typename U>
    static std::false_type checkDisplay(...);
    
    template<typename U>
    static auto checkUpdate(int) -> decltype(std::declval<U>().update(), std::true_type{});
    
    template<typename U>
    static std::false_type checkUpdate(...);
    
public:
    static constexpr bool hasDisplay = decltype(checkDisplay<T>(0))::value;
    static constexpr bool hasUpdate = decltype(checkUpdate<T>(0))::value;
    
    static void analyzeObject(const T& obj)
    {
        std::cout << "=== オブジェクト解析 ===" << std::endl;
        std::cout << "displayメソッド: " << (hasDisplay ? "有り" : "無し") << std::endl;
        std::cout << "updateメソッド: " << (hasUpdate ? "有り" : "無し") << std::endl;
        
        if constexpr (hasDisplay)
        {
            std::cout << "displayメソッドを呼び出します:" << std::endl;
            obj.display();
        }
        
        if constexpr (hasUpdate)
        {
            std::cout << "updateメソッドが利用可能です" << std::endl;
        }
    }
};

// ゲームオブジェクトの例
class GamePlayer
{
private:
    std::string name;
    int level;
    
public:
    GamePlayer(const std::string& n, int l) : name(n), level(l) {}
    
    void display() const
    {
        std::cout << "プレイヤー: " << name << " (レベル " << level << ")" << std::endl;
    }
    
    void update()
    {
        std::cout << name << " のステータスを更新中..." << std::endl;
    }
};

class GameItem
{
private:
    std::string itemName;
    
public:
    GameItem(const std::string& name) : itemName(name) {}
    
    void display() const
    {
        std::cout << "アイテム: " << itemName << std::endl;
    }
    
    // updateメソッドはない
};

class SimpleObject
{
public:
    int value;
    SimpleObject(int v) : value(v) {}
    // displayもupdateもない
};

// コンテナの型チェック
template<typename T>
struct is_container
{
private:
    template<typename U>
    static auto check(int) -> decltype(
        std::declval<U>().begin(),
        std::declval<U>().end(),
        std::declval<U>().size(),
        std::true_type{}
    );
    
    template<typename U>
    static std::false_type check(...);
    
public:
    static constexpr bool value = decltype(check<T>(0))::value;
};

template<typename T>
constexpr bool is_container_v = is_container<T>::value;

// コンテナかどうかで処理を分岐
template<typename T>
std::enable_if_t<is_container_v<T>, void>
processGameData(const T& data)
{
    std::cout << "コンテナ処理: サイズ " << data.size() << " の要素を処理中" << std::endl;
    for (const auto& item : data)
    {
        std::cout << "  - " << item << std::endl;
    }
}

template<typename T>
std::enable_if_t<!is_container_v<T>, void>
processGameData(const T& data)
{
    std::cout << "単一データ処理: " << data << std::endl;
}

// より高度なSFINAE: 戻り値型の検出
template<typename T>
class GameFunction
{
private:
    template<typename U>
    static auto checkReturnType(int) -> decltype(std::declval<U>()());
    
    template<typename U>
    static void checkReturnType(...);
    
public:
    using return_type = decltype(checkReturnType<T>(0));
    
    static constexpr bool returnsVoid = std::is_same_v<return_type, void>;
    static constexpr bool returnsInt = std::is_same_v<return_type, int>;
    static constexpr bool returnsString = std::is_same_v<return_type, std::string>;
    
    template<typename Func>
    static auto invokeFunction(Func&& func) -> std::enable_if_t<returnsVoid>
    {
        std::cout << "void戻り値関数を実行:" << std::endl;
        func();
        std::cout << "実行完了" << std::endl;
    }
    
    template<typename Func>
    static auto invokeFunction(Func&& func) -> std::enable_if_t<returnsInt, int>
    {
        std::cout << "int戻り値関数を実行:" << std::endl;
        int result = func();
        std::cout << "結果: " << result << std::endl;
        return result;
    }
    
    template<typename Func>
    static auto invokeFunction(Func&& func) -> std::enable_if_t<returnsString, std::string>
    {
        std::cout << "string戻り値関数を実行:" << std::endl;
        std::string result = func();
        std::cout << "結果: " << result << std::endl;
        return result;
    }
};

// ゲーム用のポリシークラス例
template<typename T>
class GameProcessor
{
public:
    // 数値型の場合の処理
    template<typename U = T>
    static std::enable_if_t<std::is_arithmetic_v<U>, void>
    process(const U& value)
    {
        std::cout << "数値処理: " << value << " (計算処理を実行)" << std::endl;
    }
    
    // 文字列型の場合の処理
    template<typename U = T>
    static std::enable_if_t<std::is_same_v<U, std::string>, void>
    process(const U& value)
    {
        std::cout << "文字列処理: " << value << " (文字列操作を実行)" << std::endl;
    }
    
    // ポインタ型の場合の処理
    template<typename U = T>
    static std::enable_if_t<std::is_pointer_v<U>, void>
    process(const U& value)
    {
        if (value)
        {
            std::cout << "ポインタ処理: 有効なポインタを処理中" << std::endl;
            process(*value);
        }
        else
        {
            std::cout << "ポインタ処理: nullptrのため処理をスキップ" << std::endl;
        }
    }
};

int main()
{
    std::cout << "=== SFINAE のデモ ===" << std::endl;
    
    // 1. 基本的なSFINAEの使用例
    std::cout << "\n1. 型に応じた処理の選択:" << std::endl;
    auto intResult = processGameValue(42);
    auto doubleResult = processGameValue(16.0);
    auto stringResult = processGameValue(std::string("hello"));
    
    std::cout << "結果: " << intResult << ", " << doubleResult << ", " << stringResult << std::endl;
    
    // 2. enable_if_tの使用例
    std::cout << "\n2. 数値/非数値の判定:" << std::endl;
    displayGameStat(100);
    displayGameStat(3.14);
    displayGameStat(std::string("プレイヤー名"));
    
    // 3. メンバ関数の存在チェック
    std::cout << "\n3. メンバ関数の存在チェック:" << std::endl;
    GamePlayer player("アリス", 25);
    GameItem item("魔法の剣");
    SimpleObject simple(42);
    
    GameObjectChecker<GamePlayer>::analyzeObject(player);
    std::cout << std::endl;
    GameObjectChecker<GameItem>::analyzeObject(item);
    std::cout << std::endl;
    GameObjectChecker<SimpleObject>::analyzeObject(simple);
    
    // 4. コンテナの検出
    std::cout << "\n4. コンテナの検出と処理:" << std::endl;
    std::vector<std::string> weapons = {"剣", "斧", "弓"};
    std::string singleWeapon = "特別な剣";
    
    std::cout << "vectorはコンテナ: " << is_container_v<std::vector<std::string>> << std::endl;
    std::cout << "stringはコンテナ: " << is_container_v<std::string> << std::endl;
    
    processGameData(weapons);
    processGameData(singleWeapon);
    
    // 5. 関数の戻り値型による分岐
    std::cout << "\n5. 関数の戻り値型による分岐:" << std::endl;
    
    auto voidFunc = []() { std::cout << "void関数実行中" << std::endl; };
    auto intFunc = []() { return 42; };
    auto stringFunc = []() { return std::string("結果文字列"); };
    
    GameFunction<decltype(voidFunc)>::invokeFunction(voidFunc);
    GameFunction<decltype(intFunc)>::invokeFunction(intFunc);
    GameFunction<decltype(stringFunc)>::invokeFunction(stringFunc);
    
    // 6. ポリシークラスでの型別処理
    std::cout << "\n6. ポリシークラスでの型別処理:" << std::endl;
    GameProcessor<int>::process(100);
    GameProcessor<double>::process(3.14);
    GameProcessor<std::string>::process("ゲームデータ");
    
    int value = 200;
    int* ptr = &value;
    int* nullPtr = nullptr;
    GameProcessor<int*>::process(ptr);
    GameProcessor<int*>::process(nullPtr);
    
    // 7. 複合的なSFINAE使用例
    std::cout << "\n7. 複合的なSFINAE例:" << std::endl;
    
    // 型特性の組み合わせ
    auto complexProcessor = [](auto value) {
        using T = std::decay_t<decltype(value)>;
        
        if constexpr (std::is_integral_v<T>)
        {
            std::cout << "整数型: " << value << " を処理" << std::endl;
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            std::cout << "浮動小数点型: " << value << " を処理" << std::endl;
        }
        else if constexpr (std::is_same_v<T, std::string>)
        {
            std::cout << "文字列型: " << value << " を処理" << std::endl;
        }
        else
        {
            std::cout << "その他の型を処理" << std::endl;
        }
    };
    
    complexProcessor(42);
    complexProcessor(3.14);
    complexProcessor(std::string("テスト"));
    complexProcessor(player);
    
    return 0;
}

/*
 * 学習ポイント:
 * 1. SFINAEは置換失敗時にエラーにならずオーバーロード候補から除外
 * 2. std::enable_ifで条件に応じてテンプレートを有効/無効化
 * 3. type_traitsでコンパイル時の型チェックが可能
 * 4. メンバ関数の存在をSFINAEで検出できる
 * 5. C++17のif constexprでより読みやすいコードが書ける
 * 6. ゲーム開発では型に応じた最適化処理の選択に活用
 * 7. SFINAEで型安全性を保ちながら柔軟なAPIを提供
 */