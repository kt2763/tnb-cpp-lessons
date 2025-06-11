// モジュールインターフェース設計
// 効果的なモジュールAPIの設計原則と実装パターン

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <optional>
#include <variant>
#include <concepts>

// 注意：このファイルはモジュールインターフェース設計の概念を説明するためのものです
// 実際のモジュール構文は次のようになります：
// export module game.engine;
// export class GameEngine { ... };

// =======================================
// インターフェース設計の基本原則
// =======================================

namespace interface_design_principles
{
    void explainDesignPrinciples()
    {
        std::cout << "=== モジュールインターフェース設計原則 ===" << std::endl;
        
        std::cout << "\n1. 最小公開の原則（Principle of Least Privilege）" << std::endl;
        std::cout << "   - 必要な機能のみをexport" << std::endl;
        std::cout << "   - 実装詳細は内部に隠蔽" << std::endl;
        std::cout << "   - 将来の変更に対する柔軟性を保持" << std::endl;
        
        std::cout << "\n2. 安定性の原則（Stability Principle）" << std::endl;
        std::cout << "   - 公開APIの変更を最小化" << std::endl;
        std::cout << "   - 後方互換性の維持" << std::endl;
        std::cout << "   - バージョニング戦略の考慮" << std::endl;
        
        std::cout << "\n3. 一貫性の原則（Consistency Principle）" << std::endl;
        std::cout << "   - 命名規則の統一" << std::endl;
        std::cout << "   - エラーハンドリング方式の統一" << std::endl;
        std::cout << "   - パラメータ順序の一貫性" << std::endl;
        
        std::cout << "\n4. 組み合わせ可能性（Composability）" << std::endl;
        std::cout << "   - 小さな機能の組み合わせで大きな機能を実現" << std::endl;
        std::cout << "   - 依存関係の最小化" << std::endl;
        std::cout << "   - 疎結合の設計" << std::endl;
    }
}

// =======================================
// パターン1: 基本的なクラスインターフェース
// =======================================

namespace basic_class_interface
{
    // 良いインターフェース設計の例
    // export class Vector2D  // 実際のモジュールでは export が必要
    class Vector2D
    {
    public:
        // コンストラクタ：明確で使いやすい
        Vector2D() : x_(0.0f), y_(0.0f) {}
        Vector2D(float x, float y) : x_(x), y_(y) {}
        
        // アクセサ：const correctness を遵守
        float x() const noexcept { return x_; }
        float y() const noexcept { return y_; }
        
        // ミューテータ：戻り値でチェーンを可能にする
        Vector2D& setX(float x) noexcept { x_ = x; return *this; }
        Vector2D& setY(float y) noexcept { y_ = y; return *this; }
        Vector2D& set(float x, float y) noexcept { x_ = x; y_ = y; return *this; }
        
        // 算術演算：直感的なインターフェース
        Vector2D operator+(const Vector2D& other) const noexcept
        {
            return Vector2D(x_ + other.x_, y_ + other.y_);
        }
        
        Vector2D operator-(const Vector2D& other) const noexcept
        {
            return Vector2D(x_ - other.x_, y_ - other.y_);
        }
        
        Vector2D operator*(float scalar) const noexcept
        {
            return Vector2D(x_ * scalar, y_ * scalar);
        }
        
        Vector2D& operator+=(const Vector2D& other) noexcept
        {
            x_ += other.x_;
            y_ += other.y_;
            return *this;
        }
        
        // ユーティリティ関数：よく使われる操作を提供
        float length() const noexcept
        {
            return std::sqrt(x_ * x_ + y_ * y_);
        }
        
        float lengthSquared() const noexcept
        {
            return x_ * x_ + y_ * y_;
        }
        
        Vector2D normalized() const
        {
            float len = length();
            if (len == 0.0f)
                throw std::domain_error("ゼロベクトルは正規化できません");
            return Vector2D(x_ / len, y_ / len);
        }
        
        // 比較演算：適切な精度で比較
        bool operator==(const Vector2D& other) const noexcept
        {
            const float epsilon = 1e-6f;
            return std::abs(x_ - other.x_) < epsilon && 
                   std::abs(y_ - other.y_) < epsilon;
        }
        
        bool operator!=(const Vector2D& other) const noexcept
        {
            return !(*this == other);
        }
        
