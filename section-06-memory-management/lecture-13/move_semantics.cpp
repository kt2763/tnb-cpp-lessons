// ムーブセマンティクスの最適化
// ゲーム開発での高パフォーマンスオブジェクト移動

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <utility>
#include <algorithm>

using namespace std;
using namespace chrono;

// ムーブセマンティクスを持たないクラス（非効率）
class IneffientGameAsset
{
public:
    string filename;
    char* data;
    size_t size;
    
    IneffientGameAsset(const string& filename, size_t size) 
        : filename(filename), size(size)
    {
        data = new char[size];
        // ダミーデータで初期化
        for (size_t i = 0; i < size; ++i)
        {
            data[i] = static_cast<char>(rand() % 256);
        }
        cout << "非効率アセット '" << filename << "' をロード (" << size << " bytes)\n";
    }
    
    // コピーコンストラクタ（遅い深いコピー）
    IneffientGameAsset(const IneffientGameAsset& other)
        : filename(other.filename + "_copy"), size(other.size)
    {
        data = new char[size];
        memcpy(data, other.data, size);
        cout << "非効率アセット '" << filename << "' をコピー作成\n";
    }
    
    // コピー代入演算子（遅い深いコピー）
    IneffientGameAsset& operator=(const IneffientGameAsset& other)
    {
        if (this != &other)
        {
            delete[] data;
            
            filename = other.filename + "_assigned";
            size = other.size;
            data = new char[size];
            memcpy(data, other.data, size);
            cout << "非効率アセット '" << filename << "' をコピー代入\n";
        }
        return *this;
    }
    
    ~IneffientGameAsset()
    {
        delete[] data;
        cout << "非効率アセット '" << filename << "' を削除\n";
    }
    
    void printInfo() const
    {
        cout << "  アセット: " << filename << " (" << size << " bytes)\n";
    }
};

// ムーブセマンティクスを持つ効率的なクラス
class EfficientGameAsset
{
public:
    string filename;
    char* data;
    size_t size;
    
    EfficientGameAsset(const string& filename, size_t size) 
        : filename(filename), size(size)
    {
        data = new char[size];
        // ダミーデータで初期化
        for (size_t i = 0; i < size; ++i)
        {
            data[i] = static_cast<char>(rand() % 256);
        }
        cout << "効率アセット '" << filename << "' をロード (" << size << " bytes)\n";
    }
    
    // コピーコンストラクタ（深いコピー）
    EfficientGameAsset(const EfficientGameAsset& other)
        : filename(other.filename + "_copy"), size(other.size)
    {
        data = new char[size];
        memcpy(data, other.data, size);
        cout << "効率アセット '" << filename << "' をコピー作成\n";
    }
    
    // ムーブコンストラクタ（高速なリソース移動）
    EfficientGameAsset(EfficientGameAsset&& other) noexcept
        : filename(move(other.filename)), data(other.data), size(other.size)
    {
        // 元のオブジェクトを無効化
        other.data = nullptr;
        other.size = 0;
        cout << "効率アセット '" << filename << "' をムーブ作成\n";
    }
    
    // コピー代入演算子（深いコピー）
    EfficientGameAsset& operator=(const EfficientGameAsset& other)
    {
        if (this != &other)
        {
            delete[] data;
            
            filename = other.filename + "_assigned";
            size = other.size;
            data = new char[size];
            memcpy(data, other.data, size);
            cout << "効率アセット '" << filename << "' をコピー代入\n";
        }
        return *this;
    }
    
    // ムーブ代入演算子（高速なリソース移動）
    EfficientGameAsset& operator=(EfficientGameAsset&& other) noexcept
    {
        if (this != &other)
        {
            delete[] data;
            
            filename = move(other.filename);
            data = other.data;
            size = other.size;
            
            // 元のオブジェクトを無効化
            other.data = nullptr;
            other.size = 0;
            cout << "効率アセット '" << filename << "' をムーブ代入\n";
        }
        return *this;
    }
    
