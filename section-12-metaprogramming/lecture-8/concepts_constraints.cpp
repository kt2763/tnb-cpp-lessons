// コンセプトと制約（C++20）
// 型に対する要件を明示的に表現し、より分かりやすいテンプレートを作成

#include <iostream>
#include <concepts>
#include <type_traits>
#include <vector>
#include <string>
#include <ranges>
#include <algorithm>

// 基本的なコンセプト
template<typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template<typename T>
concept Printable = requires(T t) {
    std::cout << t;
};

// ゲーム関連のコンセプト
template<typename T>
concept GameObject = requires(T t) {
    { t.getName() } -> std::convertible_to<std::string>;
    { t.getPosition() } -> std::convertible_to<float>;
    { t.update() } -> std::same_as<void>;
};

template<typename T>
concept Moveable = GameObject<T> && requires(T t, float deltaTime) {
    { t.move(deltaTime) } -> std::same_as<void>;
};

template<typename T>
concept Attackable = GameObject<T> && requires(T t, T& target) {
    { t.attack(target) } -> std::convertible_to<int>;  // ダメージ値を返す
    { t.getAttackPower() } -> std::convertible_to<int>;
};

template<typename T>
concept Damageable = GameObject<T> && requires(T t, int damage) {
    { t.takeDamage(damage) } -> std::same_as<void>;
    { t.getHealth() } -> std::convertible_to<int>;
    { t.isAlive() } -> std::convertible_to<bool>;
};

// より複雑なコンセプト
template<typename T>
concept Container = requires(T t) {
    typename T::value_type;
    typename T::iterator;
    { t.begin() } -> std::same_as<typename T::iterator>;
    { t.end() } -> std::same_as<typename T::iterator>;
    { t.size() } -> std::convertible_to<size_t>;
};

template<typename T>
concept Sortable = Container<T> && requires(T t) {
    requires std::totally_ordered<typename T::value_type>;
};

// ゲームオブジェクトの実装例
class Player
{
private:
    std::string name_;
    float x_, y_;
    int health_;
    int attackPower_;
    
public:
    Player(const std::string& name, float x, float y) 
        : name_(name), x_(x), y_(y), health_(100), attackPower_(20) {}
    
    std::string getName() const { return name_; }
    float getPosition() const { return x_; }
    void update() { /* プレイヤー更新処理 */ }
    
    void move(float deltaTime) 
    {
        // 移動処理（簡略化）
        x_ += 10.0f * deltaTime;
        std::cout << name_ << " が移動: x=" << x_ << std::endl;
    }
    
    int attack(Player& target)
    {
        int damage = attackPower_;
        target.takeDamage(damage);
        std::cout << name_ << " が " << target.getName() 
                  << " に " << damage << " のダメージ！" << std::endl;
        return damage;
    }
    
    int getAttackPower() const { return attackPower_; }
    
    void takeDamage(int damage)
    {
        health_ -= damage;
        if (health_ < 0) health_ = 0;
        std::cout << name_ << " の残りHP: " << health_ << std::endl;
    }
    
    int getHealth() const { return health_; }
    bool isAlive() const { return health_ > 0; }
};

class Enemy
{
private:
    std::string name_;
    float x_, y_;
    int health_;
    int attackPower_;
    
public:
    Enemy(const std::string& name, float x, float y) 
        : name_(name), x_(x), y_(y), health_(80), attackPower_(15) {}
    
    std::string getName() const { return name_; }
    float getPosition() const { return x_; }
    void update() { /* 敵更新処理 */ }
    
    void move(float deltaTime)
    {
        x_ -= 5.0f * deltaTime;
        std::cout << name_ << " が移動: x=" << x_ << std::endl;
    }
    
    int attack(Player& target)
    {
        int damage = attackPower_;
        target.takeDamage(damage);
        std::cout << name_ << " が " << target.getName() 
                  << " に " << damage << " のダメージ！" << std::endl;
        return damage;
    }
    
    int getAttackPower() const { return attackPower_; }
    
    void takeDamage(int damage)
    {
        health_ -= damage;
        if (health_ < 0) health_ = 0;
        std::cout << name_ << " の残りHP: " << health_ << std::endl;
    }
    
    int getHealth() const { return health_; }
    bool isAlive() const { return health_ > 0; }
};

// コンセプトを使った関数
template<Numeric T>
T add(T a, T b)
{
    return a + b;
}

template<Printable T>
void print(const T& value)
{
    std::cout << "値: " << value << std::endl;
}

template<GameObject T>
void updateGameObject(T& obj)
{
    obj.update();
    std::cout << obj.getName() << " を更新しました" << std::endl;
}

template<Moveable T>
void moveObject(T& obj, float deltaTime)
{
    obj.move(deltaTime);
}

template<Attackable Attacker, Damageable Target>
void performAttack(Attacker& attacker, Target& target)
{
    if (attacker.getAttackPower() > 0 && target.isAlive())
    {
        attacker.attack(target);
    }
}

template<Container T>
void printContainer(const T& container)
{
    std::cout << "コンテナサイズ: " << container.size() << ", 内容: ";
    for (const auto& item : container)
    {
        std::cout << item << " ";
    }
    std::cout << std::endl;
}

template<Sortable T>
void sortAndPrint(T& container)
{
    std::sort(container.begin(), container.end());
    std::cout << "ソート後: ";
    printContainer(container);
}

