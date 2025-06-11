// ポリシーベース設計
// テンプレートパラメータでアルゴリズムや動作を選択可能な柔軟な設計

#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <random>
#include <chrono>
#include <map>
#include <string>

// 前方宣言
template<typename T> class NoThreadingPolicy;
template<typename T> class MutexThreadingPolicy;

// 基本的なポリシークラス
template<typename T>
class SimpleAllocatorPolicy
{
public:
    static T* allocate(size_t n)
    {
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }
    
    static void deallocate(T* ptr, size_t)
    {
        ::operator delete(ptr);
    }
};

template<typename T>
class PoolAllocatorPolicy
{
private:
    static std::vector<T*> pool;
    
public:
    static T* allocate(size_t n)
    {
        std::cout << "プールアロケータから割り当て" << std::endl;
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }
    
    static void deallocate(T* ptr, size_t)
    {
        std::cout << "プールアロケータに返却" << std::endl;
        ::operator delete(ptr);
    }
};

// スレッドポリシー
template<typename T>
class NoThreadingPolicy
{
public:
    class Lock
    {
    public:
        Lock() {}
        ~Lock() {}
    };
};

template<typename T>
class MutexThreadingPolicy
{
private:
    static std::mutex mutex_;
    
public:
    class Lock
    {
    private:
        std::lock_guard<std::mutex> lock_;
        
    public:
        Lock() : lock_(mutex_)
        {
            std::cout << "ミューテックスをロック" << std::endl;
        }
        
        ~Lock()
        {
            std::cout << "ミューテックスをアンロック" << std::endl;
        }
    };
};

template<typename T>
std::mutex MutexThreadingPolicy<T>::mutex_;

// ポリシーベースのスマートポインタ
template<typename T, 
         template<typename> class AllocatorPolicy = SimpleAllocatorPolicy,
         template<typename> class ThreadingPolicy = NoThreadingPolicy>
class SmartPtr : private AllocatorPolicy<T>, private ThreadingPolicy<T>
{
private:
    T* ptr_;
    size_t* refCount_;
    
public:
    explicit SmartPtr(T* ptr = nullptr) : ptr_(ptr), refCount_(nullptr)
    {
        if (ptr_)
        {
            refCount_ = AllocatorPolicy<T>::allocate(1);
            new(refCount_) size_t(1);
        }
    }
    
    SmartPtr(const SmartPtr& other) : ptr_(other.ptr_), refCount_(other.refCount_)
    {
        if (refCount_)
        {
            typename ThreadingPolicy<T>::Lock lock;
            ++(*refCount_);
        }
    }
    
    ~SmartPtr()
    {
        release();
    }
    
    SmartPtr& operator=(const SmartPtr& other)
    {
        if (this != &other)
        {
            release();
            ptr_ = other.ptr_;
            refCount_ = other.refCount_;
            
            if (refCount_)
            {
                typename ThreadingPolicy<T>::Lock lock;
                ++(*refCount_);
            }
        }
        return *this;
    }
    
    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }
    T* get() const { return ptr_; }
    
    size_t use_count() const 
    {
        return refCount_ ? *refCount_ : 0;
    }
    
private:
    void release()
    {
        if (refCount_)
        {
            typename ThreadingPolicy<T>::Lock lock;
            if (--(*refCount_) == 0)
            {
                delete ptr_;
                AllocatorPolicy<T>::deallocate(refCount_, 1);
            }
        }
    }
};

// ゲーム用のポリシー：レンダリングポリシー
template<typename T>
class DirectXRenderPolicy
{
public:
    static void render(const T& object)
    {
        std::cout << "DirectXで" << object.getName() << "をレンダリング" << std::endl;
    }
};

template<typename T>
class OpenGLRenderPolicy
{
public:
    static void render(const T& object)
    {
        std::cout << "OpenGLで" << object.getName() << "をレンダリング" << std::endl;
    }
};

template<typename T>
class VulkanRenderPolicy
{
public:
    static void render(const T& object)
    {
        std::cout << "Vulkanで" << object.getName() << "をレンダリング" << std::endl;
    }
};

// 移動ポリシー
template<typename T>
class LinearMovementPolicy
{
public:
    static void move(T& object, float deltaTime)
    {
        // 直線移動
        object.setPosition(object.getPosition() + object.getVelocity() * deltaTime);
        std::cout << object.getName() << " が直線移動" << std::endl;
    }
};

template<typename T>
class SinusoidalMovementPolicy
{
public:
    static void move(T& object, float deltaTime)
    {
        // 正弦波移動
        static float time = 0;
        time += deltaTime;
        float offset = std::sin(time) * 10.0f;
        auto pos = object.getPosition();
        pos.y += offset;
        object.setPosition(pos);
        std::cout << object.getName() << " が波状移動" << std::endl;
    }
};

