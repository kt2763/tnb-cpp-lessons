/*
 * テンプレート第10講: テンプレート制約 (C++20 Concepts プレビュー)
 * 
 * C++20のコンセプトは、テンプレートパラメータに制約を設ける機能です。
 * より読みやすく、エラーメッセージがわかりやすいテンプレートコードを書けます。
 * ゲーム開発では型安全性を保ちながら柔軟なAPIを提供できます。
 * 
 * 注意: C++20対応コンパイラが必要です。対応していない場合はSFINAEで代替できます。
 */

#include <iostream>
#include <string>
#include <vector>
#include <type_traits>
#include <concepts>

// C++20のコンセプトが利用可能かチェック
#if __cplusplus >= 202002L && defined(__cpp_concepts)
#define HAS_CONCEPTS 1
#else
#define HAS_CONCEPTS 0
#endif

#if HAS_CONCEPTS

// 1. 基本的なコンセプトの定義
template<typename T>
concept Numeric = std::is_arithmetic_v<T>;

template<typename T>
concept GameString = std::is_same_v<T, std::string> || 
                     std::is_same_v<T, const char*> ||
                     (std::is_array_v<T> && std::is_same_v<std::remove_extent_t<T>, char>);

template<typename T>
concept Printable = requires(T t) {
    std::cout << t;
};

// 2. より複雑なコンセプト - ゲームオブジェクト
template<typename T>
concept GameObject = requires(T obj) {
    obj.update();
    obj.render();
    { obj.getName() } -> std::convertible_to<std::string>;
};

template<typename T>
concept Movable = requires(T obj) {
    obj.setPosition(0.0, 0.0);
    { obj.getX() } -> std::convertible_to<double>;
    { obj.getY() } -> std::convertible_to<double>;
};

template<typename T>
concept Damageable = requires(T obj, int damage) {
    { obj.takeDamage(damage) } -> std::same_as<void>;
    { obj.getHealth() } -> std::convertible_to<int>;
    { obj.isAlive() } -> std::same_as<bool>;
};

// 3. コンセプトを使った関数テンプレート
template<Numeric T>
T calculateGameDamage(T baseDamage, T multiplier)
{
    std::cout << "数値ダメージ計算: " << baseDamage << " × " << multiplier << std::endl;
    return baseDamage * multiplier;
}

template<GameString T>
void displayGameMessage(T message)
{
    std::cout << "ゲームメッセージ: " << message << std::endl;
}

// 複数のコンセプトを組み合わせ
template<typename T>
requires GameObject<T> && Movable<T>
void processMovableGameObject(T& obj)
{
    std::cout << "=== 移動可能ゲームオブジェクト処理 ===" << std::endl;
    std::cout << "名前: " << obj.getName() << std::endl;
    std::cout << "位置: (" << obj.getX() << ", " << obj.getY() << ")" << std::endl;
    
    obj.update();
    obj.render();
    
    // 新しい位置に移動
    obj.setPosition(obj.getX() + 1.0, obj.getY() + 1.0);
    std::cout << "移動後位置: (" << obj.getX() << ", " << obj.getY() << ")" << std::endl;
}

// 4. クラステンプレートでのコンセプト使用
template<Numeric T>
class GameVector2D
{
private:
    T x, y;
    
public:
    GameVector2D(T x_val, T y_val) : x(x_val), y(y_val) {}
    
    T getX() const { return x; }
    T getY() const { return y; }
    
    void setX(T val) { x = val; }
    void setY(T val) { y = val; }
    
    GameVector2D operator+(const GameVector2D& other) const
    {
        return GameVector2D(x + other.x, y + other.y);
    }
    
    T magnitude() const requires std::floating_point<T>
    {
        return std::sqrt(x * x + y * y);
    }
    
    void display() const
    {
        std::cout << "ベクトル: (" << x << ", " << y << ")" << std::endl;
    }
};

// 5. 条件付きメンバ関数（コンセプト版）
template<typename T>
class GameValue
{
private:
    T value;
    
public:
    GameValue(T v) : value(v) {}
    
    T getValue() const { return value; }
    
    // 数値型の場合のみ利用可能
    auto multiply(T factor) const requires Numeric<T>
    {
        return GameValue(value * factor);
    }
    
    // 文字列型の場合のみ利用可能
    auto toUpper() const requires GameString<T>
    {
        std::string result = value;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return GameValue<std::string>(result);
    }
    
