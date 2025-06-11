// weak_ptrと循環参照の解決
// ゲーム開発での安全なオブジェクト参照管理

#include <iostream>
#include <memory>
#include <vector>
#include <string>

using namespace std;

// 前方宣言
class Player;
class Guild;
class GameWorld;

// プレイヤークラス
class Player
{
public:
    string name;
    int level;
    shared_ptr<Guild> guild;        // 所属ギルドへの強い参照
    weak_ptr<GameWorld> world;      // ゲームワールドへの弱い参照
    weak_ptr<Player> target;        // ターゲットプレイヤーへの弱い参照
    
    Player(const string& name, int level) : name(name), level(level)
    {
        cout << "プレイヤー '" << name << "' がログイン (レベル " << level << ")\n";
    }
    
    ~Player()
    {
        cout << "プレイヤー '" << name << "' がログアウト\n";
    }
    
    void joinGuild(shared_ptr<Guild> g);
    void leaveGuild();
    void setTarget(weak_ptr<Player> t);
    void attack();
    void displayStatus() const;
};

// ギルドクラス
class Guild
{
public:
    string name;
    vector<weak_ptr<Player>> members;  // メンバーへの弱い参照
    weak_ptr<Player> leader;           // リーダーへの弱い参照
    
    Guild(const string& name) : name(name)
    {
        cout << "ギルド '" << name << "' を設立\n";
    }
    
    ~Guild()
    {
        cout << "ギルド '" << name << "' を解散\n";
    }
    
    void addMember(weak_ptr<Player> player)
    {
        // expired()でオブジェクトがまだ存在するかチェック
        if (!player.expired())
        {
            members.push_back(player);
            auto p = player.lock();
            if (p)
            {
                cout << "プレイヤー '" << p->name << "' がギルド '" << name << "' に加入\n";
            }
        }
    }
    
    void removeMember(const string& playerName)
    {
        members.erase(
            remove_if(members.begin(), members.end(),
                [&playerName](const weak_ptr<Player>& wp) {
                    auto p = wp.lock();
                    return !p || p->name == playerName;
                }),
            members.end()
        );
    }
    
    void setLeader(weak_ptr<Player> player)
    {
        leader = player;
        auto p = player.lock();
        if (p)
        {
            cout << "'" << p->name << "' がギルド '" << name << "' のリーダーに就任\n";
        }
    }
    
    void cleanupExpiredMembers()
    {
        cout << "ギルド '" << name << "' の期限切れメンバーをクリーンアップ...\n";
        
        auto originalSize = members.size();
        members.erase(
            remove_if(members.begin(), members.end(),
                [](const weak_ptr<Player>& wp) {
                    return wp.expired();
                }),
            members.end()
        );
        
        cout << originalSize - members.size() << " 人の期限切れメンバーを削除\n";
    }
    
    void printMembers() const
    {
        cout << "\n=== ギルド '" << name << "' メンバー一覧 ===\n";
        cout << "総メンバー数: " << members.size() << "\n";
        
        int activeCount = 0;
        for (const auto& memberWeak : members)
        {
            if (auto member = memberWeak.lock())
            {
                cout << "  アクティブ: " << member->name << " (レベル " << member->level << ")";
                if (!leader.expired() && leader.lock() == member)
                {
                    cout << " [リーダー]";
                }
                cout << "\n";
                activeCount++;
            }
            else
            {
                cout << "  期限切れ: (オフライン)\n";
            }
        }
        
        cout << "アクティブメンバー: " << activeCount << " / " << members.size() << "\n";
    }
};

// ゲームワールドクラス
class GameWorld
{
public:
    string name;
    vector<shared_ptr<Player>> players;  // プレイヤーへの強い参照
    vector<shared_ptr<Guild>> guilds;    // ギルドへの強い参照
    
    GameWorld(const string& name) : name(name)
    {
        cout << "ゲームワールド '" << name << "' を作成\n";
    }
    
    ~GameWorld()
    {
        cout << "ゲームワールド '" << name << "' を終了\n";
    }
    
    shared_ptr<Player> createPlayer(const string& name, int level)
    {
        auto player = make_shared<Player>(name, level);
        player->world = weak_ptr<GameWorld>(shared_from_this());
        players.push_back(player);
        return player;
    }
    
    shared_ptr<Guild> createGuild(const string& name)
    {
        auto guild = make_shared<Guild>(name);
        guilds.push_back(guild);
        return guild;
    }
    
    void removePlayer(const string& name)
    {
        cout << "プレイヤー '" << name << "' をワールドから削除...\n";
        
        players.erase(
            remove_if(players.begin(), players.end(),
                [&name](const shared_ptr<Player>& p) {
                    return p->name == name;
                }),
            players.end()
        );
    }
    
    void printWorldStatus() const
    {
        cout << "\n=== ゲームワールド '" << name << "' の状態 ===\n";
        cout << "プレイヤー数: " << players.size() << "\n";
        cout << "ギルド数: " << guilds.size() << "\n";
    }
    
