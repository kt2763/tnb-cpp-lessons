/*
 * テンプレート第12講: 実践的なテンプレート活用例
 * 
 * 実際のゲーム開発で使用される高度なテンプレート技法を学習します。
 * パフォーマンス重視のゲームエンジンコンポーネント、
 * 型安全なリソース管理、メモリ効率的なデータ構造など、
 * 実世界で求められるテンプレートの使用例を実装します。
 */

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <memory>
#include <functional>
#include <type_traits>
#include <chrono>
#include <random>
#include <algorithm>
#include <variant>

// 1. 高性能なゲームエンティティコンポーネントシステム (ECS)
namespace ecs
{
    // コンポーネントID生成器
    class ComponentTypeID
    {
    private:
        static size_t nextID;
        
    public:
        template<typename T>
        static size_t getID()
        {
            static size_t id = nextID++;
            return id;
        }
    };
    
    size_t ComponentTypeID::nextID = 0;
    
    // エンティティID型
    using EntityID = size_t;
    
    // 高速なコンポーネントストレージ
    template<typename ComponentType>
    class ComponentArray
    {
    private:
        std::array<ComponentType, 1000> components; // 固定サイズで高速アクセス
        std::array<EntityID, 1000> entityToIndex;
        std::array<EntityID, 1000> indexToEntity;
        size_t size = 0;
        
    public:
        void insertComponent(EntityID entity, const ComponentType& component)
        {
            size_t newIndex = size++;
            entityToIndex[entity] = newIndex;
            indexToEntity[newIndex] = entity;
            components[newIndex] = component;
        }
        
        void removeComponent(EntityID entity)
        {
            size_t indexToRemove = entityToIndex[entity];
            size_t lastIndex = --size;
            
            // 最後の要素を削除位置に移動
            components[indexToRemove] = components[lastIndex];
            
            EntityID lastEntity = indexToEntity[lastIndex];
            entityToIndex[lastEntity] = indexToRemove;
            indexToEntity[indexToRemove] = lastEntity;
        }
        
        ComponentType& getComponent(EntityID entity)
        {
            return components[entityToIndex[entity]];
        }
        
        bool hasComponent(EntityID entity) const
        {
            return entityToIndex[entity] < size;
        }
        
        // 全コンポーネントの反復処理
        template<typename Func>
        void forEach(Func&& func)
        {
            for (size_t i = 0; i < size; ++i)
            {
                func(indexToEntity[i], components[i]);
            }
        }
        
        size_t getSize() const { return size; }
    };
    
    // コンポーネントマネージャー
    class ComponentManager
    {
    private:
        std::unordered_map<size_t, std::unique_ptr<void, void(*)(void*)>> componentArrays;
        
    public:
        template<typename T>
        void registerComponent()
        {
            size_t typeID = ComponentTypeID::getID<T>();
            
            auto deleter = [](void* ptr) {
                delete static_cast<ComponentArray<T>*>(ptr);
            };
            
            componentArrays[typeID] = std::unique_ptr<void, void(*)(void*)>(
                new ComponentArray<T>(), deleter
            );
        }
        
        template<typename T>
        ComponentArray<T>& getComponentArray()
        {
            size_t typeID = ComponentTypeID::getID<T>();
            return *static_cast<ComponentArray<T>*>(componentArrays[typeID].get());
        }
        
        template<typename T>
        void addComponent(EntityID entity, const T& component)
        {
            getComponentArray<T>().insertComponent(entity, component);
        }
        
        template<typename T>
        void removeComponent(EntityID entity)
        {
            getComponentArray<T>().removeComponent(entity);
        }
        
        template<typename T>
        T& getComponent(EntityID entity)
        {
            return getComponentArray<T>().getComponent(entity);
        }
        
        template<typename T>
        bool hasComponent(EntityID entity)
        {
            return getComponentArray<T>().hasComponent(entity);
        }
    };
    
    // システム基底クラス
    class System
    {
    public:
        virtual ~System() = default;
        virtual void update(double deltaTime, ComponentManager& componentManager) = 0;
    };
    
    // テンプレートシステム
    template<typename... ComponentTypes>
    class GameSystem : public System
    {
    public:
        void update(double deltaTime, ComponentManager& componentManager) override
        {
            // 全エンティティをチェックして必要なコンポーネントを持つもので処理実行
            processEntities(deltaTime, componentManager);
        }
        
