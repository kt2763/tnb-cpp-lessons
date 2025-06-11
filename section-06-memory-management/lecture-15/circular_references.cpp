// 循環参照の問題と解決策
// ゲーム開発でのオブジェクト関係管理

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <weak_ptr>
#include <map>
#include <chrono>

using namespace std;
using namespace chrono;

// 循環参照の問題を示すクラス（shared_ptrのみ使用）
class BadGameCharacter
{
public:
    string name;
    int health;
    shared_ptr<BadGameCharacter> target;       // 攻撃対象
    shared_ptr<BadGameCharacter> leader;       // チームリーダー
    vector<shared_ptr<BadGameCharacter>> team; // チームメンバー
    
    BadGameCharacter(const string& name, int health = 100)
        : name(name), health(health)
    {
        cout << "問題のあるキャラクター '" << name << "' を作成\n";
    }
    
    ~BadGameCharacter()
    {
        cout << "問題のあるキャラクター '" << name << "' を削除\n";
    }
    
    void setTarget(shared_ptr<BadGameCharacter> t)
    {
        target = t;
        cout << "'" << name << "' のターゲットを '" << t->name << "' に設定\n";
    }
    
    void joinTeam(shared_ptr<BadGameCharacter> l)
    {
        leader = l;
        l->team.push_back(shared_from_this());
        cout << "'" << name << "' が '" << l->name << "' のチームに参加\n";
    }
    
    void printInfo() const
    {
        cout << "  " << name << " (HP: " << health << ")";
        if (target)
        {
            cout << " -> ターゲット: " << target->name;
        }
        if (leader)
        {
            cout << " -> リーダー: " << leader->name;
        }
        cout << " [チーム人数: " << team.size() << "]\n";
    }
};

// 循環参照を解決したクラス（weak_ptrを活用）
class GoodGameCharacter : public enable_shared_from_this<GoodGameCharacter>
{
public:
    string name;
    int health;
    weak_ptr<GoodGameCharacter> target;       // 攻撃対象（weak_ptr）
    weak_ptr<GoodGameCharacter> leader;       // チームリーダー（weak_ptr）
    vector<shared_ptr<GoodGameCharacter>> team; // チームメンバー（所有）
    
    GoodGameCharacter(const string& name, int health = 100)
        : name(name), health(health)
    {
        cout << "良いキャラクター '" << name << "' を作成\n";
    }
    
    ~GoodGameCharacter()
    {
        cout << "良いキャラクター '" << name << "' を削除\n";
    }
    
    void setTarget(shared_ptr<GoodGameCharacter> t)
    {
        target = t;  // weak_ptrに自動変換
        cout << "'" << name << "' のターゲットを '" << t->name << "' に設定\n";
    }
    
    void joinTeam(shared_ptr<GoodGameCharacter> l)
    {
        leader = l;  // weak_ptrに自動変換
        l->team.push_back(shared_from_this());
        cout << "'" << name << "' が '" << l->name << "' のチームに参加\n";
    }
    
    void attack()
    {
        auto targetPtr = target.lock();  // weak_ptrからshared_ptrを取得
        if (targetPtr && targetPtr->health > 0)
        {
            int damage = 20;
            targetPtr->health -= damage;
            cout << "'" << name << "' が '" << targetPtr->name << "' に " << damage << " ダメージ！\n";
            
            if (targetPtr->health <= 0)
            {
                cout << "'" << targetPtr->name << "' は倒された！\n";
                target.reset();  // ターゲットをクリア
            }
        }
        else
        {
            cout << "'" << name << "' のターゲットは無効です\n";
        }
    }
    
    void printInfo() const
    {
        cout << "  " << name << " (HP: " << health << ")";
        
        auto targetPtr = target.lock();
        if (targetPtr)
        {
            cout << " -> ターゲット: " << targetPtr->name;
        }
        
        auto leaderPtr = leader.lock();
        if (leaderPtr)
        {
            cout << " -> リーダー: " << leaderPtr->name;
        }
        
        cout << " [チーム人数: " << team.size() << "]\n";
    }
    