    // shared_from_thisを使用するためのenable_shared_from_thisを継承
    class GameWorld : public enable_shared_from_this<GameWorld> {};
};

// Playerクラスのメソッド実装
void Player::joinGuild(shared_ptr<Guild> g)
{
    if (guild)
    {
        leaveGuild();
    }
    
    guild = g;
    guild->addMember(weak_ptr<Player>(shared_from_this()));
}

void Player::leaveGuild()
{
    if (guild)
    {
        guild->removeMember(name);
        cout << "プレイヤー '" << name << "' がギルドを退会\n";
        guild.reset();
    }
}

void Player::setTarget(weak_ptr<Player> t)
{
    target = t;
    if (auto targetPtr = target.lock())
    {
        cout << "'" << name << "' が '" << targetPtr->name << "' をターゲットに設定\n";
    }
}

void Player::attack()
{
    if (auto targetPtr = target.lock())
    {
        cout << "'" << name << "' が '" << targetPtr->name << "' を攻撃！\n";
    }
    else
    {
        cout << "'" << name << "' のターゲットが見つからない（既にログアウト済み）\n";
    }
}

void Player::displayStatus() const
{
    cout << "\n=== プレイヤー '" << name << "' のステータス ===\n";
    cout << "レベル: " << level << "\n";
    cout << "ギルド: " << (guild ? guild->name : "無所属") << "\n";
    
    if (auto worldPtr = world.lock())
    {
        cout << "ワールド: " << worldPtr->name << "\n";
    }
    else
    {
        cout << "ワールド: 接続失敗\n";
    }
    
    if (auto targetPtr = target.lock())
    {
        cout << "ターゲット: " << targetPtr->name << "\n";
    }
    else
    {
        cout << "ターゲット: なし\n";
    }
}

// weak_ptrの基本操作
void basicWeakPtr()
{
    cout << "\n=== weak_ptrの基本操作 ===\n";
    
    weak_ptr<int> weakInt;
    
    {
        auto sharedInt = make_shared<int>(42);
        weakInt = sharedInt;
        
        cout << "shared_ptrの参照カウント: " << sharedInt.use_count() << "\n";
        cout << "weak_ptrが有効か: " << !weakInt.expired() << "\n";
        cout << "weak_ptrが管理するweakカウント: " << weakInt.use_count() << "\n";
        
        // lock()でshared_ptrを取得
        if (auto lockedPtr = weakInt.lock())
        {
            cout << "lock()で取得した値: " << *lockedPtr << "\n";
            cout << "lock後の参照カウント: " << lockedPtr.use_count() << "\n";
        }
    }
    
    cout << "\nshared_ptrがスコープを抜けた後:\n";
    cout << "weak_ptrが有効か: " << !weakInt.expired() << "\n";
    
    if (auto lockedPtr = weakInt.lock())
    {
        cout << "予期しない成功\n";
    }
    else
    {
        cout << "lock()が失敗（期待通り）\n";
    }
}

// 循環参照の問題と解決
void circularReferenceDemo()
{
    cout << "\n=== 循環参照の問題と解決 ===\n";
    
    // ゲームワールドを作成
    auto world = make_shared<GameWorld>("ファンタジーワールド");
    
    // プレイヤーを作成
    auto hero = world->createPlayer("勇者", 50);
    auto mage = world->createPlayer("魔法使い", 45);
    auto warrior = world->createPlayer("戦士", 48);
    
    // ギルドを作成
    auto adventureGuild = world->createGuild("冒険者ギルド");
    
    // プレイヤーをギルドに加入させる
    hero->joinGuild(adventureGuild);
    mage->joinGuild(adventureGuild);
    warrior->joinGuild(adventureGuild);
    
    // リーダーを設定
    adventureGuild->setLeader(weak_ptr<Player>(hero));
    
    // プレイヤー間のターゲット設定
    hero->setTarget(weak_ptr<Player>(mage));
    mage->setTarget(weak_ptr<Player>(warrior));
    
    // 現在の状態を表示
    world->printWorldStatus();
    adventureGuild->printMembers();
    
    cout << "\nプレイヤーの攻撃テスト:\n";
    hero->attack();
    mage->attack();
    
    // 一部のプレイヤーをログアウトさせる
    cout << "\n魔法使いがログアウト...\n";
    world->removePlayer("魔法使い");  // mageを削除
    
    cout << "\nログアウト後の攻撃テスト:\n";
    hero->attack();  // mageはもう存在しない
    
    // ギルドの期限切れメンバーをクリーンアップ
    adventureGuild->cleanupExpiredMembers();
    adventureGuild->printMembers();
}

// オブザーバーパターンの例
class EventSystem;

class GameEventListener
{
public:
    string name;
    weak_ptr<EventSystem> eventSystem;
    
    GameEventListener(const string& name) : name(name)
    {
        cout << "リスナー '" << name << "' を作成\n";
    }
    
    ~GameEventListener()
    {
        cout << "リスナー '" << name << "' を削除\n";
    }
    
