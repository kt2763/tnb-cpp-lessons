// タグディスパッチ
// 型を使った関数の選択により、効率的で型安全なコードを実現

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <iterator>
#include <algorithm>
#include <chrono>

// 基本的なタグ型
struct fast_tag {};
struct slow_tag {};
struct safe_tag {};

// ゲーム専用のタグ型
struct player_tag {};
struct enemy_tag {};
struct npc_tag {};
struct item_tag {};

// 難易度タグ
struct easy_tag {};
struct normal_tag {};
struct hard_tag {};
struct nightmare_tag {};

// アルゴリズムタグ
struct linear_search_tag {};
struct binary_search_tag {};
struct hash_search_tag {};

// タグディスパッチを使った距離計算
template<typename Iterator>
auto distance_impl(Iterator first, Iterator last, std::random_access_iterator_tag)
{
    std::cout << "ランダムアクセスイテレータ: O(1)計算" << std::endl;
    return last - first;
}

template<typename Iterator>
auto distance_impl(Iterator first, Iterator last, std::input_iterator_tag)
{
    std::cout << "入力イテレータ: O(n)計算" << std::endl;
    auto count = 0;
    while (first != last)
    {
        ++first;
        ++count;
    }
    return count;
}

template<typename Iterator>
auto my_distance(Iterator first, Iterator last)
{
    return distance_impl(first, last, 
                        typename std::iterator_traits<Iterator>::iterator_category{});
}

// ゲームオブジェクトのタグディスパッチ
class GameObject
{
protected:
    std::string name;
    int health;
    
public:
    GameObject(const std::string& n, int h) : name(n), health(h) {}
    virtual ~GameObject() = default;
    
    const std::string& getName() const { return name; }
    int getHealth() const { return health; }
    void setHealth(int h) { health = h; }
};

class Player : public GameObject
{
public:
    Player(const std::string& name) : GameObject(name, 100) {}
    
    // プレイヤー固有のメソッド
    void levelUp() { std::cout << name << " がレベルアップ！" << std::endl; }
};

class Enemy : public GameObject
{
public:
    Enemy(const std::string& name, int hp) : GameObject(name, hp) {}
    
    // 敵固有のメソッド
    void patrol() { std::cout << name << " が巡回中..." << std::endl; }
};

class NPC : public GameObject
{
public:
    NPC(const std::string& name) : GameObject(name, 50) {}
    
    // NPC固有のメソッド
    void talk() { std::cout << name << " が話しかけてきた" << std::endl; }
};

// タグディスパッチによる処理分岐
void updateEntity(GameObject& obj, player_tag)
{
    std::cout << "プレイヤー更新: ";
    // プレイヤー専用の最適化された処理
    if (Player* player = dynamic_cast<Player*>(&obj))
    {
        if (obj.getHealth() <= 20)
        {
            std::cout << "体力が危険な状態！" << std::endl;
        }
        // プレイヤーの入力処理など
    }
}

void updateEntity(GameObject& obj, enemy_tag)
{
    std::cout << "敵更新: ";
    // 敵専用の処理
    if (Enemy* enemy = dynamic_cast<Enemy*>(&obj))
    {
        enemy->patrol();
        // AI思考処理など
    }
}

void updateEntity(GameObject& obj, npc_tag)
{
    std::cout << "NPC更新: ";
    // NPC専用の処理
    if (NPC* npc = dynamic_cast<NPC*>(&obj))
    {
        // NPCの行動パターン
        npc->talk();
    }
}

// タグディスパッチラッパー
template<typename Tag>
void updateGameObject(GameObject& obj, Tag tag)
{
    updateEntity(obj, tag);
}

// 難易度別の敵生成
struct EnemyStats
{
    int health;
    int attack;
    int defense;
    float speed;
};

EnemyStats createEnemy(const std::string& type, easy_tag)
{
    std::cout << "イージーモードの" << type << "を生成" << std::endl;
    return {50, 10, 5, 1.0f};
}

EnemyStats createEnemy(const std::string& type, normal_tag)
{
    std::cout << "ノーマルモードの" << type << "を生成" << std::endl;
    return {100, 20, 10, 1.5f};
}

EnemyStats createEnemy(const std::string& type, hard_tag)
{
    std::cout << "ハードモードの" << type << "を生成" << std::endl;
    return {200, 40, 20, 2.0f};
}