    protected:
        virtual void processEntity(EntityID entity, ComponentTypes&... components, double deltaTime) = 0;
        
    private:
        void processEntities(double deltaTime, ComponentManager& componentManager)
        {
            // 最初のコンポーネント配列を基準に反復
            auto& firstArray = componentManager.getComponentArray<
                std::tuple_element_t<0, std::tuple<ComponentTypes...>>
            >();
            
            firstArray.forEach([&](EntityID entity, auto& firstComponent) {
                if (hasAllComponents<ComponentTypes...>(entity, componentManager))
                {
                    processEntity(entity, componentManager.getComponent<ComponentTypes>(entity)..., deltaTime);
                }
            });
        }
        
        template<typename... Types>
        bool hasAllComponents(EntityID entity, ComponentManager& componentManager)
        {
            return (componentManager.hasComponent<Types>(entity) && ...);
        }
    };
}

// 2. 型安全なリソースマネージャー
namespace resource_management
{
    // リソースタイプの型安全な識別
    template<typename ResourceType>
    class ResourceID
    {
    private:
        size_t id;
        
    public:
        explicit ResourceID(size_t value) : id(value) {}
        
        size_t getValue() const { return id; }
        
        bool operator==(const ResourceID& other) const { return id == other.id; }
        bool operator!=(const ResourceID& other) const { return id != other.id; }
        bool operator<(const ResourceID& other) const { return id < other.id; }
    };
    
    // ハッシュ関数特殊化
    template<typename ResourceType>
    struct std::hash<resource_management::ResourceID<ResourceType>>
    {
        size_t operator()(const resource_management::ResourceID<ResourceType>& id) const
        {
            return std::hash<size_t>{}(id.getValue());
        }
    };
    
    // リソースマネージャー
    template<typename ResourceType>
    class ResourceManager
    {
    private:
        std::unordered_map<ResourceID<ResourceType>, std::unique_ptr<ResourceType>> resources;
        size_t nextID = 1;
        
    public:
        template<typename... Args>
        ResourceID<ResourceType> createResource(Args&&... args)
        {
            auto id = ResourceID<ResourceType>(nextID++);
            resources[id] = std::make_unique<ResourceType>(std::forward<Args>(args)...);
            return id;
        }
        
        ResourceType* getResource(ResourceID<ResourceType> id)
        {
            auto it = resources.find(id);
            return (it != resources.end()) ? it->second.get() : nullptr;
        }
        
        bool releaseResource(ResourceID<ResourceType> id)
        {
            return resources.erase(id) > 0;
        }
        
        size_t getResourceCount() const
        {
            return resources.size();
        }
        
        // リソースの一括処理
        template<typename Func>
        void forEachResource(Func&& func)
        {
            for (auto& [id, resource] : resources)
            {
                func(id, *resource);
            }
        }
    };
}

// 3. 高性能なメモリプール
namespace memory_management
{
    template<typename T, size_t BlockSize = 1024>
    class ObjectPool
    {
    private:
        struct Block
        {
            alignas(T) std::byte data[sizeof(T)];
            bool inUse = false;
        };
        
        std::array<Block, BlockSize> pool;
        std::vector<size_t> freeIndices;
        
    public:
        ObjectPool()
        {
            // 全てのインデックスを空きリストに追加
            for (size_t i = 0; i < BlockSize; ++i)
            {
                freeIndices.push_back(i);
            }
        }
        
        ~ObjectPool()
        {
            // 使用中のオブジェクトを全て破棄
            for (size_t i = 0; i < BlockSize; ++i)
            {
                if (pool[i].inUse)
                {
                    reinterpret_cast<T*>(&pool[i].data)->~T();
                }
            }
        }
        
        template<typename... Args>
        T* acquire(Args&&... args)
        {
            if (freeIndices.empty())
            {
                return nullptr; // プール満杯
            }
            
            size_t index = freeIndices.back();
            freeIndices.pop_back();
            
            pool[index].inUse = true;
            
            // placement newでオブジェクト構築
            return new(&pool[index].data) T(std::forward<Args>(args)...);
        }
        
        void release(T* obj)
        {
            // オブジェクトがプール内のものかチェック
            size_t index = (reinterpret_cast<Block*>(obj) - pool.data());
            
            if (index < BlockSize && pool[index].inUse)
            {
                obj->~T();
                pool[index].inUse = false;
                freeIndices.push_back(index);
            }
        }
        
