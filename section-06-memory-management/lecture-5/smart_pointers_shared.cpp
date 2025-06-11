// shared_ptrの基礎と参照カウント
// ゲーム開発での共有リソース管理

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <map>

using namespace std;

// ゲームアセットクラス
class GameTexture
{
public:
    string name;
    size_t width, height;
    size_t memorySize;
    
    GameTexture(const string& name, size_t w, size_t h) 
        : name(name), width(w), height(h), memorySize(w * h * 4)
    {
        cout << "テクスチャ '" << name << "' をロード (" << width << "x" << height << ", " << memorySize << " bytes)\n";
    }
    
    ~GameTexture()
    {
        cout << "テクスチャ '" << name << "' をアンロード\n";
    }
    
    void use() const
    {
        cout << "テクスチャ '" << name << "' を使用中\n";
    }
};

// ゲームオブジェクトクラス
class GameObject
{
public:
    string name;
    shared_ptr<GameTexture> texture;
    float x, y;
    
    GameObject(const string& name, shared_ptr<GameTexture> tex, float x = 0, float y = 0)
        : name(name), texture(tex), x(x), y(y)
    {
        cout << "ゲームオブジェクト '" << name << "' を作成\n";
    }
    
    ~GameObject()
    {
        cout << "ゲームオブジェクト '" << name << "' を削除\n";
    }
    
    void render() const
    {
        cout << name << " を描画中 (位置: " << x << ", " << y << ")";
        if (texture)
        {
            cout << " - テクスチャ: " << texture->name << " (参照カウント: " << texture.use_count() << ")";
        }
        cout << "\n";
    }
};

// shared_ptrの基本操作
void basicSharedPtr()
{
    cout << "\n=== shared_ptrの基本操作 ===\n";
    
    // 基本的な作成方法
    shared_ptr<int> playerScore(new int(1500));
    cout << "プレイヤースコア: " << *playerScore << " (参照カウント: " << playerScore.use_count() << ")\n";
    
    // make_sharedを使用した推奨方法
    auto playerLevel = make_shared<int>(25);
    cout << "プレイヤーレベル: " << *playerLevel << " (参照カウント: " << playerLevel.use_count() << ")\n";
    
    // コピーで参照カウントが増加
    auto anotherRef = playerLevel;
    cout << "コピー後の参照カウント: " << playerLevel.use_count() << "\n";
    
    // スコープを抜けると参照カウントが減少
    {
        auto tempRef = playerLevel;
        cout << "スコープ内での参照カウント: " << playerLevel.use_count() << "\n";
    }
    cout << "スコープ外での参照カウント: " << playerLevel.use_count() << "\n";
}

// テクスチャ共有の例
void textureSharing()
{
    cout << "\n=== テクスチャ共有の例 ===\n";
    
    // 共有テクスチャを作成
    auto grassTexture = make_shared<GameTexture>("草テクスチャ", 256, 256);
    auto stoneTexture = make_shared<GameTexture>("石テクスチャ", 256, 256);
    
    cout << "\nテクスチャ作成後の参照カウント:\n";
    cout << "草テクスチャ: " << grassTexture.use_count() << "\n";
    cout << "石テクスチャ: " << stoneTexture.use_count() << "\n";
    
    // 複数のゲームオブジェクトで同じテクスチャを共有
    vector<shared_ptr<GameObject>> gameObjects;
    
    // 草テクスチャを使用するオブジェクトを作成
    gameObjects.push_back(make_shared<GameObject>("草ブロック1", grassTexture, 0, 0));
    gameObjects.push_back(make_shared<GameObject>("草ブロック2", grassTexture, 1, 0));
    gameObjects.push_back(make_shared<GameObject>("草ブロック3", grassTexture, 2, 0));
    
    // 石テクスチャを使用するオブジェクトを作成
    gameObjects.push_back(make_shared<GameObject>("石ブロック1", stoneTexture, 0, 1));
    gameObjects.push_back(make_shared<GameObject>("石ブロック2", stoneTexture, 1, 1));
    
    cout << "\nゲームオブジェクト作成後の参照カウント:\n";
    cout << "草テクスチャ: " << grassTexture.use_count() << "\n";
    cout << "石テクスチャ: " << stoneTexture.use_count() << "\n";
    
    cout << "\nゲームオブジェクトの描画:\n";
    for (const auto& obj : gameObjects)
    {
        obj->render();
    }
    
    // 一部のオブジェクトを削除
    cout << "\n草ブロック1を削除...\n";
    gameObjects.erase(gameObjects.begin());
    
    cout << "削除後の参照カウント:\n";
    cout << "草テクスチャ: " << grassTexture.use_count() << "\n";
    cout << "石テクスチャ: " << stoneTexture.use_count() << "\n";
}

