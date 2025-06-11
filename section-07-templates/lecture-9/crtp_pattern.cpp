/*
 * テンプレート第9講: CRTP (Curiously Recurring Template Pattern)
 * 
 * CRTPは派生クラスが基底クラスのテンプレートパラメータとして
 * 自分自身を渡すパターンです。静的ポリモルフィズムを実現し、
 * 仮想関数のオーバーヘッドなしでポリモルフィックな動作を提供します。
 * ゲーム開発では高性能なコンポーネントシステムや数学ライブラリで活用されます。
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <cmath>

// 1. 基本的なCRTPパターン
template<typename Derived>
class GameObjectBase
{
public:
    void update()
    {
        // 派生クラスの実装を呼び出す（静的ポリモルフィズム）
        static_cast<Derived*>(this)->updateImpl();
    }
    
    void render()
    {
        static_cast<Derived*>(this)->renderImpl();
    }
    
    std::string getName() const
    {
        return static_cast<const Derived*>(this)->getNameImpl();
    }
    
    // 共通の機能も提供可能
    void displayInfo()
    {
        std::cout << "=== " << getName() << " 情報 ===" << std::endl;
        std::cout << "タイプ: " << typeid(Derived).name() << std::endl;
    }
    
protected:
    ~GameObjectBase() = default; // 派生クラスからのみ削除可能
};

// 派生クラス: プレイヤー
class GamePlayer : public GameObjectBase<GamePlayer>
{
private:
    std::string name;
    int level;
    double health;
    
public:
    GamePlayer(const std::string& n, int l, double h) : name(n), level(l), health(h) {}
    
    void updateImpl()
    {
        std::cout << name << " のステータスを更新中..." << std::endl;
        // プレイヤー固有の更新処理
    }
    
    void renderImpl()
    {
        std::cout << name << " を描画中 (Lv." << level << ", HP:" << health << ")" << std::endl;
    }
    
    std::string getNameImpl() const
    {
        return "Player: " + name;
    }
    
    // プレイヤー固有のメソッド
    void levelUp()
    {
        ++level;
        health += 10;
        std::cout << name << " がレベルアップ！ (新レベル: " << level << ")" << std::endl;
    }
};

// 派生クラス: 敵
class GameEnemy : public GameObjectBase<GameEnemy>
{
private:
    std::string type;
    int attackPower;
    
public:
    GameEnemy(const std::string& t, int power) : type(t), attackPower(power) {}
    
    void updateImpl()
    {
        std::cout << type << " がAI処理中..." << std::endl;
        // 敵固有のAI処理
    }
    
    void renderImpl()
    {
        std::cout << type << " を描画中 (攻撃力:" << attackPower << ")" << std::endl;
    }
    
    std::string getNameImpl() const
    {
        return "Enemy: " + type;
    }
    
    void attack()
    {
        std::cout << type << " が攻撃！ (ダメージ: " << attackPower << ")" << std::endl;
    }
};

// 2. CRTPを使った数学ベクトルライブラリ
template<typename Derived, typename T, size_t Dimensions>
class VectorBase
{
public:
    // 派生クラスにキャストするヘルパー
    Derived& derived() { return static_cast<Derived&>(*this); }
    const Derived& derived() const { return static_cast<const Derived&>(*this); }
    
    // 共通演算子（CRTPで実装）
    Derived operator+(const Derived& other) const
    {
        Derived result;
        for (size_t i = 0; i < Dimensions; ++i)
        {
            result[i] = derived()[i] + other[i];
        }
        return result;
    }
    
    Derived operator-(const Derived& other) const
    {
        Derived result;
        for (size_t i = 0; i < Dimensions; ++i)
        {
            result[i] = derived()[i] - other[i];
        }
        return result;
    }
    
    Derived operator*(T scalar) const
    {
        Derived result;
        for (size_t i = 0; i < Dimensions; ++i)
        {
            result[i] = derived()[i] * scalar;
        }
        return result;
    }
    
    T dot(const Derived& other) const
    {
        T result = T{};
        for (size_t i = 0; i < Dimensions; ++i)
        {
            result += derived()[i] * other[i];
        }
        return result;
    }
    
    T length() const
    {
        return std::sqrt(dot(derived()));
    }
    
    Derived normalize() const
    {
        T len = length();
        return (len > T{}) ? derived() * (T{1} / len) : derived();
    }
    
    void display() const
    {
        std::cout << "(";
        for (size_t i = 0; i < Dimensions; ++i)
        {
            std::cout << derived()[i];
            if (i < Dimensions - 1) std::cout << ", ";
        }
        std::cout << ")" << std::endl;
    }
};

// 2Dベクトル
class Vector2D : public VectorBase<Vector2D, double, 2>
{
private:
    double data[2];
    
public:
    Vector2D(double x = 0.0, double y = 0.0) : data{x, y} {}
    
    double& operator[](size_t index) { return data[index]; }
    const double& operator[](size_t index) const { return data[index]; }
    
    double x() const { return data[0]; }
    double y() const { return data[1]; }
    
    // 2D固有のメソッド
    Vector2D perpendicular() const
    {
        return Vector2D(-data[1], data[0]);
    }
    
    double angle() const
    {
        return std::atan2(data[1], data[0]);
    }
};

// 3Dベクトル
class Vector3D : public VectorBase<Vector3D, double, 3>
{
private:
    double data[3];
    
public:
    Vector3D(double x = 0.0, double y = 0.0, double z = 0.0) : data{x, y, z} {}
    
    double& operator[](size_t index) { return data[index]; }
    const double& operator[](size_t index) const { return data[index]; }
    
    double x() const { return data[0]; }
    double y() const { return data[1]; }
    double z() const { return data[2]; }
    
    // 3D固有のメソッド
    Vector3D cross(const Vector3D& other) const
    {
        return Vector3D(
            data[1] * other.data[2] - data[2] * other.data[1],
            data[2] * other.data[0] - data[0] * other.data[2],
            data[0] * other.data[1] - data[1] * other.data[0]
        );
    }
};

// 3. CRTPを使ったシングルトンパターン
template<typename Derived>
class Singleton
{
public:
    static Derived& getInstance()
    {
        static Derived instance;
        return instance;
    }
    
    // コピー・ムーブを禁止
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;
    
protected:
    Singleton() = default;
    ~Singleton() = default;
};

// ゲーム設定管理クラス
class GameConfig : public Singleton<GameConfig>
{
private:
    friend class Singleton<GameConfig>;
    
    int screenWidth = 1920;
    int screenHeight = 1080;
    bool fullscreen = false;
    double volume = 1.0;
    
    GameConfig() = default;
    
public:
    void setScreenSize(int width, int height)
    {
        screenWidth = width;
        screenHeight = height;
        std::cout << "画面サイズ設定: " << width << "x" << height << std::endl;
    }
    
    void setFullscreen(bool fs)
    {
        fullscreen = fs;
        std::cout << "フルスクリーン: " << (fs ? "ON" : "OFF") << std::endl;
    }
    
    void setVolume(double vol)
    {
        volume = std::max(0.0, std::min(1.0, vol));
        std::cout << "音量設定: " << (volume * 100) << "%" << std::endl;
    }
    
    void displaySettings() const
    {
        std::cout << "=== ゲーム設定 ===" << std::endl;
        std::cout << "解像度: " << screenWidth << "x" << screenHeight << std::endl;
        std::cout << "フルスクリーン: " << (fullscreen ? "有効" : "無効") << std::endl;
        std::cout << "音量: " << (volume * 100) << "%" << std::endl;
    }
};

// 4. CRTPを使ったカウンター機能
template<typename Derived>
class ObjectCounter
{
private:
    static size_t count;
    
public:
    ObjectCounter()
    {
        ++count;
    }
    
    ObjectCounter(const ObjectCounter&)
    {
        ++count;
    }
    
    ~ObjectCounter()
    {
        --count;
    }
    
    static size_t getCount()
    {
        return count;
    }
};

template<typename Derived>
size_t ObjectCounter<Derived>::count = 0;

// カウンター機能を持つゲームアイテム
class CountedGameItem : public ObjectCounter<CountedGameItem>
{
private:
    std::string name;
    int value;
    
public:
    CountedGameItem(const std::string& n, int v) : name(n), value(v)
    {
        std::cout << "アイテム作成: " << name << " (総数: " << getCount() << ")" << std::endl;
    }
    
    ~CountedGameItem()
    {
        std::cout << "アイテム削除: " << name << " (残り: " << getCount() - 1 << ")" << std::endl;
    }
    
    void display() const
    {
        std::cout << "アイテム: " << name << " (価値: " << value << ")" << std::endl;
    }
};

// 5. 性能比較用の仮想関数版
class VirtualGameObject
{
public:
    virtual ~VirtualGameObject() = default;
    virtual void update() = 0;
    virtual void render() = 0;
    virtual std::string getName() const = 0;
};

class VirtualPlayer : public VirtualGameObject
{
private:
    std::string name;
    
public:
    VirtualPlayer(const std::string& n) : name(n) {}
    
    void update() override
    {
        // 更新処理
    }
    
    void render() override
    {
        // 描画処理
    }
    
    std::string getName() const override
    {
        return name;
    }
};

// パフォーマンス測定ヘルパー
template<typename Func>
auto measurePerformance(Func&& func, const std::string& description)
{
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    std::cout << description << ": " << duration << " マイクロ秒" << std::endl;
    return duration;
}

int main()
{
    std::cout << "=== CRTP (静的ポリモルフィズム) のデモ ===" << std::endl;
    
    // 1. 基本的なCRTPの使用例
    std::cout << "\n1. 基本的なCRTP:" << std::endl;
    GamePlayer player("勇者アリス", 25, 100.0);
    GameEnemy enemy("ドラゴン", 150);
    
    player.displayInfo();
    player.update();
    player.render();
    player.levelUp();
    
    std::cout << std::endl;
    enemy.displayInfo();
    enemy.update();
    enemy.render();
    enemy.attack();
    
    // 2. ベクトル数学ライブラリ
    std::cout << "\n2. CRTPベクトルライブラリ:" << std::endl;
    Vector2D pos1(3.0, 4.0);
    Vector2D pos2(1.0, 2.0);
    
    std::cout << "ベクトル1: ";
    pos1.display();
    std::cout << "ベクトル2: ";
    pos2.display();
    
    auto sum = pos1 + pos2;
    auto diff = pos1 - pos2;
    auto scaled = pos1 * 2.0;
    
    std::cout << "加算: ";
    sum.display();
    std::cout << "減算: ";
    diff.display();
    std::cout << "スケール: ";
    scaled.display();
    
    std::cout << "内積: " << pos1.dot(pos2) << std::endl;
    std::cout << "長さ: " << pos1.length() << std::endl;
    std::cout << "角度: " << pos1.angle() << " ラジアン" << std::endl;
    
    std::cout << "正規化: ";
    pos1.normalize().display();
    
    // 3D例
    std::cout << "\n3Dベクトル例:" << std::endl;
    Vector3D vec1(1.0, 0.0, 0.0);
    Vector3D vec2(0.0, 1.0, 0.0);
    
    auto cross = vec1.cross(vec2);
    std::cout << "外積: ";
    cross.display();
    
    // 3. シングルトンパターン
    std::cout << "\n3. CRTPシングルトン:" << std::endl;
    auto& config1 = GameConfig::getInstance();
    auto& config2 = GameConfig::getInstance();
    
    std::cout << "同じインスタンス: " << (&config1 == &config2) << std::endl;
    
    config1.setScreenSize(1280, 720);
    config1.setFullscreen(true);
    config1.setVolume(0.8);
    config1.displaySettings();
    
    // 4. オブジェクトカウンター
    std::cout << "\n4. オブジェクトカウンター:" << std::endl;
    
    {
        CountedGameItem item1("魔法の剣", 1000);
        CountedGameItem item2("体力ポーション", 50);
        
        std::cout << "現在のアイテム数: " << CountedGameItem::getCount() << std::endl;
        
        {
            CountedGameItem item3("マナポーション", 75);
            std::cout << "スコープ内アイテム数: " << CountedGameItem::getCount() << std::endl;
        }
        
        std::cout << "スコープ外アイテム数: " << CountedGameItem::getCount() << std::endl;
    }
    
    std::cout << "最終アイテム数: " << CountedGameItem::getCount() << std::endl;
    
    // 5. パフォーマンス比較
    std::cout << "\n5. パフォーマンス比較 (CRTP vs 仮想関数):" << std::endl;
    
    const int iterations = 1000000;
    
    // CRTP版
    GamePlayer crtpPlayer("CRTP Player", 1, 100.0);
    auto crtpTime = measurePerformance([&]() {
        for (int i = 0; i < iterations; ++i)
        {
            crtpPlayer.update();
        }
    }, "CRTP版");
    
    // 仮想関数版
    std::unique_ptr<VirtualGameObject> virtualPlayer = std::make_unique<VirtualPlayer>("Virtual Player");
    auto virtualTime = measurePerformance([&]() {
        for (int i = 0; i < iterations; ++i)
        {
            virtualPlayer->update();
        }
    }, "仮想関数版");
    
    std::cout << "CRTPは仮想関数より " << (static_cast<double>(virtualTime) / crtpTime) 
              << " 倍高速" << std::endl;
    
    // 6. テンプレート関数でのCRTP使用
    std::cout << "\n6. テンプレート関数でのCRTP使用:" << std::endl;
    
    auto processGameObject = [](auto& obj) {
        obj.displayInfo();
        obj.update();
        obj.render();
    };
    
    processGameObject(player);
    processGameObject(enemy);
    
    return 0;
}

/*
 * 学習ポイント:
 * 1. CRTPで静的ポリモルフィズムを実現
 * 2. 仮想関数のオーバーヘッドなしでポリモルフィック動作
 * 3. 派生クラスが基底クラスのテンプレートパラメータになる
 * 4. static_castで派生クラスの機能にアクセス
 * 5. シングルトンパターンなど他のパターンとも組み合わせ可能
 * 6. コンパイル時に型が決定されるため高性能
 * 7. ゲーム開発ではコンポーネントシステムで威力発揮
 * 8. 型安全性を保ちながらパフォーマンスを最適化
 */