EnemyStats createEnemy(const std::string& type, nightmare_tag)
{
    std::cout << "ナイトメアモードの" << type << "を生成" << std::endl;
    return {500, 100, 50, 3.0f};
}

// 検索アルゴリズムのタグディスパッチ
template<typename Container, typename Value>
bool search_impl(const Container& container, const Value& value, linear_search_tag)
{
    std::cout << "線形検索を実行..." << std::endl;
    return std::find(container.begin(), container.end(), value) != container.end();
}

template<typename Container, typename Value>
bool search_impl(const Container& container, const Value& value, binary_search_tag)
{
    std::cout << "二分検索を実行..." << std::endl;
    return std::binary_search(container.begin(), container.end(), value);
}

template<typename Container, typename Value, typename Tag>
bool search(const Container& container, const Value& value, Tag tag)
{
    return search_impl(container, value, tag);
}

// パフォーマンス最適化のタグディスパッチ
template<typename T>
void processData(const std::vector<T>& data, fast_tag)
{
    std::cout << "高速処理モード（精度を犠牲）" << std::endl;
    // 最適化された高速処理
    for (const auto& item : data)
    {
        // 簡略化された処理
    }
}

template<typename T>
void processData(const std::vector<T>& data, safe_tag)
{
    std::cout << "安全処理モード（精度重視）" << std::endl;
    // 安全で正確な処理
    for (size_t i = 0; i < data.size(); ++i)
    {
        // 境界チェック付きの処理
        if (i < data.size())
        {
            // 処理
        }
    }
}

// 型特性とタグディスパッチの組み合わせ
template<typename T>
struct processing_tag
{
    using type = std::conditional_t<
        std::is_trivially_copyable_v<T>,
        fast_tag,
        safe_tag
    >;
};

template<typename T>
using processing_tag_t = typename processing_tag<T>::type;

// ゲームのリソース管理
class ResourceManager
{
public:
    template<typename T>
    void loadResource(const std::string& filename)
    {
        loadResource_impl(filename, processing_tag_t<T>{});
    }
    
private:
    void loadResource_impl(const std::string& filename, fast_tag)
    {
        std::cout << "高速ロード: " << filename << std::endl;
        // メモリマップファイルなど
    }
    
    void loadResource_impl(const std::string& filename, safe_tag)
    {
        std::cout << "安全ロード: " << filename << std::endl;
        // エラーチェック付きの読み込み
    }
};

// 戦闘システムでのタグディスパッチ
struct physical_attack_tag {};
struct magical_attack_tag {};
struct ranged_attack_tag {};

class CombatSystem
{
public:
    template<typename AttackTag>
    int calculateDamage(int baseDamage, int attackPower, int defense, AttackTag tag)
    {
        return calculateDamage_impl(baseDamage, attackPower, defense, tag);
    }
    
private:
    int calculateDamage_impl(int baseDamage, int attackPower, int defense, physical_attack_tag)
    {
        std::cout << "物理攻撃ダメージ計算" << std::endl;
        return std::max(1, baseDamage + attackPower - defense);
    }
    
    int calculateDamage_impl(int baseDamage, int attackPower, int defense, magical_attack_tag)
    {
        std::cout << "魔法攻撃ダメージ計算" << std::endl;
        // 魔法防御は物理防御の半分の効果
        return std::max(1, baseDamage + attackPower - defense / 2);
    }
    
    int calculateDamage_impl(int baseDamage, int attackPower, int defense, ranged_attack_tag)
    {
        std::cout << "遠距離攻撃ダメージ計算" << std::endl;
        // 遠距離攻撃は防御を無視する部分がある
        return std::max(1, (baseDamage + attackPower) * 3 / 4 - defense / 3);
    }
};

// タグベースのファクトリパターン
template<typename Tag>
class EntityFactory;

template<>
class EntityFactory<player_tag>
{
public:
    static std::unique_ptr<GameObject> create(const std::string& name)
    {
        return std::make_unique<Player>(name);
    }
};

template<>
class EntityFactory<enemy_tag>
{
public:
    static std::unique_ptr<GameObject> create(const std::string& name)
    {
        return std::make_unique<Enemy>(name, 80);
    }
};