    // 出力可能な型の場合のみ利用可能
    void display() const requires Printable<T>
    {
        std::cout << "値: " << value << std::endl;
    }
};

// 6. 実際のゲームクラス例
class GamePlayer
{
private:
    std::string name;
    double x, y;
    int health, maxHealth;
    
public:
    GamePlayer(const std::string& n, double px, double py, int h) 
        : name(n), x(px), y(py), health(h), maxHealth(h) {}
    
    void update() { /* 更新処理 */ }
    void render() { std::cout << name << " を描画中" << std::endl; }
    std::string getName() const { return name; }
    
    void setPosition(double newX, double newY) { x = newX; y = newY; }
    double getX() const { return x; }
    double getY() const { return y; }
    
    void takeDamage(int damage) 
    { 
        health = std::max(0, health - damage);
        std::cout << name << " が " << damage << " ダメージを受けた" << std::endl;
    }
    int getHealth() const { return health; }
    bool isAlive() const { return health > 0; }
};

class GameEnemy
{
private:
    std::string type;
    double x, y;
    int health;
    
public:
    GameEnemy(const std::string& t, double px, double py, int h) 
        : type(t), x(px), y(py), health(h) {}
    
    void update() { /* AI処理 */ }
    void render() { std::cout << type << " を描画中" << std::endl; }
    std::string getName() const { return "敵: " + type; }
    
    void setPosition(double newX, double newY) { x = newX; y = newY; }
    double getX() const { return x; }
    double getY() const { return y; }
    
    void takeDamage(int damage) 
    { 
        health = std::max(0, health - damage);
        std::cout << type << " が " << damage << " ダメージを受けた" << std::endl;
    }
    int getHealth() const { return health; }
    bool isAlive() const { return health > 0; }
};

// 7. 汎用的なゲーム処理関数
template<Damageable T>
void processCombat(T& attacker, T& defender, int damage)
{
    std::cout << "=== 戦闘処理 ===" << std::endl;
    std::cout << "攻撃者: " << attacker.getName() << " (HP: " << attacker.getHealth() << ")" << std::endl;
    std::cout << "防御者: " << defender.getName() << " (HP: " << defender.getHealth() << ")" << std::endl;
    
    defender.takeDamage(damage);
    
    std::cout << "戦闘後状態:" << std::endl;
    std::cout << "防御者: " << defender.getName() << " (HP: " << defender.getHealth() 
              << ", 生存: " << (defender.isAlive() ? "はい" : "いいえ") << ")" << std::endl;
}

// 8. 複雑なコンセプトの組み合わせ
template<typename T>
concept GameEntity = GameObject<T> && Movable<T> && Damageable<T>;

template<GameEntity T>
void processFullGameEntity(T& entity)
{
    std::cout << "=== 完全なゲームエンティティ処理 ===" << std::endl;
    entity.update();
    entity.render();
    
    // 移動テスト
    double oldX = entity.getX(), oldY = entity.getY();
    entity.setPosition(oldX + 5.0, oldY + 5.0);
    
    // ダメージテスト
    entity.takeDamage(10);
    
    std::cout << "処理完了: " << entity.getName() << std::endl;
}

#endif // HAS_CONCEPTS

// C++20非対応の場合のSFINAE代替版
#if !HAS_CONCEPTS

template<typename T>
using is_numeric = std::is_arithmetic<T>;

template<typename T>
std::enable_if_t<is_numeric<T>::value, T>
calculateGameDamage(T baseDamage, T multiplier)
{
    std::cout << "数値ダメージ計算 (SFINAE版): " << baseDamage << " × " << multiplier << std::endl;
    return baseDamage * multiplier;
}

template<typename T>
std::enable_if_t<std::is_same_v<T, std::string>, void>
displayGameMessage(T message)
{
    std::cout << "ゲームメッセージ (SFINAE版): " << message << std::endl;
}

// 簡単なSFINAE版ベクトル
template<typename T>
class GameVector2D
{
private:
    T x, y;
    
public:
    GameVector2D(T x_val, T y_val) : x(x_val), y(y_val) {}
    
    T getX() const { return x; }
    T getY() const { return y; }
    
    void display() const
    {
        std::cout << "ベクトル (SFINAE版): (" << x << ", " << y << ")" << std::endl;
    }
};

#endif // !HAS_CONCEPTS