// アセットマネージャーの実装例
class AssetManager
{
private:
    map<string, shared_ptr<GameTexture>> textures;
    
public:
    shared_ptr<GameTexture> loadTexture(const string& name, size_t width, size_t height)
    {
        // 既にロード済みの場合は既存のものを返す
        auto it = textures.find(name);
        if (it != textures.end())
        {
            cout << "キャッシュされたテクスチャ '" << name << "' を使用\n";
            return it->second;
        }
        
        // 新しくロード
        cout << "新しいテクスチャ '" << name << "' をロード\n";
        auto texture = make_shared<GameTexture>(name, width, height);
        textures[name] = texture;
        return texture;
    }
    
    void unloadUnusedTextures()
    {
        cout << "\n未使用テクスチャのアンロードをチェック中...\n";
        
        auto it = textures.begin();
        while (it != textures.end())
        {
            // 参照カウントが1の場合、AssetManagerのみが参照している
            if (it->second.use_count() == 1)
            {
                cout << "未使用テクスチャ '" << it->first << "' をアンロード\n";
                it = textures.erase(it);
            }
            else
            {
                cout << "テクスチャ '" << it->first << "' は使用中 (参照カウント: " << it->second.use_count() << ")\n";
                ++it;
            }
        }
    }
    
    void printStatus() const
    {
        cout << "\n=== AssetManagerの状態 ===\n";
        cout << "ロード済みテクスチャ数: " << textures.size() << "\n";
        for (const auto& pair : textures)
        {
            cout << "  " << pair.first << " (参照カウント: " << pair.second.use_count() << ")\n";
        }
    }
};

void assetManagerDemo()
{
    cout << "\n=== アセットマネージャーの例 ===\n";
    
    AssetManager assetManager;
    vector<shared_ptr<GameObject>> scene;
    
    // テクスチャをロードしてオブジェクトを作成
    auto playerTexture = assetManager.loadTexture("プレイヤー", 64, 64);
    auto enemyTexture = assetManager.loadTexture("エネミー", 64, 64);
    auto backgroundTexture = assetManager.loadTexture("背景", 1024, 768);
    
    // ゲームオブジェクトを作成
    scene.push_back(make_shared<GameObject>("プレイヤー", playerTexture, 100, 100));
    scene.push_back(make_shared<GameObject>("エネミー1", enemyTexture, 200, 150));
    scene.push_back(make_shared<GameObject>("エネミー2", enemyTexture, 250, 150));
    scene.push_back(make_shared<GameObject>("背景", backgroundTexture, 0, 0));
    
    assetManager.printStatus();
    
    // 同じテクスチャを再度要求（キャッシュされたものを使用）
    auto anotherEnemyTexture = assetManager.loadTexture("エネミー", 64, 64);
    scene.push_back(make_shared<GameObject>("エネミー3", anotherEnemyTexture, 300, 150));
    
    assetManager.printStatus();
    
    // 一部のオブジェクトを削除
    cout << "\nエネミーを倍倒...\n";
    scene.erase(scene.begin() + 1);  // エネミー1
    scene.erase(scene.begin() + 1);  // エネミー2
    
    assetManager.printStatus();
    
    // 未使用テクスチャのアンロードを試みる
    assetManager.unloadUnusedTextures();
    
    assetManager.printStatus();
}