    private:
        float x_, y_;
    };
    
    // フリー関数：自然な使用法を提供
    // export float dot(const Vector2D& a, const Vector2D& b)
    float dot(const Vector2D& a, const Vector2D& b) noexcept
    {
        return a.x() * b.x() + a.y() * b.y();
    }
    
    // export float cross(const Vector2D& a, const Vector2D& b)
    float cross(const Vector2D& a, const Vector2D& b) noexcept
    {
        return a.x() * b.y() - a.y() * b.x();
    }
    
    // export Vector2D lerp(const Vector2D& a, const Vector2D& b, float t)
    Vector2D lerp(const Vector2D& a, const Vector2D& b, float t) noexcept
    {
        return a + (b - a) * t;
    }
}

// =======================================
// パターン2: ファクトリーパターンによるインターフェース
// =======================================

namespace factory_interface
{
    // 抽象基底クラス
    // export class Weapon  // 実際のモジュールでは export
    class Weapon
    {
    public:
        virtual ~Weapon() = default;
        
        virtual std::string getName() const = 0;
        virtual int getDamage() const = 0;
        virtual float getRange() const = 0;
        virtual void attack() const = 0;
        
        // 共通の実装
        void displayInfo() const
        {
            std::cout << "武器: " << getName() 
                      << ", ダメージ: " << getDamage()
                      << ", 射程: " << getRange() << std::endl;
        }
    };
    
    // 具体的な実装（モジュール内部に隠蔽）
    class Sword : public Weapon
    {
    public:
        std::string getName() const override { return "剣"; }
        int getDamage() const override { return 50; }
        float getRange() const override { return 1.5f; }
        
        void attack() const override
        {
            std::cout << "剣で斬りつけた！" << std::endl;
        }
    };
    
    class Bow : public Weapon
    {
    public:
        std::string getName() const override { return "弓"; }
        int getDamage() const override { return 35; }
        float getRange() const override { return 10.0f; }
        
        void attack() const override
        {
            std::cout << "矢を放った！" << std::endl;
        }
    };
    
    class Staff : public Weapon
    {
    public:
        std::string getName() const override { return "杖"; }
        int getDamage() const override { return 40; }
        float getRange() const override { return 8.0f; }
        
        void attack() const override
        {
            std::cout << "魔法弾を発射した！" << std::endl;
        }
    };
    
    // ファクトリー関数（公開インターフェース）
    enum class WeaponType { Sword, Bow, Staff };
    
    // export std::unique_ptr<Weapon> createWeapon(WeaponType type)
    std::unique_ptr<Weapon> createWeapon(WeaponType type)
    {
        switch (type)
        {
            case WeaponType::Sword: return std::make_unique<Sword>();
            case WeaponType::Bow:   return std::make_unique<Bow>();
            case WeaponType::Staff: return std::make_unique<Staff>();
            default:
                throw std::invalid_argument("不明な武器タイプ");
        }
    }
    
    // export std::vector<WeaponType> getAvailableWeaponTypes()
    std::vector<WeaponType> getAvailableWeaponTypes()
    {
        return {WeaponType::Sword, WeaponType::Bow, WeaponType::Staff};
    }
}

// =======================================
// パターン3: 設定ベースのインターフェース
// =======================================

namespace configuration_interface
{
    // 設定構造体
    // export struct GameSettings  // 実際のモジュールでは export
    struct GameSettings
    {
        struct Graphics
        {
            int width = 1920;
            int height = 1080;
            bool fullscreen = false;
            int antiAliasing = 4;
            bool vsync = true;
        } graphics;
        
        struct Audio
        {
            float masterVolume = 1.0f;
            float musicVolume = 0.8f;
            float effectVolume = 0.9f;
            bool muted = false;
        } audio;
        
        struct Gameplay
        {
            float mouseSensitivity = 1.0f;
            bool autoSave = true;
            int difficulty = 2;  // 1=Easy, 2=Normal, 3=Hard
        } gameplay;
        
        // バリデーション関数
        bool isValid() const
        {
            return graphics.width > 0 && graphics.height > 0 &&
                   audio.masterVolume >= 0.0f && audio.masterVolume <= 1.0f &&
                   gameplay.difficulty >= 1 && gameplay.difficulty <= 3;
        }
        
        void applyDefaults()
        {
            *this = GameSettings{};  // デフォルト値で初期化
        }
    };
    
