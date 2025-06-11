// 高度なSFINAE（Substitution Failure Is Not An Error）
// より複雑な型チェックと条件付きコンパイルを実現

#include <iostream>
#include <type_traits>
#include <vector>
#include <string>
#include <utility>

// 基本的なSFINAEの復習
template<typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
processValue(T value)
{
    std::cout << "整数処理: " << value << std::endl;
    return value * 2;
}

template<typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type
processValue(T value)
{
    std::cout << "浮動小数点処理: " << value << std::endl;
    return value * 1.5f;
}

// void_tを使ったメンバー検出
template<typename, typename = void>
struct has_size_method : std::false_type {};

template<typename T>
struct has_size_method<T, std::void_t<decltype(std::declval<T>().size())>>
    : std::true_type {};

template<typename T>
constexpr bool has_size_method_v = has_size_method<T>::value;

// より複雑なメンバー検出
template<typename, typename = void>
struct has_push_back : std::false_type {};

template<typename T>
struct has_push_back<T, std::void_t<
    decltype(std::declval<T>().push_back(std::declval<typename T::value_type>()))
>> : std::true_type {};

template<typename T>
constexpr bool has_push_back_v = has_push_back<T>::value;

// ゲーム固有のメンバー検出
template<typename, typename = void>
struct has_health : std::false_type {};

template<typename T>
struct has_health<T, std::void_t<decltype(std::declval<T>().getHealth())>>
    : std::true_type {};

template<typename, typename = void>
struct has_attack : std::false_type {};

template<typename T>
struct has_attack<T, std::void_t<
    decltype(std::declval<T>().attack(std::declval<T&>()))
>> : std::true_type {};

template<typename, typename = void>
struct has_move : std::false_type {};

template<typename T>
struct has_move<T, std::void_t<
    decltype(std::declval<T>().move(std::declval<float>()))
>> : std::true_type {};

// 戻り値型による検出
template<typename, typename = void>
struct returns_int_health : std::false_type {};

template<typename T>
struct returns_int_health<T, std::void_t<
    decltype(static_cast<int>(std::declval<T>().getHealth()))
>> : std::true_type {};

// 複数条件の組み合わせ
template<typename T>
constexpr bool is_game_character_v = 
    has_health<T>::value && 
    has_move<T>::value && 
    returns_int_health<T>::value;

template<typename T>
constexpr bool is_combatant_v = 
    is_game_character_v<T> && 
    has_attack<T>::value;

// Expression SFINAEの例
template<typename T, typename U>
auto add_if_possible(T&& t, U&& u) 
    -> decltype(std::forward<T>(t) + std::forward<U>(u))
{
    std::cout << "加算可能な型です" << std::endl;
    return std::forward<T>(t) + std::forward<U>(u);
}

template<typename T, typename U>
auto add_if_possible(T&& t, U&& u) 
    -> std::enable_if_t<!std::is_same_v<
        decltype(std::forward<T>(t) + std::forward<U>(u)), void>, std::string>
{
    return "加算不可能な型の組み合わせです";
}

// ゲームオブジェクトの例
class BasicCharacter
{
private:
    int health_;
    
public:
    BasicCharacter(int h) : health_(h) {}
    int getHealth() const { return health_; }
    void move(float deltaTime) { /* 移動処理 */ }
};

class Warrior : public BasicCharacter
{
public:
    Warrior() : BasicCharacter(100) {}
    
    void attack(Warrior& target)
    {
        // 攻撃処理
        std::cout << "戦士が攻撃！" << std::endl;
    }
};

class Mage : public BasicCharacter
{
public:
    Mage() : BasicCharacter(70) {}
    
    void castSpell()
    {
        std::cout << "魔法使いが呪文を唱えた！" << std::endl;
    }
};

class Item
{
private:
    std::string name_;
    
public:
    Item(const std::string& name) : name_(name) {}
    size_t size() const { return name_.size(); }
};

// SFINAEによる関数選択
template<typename T>
std::enable_if_t<has_size_method_v<T>, size_t>
getSize(const T& obj)
{
    std::cout << "size()メソッドを使用" << std::endl;
    return obj.size();
}

template<typename T>
std::enable_if_t<!has_size_method_v<T> && std::is_arithmetic_v<T>, size_t>
getSize(const T&)
{
    std::cout << "算術型のサイズを返却" << std::endl;
    return sizeof(T);
}

// より高度な条件分岐
template<typename Container>
std::enable_if_t<has_push_back_v<Container>, void>
addElement(Container& container, const typename Container::value_type& element)
{
    std::cout << "push_back()を使用して要素を追加" << std::endl;
    container.push_back(element);
}

template<typename Container>
std::enable_if_t<!has_push_back_v<Container>, void>
addElement(Container& container, const typename Container::value_type& element)
{
    std::cout << "insertを使用して요素を追加" << std::endl;
    container.insert(element);
}

// タグディスパッチとSFINAEの組み合わせ
struct character_tag {};
struct combatant_tag {};

template<typename T>
std::enable_if_t<is_game_character_v<T> && !is_combatant_v<T>, void>
processGameObject(T& obj)
{
    std::cout << "基本キャラクターとして処理" << std::endl;
    obj.move(0.1f);
    std::cout << "体力: " << obj.getHealth() << std::endl;
}

