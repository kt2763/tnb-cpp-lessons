// unique_ptrの基礎と活用
// ゲーム開発でのRAIIパターンと自動メモリ管理

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

// ゲームアセットクラス
class GameAsset
{
public:
    string name;
    size_t size;
    
    GameAsset(const string& name, size_t size) : name(name), size(size)
    {
        cout << "アセット '" << name << "' をロード (サイズ: " << size << "KB)\n";
    }
    
    ~GameAsset()
    {
        cout << "アセット '" << name << "' をアンロード\n";
    }
    
    void use() const
    {
        cout << "アセット '" << name << "' を使用中\n";
    }
};

// ゲームキャラクタークラス
class Character
{
public:
    string name;
    int level;
    unique_ptr<GameAsset> weapon;    // 武器アセット
    unique_ptr<GameAsset> armor;     // 防具アセット
    
    Character(const string& name, int level) : name(name), level(level)
    {
        cout << "キャラクター '" << name << "' を作成 (レベル " << level << ")\n";
    }
    
    ~Character()
    {
        cout << "キャラクター '" << name << "' を削除\n";
        // unique_ptrは自動的に解放される
    }
    
    void equipWeapon(unique_ptr<GameAsset> newWeapon)
    {
        weapon = move(newWeapon);
        cout << name << " が武器を装備: " << weapon->name << "\n";
    }
    
    void equipArmor(unique_ptr<GameAsset> newArmor)
    {
        armor = move(newArmor);
        cout << name << " が防具を装備: " << armor->name << "\n";
    }
    
    void displayStatus() const
    {
        cout << "=== " << name << " のステータス ===\n";
        cout << "レベル: " << level << "\n";
        cout << "武器: " << (weapon ? weapon->name : "なし") << "\n";
        cout << "防具: " << (armor ? armor->name : "なし") << "\n";
    }
};

// unique_ptrの基本操作
void basicUniquePtr()
{
    cout << "\n=== unique_ptrの基本操作 ===\n";
    
    // 基本的な作成方法
    unique_ptr<int> playerScore(new int(1500));
    cout << "プレイヤースコア: " << *playerScore << "\n";
    
    // make_uniqueを使用した推奨方法（C++14以降）
    auto playerLevel = make_unique<int>(25);
    cout << "プレイヤーレベル: " << *playerLevel << "\n";
    
    // オブジェクトの作成
    auto hero = make_unique<Character>("勇者", 10);
    hero->displayStatus();
    
    // unique_ptrはスコープを抜けると自動的に削除される
    cout << "関数終了時に自動的にメモリが解放される\n";
}

// 所有権の転送（moveセマンティクス）
void ownershipTransfer()
{
    cout << "\n=== 所有権の転送 ===\n";
    
    // 武器を作成
    auto sword = make_unique<GameAsset>("エクスカリバー", 50);
    cout << "武器作成完了\n";
    
    // キャラクター作成
    auto knight = make_unique<Character>("騎士", 15);
    
    // 所有権を転送（moveセマンティクス）
    knight->equipWeapon(move(sword));
    
    // swordはもう使用できない（nullptrになっている）
    if (!sword)
    {
        cout << "元のswordポインタはnullptrになっている\n";
    }
    
    knight->displayStatus();
}

// コンテナでのunique_ptr使用
void uniquePtrInContainers()
{
    cout << "\n=== コンテナでのunique_ptr使用 ===\n";
    
    // キャラクターの配列を作成
    vector<unique_ptr<Character>> party;
    
    // パーティメンバーを追加
    party.push_back(make_unique<Character>("戦士", 20));
    party.push_back(make_unique<Character>("魔法使い", 18));
    party.push_back(make_unique<Character>("僧侶", 16));
    party.push_back(make_unique<Character>("盗賊", 19));
    
    cout << "\nパーティメンバー一覧:\n";
    for (const auto& member : party)
    {
        member->displayStatus();
        cout << "\n";
    }
    
    cout << "パーティサイズ: " << party.size() << "\n";
    
    // 特定のメンバーを削除（イテレータを使用）
    cout << "\n魔法使いがパーティから離脱...\n";
    party.erase(party.begin() + 1);  // 2番目を削除
    
    cout << "現在のパーティサイズ: " << party.size() << "\n";
}

// カスタムデリータの使用
class FileResource
{
public:
    FILE* file;
    string filename;
    
    FileResource(const string& filename) : filename(filename)
    {
        file = fopen(filename.c_str(), "w");
        if (file)
        {
            cout << "ファイル '" << filename << "' をオープン\n";
        }
        else
        {
            cout << "ファイル '" << filename << "' のオープンに失敗\n";
        }
    }
};