    ~EfficientGameAsset()
    {
        if (data)
        {
            delete[] data;
            cout << "効率アセット '" << filename << "' を削除\n";
        }
        else
        {
            cout << "効率アセット '" << filename << "' (ムーブ済み) を削除\n";
        }
    }
    
    void printInfo() const
    {
        if (data)
        {
            cout << "  アセット: " << filename << " (" << size << " bytes)\n";
        }
        else
        {
            cout << "  アセット: " << filename << " (ムーブ済み)\n";
        }
    }
    
    bool isValid() const { return data != nullptr; }
};

// ゲームオブジェクトコンテナでのムーブ活用例
class GameObjectManager
{
private:
    vector<EfficientGameAsset> assets;
    
public:
    void addAsset(EfficientGameAsset asset)
    {
        cout << "アセットをマネージャーに追加中...\n";
        assets.push_back(move(asset));  // ムーブで効率的に追加
    }
    
    EfficientGameAsset removeAsset(size_t index)
    {
        if (index < assets.size())
        {
            cout << "アセットをマネージャーから削除中...\n";
            EfficientGameAsset asset = move(assets[index]);
            assets.erase(assets.begin() + index);
            return asset;  // ムーブで返す
        }
        return EfficientGameAsset("無効", 0);
    }
    
    void printAllAssets() const
    {
        cout << "\nマネージャー内のアセット一覧:\n";
        for (size_t i = 0; i < assets.size(); ++i)
        {
            cout << "  [" << i << "] ";
            assets[i].printInfo();
        }
    }
    
    size_t getAssetCount() const { return assets.size(); }
};

// ファクトリーパターンでのムーブ活用
class AssetFactory
{
public:
    static EfficientGameAsset createTexture(const string& name, size_t width, size_t height)
    {
        size_t dataSize = width * height * 4; // RGBA
        cout << "ファクトリーでテクスチャを作成: " << name << "\n";
        return EfficientGameAsset(name + ".png", dataSize);  // ムーブで返す
    }
    
    static EfficientGameAsset createAudio(const string& name, size_t sampleRate, size_t duration)
    {
        size_t dataSize = sampleRate * duration * 2; // 16-bit mono
        cout << "ファクトリーでオーディオを作成: " << name << "\n";
        return EfficientGameAsset(name + ".wav", dataSize);  // ムーブで返す
    }
    
    static EfficientGameAsset createModel(const string& name, size_t vertexCount)
    {
        size_t dataSize = vertexCount * 32; // 頂点あたり32バイトと仮定
        cout << "ファクトリーでモデルを作成: " << name << "\n";
        return EfficientGameAsset(name + ".obj", dataSize);  // ムーブで返す
    }
};

// Perfect Forwardingの例
template<typename T, typename... Args>
unique_ptr<T> make_game_object(Args&&... args)
{
    cout << "Perfect Forwardingでオブジェクトを作成\n";
    return make_unique<T>(forward<Args>(args)...);
}

// ムーブセマンティクスの基本デモ
void basicMoveDemo()
{
    cout << "\n=== ムーブセマンティクスの基本デモ ===\n";
    
    // ムーブコンストラクタのデモ
    {
        EfficientGameAsset asset1("original_texture.png", 1024);
        asset1.printInfo();
        
        // ムーブコンストラクタで高速に移動
        EfficientGameAsset asset2 = move(asset1);
        
        cout << "\nムーブ後の状態:\n";
        cout << "asset1: ";
        asset1.printInfo();
        cout << "asset2: ";
        asset2.printInfo();
    }
    
    cout << "\nムーブコンストラクタのデモ終了\n";
    
    // ムーブ代入演算子のデモ
    {
        EfficientGameAsset asset1("texture1.png", 512);
        EfficientGameAsset asset2("texture2.png", 256);
        
        cout << "\nムーブ代入前:\n";
        cout << "asset1: ";
        asset1.printInfo();
        cout << "asset2: ";
        asset2.printInfo();
        
        asset2 = move(asset1);
        
        cout << "\nムーブ代入後:\n";
        cout << "asset1: ";
        asset1.printInfo();
        cout << "asset2: ";
        asset2.printInfo();
    }
    
    cout << "\nムーブ代入演算子のデモ終了\n";
}