    bool isTargetValid() const
    {
        return !target.expired();
    }
    
    bool isLeaderValid() const
    {
        return !leader.expired();
    }
};

// ゲームシーンノード（親子関係を持つ）
class SceneNode : public enable_shared_from_this<SceneNode>
{
public:
    string name;
    weak_ptr<SceneNode> parent;                    // 親ノード（weak_ptr）
    vector<shared_ptr<SceneNode>> children;        // 子ノード（所有）
    map<string, string> properties;               // ノードプロパティ
    
    SceneNode(const string& name) : name(name)
    {
        cout << "シーンノード '" << name << "' を作成\n";
    }
    
    ~SceneNode()
    {
        cout << "シーンノード '" << name << "' を削除\n";
    }
    
    void addChild(shared_ptr<SceneNode> child)
    {
        child->parent = shared_from_this();
        children.push_back(child);
        cout << "'" << name << "' に子ノード '" << child->name << "' を追加\n";
    }
    
    void removeChild(const string& childName)
    {
        auto it = find_if(children.begin(), children.end(),
            [&childName](const shared_ptr<SceneNode>& child) {
                return child->name == childName;
            });
        
        if (it != children.end())
        {
            (*it)->parent.reset();
            children.erase(it);
            cout << "'" << name << "' から子ノード '" << childName << "' を削除\n";
        }
    }
    
    shared_ptr<SceneNode> findChild(const string& childName)
    {
        auto it = find_if(children.begin(), children.end(),
            [&childName](const shared_ptr<SceneNode>& child) {
                return child->name == childName;
            });
        
        return (it != children.end()) ? *it : nullptr;
    }
    
    void printHierarchy(int depth = 0) const
    {
        string indent(depth * 2, ' ');
        cout << indent << "- " << name << " (子: " << children.size() << "個)\n";
        
        for (const auto& child : children)
        {
            child->printHierarchy(depth + 1);
        }
    }
    
    string getPath() const
    {
        auto parentPtr = parent.lock();
        if (parentPtr)
        {
            return parentPtr->getPath() + "/" + name;
        }
        return "/" + name;
    }
    
    void setProperty(const string& key, const string& value)
    {
        properties[key] = value;
    }
    
    string getProperty(const string& key) const
    {
        auto it = properties.find(key);
        return (it != properties.end()) ? it->second : "";
    }
};

// 観察者パターンの実装（循環参照回避）
class GameEvent
{
public:
    string eventType;
    string data;
    
    GameEvent(const string& type, const string& data) : eventType(type), data(data) {}
};

class EventObserver
{
public:
    virtual ~EventObserver() = default;
    virtual void onEvent(const GameEvent& event) = 0;
    virtual string getObserverName() const = 0;
};

class EventPublisher
{
private:
    vector<weak_ptr<EventObserver>> observers;
    
public:
    void subscribe(shared_ptr<EventObserver> observer)
    {
        observers.push_back(observer);
        cout << "観察者 '" << observer->getObserverName() << "' を登録\n";
    }
    
    void unsubscribe(shared_ptr<EventObserver> observer)
    {
        observers.erase(
            remove_if(observers.begin(), observers.end(),
                [&observer](const weak_ptr<EventObserver>& weak) {
                    auto locked = weak.lock();
                    return !locked || locked == observer;
                }),
            observers.end()
        );
        cout << "観察者 '" << observer->getObserverName() << "' を削除\n";
    }
    
    void notifyObservers(const GameEvent& event)
    {
        cout << "イベント '" << event.eventType << "' を通知: " << event.data << "\n";
        
        // 期限切れのweak_ptrを削除しながら通知
        auto it = observers.begin();
        while (it != observers.end())
        {
            auto observer = it->lock();
            if (observer)
            {
                observer->onEvent(event);
                ++it;
            }
            else
            {
                cout << "  期限切れの観察者を削除\n";
                it = observers.erase(it);
            }
        }
    }
    
