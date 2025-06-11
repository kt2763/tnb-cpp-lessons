// 第4講: 静的ライブラリの作成と使用
// 再利用可能なコードをライブラリとして整理する方法を学ぶ

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

// このファイルは、実際のプロジェクトでは複数のファイルに分割される
// ここではライブラリの概念を説明するため、全てを一つのファイルに記述

// === Math Library (libmath.a) ===
// 数学関連のユーティリティライブラリ

namespace MathLib 
{
    // === 基本的な数学関数 ===
    class Vector2D 
    {
    public:
        double x, y;
        
        Vector2D(double x = 0, double y = 0) : x(x), y(y) {}
        
        // ベクトル演算
        Vector2D operator+(const Vector2D& other) const 
        {
            return Vector2D(x + other.x, y + other.y);
        }
        
        Vector2D operator-(const Vector2D& other) const 
        {
            return Vector2D(x - other.x, y - other.y);
        }
        
        Vector2D operator*(double scalar) const 
        {
            return Vector2D(x * scalar, y * scalar);
        }
        
        // 長さ計算
        double length() const 
        {
            return std::sqrt(x * x + y * y);
        }
        
        // 正規化
        Vector2D normalize() const 
        {
            double len = length();
            if (len == 0) return Vector2D(0, 0);
            return Vector2D(x / len, y / len);
        }
        
        // 距離計算
        double distance(const Vector2D& other) const 
        {
            return (*this - other).length();
        }
        
        void print() const 
        {
            std::cout << "(" << x << ", " << y << ")";
        }
    };
    
    // === 三角関数ユーティリティ ===
    class Trigonometry 
    {
    public:
        static constexpr double PI = 3.14159265359;
        
        static double degreesToRadians(double degrees) 
        {
            return degrees * PI / 180.0;
        }
        
        static double radiansToDegrees(double radians) 
        {
            return radians * 180.0 / PI;
        }
        
        static Vector2D angleToVector(double angleInDegrees) 
        {
            double radians = degreesToRadians(angleInDegrees);
            return Vector2D(std::cos(radians), std::sin(radians));
        }
    };
    
    // === 乱数生成 ===
    class Random 
    {
    private:
        static int seed;
        
    public:
        static void setSeed(int newSeed) 
        {
            seed = newSeed;
            std::srand(seed);
        }
        
        static int range(int min, int max) 
        {
            return min + (std::rand() % (max - min + 1));
        }
        
        static double range(double min, double max) 
        {
            double r = static_cast<double>(std::rand()) / RAND_MAX;
            return min + r * (max - min);
        }
        
        static bool chance(double probability) 
        {
            return range(0.0, 1.0) < probability;
        }
    };
    
    int Random::seed = 0;
}

// === Game Utils Library (libgameutils.a) ===
// ゲーム開発で共通して使う機能のライブラリ

namespace GameUtils 
{
    // === タイマークラス ===
    class Timer 
    {
    private:
        double startTime;
        double duration;
        bool isActive;
        
    public:
        Timer(double durationInSeconds = 1.0) 
            : startTime(0), duration(durationInSeconds), isActive(false) {}
        
        void start() 
        {
            startTime = 0;  // 実際はシステム時刻を取得
            isActive = true;
        }
        
        bool isFinished() const 
        {
            if (!isActive) return false;
            return getCurrentTime() >= startTime + duration;
        }
        
        double getProgress() const 
        {
            if (!isActive) return 0.0;
            double elapsed = getCurrentTime() - startTime;
            return std::min(elapsed / duration, 1.0);
        }
        
    private:
        double getCurrentTime() const 
        {
            // 実際はシステム時刻を返す
            static double time = 0;
            return time += 0.1;  // シミュレーション用
        }
    };
    
    // === カラーユーティリティ ===
    struct Color 
    {
        int r, g, b, a;
        
        Color(int red = 255, int green = 255, int blue = 255, int alpha = 255)
            : r(red), g(green), b(blue), a(alpha) {}
        
        static Color red() { return Color(255, 0, 0); }
        static Color green() { return Color(0, 255, 0); }
        static Color blue() { return Color(0, 0, 255); }
        static Color white() { return Color(255, 255, 255); }
        static Color black() { return Color(0, 0, 0); }
        
        Color blend(const Color& other, double ratio) const 
        {
            return Color(
                r + (other.r - r) * ratio,
                g + (other.g - g) * ratio,
                b + (other.b - b) * ratio,
                a + (other.a - a) * ratio
            );
        }
        
        void print() const 
        {
            std::cout << "RGBA(" << r << ", " << g << ", " << b << ", " << a << ")";
        }
    };
    
