// 式テンプレート（Expression Templates）
// 演算の遅延評価により、効率的な数値計算を実現

#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <algorithm>

// 前方宣言
template<typename E> class VectorExpression;
template<typename L, typename R, typename Op> class BinaryExpression;
template<typename E, typename Op> class UnaryExpression;

// 演算子の定義
struct AddOp
{
    template<typename T>
    static T apply(T a, T b) { return a + b; }
};

struct SubOp
{
    template<typename T>
    static T apply(T a, T b) { return a - b; }
};

struct MulOp
{
    template<typename T>
    static T apply(T a, T b) { return a * b; }
};

struct DivOp
{
    template<typename T>
    static T apply(T a, T b) { return a / b; }
};

// 単項演算子
struct NegateOp
{
    template<typename T>
    static T apply(T a) { return -a; }
};

struct SqrtOp
{
    template<typename T>
    static T apply(T a) { return std::sqrt(a); }
};

// 式の基底クラス
template<typename E>
class VectorExpression
{
public:
    float operator[](size_t i) const
    {
        return static_cast<const E&>(*this)[i];
    }
    
    size_t size() const
    {
        return static_cast<const E&>(*this).size();
    }
};

// 実際のベクトルクラス
class GameVector : public VectorExpression<GameVector>
{
private:
    std::vector<float> data_;
    
public:
    GameVector(size_t n) : data_(n) {}
    GameVector(size_t n, float value) : data_(n, value) {}
    GameVector(std::initializer_list<float> init) : data_(init) {}
    
    // 式テンプレートからの構築
    template<typename E>
    GameVector(const VectorExpression<E>& expr) : data_(expr.size())
    {
        for (size_t i = 0; i < data_.size(); ++i)
        {
            data_[i] = expr[i];
        }
    }
    
    // 式テンプレートからの代入
    template<typename E>
    GameVector& operator=(const VectorExpression<E>& expr)
    {
        for (size_t i = 0; i < data_.size(); ++i)
        {
            data_[i] = expr[i];
        }
        return *this;
    }
    
    float operator[](size_t i) const { return data_[i]; }
    float& operator[](size_t i) { return data_[i]; }
    size_t size() const { return data_.size(); }
    
    // イテレータ
    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
};

// 二項演算式
template<typename L, typename R, typename Op>
class BinaryExpression : public VectorExpression<BinaryExpression<L, R, Op>>
{
private:
    const L& left_;
    const R& right_;
    
public:
    BinaryExpression(const L& l, const R& r) : left_(l), right_(r) {}
    
    float operator[](size_t i) const
    {
        return Op::apply(left_[i], right_[i]);
    }
    
    size_t size() const { return left_.size(); }
};

// 単項演算式
template<typename E, typename Op>
class UnaryExpression : public VectorExpression<UnaryExpression<E, Op>>
{
private:
    const E& expr_;
    
public:
    explicit UnaryExpression(const E& e) : expr_(e) {}
    
    float operator[](size_t i) const
    {
        return Op::apply(expr_[i]);
    }
    
    size_t size() const { return expr_.size(); }
};

// スカラーとベクトルの演算
template<typename E>
class ScalarExpression : public VectorExpression<ScalarExpression<E>>
{
private:
    const E& expr_;
    float scalar_;
    bool scalar_first_;
    
public:
    ScalarExpression(float s, const E& e, bool sf = true) 
        : expr_(e), scalar_(s), scalar_first_(sf) {}
    
    float operator[](size_t i) const
    {
        if (scalar_first_)
            return scalar_ * expr_[i];
        else
            return expr_[i] * scalar_;
    }
    
    size_t size() const { return expr_.size(); }
};

// 演算子オーバーロード
template<typename L, typename R>
BinaryExpression<L, R, AddOp> operator+(const VectorExpression<L>& l, const VectorExpression<R>& r)
{
    return BinaryExpression<L, R, AddOp>(static_cast<const L&>(l), static_cast<const R&>(r));
}

template<typename L, typename R>
BinaryExpression<L, R, SubOp> operator-(const VectorExpression<L>& l, const VectorExpression<R>& r)
{
    return BinaryExpression<L, R, SubOp>(static_cast<const L&>(l), static_cast<const R&>(r));
}