        size_t getFreeCount() const
        {
            return freeIndices.size();
        }
        
        size_t getUsedCount() const
        {
            return BlockSize - freeIndices.size();
        }
        
        constexpr size_t getCapacity() const
        {
            return BlockSize;
        }
    };
}

// 4. 関数型プログラミング風のゲームイベントシステム
namespace event_system
{
    template<typename EventType>
    class EventDispatcher
    {
    private:
        std::vector<std::function<void(const EventType&)>> handlers;
        
    public:
        template<typename Handler>
        void subscribe(Handler&& handler)
        {
            handlers.emplace_back(std::forward<Handler>(handler));
        }
        
        void dispatch(const EventType& event)
        {
            for (auto& handler : handlers)
            {
                handler(event);
            }
        }
        
        void clear()
        {
            handlers.clear();
        }
        
        size_t getHandlerCount() const
        {
            return handlers.size();
        }
    };
    
    // 型安全なイベントバス
    class EventBus
    {
    private:
        std::unordered_map<size_t, std::unique_ptr<void, void(*)(void*)>> dispatchers;
        
        template<typename T>
        size_t getTypeHash()
        {
            return typeid(T).hash_code();
        }
        
    public:
        template<typename EventType>
        void subscribe(std::function<void(const EventType&)> handler)
        {
            size_t typeHash = getTypeHash<EventType>();
            
            if (dispatchers.find(typeHash) == dispatchers.end())
            {
                auto deleter = [](void* ptr) {
                    delete static_cast<EventDispatcher<EventType>*>(ptr);
                };
                
                dispatchers[typeHash] = std::unique_ptr<void, void(*)(void*)>(
                    new EventDispatcher<EventType>(), deleter
                );
            }
            
            auto* dispatcher = static_cast<EventDispatcher<EventType>*>(dispatchers[typeHash].get());
            dispatcher->subscribe(std::move(handler));
        }
        
        template<typename EventType>
        void dispatch(const EventType& event)
        {
            size_t typeHash = getTypeHash<EventType>();
            auto it = dispatchers.find(typeHash);
            
            if (it != dispatchers.end())
            {
                auto* dispatcher = static_cast<EventDispatcher<EventType>*>(it->second.get());
                dispatcher->dispatch(event);
            }
        }
    };
}

// 実際のゲームコンポーネントとシステムの例
namespace game_example
{
    // コンポーネント定義
    struct Position
    {
        double x, y;
        Position(double x_val = 0.0, double y_val = 0.0) : x(x_val), y(y_val) {}
    };
    
    struct Velocity
    {
        double dx, dy;
        Velocity(double dx_val = 0.0, double dy_val = 0.0) : dx(dx_val), dy(dy_val) {}
    };
    
    struct Health
    {
        int current, maximum;
        Health(int hp = 100) : current(hp), maximum(hp) {}
    };
    
    struct Sprite
    {
        std::string textureName;
        int width, height;
        Sprite(const std::string& name = "", int w = 32, int h = 32) 
            : textureName(name), width(w), height(h) {}
    };
    
    // 移動システム
    class MovementSystem : public ecs::GameSystem<Position, Velocity>
    {
    protected:
        void processEntity(ecs::EntityID entity, Position& pos, Velocity& vel, double deltaTime) override
        {
            pos.x += vel.dx * deltaTime;
            pos.y += vel.dy * deltaTime;
            
            // 画面境界チェック
            if (pos.x < 0.0 || pos.x > 800.0)
            {
                vel.dx = -vel.dx;
                pos.x = std::clamp(pos.x, 0.0, 800.0);
            }
            
            if (pos.y < 0.0 || pos.y > 600.0)
            {
                vel.dy = -vel.dy;
                pos.y = std::clamp(pos.y, 0.0, 600.0);
            }
        }
    };
    
    // 描画システム
    class RenderSystem : public ecs::GameSystem<Position, Sprite>
    {
    protected:
        void processEntity(ecs::EntityID entity, Position& pos, Sprite& sprite, double deltaTime) override
        {
            std::cout << "描画: " << sprite.textureName 
                      << " at (" << pos.x << ", " << pos.y << ")" << std::endl;
        }
    };
    
    // イベント定義
    struct PlayerDamagedEvent
    {
        ecs::EntityID playerId;
        int damage;
        PlayerDamagedEvent(ecs::EntityID id, int dmg) : playerId(id), damage(dmg) {}
    };
    