template<typename T>
std::enable_if_t<is_combatant_v<T>, void>
processGameObject(T& obj)
{
    std::cout << "戦闘可能キャラクターとして処理" << std::endl;
    obj.move(0.1f);
    std::cout << "体力: " << obj.getHealth() << std::endl;
    // 攻撃は別途実装
}

// C++17 if constexprとの比較
template<typename T>
void processModern(T& obj)
{
    if constexpr (is_combatant_v<T>)
    {
        std::cout << "C++17: 戦闘可能キャラクター" << std::endl;
        obj.move(0.1f);
        // obj.attack() は別途実装が必要
    }
    else if constexpr (is_game_character_v<T>)
    {
        std::cout << "C++17: 基本キャラクター" << std::endl;
        obj.move(0.1f);
    }
    else
    {
        std::cout << "C++17: その他のオブジェクト" << std::endl;
    }
}

// 複雑なSFINAE：メソッドチェーンの検出
template<typename, typename = void>
struct has_method_chain : std::false_type {};

template<typename T>
struct has_method_chain<T, std::void_t<
    decltype(std::declval<T>().move(0.1f).attack(std::declval<T&>()))
>> : std::true_type {};

// ジェネリックアルゴリズム
template<typename Iterator>
std::enable_if_t<
    std::is_same_v<
        typename std::iterator_traits<Iterator>::iterator_category,
        std::random_access_iterator_tag
    >,
    typename std::iterator_traits<Iterator>::difference_type
>
distance_impl(Iterator first, Iterator last)
{
    std::cout << "ランダムアクセスイテレータ: O(1)" << std::endl;
    return last - first;
}

template<typename Iterator>
std::enable_if_t<
    !std::is_same_v<
        typename std::iterator_traits<Iterator>::iterator_category,
        std::random_access_iterator_tag
    >,
    typename std::iterator_traits<Iterator>::difference_type
>
distance_impl(Iterator first, Iterator last)
{
    std::cout << "その他のイテレータ: O(n)" << std::endl;
    typename std::iterator_traits<Iterator>::difference_type count = 0;
    while (first != last)
    {
        ++first;
        ++count;
    }
    return count;
}

// オーバーロード解決の優先順位制御
template<typename T>
std::enable_if_t<std::is_integral_v<T>, std::string>
stringify(T value, int priority = 0)
{
    return "Integer: " + std::to_string(value);
}

template<typename T>
std::enable_if_t<std::is_floating_point_v<T>, std::string>
stringify(T value, char priority = 0)  // charは整数より優先度が低い
{
    return "Float: " + std::to_string(value);
}

template<typename T>
std::enable_if_t<std::is_same_v<T, std::string>, std::string>
stringify(const T& value, void* priority = nullptr)  // void*は最低優先度
{
    return "String: " + value;
}

int main()
{
    std::cout << "=== 高度なSFINAE ===" << std::endl;
    
    // 1. 基本的なSFINAE
    std::cout << "\n--- 基本的なSFINAE ---" << std::endl;
    processValue(42);
    processValue(3.14f);
    
    // 2. メンバー検出
    std::cout << "\n--- メンバー検出 ---" << std::endl;
    std::cout << "vector has size(): " << has_size_method_v<std::vector<int>> << std::endl;
    std::cout << "int has size(): " << has_size_method_v<int> << std::endl;
    std::cout << "vector has push_back(): " << has_push_back_v<std::vector<int>> << std::endl;
    
    // 3. ゲームキャラクター検出
    std::cout << "\n--- ゲームキャラクター検出 ---" << std::endl;
    std::cout << "Warrior is game character: " << is_game_character_v<Warrior> << std::endl;
    std::cout << "Warrior is combatant: " << is_combatant_v<Warrior> << std::endl;
    std::cout << "Mage is combatant: " << is_combatant_v<Mage> << std::endl;
    
    // 4. 条件付き処理
    std::cout << "\n--- 条件付き処理 ---" << std::endl;
    Warrior warrior;
    Mage mage;
    Item item("魔法の剣");
    
    processGameObject(warrior);
    processGameObject(mage);
    
    std::cout << "\nモダンな処理:" << std::endl;
    processModern(warrior);
    processModern(mage);
    
    // 5. サイズ取得
    std::cout << "\n--- サイズ取得 ---" << std::endl;
    std::vector<int> vec{1, 2, 3, 4, 5};
    std::cout << "vector size: " << getSize(vec) << std::endl;
    std::cout << "int size: " << getSize(42) << std::endl;
    std::cout << "item size: " << getSize(item) << std::endl;
    
    // 6. イテレータカテゴリ
    std::cout << "\n--- イテレータ最適化 ---" << std::endl;
    std::cout << "vector distance: " << distance_impl(vec.begin(), vec.end()) << std::endl;
    
    // 7. 文字列化
    std::cout << "\n--- 優先順位制御 ---" << std::endl;
    std::cout << stringify(42) << std::endl;
    std::cout << stringify(3.14) << std::endl;
    std::cout << stringify(std::string("Hello")) << std::endl;
    
    return 0;
}

// まとめ：
// - SFINAEで複雑な型チェックと条件分岐を実現
// - void_tとdecltypeで高度なメンバー検出
// - Expression SFINAEで式の有効性をチェック
// - オーバーロード解決の優先順位を制御
// - C++17のif constexprでより簡潔に記述可能