template<typename L, typename R>
BinaryExpression<L, R, MulOp> operator*(const VectorExpression<L>& l, const VectorExpression<R>& r)
{
    return BinaryExpression<L, R, MulOp>(static_cast<const L&>(l), static_cast<const R&>(r));
}

template<typename E>
ScalarExpression<E> operator*(float s, const VectorExpression<E>& e)
{
    return ScalarExpression<E>(s, static_cast<const E&>(e), true);
}

template<typename E>
ScalarExpression<E> operator*(const VectorExpression<E>& e, float s)
{
    return ScalarExpression<E>(s, static_cast<const E&>(e), false);
}

template<typename E>
UnaryExpression<E, NegateOp> operator-(const VectorExpression<E>& e)
{
    return UnaryExpression<E, NegateOp>(static_cast<const E&>(e));
}

// 数学関数
template<typename E>
UnaryExpression<E, SqrtOp> sqrt(const VectorExpression<E>& e)
{
    return UnaryExpression<E, SqrtOp>(static_cast<const E&>(e));
}

// ゲームでの応用例：物理演算
class PhysicsEngine
{
public:
    // 位置更新（オイラー法）
    static void updatePosition(GameVector& position, 
                             const GameVector& velocity, 
                             float deltaTime)
    {
        // 式テンプレートにより、一時オブジェクトなしで計算
        position = position + velocity * deltaTime;
    }
    
    // 速度更新
    static void updateVelocity(GameVector& velocity,
                             const GameVector& acceleration,
                             float deltaTime)
    {
        velocity = velocity + acceleration * deltaTime;
    }
    
    // 弾性衝突
    static void elasticCollision(GameVector& vel1, GameVector& vel2,
                                float mass1, float mass2)
    {
        GameVector temp = vel1;
        vel1 = ((mass1 - mass2) * vel1 + 2.0f * mass2 * vel2) * (1.0f / (mass1 + mass2));
        vel2 = ((mass2 - mass1) * vel2 + 2.0f * mass1 * temp) * (1.0f / (mass1 + mass2));
    }
};

// より複雑な式：ゲームの計算
class GameCalculations
{
public:
    // ダメージ計算（複雑な式）
    static GameVector calculateDamage(const GameVector& baseDamage,
                                    const GameVector& attackMultiplier,
                                    const GameVector& defenseReduction)
    {
        // (baseDamage * attackMultiplier) * (1 - defenseReduction)
        return baseDamage * attackMultiplier * (GameVector(baseDamage.size(), 1.0f) - defenseReduction);
    }
    
    // 距離計算
    static GameVector calculateDistance(const GameVector& pos1, const GameVector& pos2)
    {
        // sqrt((pos1 - pos2) * (pos1 - pos2))
        GameVector diff = pos1 - pos2;
        return sqrt(diff * diff);
    }
};

// パフォーマンステスト
class PerformanceTest
{
private:
    using Clock = std::chrono::high_resolution_clock;
    
public:
    // 従来の方法（一時オブジェクトあり）
    static void traditionalMethod(size_t size, int iterations)
    {
        std::vector<float> a(size, 1.0f);
        std::vector<float> b(size, 2.0f);
        std::vector<float> c(size, 3.0f);
        std::vector<float> result(size);
        
        auto start = Clock::now();
        
        for (int iter = 0; iter < iterations; ++iter)
        {
            // 複数の一時オブジェクトが作成される
            std::vector<float> temp1(size);
            std::vector<float> temp2(size);
            
            for (size_t i = 0; i < size; ++i)
            {
                temp1[i] = a[i] + b[i];
            }
            for (size_t i = 0; i < size; ++i)
            {
                temp2[i] = temp1[i] * c[i];
            }
            for (size_t i = 0; i < size; ++i)
            {
                result[i] = temp2[i] + a[i];
            }
        }
        
        auto end = Clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "従来の方法: " << duration.count() << " μs" << std::endl;
    }
    