// コンテナでのムーブ活用
void containerMoveDemo()
{
    cout << "\n=== コンテナでのムーブ活用 ===\n";
    
    GameObjectManager manager;
    
    // アセットを作成してマネージャーに追加
    cout << "\nアセットを作成してマネージャーに追加...\n";
    
    EfficientGameAsset playerTexture("player.png", 2048);
    EfficientGameAsset enemyTexture("enemy.png", 1024);
    EfficientGameAsset backgroundTexture("background.png", 4096);
    
    manager.addAsset(move(playerTexture));     // ムーブで追加
    manager.addAsset(move(enemyTexture));      // ムーブで追加
    manager.addAsset(move(backgroundTexture)); // ムーブで追加
    
    cout << "\n元のアセットの状態（ムーブ済み）:\n";
    cout << "playerTexture: ";
    playerTexture.printInfo();
    cout << "enemyTexture: ";
    enemyTexture.printInfo();
    cout << "backgroundTexture: ";
    backgroundTexture.printInfo();
    
    manager.printAllAssets();
    
    // アセットを取り出し
    cout << "\nアセットを取り出し...\n";
    EfficientGameAsset retrievedAsset = manager.removeAsset(1);
    
    cout << "\n取り出したアセット:\n";
    retrievedAsset.printInfo();
    
    manager.printAllAssets();
}

// ファクトリーパターンでのムーブ
void factoryMoveDemo()
{
    cout << "\n=== ファクトリーパターンでのムーブ ===\n";
    
    vector<EfficientGameAsset> gameAssets;
    
    // ファクトリーで作成したアセットを直接ムーブ
    gameAssets.push_back(AssetFactory::createTexture("ui_button", 128, 64));
    gameAssets.push_back(AssetFactory::createAudio("explosion_sound", 44100, 3));
    gameAssets.push_back(AssetFactory::createModel("character_model", 5000));
    gameAssets.push_back(AssetFactory::createTexture("skybox", 1024, 1024));
    
    cout << "\nファクトリーで作成したアセット一覧:\n";
    for (size_t i = 0; i < gameAssets.size(); ++i)
    {
        cout << "  [" << i << "] ";
        gameAssets[i].printInfo();
    }
    
    // emplace_backでインプレース構築
    cout << "\nemplace_backでインプレース構築...\n";
    gameAssets.emplace_back("in_place_texture.png", 512);
    
    cout << "\n最終的なアセット一覧:\n";
    for (size_t i = 0; i < gameAssets.size(); ++i)
    {
        cout << "  [" << i << "] ";
        gameAssets[i].printInfo();
    }
}

// Perfect Forwardingのデモ
void perfectForwardingDemo()
{
    cout << "\n=== Perfect Forwardingのデモ ===\n";
    
    // 各種パラメータでオブジェクトを作成
    auto asset1 = make_game_object<EfficientGameAsset>("forwarded1.png", 1024);
    
    string filename = "forwarded2.png";
    auto asset2 = make_game_object<EfficientGameAsset>(filename, 2048);
    
    cout << "\nPerfect Forwardingで作成したアセット:\n";
    asset1->printInfo();
    asset2->printInfo();
}