    size_t getObserverCount() const
    {
        return count_if(observers.begin(), observers.end(),
            [](const weak_ptr<EventObserver>& weak) {
                return !weak.expired();
            });
    }
};

class GameAI : public EventObserver, public enable_shared_from_this<GameAI>
{
private:
    string name;
    shared_ptr<EventPublisher> publisher;
    
public:
    GameAI(const string& name, shared_ptr<EventPublisher> pub)
        : name(name), publisher(pub)
    {
        cout << "AI '" << name << "' を作成\n";
    }
    
    ~GameAI()
    {
        cout << "AI '" << name << "' を削除\n";
    }
    
    void initialize()
    {
        publisher->subscribe(shared_from_this());
    }
    
    void onEvent(const GameEvent& event) override
    {
        cout << "  AI '" << name << "' がイベント '" << event.eventType << "' を受信\n";
        
        if (event.eventType == "enemy_spotted")
        {
            cout << "    -> AI '" << name << "' が敵発見に対応\n";
        }
        else if (event.eventType == "player_died")
        {
            cout << "    -> AI '" << name << "' がプレイヤー死亡に対応\n";
        }
    }
    
    string getObserverName() const override
    {
        return name;
    }
};

// 循環参照の問題をデモ
void circularReferenceDemo()
{
    cout << "\n=== 循環参照の問題デモ ===\n";
    
    cout << "\n1. 悪い例（shared_ptrのみ使用）:\n";
    {
        // この実装は実際には循環参照を作成しないが、
        // より複雑なシナリオでは問題となる
        cout << "注意: 実際の循環参照は複雑なオブジェクト関係で発生します\n";
        cout << "例: A -> B -> C -> A のような関係\n";
    }
    
    cout << "\n2. 良い例（weak_ptrを使用）:\n";
    {
        auto hero = make_shared<GoodGameCharacter>("勇者", 100);
        auto enemy1 = make_shared<GoodGameCharacter>("オーク", 80);
        auto enemy2 = make_shared<GoodGameCharacter>("ゴブリン", 60);
        
        // 相互にターゲット設定
        hero->setTarget(enemy1);
        enemy1->setTarget(hero);
        enemy2->setTarget(hero);
        
        cout << "\n戦闘開始:\n";
        hero->printInfo();
        enemy1->printInfo();
        enemy2->printInfo();
        
        // 戦闘シミュレーション
        hero->attack();  // 勇者がオークを攻撃
        enemy1->attack(); // オークが勇者を攻撃
        enemy2->attack(); // ゴブリンが勇者を攻撃
        
        cout << "\n戦闘後:\n";
        hero->printInfo();
        enemy1->printInfo();
        enemy2->printInfo();
    }
    
    cout << "\nスコープを抜ける際に適切に削除されます\n";
}

// シーンノードの階層管理デモ
void sceneHierarchyDemo()
{
    cout << "\n=== シーンノード階層管理デモ ===\n";
    
    {
        // ルートノードを作成
        auto root = make_shared<SceneNode>("Root");
        root->setProperty("type", "scene_root");
        
        // 中間ノードを作成
        auto world = make_shared<SceneNode>("World");
        auto ui = make_shared<SceneNode>("UI");
        
        root->addChild(world);
        root->addChild(ui);
        
        // ワールドの子ノード
        auto player = make_shared<SceneNode>("Player");
        auto enemies = make_shared<SceneNode>("Enemies");
        auto environment = make_shared<SceneNode>("Environment");
        
        world->addChild(player);
        world->addChild(enemies);
        world->addChild(environment);
        
        // 敵の個別ノード
        auto orc = make_shared<SceneNode>("Orc1");
        auto goblin = make_shared<SceneNode>("Goblin1");
        
        enemies->addChild(orc);
        enemies->addChild(goblin);
        
        // UIの子ノード
        auto hud = make_shared<SceneNode>("HUD");
        auto menu = make_shared<SceneNode>("Menu");
        
        ui->addChild(hud);
        ui->addChild(menu);
        
        cout << "\nシーン階層:\n";
        root->printHierarchy();
        
        cout << "\nノードパス例:\n";
        cout << "Player: " << player->getPath() << "\n";
        cout << "Orc1: " << orc->getPath() << "\n";
        cout << "HUD: " << hud->getPath() << "\n";
        
        // 子ノードの検索と削除
        cout << "\nGoblin1を削除...\n";
        enemies->removeChild("Goblin1");
        
        cout << "\n削除後の階層:\n";
        enemies->printHierarchy();
    }
    
    cout << "\nシーンノードが適切に削除されました\n";
}