// シーングラフの例
class SceneNode
{
public:
    string name;
    vector<shared_ptr<SceneNode>> children;
    shared_ptr<SceneNode> parent;
    
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
        children.push_back(child);
        child->parent = shared_ptr<SceneNode>(this, [](SceneNode*){});
        cout << "'" << name << "' に子ノード '" << child->name << "' を追加\n";
    }
    
    void printHierarchy(int depth = 0) const
    {
        for (int i = 0; i < depth; i++) cout << "  ";
        cout << name << " (参照カウント: " << children.size() << " 子ノード)\n";
        
        for (const auto& child : children)
        {
            child->printHierarchy(depth + 1);
        }
    }
};

void sceneGraphDemo()
{
    cout << "\n=== シーングラフの例 ===\n";
    
    // ルートノードを作成
    auto root = make_shared<SceneNode>("ルート");
    
    // ゲームオブジェクト用のグループを作成
    auto playerGroup = make_shared<SceneNode>("プレイヤーグループ");
    auto enemyGroup = make_shared<SceneNode>("エネミーグループ");
    auto uiGroup = make_shared<SceneNode>("UIグループ");
    
    // ルートにグループを追加
    root->addChild(playerGroup);
    root->addChild(enemyGroup);
    root->addChild(uiGroup);
    
    // 各グループに子ノードを追加
    playerGroup->addChild(make_shared<SceneNode>("プレイヤーキャラクター"));
    playerGroup->addChild(make_shared<SceneNode>("プレイヤーの影"));
    
    enemyGroup->addChild(make_shared<SceneNode>("エネミー1"));
    enemyGroup->addChild(make_shared<SceneNode>("エネミー2"));
    enemyGroup->addChild(make_shared<SceneNode>("エネミー3"));
    
    uiGroup->addChild(make_shared<SceneNode>("ヘルスバー"));
    uiGroup->addChild(make_shared<SceneNode>("ミニマップ"));
    
    cout << "\nシーングラフの階層構造:\n";
    root->printHierarchy();
    
    cout << "\nエネミーグループを削除...\n";
    root->children.erase(root->children.begin() + 1);  // enemyGroupを削除
    
    cout << "\n削除後のシーングラフ:\n";
    root->printHierarchy();
}

int main()
{
    cout << "=== shared_ptrの基礎と参照カウント ===\n";
    
    // 基本操作
    basicSharedPtr();
    
    // テクスチャ共有の例
    textureSharing();
    
    // アセットマネージャーの例
    assetManagerDemo();
    
    // シーングラフの例
    sceneGraphDemo();
    
    cout << "\n=== shared_ptrのメリット ===\n";
    cout << "1. 必要な間だけリソースが保持される\n";
    cout << "2. 複数のオーナーが同じリソースを安全に共有できる\n";
    cout << "3. 参照カウントによる自動的なメモリ管理\n";
    cout << "4. コピーコストの割りに参照を渡すことができる\n";
    
    cout << "\n=== 使用指針 ===\n";
    cout << "1. アセットの共有とキャッシュ管理\n";
    cout << "2. シーングラフや階層構造の構築\n";
    cout << "3. イベントシステムでのリスナー管理\n";
    cout << "4. サウンドやエフェクトのシステム間共有\n";
    cout << "5. マルチスレッド環境でのスレッドセーフなリソース管理\n";
    
    cout << "\n=== 注意点 ===\n";
    cout << "1. 循環参照に注意（weak_ptrを使用）\n";
    cout << "2. コピーのコストに注意（参照で渡し、ムーブで取得）\n";
    cout << "3. スレッド安全性はatomicな参照カウントのみ\n";
    cout << "4. メモリオーバーヘッドがある（コントロールブロックのサイズ）\n";
    
    return 0;
}

// shared_ptrをゲーム開発で活用する具体例：
// 1. テクスチャ、メッシュ、サウンドなどのアセット共有
// 2. シーングラフやゲームオブジェクトの階層管理
// 3. アニメーションシステムでのボーンやキーフレーム共有
// 4. パーティクルシステムでのエミッターとパーティクルの関係
// 5. ゲームステートの共有（マルチプレイヤーゲーム等）