    // 式テンプレートを使った方法
    static void expressionTemplateMethod(size_t size, int iterations)
    {
        GameVector a(size, 1.0f);
        GameVector b(size, 2.0f);
        GameVector c(size, 3.0f);
        GameVector result(size);
        
        auto start = Clock::now();
        
        for (int iter = 0; iter < iterations; ++iter)
        {
            // 一時オブジェクトなしで、1回のループで計算
            result = (a + b) * c + a;
        }
        
        auto end = Clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "式テンプレート: " << duration.count() << " μs" << std::endl;
    }
};

// デバッグ用：式の構造を表示
template<typename E>
void printExpression(const VectorExpression<E>& expr, const std::string& name)
{
    std::cout << name << " = [";
    for (size_t i = 0; i < expr.size(); ++i)
    {
        if (i > 0) std::cout << ", ";
        std::cout << expr[i];
    }
    std::cout << "]" << std::endl;
}

int main()
{
    std::cout << "=== 式テンプレート（Expression Templates）===" << std::endl;
    
    // 1. 基本的な使用例
    std::cout << "\n--- 基本的な演算 ---" << std::endl;
    GameVector a{1.0f, 2.0f, 3.0f};
    GameVector b{4.0f, 5.0f, 6.0f};
    GameVector c{7.0f, 8.0f, 9.0f};
    
    GameVector result = a + b * c;
    printExpression(result, "a + b * c");
    
    // 2. 複雑な式
    std::cout << "\n--- 複雑な式 ---" << std::endl;
    GameVector complex = (a + b) * 2.0f - c / 3.0f;
    printExpression(complex, "(a + b) * 2 - c / 3");
    
    // 3. 物理演算
    std::cout << "\n--- 物理演算 ---" << std::endl;
    GameVector position{0.0f, 0.0f, 0.0f};
    GameVector velocity{10.0f, 20.0f, 0.0f};
    GameVector acceleration{0.0f, -9.8f, 0.0f};
    
    float deltaTime = 0.1f;
    for (int i = 0; i < 3; ++i)
    {
        PhysicsEngine::updateVelocity(velocity, acceleration, deltaTime);
        PhysicsEngine::updatePosition(position, velocity, deltaTime);
        std::cout << "t=" << (i + 1) * deltaTime << ": ";
        printExpression(position, "position");
    }
    
    // 4. ゲーム計算
    std::cout << "\n--- ダメージ計算 ---" << std::endl;
    GameVector baseDamage{100.0f, 150.0f, 200.0f};
    GameVector attackMult{1.5f, 1.2f, 1.8f};
    GameVector defenseRed{0.2f, 0.3f, 0.1f};
    
    GameVector damage = GameCalculations::calculateDamage(baseDamage, attackMult, defenseRed);
    printExpression(damage, "最終ダメージ");
    
    // 5. 数学関数
    std::cout << "\n--- 数学関数 ---" << std::endl;
    GameVector values{4.0f, 9.0f, 16.0f, 25.0f};
    GameVector sqrtValues = sqrt(values);
    printExpression(sqrtValues, "sqrt(values)");
    
    // 6. パフォーマンス比較
    std::cout << "\n--- パフォーマンス比較 ---" << std::endl;
    const size_t vectorSize = 10000;
    const int iterations = 1000;
    
    std::cout << "ベクトルサイズ: " << vectorSize << std::endl;
    std::cout << "反復回数: " << iterations << std::endl;
    
    PerformanceTest::traditionalMethod(vectorSize, iterations);
    PerformanceTest::expressionTemplateMethod(vectorSize, iterations);
    
    // 7. 連鎖演算
    std::cout << "\n--- 連鎖演算 ---" << std::endl;
    GameVector v1{1.0f, 2.0f, 3.0f};
    GameVector v2{4.0f, 5.0f, 6.0f};
    GameVector v3{7.0f, 8.0f, 9.0f};
    GameVector v4{10.0f, 11.0f, 12.0f};
    
    // 複数の演算を連鎖
    GameVector chain = v1 + v2 * v3 - v4 / 2.0f + sqrt(v1 * v1 + v2 * v2);
    printExpression(chain, "複雑な連鎖演算");
    
    return 0;
}

// まとめ：
// - 式テンプレートで演算を遅延評価
// - 一時オブジェクトの生成を回避して高速化
// - 複雑な数式を自然な形で記述
// - ゲームの物理演算や数値計算で活用
// - コンパイラの最適化と組み合わせて最大の効果