// 制約を使った特殊化
template<typename T>
requires std::integral<T>
T factorial(T n)
{
    std::cout << "整数版factorial" << std::endl;
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

template<typename T>
requires std::floating_point<T>
T factorial(T n)
{
    std::cout << "浮動小数点版factorial（ガンマ関数近似）" << std::endl;
    // 簡易実装
    return n * (n - 1) * (n - 2);  // 実際はガンマ関数を使用
}

// 複合制約
template<typename T>
concept NumericContainer = Container<T> && Numeric<typename T::value_type>;

template<NumericContainer T>
auto calculateSum(const T& container)
{
    typename T::value_type sum{};
    for (const auto& item : container)
    {
        sum += item;
    }
    return sum;
}

// ゲーム特化のコンセプト組み合わせ
template<typename T>
concept Combatant = Attackable<T> && Damageable<T> && Moveable<T>;

template<Combatant T1, Combatant T2>
void combat(T1& fighter1, T2& fighter2)
{
    std::cout << "\n=== 戦闘開始 ===" << std::endl;
    std::cout << fighter1.getName() << " vs " << fighter2.getName() << std::endl;
    
    while (fighter1.isAlive() && fighter2.isAlive())
    {
        // ファイター1の攻撃
        if (fighter1.isAlive())
        {
            performAttack(fighter1, fighter2);
        }
        
        // ファイター2の攻撃
        if (fighter2.isAlive())
        {
            performAttack(fighter2, fighter1);
        }
        
        // 簡単なターン制限
        static int turns = 0;
        if (++turns > 3) break;
    }
    
    std::cout << "=== 戦闘終了 ===" << std::endl;
}

// requires式での複雑な制約
template<typename T>
concept ComplexGameObject = requires(T t) {
    // 基本的なインターフェース
    { t.getName() } -> std::convertible_to<std::string>;
    { t.getPosition() } -> std::convertible_to<float>;
    
    // オプショナルなインターフェース
    requires requires { t.getHealth(); } || requires { t.getDurability(); };
    
    // 型の制約
    typename T::position_type;
    requires std::is_arithmetic_v<typename T::position_type>;
};

// subsumption（包含関係）の例
template<typename T>
concept Animal = requires(T t) {
    { t.move() } -> std::same_as<void>;
};

template<typename T>
concept Dog = Animal<T> && requires(T t) {
    { t.bark() } -> std::same_as<void>;
};

// より特殊化されたコンセプトが優先される
void makeSound(Animal auto& animal)
{
    std::cout << "動物が何かしている" << std::endl;
}

void makeSound(Dog auto& dog)  // より特殊化されているので、こちらが選ばれる
{
    std::cout << "犬が吠えている" << std::endl;
    dog.bark();
}

// C++20 ranges との組み合わせ
template<std::ranges::range R>
requires Numeric<std::ranges::range_value_t<R>>
auto processRange(R&& range)
{
    std::cout << "数値レンジを処理中..." << std::endl;
    auto squared = range | std::views::transform([](auto x) { return x * x; });
    
    for (auto value : squared | std::views::take(5))
    {
        std::cout << value << " ";
    }
    std::cout << std::endl;
}

int main()
{
    std::cout << "=== コンセプトと制約（C++20）===" << std::endl;
    
    // 1. 基本的なコンセプト
    std::cout << "\n--- 基本的なコンセプト ---" << std::endl;
    std::cout << "add(5, 3) = " << add(5, 3) << std::endl;
    std::cout << "add(2.5, 1.7) = " << add(2.5, 1.7) << std::endl;
    
    print(42);
    print("Hello World");
    
    // 2. ゲームオブジェクト
    std::cout << "\n--- ゲームオブジェクト ---" << std::endl;
    Player hero("勇者", 0.0f, 0.0f);
    Enemy goblin("ゴブリン", 100.0f, 0.0f);
    
    updateGameObject(hero);
    updateGameObject(goblin);
    
    moveObject(hero, 0.1f);
    moveObject(goblin, 0.1f);
    
    // 3. 戦闘システム
    std::cout << "\n--- 戦闘システム ---" << std::endl;
    performAttack(hero, goblin);
    performAttack(goblin, hero);
    
    // 4. コンテナ操作
    std::cout << "\n--- コンテナ操作 ---" << std::endl;
    std::vector<int> numbers{5, 2, 8, 1, 9, 3};
    printContainer(numbers);
    sortAndPrint(numbers);
    
    auto sum = calculateSum(numbers);
    std::cout << "合計: " << sum << std::endl;
    
    // 5. 制約による特殊化
    std::cout << "\n--- 制約による特殊化 ---" << std::endl;
    std::cout << "factorial(5) = " << factorial(5) << std::endl;
    std::cout << "factorial(5.0) = " << factorial(5.0) << std::endl;
    
    // 6. 複合戦闘
    std::cout << "\n--- 複合戦闘 ---" << std::endl;
    Player player1("戦士", 0, 0);
    Player player2("魔法使い", 10, 0);
    combat(player1, player2);
    
    // 7. ranges との組み合わせ
    std::cout << "\n--- ranges処理 ---" << std::endl;
    std::vector<int> values{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    processRange(values);
    
    return 0;
}

// まとめ：
// - コンセプトで型の要件を明示的に表現
// - requires式で複雑な制約を記述
// - より分かりやすいエラーメッセージ
// - ゲーム開発では型安全性と表現力を向上
// - C++20 rangesと組み合わせて強力な抽象化を実現