    // 設定管理クラス
    // export class GameConfiguration  // 実際のモジュールでは export
    class GameConfiguration
    {
    private:
        GameSettings settings_;
        std::vector<std::function<void(const GameSettings&)>> changeCallbacks_;
        
    public:
        // 設定の取得（読み取り専用）
        const GameSettings& getSettings() const noexcept
        {
            return settings_;
        }
        
        // 設定の更新（バリデーション付き）
        bool updateSettings(const GameSettings& newSettings)
        {
            if (!newSettings.isValid())
            {
                std::cout << "無効な設定が指定されました" << std::endl;
                return false;
            }
            
            settings_ = newSettings;
            notifyChanges();
            std::cout << "設定が更新されました" << std::endl;
            return true;
        }
        
        // 部分的な更新
        void updateGraphics(const GameSettings::Graphics& graphics)
        {
            GameSettings newSettings = settings_;
            newSettings.graphics = graphics;
            updateSettings(newSettings);
        }
        
        void updateAudio(const GameSettings::Audio& audio)
        {
            GameSettings newSettings = settings_;
            newSettings.audio = audio;
            updateSettings(newSettings);
        }
        
        // 変更通知の登録
        void addChangeCallback(std::function<void(const GameSettings&)> callback)
        {
            changeCallbacks_.push_back(callback);
        }
        
        // ファイル操作
        bool loadFromFile(const std::string& filename)
        {
            std::cout << "設定ファイル読み込み: " << filename << std::endl;
            // 実際の実装では JSON や YAML パーサーを使用
            return true;
        }
        
        bool saveToFile(const std::string& filename) const
        {
            std::cout << "設定ファイル保存: " << filename << std::endl;
            // 実際の実装では JSON や YAML 出力を使用
            return true;
        }
        
    private:
        void notifyChanges()
        {
            for (const auto& callback : changeCallbacks_)
            {
                callback(settings_);
            }
        }
    };
}

// =======================================
// パターン4: イベントシステムインターフェース
// =======================================

namespace event_system_interface
{
    // イベント基底クラス
    // export class Event  // 実際のモジュールでは export
    class Event
    {
    public:
        virtual ~Event() = default;
        virtual std::string getType() const = 0;
        virtual std::string toString() const = 0;
    };
    
    // 具体的なイベント型
    class PlayerMovedEvent : public Event
    {
    private:
        float x_, y_;
        
    public:
        PlayerMovedEvent(float x, float y) : x_(x), y_(y) {}
        
        std::string getType() const override { return "PlayerMoved"; }
        
        std::string toString() const override
        {
            return "PlayerMoved(" + std::to_string(x_) + ", " + std::to_string(y_) + ")";
        }
        
        float x() const { return x_; }
        float y() const { return y_; }
    };
    
    class EnemyDefeatedEvent : public Event
    {
    private:
        std::string enemyType_;
        int experience_;
        
    public:
        EnemyDefeatedEvent(const std::string& type, int exp) 
            : enemyType_(type), experience_(exp) {}
        
        std::string getType() const override { return "EnemyDefeated"; }
        
        std::string toString() const override
        {
            return "EnemyDefeated(" + enemyType_ + ", " + std::to_string(experience_) + "exp)";
        }
        
        const std::string& getEnemyType() const { return enemyType_; }
        int getExperience() const { return experience_; }
    };
    
    // イベントハンドラー型
    using EventHandler = std::function<void(const Event&)>;
    
    // イベントシステム
    // export class EventSystem  // 実際のモジュールでは export
    class EventSystem
    {
    private:
        std::unordered_map<std::string, std::vector<EventHandler>> handlers_;
        
    public:
        // イベントハンドラーの登録
        void subscribe(const std::string& eventType, EventHandler handler)
        {
            handlers_[eventType].push_back(handler);
            std::cout << "イベントハンドラー登録: " << eventType << std::endl;
        }
        
        // テンプレート版（型安全）
        template<typename EventType>
        void subscribe(std::function<void(const EventType&)> handler)
        {
            static_assert(std::is_base_of_v<Event, EventType>, "EventType は Event を継承する必要があります");
            
            EventType dummy{};  // 型推定のため
            auto typeErased = [handler](const Event& e) {
                if (auto* specificEvent = dynamic_cast<const EventType*>(&e))
                {
                    handler(*specificEvent);
                }
            };
            
            subscribe(dummy.getType(), typeErased);
        }
        