int main()
{
    std::cout << "=== テンプレート制約 (C++20 Concepts) のデモ ===" << std::endl;
    
#if HAS_CONCEPTS
    std::cout << "C++20 Concepts を使用中" << std::endl;
    
    // 1. 基本的なコンセプトの使用
    std::cout << "\n1. 基本的なコンセプトの使用:" << std::endl;
    
    auto damage1 = calculateGameDamage(50, 2);     // int
    auto damage2 = calculateGameDamage(75.5, 1.5); // double
    
    std::cout << "整数ダメージ: " << damage1 << std::endl;
    std::cout << "小数ダメージ: " << damage2 << std::endl;
    
    displayGameMessage(std::string("プレイヤーがレベルアップしました！"));
    displayGameMessage("敵を倒しました！");
    
    // 2. 数値ベクトルクラス
    std::cout << "\n2. 制約付きベクトルクラス:" << std::endl;
    GameVector2D<int> intVec(3, 4);
    GameVector2D<double> doubleVec(2.5, 3.7);
    
    intVec.display();
    doubleVec.display();
    
    // 浮動小数点数のみで利用可能なメソッド
    std::cout << "ベクトルの大きさ: " << doubleVec.magnitude() << std::endl;
    
    auto sum = intVec + GameVector2D<int>(1, 1);
    sum.display();
    
    // 3. ゲームオブジェクトの処理
    std::cout << "\n3. ゲームオブジェクトの処理:" << std::endl;
    GamePlayer player("勇者アリス", 10.0, 20.0, 100);
    GameEnemy enemy("ゴブリン", 5.0, 15.0, 50);
    
    processMovableGameObject(player);
    std::cout << std::endl;
    processMovableGameObject(enemy);
    
    // 4. GameValue クラスの使用
    std::cout << "\n4. 条件付きメンバ関数:" << std::endl;
    GameValue<int> numValue(42);
    GameValue<std::string> strValue("hello world");
    
    auto multiplied = numValue.multiply(3);
    auto uppercased = strValue.toUpper();
    
    numValue.display();
    multiplied.display();
    strValue.display();
    uppercased.display();
    
    // 5. 戦闘システム
    std::cout << "\n5. 戦闘システム:" << std::endl;
    GamePlayer player2("戦士ボブ", 0.0, 0.0, 120);
    GameEnemy enemy2("オーク", 0.0, 0.0, 80);
    
    processCombat(player2, enemy2, 30);
    std::cout << std::endl;
    processCombat(enemy2, player2, 25);
    
    // 6. 完全なゲームエンティティ処理
    std::cout << "\n6. 完全なゲームエンティティ処理:" << std::endl;
    processFullGameEntity(player);
    std::cout << std::endl;
    processFullGameEntity(enemy);
    
    // 7. コンセプトの検証
    std::cout << "\n7. コンセプトの検証:" << std::endl;
    std::cout << "intは数値型: " << Numeric<int> << std::endl;
    std::cout << "stringは数値型: " << Numeric<std::string> << std::endl;
    std::cout << "stringは文字列型: " << GameString<std::string> << std::endl;
    std::cout << "intは文字列型: " << GameString<int> << std::endl;
    std::cout << "GamePlayerはGameObject: " << GameObject<GamePlayer> << std::endl;
    std::cout << "GamePlayerはMovable: " << Movable<GamePlayer> << std::endl;
    std::cout << "GamePlayerはDamageable: " << Damageable<GamePlayer> << std::endl;
    std::cout << "GamePlayerはGameEntity: " << GameEntity<GamePlayer> << std::endl;
    
#else
    std::cout << "C++20 Concepts 非対応 - SFINAE版を使用中" << std::endl;
    
    // SFINAE版のデモ
    std::cout << "\n1. SFINAE版の使用:" << std::endl;
    auto damage = calculateGameDamage(50, 2);
    std::cout << "ダメージ: " << damage << std::endl;
    
    displayGameMessage(std::string("SFINAE版メッセージ"));
    
    GameVector2D<int> vec(3, 4);
    vec.display();
    
#endif
    
    return 0;
}

/*
 * 学習ポイント:
 * 1. C++20のコンセプトでテンプレート制約を明確に表現
 * 2. requires文で複雑な制約条件を定義可能
 * 3. 複数のコンセプトを&&や||で組み合わせ可能
 * 4. コンパイルエラーメッセージが大幅に改善
 * 5. SFINAEより読みやすく理解しやすい制約記述
 * 6. ゲーム開発では型安全なAPIの設計に威力発揮
 * 7. requires句で条件付きメンバ関数も実現可能
 * 8. 旧バージョンではSFINAEで代替可能
 */