// AI思考ポリシー
template<typename T>
class AggressiveAIPolicy
{
public:
    static void think(T& enemy)
    {
        std::cout << enemy.getName() << " は積極的に攻撃してくる！" << std::endl;
    }
};

template<typename T>
class DefensiveAIPolicy
{
public:
    static void think(T& enemy)
    {
        std::cout << enemy.getName() << " は慎重に行動している" << std::endl;
    }
};

// ゲームオブジェクトの基底クラス
struct Vector2D
{
    float x, y;
    Vector2D(float px = 0, float py = 0) : x(px), y(py) {}
    Vector2D operator+(const Vector2D& other) const { return {x + other.x, y + other.y}; }
    Vector2D operator*(float scalar) const { return {x * scalar, y * scalar}; }
};

class GameObject
{
protected:
    std::string name_;
    Vector2D position_;
    Vector2D velocity_;
    
public:
    GameObject(const std::string& name) : name_(name), position_(0, 0), velocity_(0, 0) {}
    virtual ~GameObject() = default;
    
    const std::string& getName() const { return name_; }
    const Vector2D& getPosition() const { return position_; }
    const Vector2D& getVelocity() const { return velocity_; }
    void setPosition(const Vector2D& pos) { position_ = pos; }
    void setVelocity(const Vector2D& vel) { velocity_ = vel; }
};

// ポリシーベースのゲームエンティティ
template<template<typename> class RenderPolicy = OpenGLRenderPolicy,
         template<typename> class MovementPolicy = LinearMovementPolicy,
         template<typename> class AIPolicy = AggressiveAIPolicy>
class Enemy : public GameObject, 
              private RenderPolicy<Enemy<RenderPolicy, MovementPolicy, AIPolicy>>,
              private MovementPolicy<Enemy<RenderPolicy, MovementPolicy, AIPolicy>>,
              private AIPolicy<Enemy<RenderPolicy, MovementPolicy, AIPolicy>>
{
public:
    using SelfType = Enemy<RenderPolicy, MovementPolicy, AIPolicy>;
    
    explicit Enemy(const std::string& name) : GameObject(name)
    {
        setVelocity({5.0f, 0.0f});
    }
    
    void update(float deltaTime)
    {
        MovementPolicy<SelfType>::move(*this, deltaTime);
        AIPolicy<SelfType>::think(*this);
    }
    
    void render()
    {
        RenderPolicy<SelfType>::render(*this);
    }
};

// ソートポリシー
template<typename T>
class QuickSortPolicy
{
public:
    template<typename Iterator>
    static void sort(Iterator first, Iterator last)
    {
        std::cout << "クイックソートを実行" << std::endl;
        std::sort(first, last);
    }
};

template<typename T>
class BubbleSortPolicy
{
public:
    template<typename Iterator>
    static void sort(Iterator first, Iterator last)
    {
        std::cout << "バブルソートを実行" << std::endl;
        // バブルソートの実装
        for (auto i = first; i != last; ++i)
        {
            for (auto j = first; j != last - 1; ++j)
            {
                if (*j > *(j + 1))
                {
                    std::swap(*j, *(j + 1));
                }
            }
        }
    }
};

// ログポリシー
template<typename T>
class FileLogPolicy
{
public:
    static void log(const std::string& message)
    {
        std::cout << "[FILE] " << message << std::endl;
    }
};

template<typename T>
class ConsoleLogPolicy
{
public:
    static void log(const std::string& message)
    {
        std::cout << "[CONSOLE] " << message << std::endl;
    }
};

// ポリシーベースのコンテナ
template<typename T,
         template<typename> class SortPolicy = QuickSortPolicy,
         template<typename> class LogPolicy = ConsoleLogPolicy>
class GameContainer : private SortPolicy<T>, private LogPolicy<T>
{
private:
    std::vector<T> data_;
    
public:
    void add(const T& item)
    {
        data_.push_back(item);
        LogPolicy<T>::log("アイテムを追加: " + std::to_string(item));
    }
    
    void sort()
    {
        LogPolicy<T>::log("ソートを開始");
        SortPolicy<T>::sort(data_.begin(), data_.end());
        LogPolicy<T>::log("ソート完了");
    }
    