    virtual void onEvent(const string& eventName)
    {
        cout << "リスナー '" << name << "' がイベント '" << eventName << "' を受信\n";
    }
    
    void unregister();
};

class EventSystem
{
public:
    vector<weak_ptr<GameEventListener>> listeners;
    
    void registerListener(weak_ptr<GameEventListener> listener)
    {
        listeners.push_back(listener);
        if (auto l = listener.lock())
        {
            cout << "リスナー '" << l->name << "' を登録\n";
        }
    }
    
    void unregisterListener(const string& name)
    {
        listeners.erase(
            remove_if(listeners.begin(), listeners.end(),
                [&name](const weak_ptr<GameEventListener>& wp) {
                    auto p = wp.lock();
                    return !p || p->name == name;
                }),
            listeners.end()
        );
    }
    
    void fireEvent(const string& eventName)
    {
        cout << "\nイベント '" << eventName << "' を発火...\n";
        
        // 期限切れリスナーをクリーンアップしながらイベントを送信
        auto it = listeners.begin();
        while (it != listeners.end())
        {
            if (auto listener = it->lock())
            {
                listener->onEvent(eventName);
                ++it;
            }
            else
            {
                it = listeners.erase(it);
            }
        }
    }
    
    void printListeners() const
    {
        cout << "\n登録されたリスナー数: " << listeners.size() << "\n";
        for (const auto& listenerWeak : listeners)
        {
            if (auto listener = listenerWeak.lock())
            {
                cout << "  アクティブ: " << listener->name << "\n";
            }
            else
            {
                cout << "  期限切れ: (無効)\n";
            }
        }
    }
};

void GameEventListener::unregister()
{
    if (auto es = eventSystem.lock())
    {
        es->unregisterListener(name);
        cout << "リスナー '" << name << "' が登録を解除\n";
    }
}

void observerPatternDemo()
{
    cout << "\n=== オブザーバーパターンのweak_ptr活用 ===\n";
    
    auto eventSystem = make_shared<EventSystem>();
    
    {
        // リスナーを作成
        auto uiListener = make_shared<GameEventListener>("UIシステム");
        auto audioListener = make_shared<GameEventListener>("オーディオシステム");
        auto gameplayListener = make_shared<GameEventListener>("ゲームプレイシステム");
        
        // リスナーを登録
        eventSystem->registerListener(weak_ptr<GameEventListener>(uiListener));
        eventSystem->registerListener(weak_ptr<GameEventListener>(audioListener));
        eventSystem->registerListener(weak_ptr<GameEventListener>(gameplayListener));
        
        eventSystem->printListeners();
        
        // イベントを発火
        eventSystem->fireEvent("プレイヤーレベルアップ");
        
        // 一部のリスナーを手動で解除
        audioListener->unregister();
        
        eventSystem->printListeners();
        eventSystem->fireEvent("アイテム取得");
        
        // audioListenerをスコープ外で削除
    }
    
    cout << "\nリスナーの一部がスコープを抜けた後:\n";
    eventSystem->printListeners();
    eventSystem->fireEvent("ゲームオーバー");  // 期限切れリスナーが自動でクリーンアップされる
    
    eventSystem->printListeners();
}

int main()
{
    cout << "=== weak_ptrと循環参照の解決 ===\n";
    
    // weak_ptrの基本操作
    basicWeakPtr();
    
    // 循環参照の問題と解決
    circularReferenceDemo();
    
    // オブザーバーパターンでの活用
    observerPatternDemo();
    
    cout << "\n=== weak_ptrのメリット ===\n";
    cout << "1. 循環参照を防いでメモリリークを防止\n";
    cout << "2. オブジェクトの生存に影響しない参照\n";
    cout << "3. オブザーバーパターンでの安全なリスナー管理\n";
    cout << "4. キャッシュや一時的な参照の実装\n";
    
    cout << "\n=== 使用指針 ===\n";
    cout << "1. 親子関係での子から親への参照\n";
    cout << "2. オブザーバーパターンのリスナー管理\n";
    cout << "3. キャッシュシステムでの一時的な参照\n";
    cout << "4. ゲームエンティティ間の結合防止\n";
    cout << "5. UIシステムでのイベントハンドラー管理\n";
    
    cout << "\n=== 注意点 ===\n";
    cout << "1. lock()の結果は必ずチェックする\n";
    cout << "2. expired()とlock()の結果に時間差がある可能性\n";
    cout << "3. マルチスレッド環境では治合的なロックが必要\n";
    cout << "4. パフォーマンスのオーバーヘッドはlook()呼び出しで発生\n";
    
    return 0;
}

// weak_ptrをゲーム開発で活用する具体例：
// 1. シーングラフでの親ノードへの参照
// 2. AIシステムでのターゲット管理
// 3. アニメーションシステムでのオブジェクト参照
// 4. ネットワークシステムでのプレイヤー管理
// 5. サウンドシステムでのコールバック管理