    // === インベントリシステム ===
    template<typename T>
    class Inventory 
    {
    private:
        std::vector<T> items;
        size_t maxCapacity;
        
    public:
        Inventory(size_t capacity = 10) : maxCapacity(capacity) {}
        
        bool addItem(const T& item) 
        {
            if (items.size() >= maxCapacity) 
            {
                return false;  // 満杯
            }
            items.push_back(item);
            return true;
        }
        
        bool removeItem(const T& item) 
        {
            auto it = std::find(items.begin(), items.end(), item);
            if (it != items.end()) 
            {
                items.erase(it);
                return true;
            }
            return false;
        }
        
        size_t getItemCount() const { return items.size(); }
        size_t getMaxCapacity() const { return maxCapacity; }
        bool isFull() const { return items.size() >= maxCapacity; }
        
        const std::vector<T>& getItems() const { return items; }
        
        void display() const 
        {
            std::cout << "インベントリ (" << items.size() << "/" << maxCapacity << "):" << std::endl;
            for (size_t i = 0; i < items.size(); i++) 
            {
                std::cout << "  " << i + 1 << ". " << items[i] << std::endl;
            }
        }
    };
}

// === Core Engine Library (libengine.a) ===
// ゲームエンジンのコア機能

namespace Engine 
{
    // === ゲームオブジェクト基底クラス ===
    class GameObject 
    {
    protected:
        MathLib::Vector2D position;
        std::string name;
        bool active;
        
    public:
        GameObject(const std::string& objectName, double x = 0, double y = 0)
            : position(x, y), name(objectName), active(true) {}
        
        virtual ~GameObject() = default;
        
        virtual void update(double deltaTime) = 0;
        virtual void render() const = 0;
        
        // アクセサ
        const MathLib::Vector2D& getPosition() const { return position; }
        void setPosition(const MathLib::Vector2D& pos) { position = pos; }
        const std::string& getName() const { return name; }
        bool isActive() const { return active; }
        void setActive(bool isActive) { active = isActive; }
    };
    
    // === プレイヤークラス ===
    class Player : public GameObject 
    {
    private:
        int health;
        int maxHealth;
        double speed;
        GameUtils::Inventory<std::string> inventory;
        
    public:
        Player(const std::string& playerName, double x = 0, double y = 0)
            : GameObject(playerName, x, y), health(100), maxHealth(100), 
              speed(5.0), inventory(10) {}
        
        void update(double deltaTime) override 
        {
            // プレイヤーの更新処理
        }
        
        void render() const override 
        {
            std::cout << "プレイヤー \"" << name << "\" を描画 at ";
            position.print();
            std::cout << std::endl;
        }
        
        void move(const MathLib::Vector2D& direction, double deltaTime) 
        {
            MathLib::Vector2D movement = direction.normalize() * speed * deltaTime;
            position = position + movement;
        }
        
        void takeDamage(int damage) 
        {
            health -= damage;
            if (health < 0) health = 0;
            std::cout << name << "は" << damage << "のダメージを受けた！ (HP: " 
                      << health << "/" << maxHealth << ")" << std::endl;
        }
        
        bool addToInventory(const std::string& item) 
        {
            if (inventory.addItem(item)) 
            {
                std::cout << item << "をインベントリに追加しました" << std::endl;
                return true;
            }
            else 
            {
                std::cout << "インベントリが満杯です！" << std::endl;
                return false;
            }
        }
        
        void showInventory() const 
        {
            inventory.display();
        }
        
        int getHealth() const { return health; }
        bool isAlive() const { return health > 0; }
    };
    
    // === ゲームシーン管理 ===
    class Scene 
    {
    protected:
        std::vector<std::unique_ptr<GameObject>> gameObjects;
        std::string sceneName;
        
    public:
        Scene(const std::string& name) : sceneName(name) {}
        virtual ~Scene() = default;
        
        virtual void initialize() = 0;
        virtual void update(double deltaTime) 
        {
            for (auto& obj : gameObjects) 
            {
                if (obj->isActive()) 
                {
                    obj->update(deltaTime);
                }
            }
        }
        
        virtual void render() const 
        {
            std::cout << "=== シーン: " << sceneName << " ===" << std::endl;
            for (const auto& obj : gameObjects) 
            {
                if (obj->isActive()) 
                {
                    obj->render();
                }
            }
        }
        
        void addGameObject(std::unique_ptr<GameObject> obj) 
        {
            gameObjects.push_back(std::move(obj));
        }
    };
}

// === ライブラリの使用例 ===
// 実際のゲームでこれらのライブラリを使用

class GameScene : public Engine::Scene 
{
private:
    Engine::Player* player;  // シーン内のプレイヤーへの参照
    
public:
    GameScene() : Engine::Scene("メインゲーム") {}
    