// パフォーマンス比較
void performanceComparison()
{
    cout << "\n=== パフォーマンス比較 ===\n";
    
    const int iterations = 1000;
    const size_t assetSize = 1024;
    
    // コピーでのパフォーマンス（非効率）
    {
        auto start = high_resolution_clock::now();
        
        vector<IneffientGameAsset> assets;
        
        for (int i = 0; i < iterations; ++i)
        {
            IneffientGameAsset asset("copy_test_" + to_string(i), assetSize);
            assets.push_back(asset);  // コピーが発生
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        
        cout << "コピー版 (" << iterations << "回): " << duration << " マイクロ秒\n";
    }
    
    // ムーブでのパフォーマンス（効率的）
    {
        auto start = high_resolution_clock::now();
        
        vector<EfficientGameAsset> assets;
        
        for (int i = 0; i < iterations; ++i)
        {
            EfficientGameAsset asset("move_test_" + to_string(i), assetSize);
            assets.push_back(move(asset));  // ムーブが発生
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        
        cout << "ムーブ版 (" << iterations << "回): " << duration << " マイクロ秒\n";
    }
    
    // emplace_backでのパフォーマンス（最高効率）
    {
        auto start = high_resolution_clock::now();
        
        vector<EfficientGameAsset> assets;
        
        for (int i = 0; i < iterations; ++i)
        {
            assets.emplace_back("emplace_test_" + to_string(i), assetSize);
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        
        cout << "emplace_back版 (" << iterations << "回): " << duration << " マイクロ秒\n";
    }
}

// RVO (Return Value Optimization)のデモ
EfficientGameAsset createAssetRVO(const string& name, size_t size)
{
    cout << "RVO関数内でアセットを作成: " << name << "\n";
    return EfficientGameAsset(name, size);  // RVOが適用される
}

void rvoDemo()
{
    cout << "\n=== RVO (Return Value Optimization)のデモ ===\n";
    
    cout << "RVOでアセットを作成中...\n";
    EfficientGameAsset asset = createAssetRVO("rvo_texture.png", 1024);
    
    cout << "\nRVOで作成されたアセット:\n";
    asset.printInfo();
    
    cout << "\nRVOにより、不要なコピーやムーブが省略されました\n";
}

int main()
{
    cout << "=== ムーブセマンティクスの最適化 ===\n";
    
    // ムーブセマンティクスの基本
    basicMoveDemo();
    
    // コンテナでのムーブ活用
    containerMoveDemo();
    
    // ファクトリーパターンでのムーブ
    factoryMoveDemo();
    
    // Perfect Forwardingのデモ
    perfectForwardingDemo();
    
    // RVOのデモ
    rvoDemo();
    
    // パフォーマンス比較
    performanceComparison();
    
    cout << "\n=== ムーブセマンティクスのメリット ===\n";
    cout << "1. 不要なコピーを防いでパフォーマンス向上\n";
    cout << "2. 一時オブジェクトからの効率的なリソース移動\n";
    cout << "3. コンテナへの高速な要素追加\n";
    cout << "4. 関数からの高速な戻り値返却\n";
    cout << "5. メモリ使用量の削減（一時オブジェクトの減少）\n";
    
    cout << "\n=== ゲーム開発での活用例 ===\n";
    cout << "1. アセットローダー: 大きなテクスチャやモデルの効率的な移動\n";
    cout << "2. ゲームオブジェクト管理: コンテナ間でのオブジェクト移動\n";
    cout << "3. レベルローダー: シーンデータの効率的な読み込み\n";
    cout << "4. パーティクルシステム: 動的オブジェクトの高速生成\n";
    cout << "5. UIシステム: ウィジェットの効率的な作成と管理\n";
    
    cout << "\n=== ベストプラクティス ===\n";
    cout << "1. Rule of Five: ムーブコンストラクタとムーブ代入を実装\n";
    cout << "2. noexcept指定: ムーブ操作は例外を投げないように設計\n";
    cout << "3. std::moveの使用: 明示的なムーブで意図を明確化\n";
    cout << "4. emplace系関数: コンテナでのインプレース構築を活用\n";
    cout << "5. Perfect Forwarding: テンプレートでの効率的なパラメータ転送\n";
    
    return 0;
}

// ムーブセマンティクスの重要なポイント：
// 1. リソースの所有権を移動し、元のオブジェクトを無効化
// 2. コピーよりも高速で、特に大きなオブジェクトで効果的
// 3. 一時オブジェクトや戻り値での活用が特に有効
// 4. noexcept指定でSTLコンテナがムーブを優先的に使用
// 5. ゲーム開発ではメモリやパフォーマンスが重要なので積極活用