        // イベントの発行
        void publish(const Event& event)
        {
            std::cout << "イベント発行: " << event.toString() << std::endl;
            
            auto it = handlers_.find(event.getType());
            if (it != handlers_.end())
            {
                for (const auto& handler : it->second)
                {
                    handler(event);
                }
            }
        }
        
        // テンプレート版（簡潔な使用法）
        template<typename EventType, typename... Args>
        void publish(Args&&... args)
        {
            EventType event(std::forward<Args>(args)...);
            publish(event);
        }
        
        // 統計情報
        size_t getHandlerCount(const std::string& eventType) const
        {
            auto it = handlers_.find(eventType);
            return (it != handlers_.end()) ? it->second.size() : 0;
        }
    };
}

// =======================================
// パターン5: コンセプトベースのインターフェース
// =======================================

namespace concept_based_interface
{
    // ゲームオブジェクトのコンセプト
    template<typename T>
    concept GameObject = requires(T obj) {
        { obj.getPosition() } -> std::same_as<basic_class_interface::Vector2D>;
        { obj.setPosition(basic_class_interface::Vector2D{}) } -> std::same_as<void>;
        { obj.update(1.0f) } -> std::same_as<void>;
        { obj.render() } -> std::same_as<void>;
    };
    
    // 攻撃可能オブジェクトのコンセプト
    template<typename T>
    concept Attackable = requires(T obj) {
        { obj.takeDamage(1) } -> std::same_as<void>;
        { obj.getHealth() } -> std::convertible_to<int>;
        { obj.isAlive() } -> std::same_as<bool>;
    };
    
    // 移動可能オブジェクトのコンセプト
    template<typename T>
    concept Movable = requires(T obj) {
        { obj.getVelocity() } -> std::same_as<basic_class_interface::Vector2D>;
        { obj.setVelocity(basic_class_interface::Vector2D{}) } -> std::same_as<void>;
        { obj.getMaxSpeed() } -> std::convertible_to<float>;
    };
    
    // 汎用的な処理関数
    // export template<GameObject T> void updateGameObject(T& obj, float deltaTime)
    template<GameObject T>
    void updateGameObject(T& obj, float deltaTime)
    {
        obj.update(deltaTime);
        std::cout << "ゲームオブジェクト更新完了" << std::endl;
    }
    
    // export template<Attackable T> bool dealDamage(T& target, int damage)
    template<Attackable T>
    bool dealDamage(T& target, int damage)
    {
        target.takeDamage(damage);
        std::cout << "ダメージ " << damage << " を与えた" << std::endl;
        return target.isAlive();
    }
    
    // export template<Movable T> void applyPhysics(T& obj, float deltaTime)
    template<Movable T>
    void applyPhysics(T& obj, float deltaTime)
    {
        auto velocity = obj.getVelocity();
        auto position = obj.getPosition();
        
        // 簡単な物理計算
        position += velocity * deltaTime;
        obj.setPosition(position);
        
        std::cout << "物理計算適用: 新しい位置(" << position.x() << ", " << position.y() << ")" << std::endl;
    }
    
    // 複合コンセプトの例
    template<typename T>
    concept GameEntity = GameObject<T> && Attackable<T> && Movable<T>;
    
    // export template<GameEntity T> void processEntity(T& entity, float deltaTime)
    template<GameEntity T>
    void processEntity(T& entity, float deltaTime)
    {
        updateGameObject(entity, deltaTime);
        applyPhysics(entity, deltaTime);
        
        std::cout << "エンティティ処理完了: 体力 " << entity.getHealth() << std::endl;
    }
}

// =======================================
// 使用例とテスト
// =======================================

namespace usage_examples
{
    void demonstrateBasicInterface()
    {
        std::cout << "\n=== 基本インターフェースの使用例 ===" << std::endl;
        
        using namespace basic_class_interface;
        
        Vector2D v1(3.0f, 4.0f);
        Vector2D v2(1.0f, 2.0f);
        
        std::cout << "v1: (" << v1.x() << ", " << v1.y() << ")" << std::endl;
        std::cout << "v2: (" << v2.x() << ", " << v2.y() << ")" << std::endl;
        
        Vector2D v3 = v1 + v2;
        std::cout << "v1 + v2: (" << v3.x() << ", " << v3.y() << ")" << std::endl;
        
        std::cout << "v1の長さ: " << v1.length() << std::endl;
        std::cout << "ドット積: " << dot(v1, v2) << std::endl;
        
        Vector2D v4 = lerp(v1, v2, 0.5f);
        std::cout << "補間(0.5): (" << v4.x() << ", " << v4.y() << ")" << std::endl;
    }
    