    struct EnemyDefeatedEvent
    {
        ecs::EntityID enemyId;
        int expGained;
        EnemyDefeatedEvent(ecs::EntityID id, int exp) : enemyId(id), expGained(exp) {}
    };
    
    // リソース定義
    struct Texture
    {
        std::string filename;
        int width, height;
        
        Texture(const std::string& file, int w, int h) 
            : filename(file), width(w), height(h)
        {
            std::cout << "テクスチャ読み込み: " << filename << std::endl;
        }
        
        ~Texture()
        {
            std::cout << "テクスチャ解放: " << filename << std::endl;
        }
    };
    
    struct Sound
    {
        std::string filename;
        double duration;
        
        Sound(const std::string& file, double dur) : filename(file), duration(dur)
        {
            std::cout << "サウンド読み込み: " << filename << std::endl;
        }
        
        ~Sound()
        {
            std::cout << "サウンド解放: " << filename << std::endl;
        }
    };
}

// パフォーマンス測定ヘルパー
template<typename Func>
auto benchmarkFunction(Func&& func, int iterations = 1000, const std::string& name = "")
{
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i)
    {
        func();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    if (!name.empty())
    {
        std::cout << name << " (" << iterations << "回): " 
                  << duration.count() << " μs (平均: " 
                  << (duration.count() / iterations) << " μs/回)" << std::endl;
    }
    
    return duration;
}