// 観察者パターンでの循環参照回避デモ
void observerPatternDemo()
{
    cout << "\n=== 観察者パターンでの循環参照回避デモ ===\n";
    
    {
        auto publisher = make_shared<EventPublisher>();
        
        // AI エージェントを作成
        auto ai1 = make_shared<GameAI>("戦略AI", publisher);
        auto ai2 = make_shared<GameAI>("戦術AI", publisher);
        auto ai3 = make_shared<GameAI>("支援AI", publisher);
        
        // 観察者として登録
        ai1->initialize();
        ai2->initialize();
        ai3->initialize();
        
        cout << "\n観察者数: " << publisher->getObserverCount() << "\n";
        
        // イベントを発行
        publisher->notifyObservers(GameEvent("enemy_spotted", "座標(100, 200)で敵発見"));
        publisher->notifyObservers(GameEvent("player_died", "プレイヤーがダメージで倒れた"));
        
        // 一部のAIを削除
        cout << "\n戦略AIを削除...\n";
        ai1.reset();
        
        cout << "\n削除後の観察者数: " << publisher->getObserverCount() << "\n";
        
        // 再度イベントを発行（期限切れの観察者は自動削除される）
        publisher->notifyObservers(GameEvent("level_complete", "レベルクリア"));
        
        cout << "\n最終観察者数: " << publisher->getObserverCount() << "\n";
    }
    
    cout << "\n全ての観察者とパブリッシャーが削除されました\n";
}

// weak_ptrの詳細使用例
void weakPtrDetailsDemo()
{
    cout << "\n=== weak_ptrの詳細使用例 ===\n";
    
    {
        auto team = make_shared<GoodGameCharacter>("チームリーダー", 120);
        weak_ptr<GoodGameCharacter> teamWeakRef = team;
        
        cout << "\n1. weak_ptrの基本操作:\n";
        cout << "expired(): " << (teamWeakRef.expired() ? "期限切れ" : "有効") << "\n";
        cout << "use_count(): " << teamWeakRef.use_count() << "\n";
        
        {
            auto member1 = make_shared<GoodGameCharacter>("メンバー1", 100);
            auto member2 = make_shared<GoodGameCharacter>("メンバー2", 100);
            
            member1->joinTeam(team);
            member2->joinTeam(team);
            
            cout << "\nチームメンバー追加後:\n";
            cout << "use_count(): " << teamWeakRef.use_count() << "\n";
            team->printInfo();
            
            // lock()でshared_ptrを取得
            if (auto lockedTeam = teamWeakRef.lock())
            {
                cout << "\nlock()成功: " << lockedTeam->name << "\n";
            }
        }
        
        cout << "\nメンバーのスコープ終了後:\n";
        cout << "use_count(): " << teamWeakRef.use_count() << "\n";
        
        // チームリーダーを削除
        team.reset();
        
        cout << "\nチームリーダー削除後:\n";
        cout << "expired(): " << (teamWeakRef.expired() ? "期限切れ" : "有効") << "\n";
        
        if (auto lockedTeam = teamWeakRef.lock())
        {
            cout << "lock()成功: " << lockedTeam->name << "\n";
        }
        else
        {
            cout << "lock()失敗: オブジェクトは既に削除されています\n";
        }
    }
}