// カスタムデリータ
void fileDeleter(FileResource* resource)
{
    if (resource && resource->file)
    {
        fclose(resource->file);
        cout << "ファイル '" << resource->filename << "' をクローズ\n";
    }
    delete resource;
}

void customDeleterDemo()
{
    cout << "\n=== カスタムデリータの使用 ===\n";
    
    // カスタムデリータを指定したunique_ptr
    unique_ptr<FileResource, decltype(&fileDeleter)> gameFile(
        new FileResource("savegame.txt"), fileDeleter
    );
    
    if (gameFile && gameFile->file)
    {
        fprintf(gameFile->file, "ゲームセーブデータ\n");
        fprintf(gameFile->file, "プレイヤーレベル: 50\n");
        cout << "セーブデータを書き込み中...\n";
    }
    
    // スコープを抜けるとカスタムデリータが呼ばれる
}

// ゲームエンジンでのリソース管理の例
class GameEngine
{
private:
    unique_ptr<GameAsset> backgroundMusic;
    unique_ptr<GameAsset> currentLevel;
    vector<unique_ptr<Character>> activeEnemies;
    
public:
    GameEngine()
    {
        cout << "\nゲームエンジンを初期化\n";
    }
    
    ~GameEngine()
    {
        cout << "ゲームエンジンを終了\n";
        // すべてのunique_ptrは自動的に解放される
    }
    
    void loadLevel(const string& levelName)
    {
        cout << "\nレベルをロード中: " << levelName << "\n";
        
        // 既存のレベルがあれば自動的に解放される
        currentLevel = make_unique<GameAsset>(levelName, 2048);
        
        // BGMをロード
        backgroundMusic = make_unique<GameAsset>(levelName + "_bgm", 512);
        
        // 敵を生成
        activeEnemies.clear();
        for (int i = 0; i < 3; i++)
        {
            activeEnemies.push_back(
                make_unique<Character>("ゴブリン" + to_string(i + 1), 1 + i)
            );
        }
    }
    
    void clearLevel()
    {
        cout << "\nレベルをクリア\n";
        currentLevel.reset();      // 明示的に解放
        backgroundMusic.reset();   // 明示的に解放
        activeEnemies.clear();     // すべての敵を削除
    }
    
    void displayStatus() const
    {
        cout << "\n=== ゲームエンジンの状態 ===\n";
        cout << "現在のレベル: " << (currentLevel ? currentLevel->name : "なし") << "\n";
        cout << "BGM: " << (backgroundMusic ? backgroundMusic->name : "なし") << "\n";
        cout << "アクティブな敵の数: " << activeEnemies.size() << "\n";
    }
};

void gameEngineDemo()
{
    cout << "\n=== ゲームエンジンでのリソース管理 ===\n";
    
    GameEngine engine;
    
    // レベル1をロード
    engine.loadLevel("森のダンジョン");
    engine.displayStatus();
    
    // レベル2をロード（既存のリソースが自動的に解放される）
    engine.loadLevel("火山の洞窟");
    engine.displayStatus();
    
    // レベルをクリア
    engine.clearLevel();
    engine.displayStatus();
    
    // GameEngineのデストラクタですべてが解放される
}

int main()
{
    cout << "=== unique_ptrの基礎と活用 ===\n";
    
    // 基本操作
    basicUniquePtr();
    
    // 所有権の転送
    ownershipTransfer();
    
    // コンテナでの使用
    uniquePtrInContainers();
    
    // カスタムデリータ
    customDeleterDemo();
    
    // ゲームエンジンでの例
    gameEngineDemo();
    
    cout << "\n=== unique_ptrのメリット ===\n";
    cout << "1. 自動的なメモリ管理（RAII）\n";
    cout << "2. コピー禁止による明確な所有権\n";
    cout << "3. メモリリークの防止\n";
    cout << "4. 例外安全性\n";
    cout << "5. パフォーマンスのオーバーヘッドがほとんどない\n";
    
    cout << "\n=== 使用指針 ===\n";
    cout << "1. 単一所有者のリソース管理\n";
    cout << "2. ファクトリーパターンでのオブジェクト作成\n";
    cout << "3. PIMPLイディオムの実装\n";
    cout << "4. コンテナ内での多態性オブジェクト管理\n";
    cout << "5. リソースのライフサイクルがスコープと一致する場合\n";
    
    return 0;
}

// unique_ptrをゲーム開発で活用する具体例：
// 1. アセットマネージャーでのリソース管理
// 2. ゲームオブジェクトのライフサイクル管理
// 3. UIシステムでのウィジェット管理
// 4. サウンドシステムでのオーディオリソース管理
// 5. レンダリングシステムでのグラフィックスリソース管理