int main()
{
    std::cout << "=== 実践的なテンプレート活用例のデモ ===" << std::endl;
    
    // 1. ECS (Entity Component System) のデモ
    std::cout << "\n1. Entity Component System デモ:" << std::endl;
    
    ecs::ComponentManager componentManager;
    componentManager.registerComponent<game_example::Position>();
    componentManager.registerComponent<game_example::Velocity>();
    componentManager.registerComponent<game_example::Health>();
    componentManager.registerComponent<game_example::Sprite>();
    
    // エンティティ作成
    ecs::EntityID player = 1;
    ecs::EntityID enemy = 2;
    
    componentManager.addComponent(player, game_example::Position(100.0, 200.0));
    componentManager.addComponent(player, game_example::Velocity(50.0, 30.0));
    componentManager.addComponent(player, game_example::Health(100));
    componentManager.addComponent(player, game_example::Sprite("player.png", 32, 32));
    
    componentManager.addComponent(enemy, game_example::Position(300.0, 150.0));
    componentManager.addComponent(enemy, game_example::Velocity(-20.0, 40.0));
    componentManager.addComponent(enemy, game_example::Health(75));
    componentManager.addComponent(enemy, game_example::Sprite("enemy.png", 24, 24));
    
    // システム作成と更新
    game_example::MovementSystem movementSystem;
    game_example::RenderSystem renderSystem;
    
    std::cout << "ゲームループ実行:" << std::endl;
    double deltaTime = 0.016; // 60 FPS
    
    for (int frame = 0; frame < 3; ++frame)
    {
        std::cout << "\n--- フレーム " << frame + 1 << " ---" << std::endl;
        movementSystem.update(deltaTime, componentManager);
        renderSystem.update(deltaTime, componentManager);
    }
    
    // 2. 型安全なリソースマネージャーのデモ
    std::cout << "\n2. 型安全なリソースマネージャー デモ:" << std::endl;
    
    resource_management::ResourceManager<game_example::Texture> textureManager;
    resource_management::ResourceManager<game_example::Sound> soundManager;
    
    auto playerTexID = textureManager.createResource("player.png", 32, 32);
    auto enemyTexID = textureManager.createResource("enemy.png", 24, 24);
    auto bgmID = soundManager.createResource("bgm.ogg", 180.5);
    auto sfxID = soundManager.createResource("hit.wav", 0.3);
    
    std::cout << "テクスチャ数: " << textureManager.getResourceCount() << std::endl;
    std::cout << "サウンド数: " << soundManager.getResourceCount() << std::endl;
    
    // リソースアクセス
    if (auto* texture = textureManager.getResource(playerTexID))
    {
        std::cout << "プレイヤーテクスチャ: " << texture->filename 
                  << " (" << texture->width << "x" << texture->height << ")" << std::endl;
    }
    
    // リソース解放
    textureManager.releaseResource(enemyTexID);
    std::cout << "解放後のテクスチャ数: " << textureManager.getResourceCount() << std::endl;
    
    // 3. オブジェクトプールのデモ
    std::cout << "\n3. オブジェクトプール デモ:" << std::endl;
    
    memory_management::ObjectPool<game_example::Position, 100> positionPool;
    
    std::vector<game_example::Position*> positions;
    
    // オブジェクト取得
    for (int i = 0; i < 10; ++i)
    {
        if (auto* pos = positionPool.acquire(i * 10.0, i * 5.0))
        {
            positions.push_back(pos);
        }
    }
    
    std::cout << "使用中: " << positionPool.getUsedCount() 
              << " / " << positionPool.getCapacity() << std::endl;
    
    // オブジェクト解放
    for (auto* pos : positions)
    {
        positionPool.release(pos);
    }
    
    std::cout << "解放後使用中: " << positionPool.getUsedCount() << std::endl;
    
    // 4. イベントシステムのデモ
    std::cout << "\n4. イベントシステム デモ:" << std::endl;
    
    event_system::EventBus eventBus;
    
    // イベントハンドラー登録
    eventBus.subscribe<game_example::PlayerDamagedEvent>([](const auto& event) {
        std::cout << "プレイヤー " << event.playerId << " が " 
                  << event.damage << " ダメージを受けました" << std::endl;
    });
    
    eventBus.subscribe<game_example::EnemyDefeatedEvent>([](const auto& event) {
        std::cout << "敵 " << event.enemyId << " を倒して " 
                  << event.expGained << " 経験値を獲得！" << std::endl;
    });
    
    // イベント発生
    eventBus.dispatch(game_example::PlayerDamagedEvent(player, 15));
    eventBus.dispatch(game_example::EnemyDefeatedEvent(enemy, 250));
    
    // 5. パフォーマンス比較
    std::cout << "\n5. パフォーマンス比較:" << std::endl;
    
    // 標準vectorとオブジェクトプールの比較
    std::vector<std::unique_ptr<game_example::Position>> vectorStorage;
    memory_management::ObjectPool<game_example::Position, 1000> poolStorage;
    
    // vector版
    benchmarkFunction([&]() {
        vectorStorage.clear();
        for (int i = 0; i < 100; ++i)
        {
            vectorStorage.push_back(std::make_unique<game_example::Position>(i, i));
        }
    }, 1000, "std::vector + unique_ptr");
    
    // プール版
    std::vector<game_example::Position*> pooledObjects;
    benchmarkFunction([&]() {
        for (auto* obj : pooledObjects)
        {
            poolStorage.release(obj);
        }
        pooledObjects.clear();
        
        for (int i = 0; i < 100; ++i)
        {
            if (auto* pos = poolStorage.acquire(i, i))
            {
                pooledObjects.push_back(pos);
            }
        }
    }, 1000, "ObjectPool");
    
    // クリーンアップ
    for (auto* obj : pooledObjects)
    {
        poolStorage.release(obj);
    }
    
    // 6. メモリ使用量の比較
    std::cout << "\n6. メモリ使用量情報:" << std::endl;
    std::cout << "Position構造体サイズ: " << sizeof(game_example::Position) << " バイト" << std::endl;
    std::cout << "ObjectPoolブロックサイズ: " << sizeof(memory_management::ObjectPool<game_example::Position, 100>) << " バイト" << std::endl;
    std::cout << "ComponentArrayサイズ: " << sizeof(ecs::ComponentArray<game_example::Position>) << " バイト" << std::endl;
    
    return 0;
}

/*
 * 実践的なテンプレート活用のポイント:
 * 
 * 1. ECS (Entity Component System)
 *    - テンプレートで型安全なコンポーネント管理
 *    - 高速な配列ベースのデータ構造
 *    - システムの可変長テンプレート活用
 * 
 * 2. 型安全なリソース管理
 *    - テンプレートで型別のリソースID
 *    - 完全転送でパフォーマンス最適化
 *    - RAII原則に基づく自動リソース管理
 * 
 * 3. メモリプール
 *    - テンプレートで任意の型に対応
 *    - placement newでコンストラクタ呼び出し
 *    - メモリ断片化の回避
 * 
 * 4. イベントシステム
 *    - 型安全なイベントディスパッチ
 *    - 関数型プログラミングの要素
 *    - テンプレートによる柔軟性
 * 
 * これらの技法により、高性能で保守性の高い
 * ゲームエンジンコンポーネントを構築できます。
 */