// パフォーマンス比較
void performanceComparison()
{
    cout << "\n=== パフォーマンス比較 ===\n";
    
    const int iterations = 100000;
    
    // shared_ptrの作成と削除
    {
        auto start = high_resolution_clock::now();
        
        for (int i = 0; i < iterations; ++i)
        {
            auto obj = make_shared<GoodGameCharacter>("テスト", 100);
            weak_ptr<GoodGameCharacter> weak = obj;
            (void)weak; // 未使用警告を防ぐ
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        
        cout << "shared_ptr + weak_ptr (" << iterations << "回): " << duration << " マイクロ秒\n";
    }
    
    // 生ポインタの作成と削除
    {
        auto start = high_resolution_clock::now();
        
        for (int i = 0; i < iterations; ++i)
        {
            auto* obj = new GoodGameCharacter("テスト", 100);
            delete obj;
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        
        cout << "生ポインタ (" << iterations << "回): " << duration << " マイクロ秒\n";
    }
}

int main()
{
    cout << "=== 循環参照の問題と解決策 ===\n";
    
    // 循環参照の問題デモ
    circularReferenceDemo();
    
    // シーンノード階層管理デモ
    sceneHierarchyDemo();
    
    // 観察者パターンでの循環参照回避デモ
    observerPatternDemo();
    
    // weak_ptrの詳細使用例
    weakPtrDetailsDemo();
    
    // パフォーマンス比較
    performanceComparison();
    
    cout << "\n=== 循環参照の問題 ===\n";
    cout << "1. shared_ptrだけを使用すると相互参照でメモリリークが発生\n";
    cout << "2. オブジェクトの参照カウントが0にならず、自動削除されない\n";
    cout << "3. 特に親子関係や観察者パターンで発生しやすい\n";
    cout << "4. 複雑なオブジェクト関係では発見が困難\n";
    
    cout << "\n=== weak_ptrによる解決策 ===\n";
    cout << "1. 所有権を持たない弱い参照でオブジェクトを指す\n";
    cout << "2. 参照カウントに影響しないため循環参照を防ぐ\n";
    cout << "3. lock()でshared_ptrに変換して安全にアクセス\n";
    cout << "4. expired()でオブジェクトの生存状態を確認\n";
    
    cout << "\n=== ゲーム開発での活用例 ===\n";
    cout << "1. シーングラフの親子関係: 親→子はshared_ptr、子→親はweak_ptr\n";
    cout << "2. ゲームオブジェクトの関係: 所有関係はshared_ptr、参照関係はweak_ptr\n";
    cout << "3. 観察者パターン: パブリッシャー→観察者はweak_ptr\n";
    cout << "4. AIシステム: エージェント間の相互参照をweak_ptrで管理\n";
    cout << "5. UIシステム: ウィジェット間の関係でweak_ptrを活用\n";
    
    cout << "\n=== 設計指針 ===\n";
    cout << "1. 所有権を明確に定義する（誰がオブジェクトの生存期間を管理するか）\n";
    cout << "2. 所有関係はshared_ptr、参照関係はweak_ptrを使用\n";
    cout << "3. weak_ptrの使用時は必ずlock()とnullチェックを行う\n";
    cout << "4. 複雑な関係ではオブジェクト設計を見直すことも検討\n";
    cout << "5. デバッグ時にメモリリークを定期的にチェック\n";
    
    return 0;
}

// 循環参照回避の重要なポイント：
// 1. 所有権（ownership）と参照（reference）を明確に区別
// 2. 親から子への関係は所有（shared_ptr）、子から親は参照（weak_ptr）
// 3. weak_ptrは常にlock()を使ってアクセスし、nullチェックを行う
// 4. 観察者パターンではパブリッシャーからの参照をweak_ptrにする
// 5. ゲームオブジェクト間の複雑な関係は設計時に慎重に検討する