    void initialize() override 
    {
        std::cout << "ゲームシーンを初期化中..." << std::endl;
        
        // プレイヤーを作成
        auto playerObj = std::make_unique<Engine::Player>("勇者", 100, 100);
        player = playerObj.get();  // 参照を保存
        addGameObject(std::move(playerObj));
        
        // 初期アイテムを追加
        player->addToInventory("ポーション");
        player->addToInventory("鍵");
        player->addToInventory("パン");
    }
    
    void simulateGameplay() 
    {
        std::cout << "\n=== ゲームプレイシミュレーション ===" << std::endl;
        
        // 移動テスト
        MathLib::Vector2D moveDirection = MathLib::Trigonometry::angleToVector(45);  // 45度方向
        std::cout << "45度方向に移動: ";
        moveDirection.print();
        std::cout << std::endl;
        
        player->move(moveDirection, 1.0);  // 1秒間移動
        
        // ダメージテスト
        player->takeDamage(30);
        
        // 乱数テスト
        MathLib::Random::setSeed(12345);
        std::cout << "\nランダムダメージテスト:" << std::endl;
        for (int i = 0; i < 3; i++) 
        {
            int randomDamage = MathLib::Random::range(10, 25);
            std::cout << "ランダムダメージ: " << randomDamage << std::endl;
        }
        
        // クリティカル判定
        bool critical = MathLib::Random::chance(0.2);  // 20%の確率
        std::cout << "クリティカル判定: " << (critical ? "成功" : "失敗") << std::endl;
        
        // 色のブレンドテスト
        GameUtils::Color red = GameUtils::Color::red();
        GameUtils::Color blue = GameUtils::Color::blue();
        GameUtils::Color purple = red.blend(blue, 0.5);
        
        std::cout << "\n色のブレンド:" << std::endl;
        std::cout << "赤: "; red.print(); std::cout << std::endl;
        std::cout << "青: "; blue.print(); std::cout << std::endl;
        std::cout << "紫: "; purple.print(); std::cout << std::endl;
        
        // インベントリ表示
        std::cout << std::n";
        player->showInventory();
    }
};

int main() 
{
    std::cout << "=== 静的ライブラリの使用例 ===" << std::endl;
    
    // 各ライブラリの機能テスト
    std::cout << "\n=== MathLibテスト ===" << std::endl;
    MathLib::Vector2D v1(3, 4);
    MathLib::Vector2D v2(1, 2);
    
    std::cout << "ベクトルv1: "; v1.print(); std::cout << std::endl;
    std::cout << "ベクトルv2: "; v2.print(); std::cout << std::endl;
    std::cout << "v1の長さ: " << v1.length() << std::endl;
    std::cout << "v1とv2の距離: " << v1.distance(v2) << std::endl;
    
    MathLib::Vector2D sum = v1 + v2;
    std::cout << "v1 + v2 = "; sum.print(); std::cout << std::endl;
    
    std::cout << "\n=== GameUtilsテスト ===" << std::endl;
    GameUtils::Timer timer(2.0);
    timer.start();
    
    std::cout << "タイマー開始..." << std::endl;
    while (!timer.isFinished()) 
    {
        double progress = timer.getProgress();
        std::cout << "進行度: " << (progress * 100) << "%" << std::endl;
    }
    std::cout << "タイマー完了！" << std::endl;
    
    std::cout << "\n=== ゲームシーンテスト ===" << std::endl;
    GameScene scene;
    scene.initialize();
    scene.render();
    scene.update(1.0/60.0);  // 60FPSを想定
    scene.simulateGameplay();
    
    return 0;
}

/*
    まとめ - 静的ライブラリの活用:
    
    1. ライブラリの構成例:
       - libmath.a: 数学関数、ベクトル演算
       - libgameutils.a: ゲーム開発共通機能
       - libengine.a: ゲームエンジンコア
    
    2. ライブラリ設計の原則:
       - 単一責任の原則
       - 再利用性の重視
       - 依存関係の最小化
       - 明確なAPI設計
    
    3. 実際のビルドプロセス:
       // ライブラリ作成
       g++ -c math.cpp -o math.o
       ar rcs libmath.a math.o
       
       // ライブラリ使用
       g++ main.cpp -L. -lmath -o game
    
    4. 利点:
       - コードの再利用
       - プロジェクト間での共有
       - コンパイル時間の短縮
       - モジュラーな設計
       - チーム開発での分担
    
    5. ゲーム開発での活用:
       - 数学ライブラリ（物理計算）
       - グラフィックライブラリ
       - オーディオライブラリ
       - ネットワークライブラリ
       - AI・ゲームロジックライブラリ
*/