    void demonstrateFactoryInterface()
    {
        std::cout << "\n=== ファクトリーインターフェースの使用例 ===" << std::endl;
        
        using namespace factory_interface;
        
        auto types = getAvailableWeaponTypes();
        std::cout << "利用可能な武器数: " << types.size() << std::endl;
        
        for (auto type : types)
        {
            auto weapon = createWeapon(type);
            weapon->displayInfo();
            weapon->attack();
        }
    }
    
    void demonstrateConfigurationInterface()
    {
        std::cout << "\n=== 設定インターフェースの使用例 ===" << std::endl;
        
        using namespace configuration_interface;
        
        GameConfiguration config;
        
        // 現在の設定を表示
        auto settings = config.getSettings();
        std::cout << "解像度: " << settings.graphics.width << "x" << settings.graphics.height << std::endl;
        std::cout << "音量: " << settings.audio.masterVolume << std::endl;
        
        // 変更通知の登録
        config.addChangeCallback([](const GameSettings& newSettings) {
            std::cout << "設定変更通知: 解像度 " << newSettings.graphics.width 
                      << "x" << newSettings.graphics.height << std::endl;
        });
        
        // 設定の更新
        GameSettings newSettings = settings;
        newSettings.graphics.width = 2560;
        newSettings.graphics.height = 1440;
        config.updateSettings(newSettings);
    }
    
    void demonstrateEventInterface()
    {
        std::cout << "\n=== イベントインターフェースの使用例 ===" << std::endl;
        
        using namespace event_system_interface;
        
        EventSystem eventSystem;
        
        // ハンドラーの登録
        eventSystem.subscribe<PlayerMovedEvent>([](const PlayerMovedEvent& e) {
            std::cout << "プレイヤー移動処理: (" << e.x() << ", " << e.y() << ")" << std::endl;
        });
        
        eventSystem.subscribe<EnemyDefeatedEvent>([](const EnemyDefeatedEvent& e) {
            std::cout << "敵撃破処理: " << e.getEnemyType() 
                      << " から " << e.getExperience() << " exp獲得" << std::endl;
        });
        
        // イベントの発行
        eventSystem.publish<PlayerMovedEvent>(10.5f, 20.3f);
        eventSystem.publish<EnemyDefeatedEvent>("ゴブリン", 50);
    }
}

int main()
{
    std::cout << "=== モジュールインターフェース設計 ===" << std::endl;
    
    // 1. 設計原則の説明
    interface_design_principles::explainDesignPrinciples();
    
    // 2. 各種インターフェースパターンの使用例
    usage_examples::demonstrateBasicInterface();
    usage_examples::demonstrateFactoryInterface();
    usage_examples::demonstrateConfigurationInterface();
    usage_examples::demonstrateEventInterface();
    
    std::cout << "\n=== インターフェース設計のベストプラクティス ===" << std::endl;
    std::cout << "1. 最小公開の原則：必要な機能のみをexport" << std::endl;
    std::cout << "2. 型安全性：テンプレートとコンセプトを活用" << std::endl;
    std::cout << "3. 例外安全性：適切なエラーハンドリング" << std::endl;
    std::cout << "4. 拡張性：将来の機能追加を考慮した設計" << std::endl;
    std::cout << "5. 使いやすさ：直感的で一貫したAPI" << std::endl;
    std::cout << "6. パフォーマンス：効率的な実装の隠蔽" << std::endl;
    
    return 0;
}

// まとめ：
// - モジュールインターフェースは最小公開の原則に従う
// - ファクトリーパターンで実装詳細を隠蔽
// - 設定ベースのインターフェースで柔軟性を提供
// - イベントシステムで疎結合を実現
// - コンセプトで型安全な汎用処理を提供
// - 一貫性のあるAPIでユーザビリティを向上
// - ゲーム開発では特に拡張性と保守性が重要