    void display() const
    {
        std::cout << "コンテナの内容: ";
        for (const auto& item : data_)
        {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    }
};

// セーブポリシー
template<typename T>
class BinarySavePolicy
{
public:
    static void save(const T& object, const std::string& filename)
    {
        std::cout << "バイナリ形式で" << filename << "に保存" << std::endl;
    }
};

template<typename T>
class JSONSavePolicy
{
public:
    static void save(const T& object, const std::string& filename)
    {
        std::cout << "JSON形式で" << filename << "に保存" << std::endl;
    }
};

// ポリシーベースのゲームマネージャー
template<template<typename> class SavePolicy = JSONSavePolicy>
class GameManager : private SavePolicy<GameManager<SavePolicy>>
{
private:
    std::map<std::string, int> gameState_;
    
public:
    void setGameState(const std::string& key, int value)
    {
        gameState_[key] = value;
    }
    
    void saveGame(const std::string& filename)
    {
        SavePolicy<GameManager<SavePolicy>>::save(*this, filename);
    }
    
    const auto& getGameState() const { return gameState_; }
};

int main()
{
    std::cout << "=== ポリシーベース設計 ===" << std::endl;
    
    // 1. ポリシーベーススマートポインタ
    std::cout << "\n--- スマートポインタ ---" << std::endl;
    {
        SmartPtr<int> ptr1(new int(42));
        SmartPtr<int> ptr2 = ptr1;
        std::cout << "参照カウント: " << ptr1.use_count() << std::endl;
        std::cout << "値: " << *ptr1 << std::endl;
    }
    
    // プールアロケータ版
    {
        SmartPtr<int, PoolAllocatorPolicy> poolPtr(new int(100));
        std::cout << "プール版の値: " << *poolPtr << std::endl;
    }
    
    // 2. ゲームエンティティ
    std::cout << "\n--- ゲームエンティティ ---" << std::endl;
    
    // 異なるポリシーの組み合わせ
    Enemy<DirectXRenderPolicy, LinearMovementPolicy, AggressiveAIPolicy> 
        directxEnemy("DirectX敵");
        
    Enemy<OpenGLRenderPolicy, SinusoidalMovementPolicy, DefensiveAIPolicy> 
        openglEnemy("OpenGL敵");
        
    Enemy<VulkanRenderPolicy, LinearMovementPolicy, DefensiveAIPolicy> 
        vulkanEnemy("Vulkan敵");
    
    std::cout << "エンティティ更新:" << std::endl;
    directxEnemy.update(0.016f);
    directxEnemy.render();
    std::cout << std::endl;
    
    openglEnemy.update(0.016f);
    openglEnemy.render();
    std::cout << std::endl;
    
    vulkanEnemy.update(0.016f);
    vulkanEnemy.render();
    
    // 3. ソートポリシー
    std::cout << "\n--- ソートポリシー ---" << std::endl;
    GameContainer<int, QuickSortPolicy, ConsoleLogPolicy> quickContainer;
    GameContainer<int, BubbleSortPolicy, FileLogPolicy> bubbleContainer;
    
    quickContainer.add(5);
    quickContainer.add(2);
    quickContainer.add(8);
    quickContainer.add(1);
    quickContainer.display();
    quickContainer.sort();
    quickContainer.display();
    
    std::cout << std::endl;
    
    bubbleContainer.add(9);
    bubbleContainer.add(3);
    bubbleContainer.add(7);
    bubbleContainer.display();
    bubbleContainer.sort();
    bubbleContainer.display();
    
    // 4. セーブポリシー
    std::cout << "\n--- セーブポリシー ---" << std::endl;
    GameManager<JSONSavePolicy> jsonManager;
    GameManager<BinarySavePolicy> binaryManager;
    
    jsonManager.setGameState("level", 25);
    jsonManager.setGameState("score", 10000);
    jsonManager.saveGame("save_json.dat");
    
    binaryManager.setGameState("level", 30);
    binaryManager.setGameState("score", 15000);
    binaryManager.saveGame("save_binary.dat");
    
    // 5. 実行時ポリシー選択のシミュレーション
    std::cout << "\n--- 実行時ポリシー選択 ---" << std::endl;
    std::string renderAPI = "OpenGL";  // 設定から読み込み
    
    if (renderAPI == "DirectX")
    {
        Enemy<DirectXRenderPolicy> enemy("設定済み敵");
        enemy.render();
    }
    else if (renderAPI == "OpenGL")
    {
        Enemy<OpenGLRenderPolicy> enemy("設定済み敵");
        enemy.render();
    }
    else if (renderAPI == "Vulkan")
    {
        Enemy<VulkanRenderPolicy> enemy("設定済み敵");
        enemy.render();
    }
    
    return 0;
}

// まとめ：
// - ポリシーテンプレートで動作をカスタマイズ
// - コンパイル時に最適なコードを生成
// - 柔軟性と性能を両立
// - ゲーム開発では描画API、AI、物理エンジンの選択に有効
// - 複数のポリシーを組み合わせて複雑な動作を実現