template<>
class EntityFactory<npc_tag>
{
public:
    static std::unique_ptr<GameObject> create(const std::string& name)
    {
        return std::make_unique<NPC>(name);
    }
};

int main()
{
    std::cout << "=== タグディスパッチ ===" << std::endl;
    
    // 1. イテレータカテゴリによる最適化
    std::cout << "\n--- イテレータベースの最適化 ---" << std::endl;
    std::vector<int> vec{1, 2, 3, 4, 5};
    std::list<int> lst{1, 2, 3, 4, 5};
    
    std::cout << "vectorの距離: " << my_distance(vec.begin(), vec.end()) << std::endl;
    std::cout << "listの距離: " << my_distance(lst.begin(), lst.end()) << std::endl;
    
    // 2. ゲームオブジェクトの処理
    std::cout << "\n--- ゲームオブジェクト更新 ---" << std::endl;
    Player player("勇者");
    Enemy enemy("ゴブリン", 60);
    NPC npc("村人");
    
    updateGameObject(player, player_tag{});
    updateGameObject(enemy, enemy_tag{});
    updateGameObject(npc, npc_tag{});
    
    // 3. 難易度別敵生成
    std::cout << "\n--- 難易度別敵生成 ---" << std::endl;
    auto easySlime = createEnemy("スライム", easy_tag{});
    auto hardDragon = createEnemy("ドラゴン", hard_tag{});
    auto nightmareDevil = createEnemy("魔王", nightmare_tag{});
    
    std::cout << "ハードドラゴン HP:" << hardDragon.health 
              << " ATK:" << hardDragon.attack << std::endl;
    
    // 4. 検索アルゴリズム選択
    std::cout << "\n--- 検索アルゴリズム ---" << std::endl;
    std::vector<int> numbers{1, 3, 5, 7, 9, 11, 13, 15};
    
    bool found1 = search(numbers, 7, linear_search_tag{});
    bool found2 = search(numbers, 7, binary_search_tag{});
    
    std::cout << "線形検索結果: " << found1 << std::endl;
    std::cout << "二分検索結果: " << found2 << std::endl;
    
    // 5. パフォーマンス最適化
    std::cout << "\n--- パフォーマンス最適化 ---" << std::endl;
    std::vector<int> intData{1, 2, 3, 4, 5};
    std::vector<std::string> stringData{"A", "B", "C"};
    
    processData(intData, fast_tag{});      // 手動指定
    processData(stringData, safe_tag{});   // 手動指定
    
    // 6. 型特性による自動選択
    std::cout << "\n--- 自動タグ選択 ---" << std::endl;
    ResourceManager rm;
    rm.loadResource<int>("numbers.dat");     // trivially_copyable -> fast_tag
    rm.loadResource<std::string>("text.dat"); // non-trivial -> safe_tag
    
    // 7. 戦闘システム
    std::cout << "\n--- 戦闘システム ---" << std::endl;
    CombatSystem combat;
    
    int physicalDmg = combat.calculateDamage(50, 30, 20, physical_attack_tag{});
    int magicalDmg = combat.calculateDamage(50, 30, 20, magical_attack_tag{});
    int rangedDmg = combat.calculateDamage(50, 30, 20, ranged_attack_tag{});
    
    std::cout << "物理ダメージ: " << physicalDmg << std::endl;
    std::cout << "魔法ダメージ: " << magicalDmg << std::endl;
    std::cout << "遠距離ダメージ: " << rangedDmg << std::endl;
    
    // 8. ファクトリパターン
    std::cout << "\n--- エンティティファクトリ ---" << std::endl;
    auto newPlayer = EntityFactory<player_tag>::create("新人勇者");
    auto newEnemy = EntityFactory<enemy_tag>::create("オーク");
    auto newNPC = EntityFactory<npc_tag>::create("商人");
    
    std::cout << "作成されたプレイヤー: " << newPlayer->getName() << std::endl;
    std::cout << "作成された敵: " << newEnemy->getName() << std::endl;
    std::cout << "作成されたNPC: " << newNPC->getName() << std::endl;
    
    return 0;
}

// まとめ：
// - タグ型で関数オーバーロードを制御
// - コンパイル時に最適なアルゴリズムを選択
// - 型安全で効率的なディスパッチ機構
// - STLのイテレータカテゴリも同様の仕組み
// - ゲーム開発での実用的な応用例が豊富