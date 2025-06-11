// 第5講: プロジェクト構造のベストプラクティス
// 大規模ゲームプロジェクトの適切なファイル構成と管理方法を学ぶ

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>

// このファイルは、実際のプロジェクト構造を説明するためのサンプル
// 実際の開発では、これらは別々のファイルに分割される

/*
推奨プロジェクト構造:

MyGame/
├── src/                    # ソースファイル
│   ├── core/              # エンジンコア
│   │   ├── Engine.h/cpp   # メインエンジン
│   │   ├── Scene.h/cpp    # シーン管理
│   │   └── GameObject.h/cpp # ゲームオブジェクト基底
│   ├── graphics/          # グラフィック関連
│   │   ├── Renderer.h/cpp # レンダラー
│   │   ├── Texture.h/cpp  # テクスチャ
│   │   └── Camera.h/cpp   # カメラ
│   ├── audio/             # オーディオ関連
│   │   ├── AudioManager.h/cpp
│   │   └── Sound.h/cpp
│   ├── input/             # 入力処理
│   │   └── InputManager.h/cpp
│   ├── game/              # ゲーム固有のロジック
│   │   ├── Player.h/cpp   # プレイヤー
│   │   ├── Enemy.h/cpp    # 敵
│   │   └── GameLogic.h/cpp
│   ├── ui/                # ユーザーインターフェース
│   │   ├── Menu.h/cpp     # メニューシステム
│   │   └── HUD.h/cpp      # ゲーム内UI
│   ├── utils/             # ユーティリティ
│   │   ├── Math.h/cpp     # 数学関数
│   │   └── Logger.h/cpp   # ログシステム
│   └── main.cpp           # エントリーポイント
├── include/               # 外部ヘッダファイル
├── assets/                # ゲームアセット
│   ├── textures/         # テクスチャファイル
│   ├── sounds/           # 音声ファイル
│   ├── models/           # 3Dモデル
│   └── data/             # ゲームデータ
├── libs/                  # 外部ライブラリ
├── build/                 # ビルド出力
├── tests/                 # テストコード
├── docs/                  # ドキュメント
├── CMakeLists.txt         # ビルド設定
└── README.md              # プロジェクト説明
*/

// === Core Engine (src/core/) ===
namespace Core 
{
    // ゲームオブジェクトの基底クラス
    class GameObject 
    {
    protected:
        std::string name;
        bool active;
        
    public:
        GameObject(const std::string& objectName) : name(objectName), active(true) {}
        virtual ~GameObject() = default;
        
        virtual void initialize() {}
        virtual void update(double deltaTime) = 0;
        virtual void render() const = 0;
        virtual void cleanup() {}
        
        const std::string& getName() const { return name; }
        bool isActive() const { return active; }
        void setActive(bool isActive) { active = isActive; }
    };
    
    // シーン管理
    class Scene 
    {
    protected:
        std::string sceneName;
        std::vector<std::unique_ptr<GameObject>> gameObjects;
        bool initialized;
        
    public:
        Scene(const std::string& name) : sceneName(name), initialized(false) {}
        virtual ~Scene() = default;
        
        virtual void load() = 0;
        virtual void unload() = 0;
        virtual void update(double deltaTime);
        virtual void render() const;
        
        void addGameObject(std::unique_ptr<GameObject> obj);
        const std::string& getName() const { return sceneName; }
    };
    
    // エンジンの中心クラス
    class Engine 
    {
    private:
        static Engine* instance;
        bool running;
        std::unique_ptr<Scene> currentScene;
        double deltaTime;
        
        Engine() : running(false), deltaTime(1.0/60.0) {}
        
    public:
        static Engine& getInstance() 
        {
            if (!instance) 
            {
                instance = new Engine();
            }
            return *instance;
        }
        
        void initialize();
        void run();
        void shutdown();
        
        void changeScene(std::unique_ptr<Scene> newScene);
        double getDeltaTime() const { return deltaTime; }
    };
    
    Engine* Engine::instance = nullptr;
}

// === Graphics System (src/graphics/) ===
namespace Graphics 
{
    // カメラクラス
    class Camera 
    {
    private:
        double x, y;
        double zoom;
        
    public:
        Camera() : x(0), y(0), zoom(1.0) {}
        
        void setPosition(double posX, double posY) 
        {
            x = posX;
            y = posY;
        }
        
        void setZoom(double zoomLevel) 
        {
            zoom = zoomLevel;
        }
        
        void follow(const Core::GameObject& target) 
        {
            // ターゲットを追従する処理
            std::cout << "カメラが" << target.getName() << "を追従中" << std::endl;
        }
    };
    
    // テクスチャ管理
    class Texture 
    {
    private:
        std::string filename;
        int width, height;
        
    public:
        Texture(const std::string& file) : filename(file), width(0), height(0) 
        {
            // テクスチャ読み込み処理
            std::cout << "テクスチャ読み込み: " << filename << std::endl;
        }
        
        void render(double x, double y) const 
        {
            std::cout << "テクスチャ描画: " << filename << " at (" << x << ", " << y << ")" << std::endl;
        }
    };
    
    // レンダラー
    class Renderer 
    {
    private:
        Camera camera;
        
    public:
        void clear() 
        {
            std::cout << "画面をクリア" << std::endl;
        }
        
        void present() 
        {
            std::cout << "画面を更新" << std::endl;
        }
        
        Camera& getCamera() { return camera; }
    };
}

// === Audio System (src/audio/) ===
namespace Audio 
{
    class Sound 
    {
    private:
        std::string filename;
        bool isLoaded;
        
    public:
        Sound(const std::string& file) : filename(file), isLoaded(false) 
        {
            load();
        }
        
        void load() 
        {
            std::cout << "音声ファイル読み込み: " << filename << std::endl;
            isLoaded = true;
        }
        
        void play() 
        {
            if (isLoaded) 
            {
                std::cout << "音声再生: " << filename << std::endl;
            }
        }
    };
    
    class AudioManager 
    {
    private:
        std::map<std::string, std::unique_ptr<Sound>> sounds;
        
    public:
        void loadSound(const std::string& name, const std::string& filename) 
        {
            sounds[name] = std::make_unique<Sound>(filename);
        }
        
        void playSound(const std::string& name) 
        {
            auto it = sounds.find(name);
            if (it != sounds.end()) 
            {
                it->second->play();
            }
        }
    };
}

// === Input System (src/input/) ===
namespace Input 
{
    enum class Key 
    {
        W, A, S, D, Space, Escape
    };
    
    class InputManager 
    {
    private:
        std::map<Key, bool> keyStates;
        
    public:
        void update() 
        {
            // 実際はシステムから入力状態を取得
        }
        
        bool isKeyPressed(Key key) 
        {
            return keyStates[key];
        }
        
        bool isKeyDown(Key key) 
        {
            // 押された瞬間を判定
            return keyStates[key];  // 簡略化
        }
    };
}

// === Game Logic (src/game/) ===
namespace Game 
{
    class Player : public Core::GameObject 
    {
    private:
        double x, y;
        double speed;
        int health;
        Graphics::Texture texture;
        
    public:
        Player() : Core::GameObject("Player"), x(0), y(0), speed(100), 
                   health(100), texture("assets/textures/player.png") {}
        
        void update(double deltaTime) override 
        {
            // 入力処理
            Input::InputManager& input = getInputManager();
            
            if (input.isKeyPressed(Input::Key::W)) y -= speed * deltaTime;
            if (input.isKeyPressed(Input::Key::S)) y += speed * deltaTime;
            if (input.isKeyPressed(Input::Key::A)) x -= speed * deltaTime;
            if (input.isKeyPressed(Input::Key::D)) x += speed * deltaTime;
        }
        
        void render() const override 
        {
            texture.render(x, y);
        }
        
        double getX() const { return x; }
        double getY() const { return y; }
        
    private:
        Input::InputManager& getInputManager() 
        {
            static Input::InputManager instance;
            return instance;
        }
    };
    
    class Enemy : public Core::GameObject 
    {
    private:
        double x, y;
        int health;
        Graphics::Texture texture;
        
    public:
        Enemy(const std::string& name, double posX, double posY) 
            : Core::GameObject(name), x(posX), y(posY), health(50),
              texture("assets/textures/enemy.png") {}
        
        void update(double deltaTime) override 
        {
            // AI処理
        }
        
        void render() const override 
        {
            texture.render(x, y);
        }
    };
}

// === UI System (src/ui/) ===
namespace UI 
{
    class HUD 
    {
    private:
        const Game::Player* player;
        
    public:
        HUD(const Game::Player* playerRef) : player(playerRef) {}
        
        void render() const 
        {
            if (player) 
            {
                std::cout << "=== HUD ===" << std::endl;
                std::cout << "位置: (" << player->getX() << ", " << player->getY() << ")" << std::endl;
            }
        }
    };
    
    class Menu 
    {
    private:
        std::vector<std::string> menuItems;
        int selectedIndex;
        
    public:
        Menu() : selectedIndex(0) 
        {
            menuItems = {"ニューゲーム", "コンティニュー", "オプション", "終了"};
        }
        
        void render() const 
        {
            std::cout << "=== メインメニュー ===" << std::endl;
            for (size_t i = 0; i < menuItems.size(); i++) 
            {
                if (i == selectedIndex) 
                {
                    std::cout << "> " << menuItems[i] << std::endl;
                }
                else 
                {
                    std::cout << "  " << menuItems[i] << std::endl;
                }
            }
        }
        
        void selectNext() 
        {
            selectedIndex = (selectedIndex + 1) % menuItems.size();
        }
        
        int getSelectedIndex() const { return selectedIndex; }
    };
}

// === Utility Systems (src/utils/) ===
namespace Utils 
{
    enum class LogLevel 
    {
        Debug, Info, Warning, Error
    };
    
    class Logger 
    {
    public:
        static void log(LogLevel level, const std::string& message) 
        {
            std::string levelStr;
            switch (level) 
            {
                case LogLevel::Debug: levelStr = "DEBUG"; break;
                case LogLevel::Info: levelStr = "INFO"; break;
                case LogLevel::Warning: levelStr = "WARNING"; break;
                case LogLevel::Error: levelStr = "ERROR"; break;
            }
            
            std::cout << "[" << levelStr << "] " << message << std::endl;
        }
    };
    
    class AssetManager 
    {
    private:
        std::map<std::string, std::string> assetPaths;
        
    public:
        void loadAssetDatabase() 
        {
            assetPaths["player_texture"] = "assets/textures/player.png";
            assetPaths["enemy_texture"] = "assets/textures/enemy.png";
            assetPaths["bgm_main"] = "assets/sounds/main_theme.ogg";
            assetPaths["sfx_attack"] = "assets/sounds/attack.wav";
            
            Utils::Logger::log(Utils::LogLevel::Info, "アセットデータベースを読み込みました");
        }
        
        std::string getAssetPath(const std::string& assetName) 
        {
            auto it = assetPaths.find(assetName);
            return (it != assetPaths.end()) ? it->second : "";
        }
    };
}

// === Game Scene (具体的なシーンの実装) ===
class GameScene : public Core::Scene 
{
private:
    std::unique_ptr<Game::Player> player;
    std::unique_ptr<UI::HUD> hud;
    Graphics::Renderer renderer;
    Audio::AudioManager audioManager;
    
public:
    GameScene() : Core::Scene("GameScene") {}
    
    void load() override 
    {
        Utils::Logger::log(Utils::LogLevel::Info, "ゲームシーンを読み込み中...");
        
        // プレイヤー作成
        player = std::make_unique<Game::Player>();
        addGameObject(std::unique_ptr<Core::GameObject>(player.get()));
        
        // 敵を配置
        auto enemy1 = std::make_unique<Game::Enemy>("ゴブリン", 200, 100);
        addGameObject(std::move(enemy1));
        
        // HUD作成
        hud = std::make_unique<UI::HUD>(player.get());
        
        // BGM再生
        audioManager.loadSound("bgm", "assets/sounds/game_bgm.ogg");
        audioManager.playSound("bgm");
        
        initialized = true;
    }
    
    void unload() override 
    {
        Utils::Logger::log(Utils::LogLevel::Info, "ゲームシーンをアンロード中...");
        gameObjects.clear();
        player.reset();
        hud.reset();
    }
    
    void update(double deltaTime) override 
    {
        if (!initialized) return;
        
        // 基底クラスの更新処理
        Core::Scene::update(deltaTime);
        
        // カメラがプレイヤーを追従
        renderer.getCamera().follow(*player);
    }
    
    void render() const override 
    {
        renderer.clear();
        
        // ゲームオブジェクトの描画
        Core::Scene::render();
        
        // UI描画
        if (hud) 
        {
            hud->render();
        }
        
        renderer.present();
    }
};

// === Main Game Class ===
class MyGame 
{
private:
    Utils::AssetManager assetManager;
    
public:
    void initialize() 
    {
        Utils::Logger::log(Utils::LogLevel::Info, "ゲームを初期化中...");
        
        // アセットマネージャーの初期化
        assetManager.loadAssetDatabase();
        
        // エンジンの初期化
        Core::Engine::getInstance().initialize();
        
        Utils::Logger::log(Utils::LogLevel::Info, "初期化完了");
    }
    
    void run() 
    {
        // ゲームシーンを作成して開始
        auto gameScene = std::make_unique<GameScene>();
        Core::Engine::getInstance().changeScene(std::move(gameScene));
        
        // メインループ開始
        Core::Engine::getInstance().run();
    }
    
    void shutdown() 
    {
        Utils::Logger::log(Utils::LogLevel::Info, "ゲームを終了中...");
        Core::Engine::getInstance().shutdown();
    }
};

// === Implementation of Scene methods ===
void Core::Scene::update(double deltaTime) 
{
    for (auto& obj : gameObjects) 
    {
        if (obj->isActive()) 
        {
            obj->update(deltaTime);
        }
    }
}

void Core::Scene::render() const 
{
    for (const auto& obj : gameObjects) 
    {
        if (obj->isActive()) 
        {
            obj->render();
        }
    }
}

void Core::Scene::addGameObject(std::unique_ptr<GameObject> obj) 
{
    gameObjects.push_back(std::move(obj));
}

void Core::Engine::initialize() 
{
    Utils::Logger::log(Utils::LogLevel::Info, "エンジンを初期化しています...");
    running = true;
}

void Core::Engine::run() 
{
    Utils::Logger::log(Utils::LogLevel::Info, "メインループを開始します");
    
    int frameCount = 0;
    const int maxFrames = 10;  // デモ用に制限
    
    while (running && frameCount < maxFrames) 
    {
        if (currentScene) 
        {
            currentScene->update(deltaTime);
            currentScene->render();
        }
        
        frameCount++;
        std::cout << "--- フレーム " << frameCount << " ---" << std::endl;
    }
    
    Utils::Logger::log(Utils::LogLevel::Info, "メインループを終了しました");
}

void Core::Engine::shutdown() 
{
    if (currentScene) 
    {
        currentScene->unload();
        currentScene.reset();
    }
    running = false;
    Utils::Logger::log(Utils::LogLevel::Info, "エンジンをシャットダウンしました");
}

void Core::Engine::changeScene(std::unique_ptr<Scene> newScene) 
{
    if (currentScene) 
    {
        currentScene->unload();
    }
    
    currentScene = std::move(newScene);
    if (currentScene) 
    {
        currentScene->load();
    }
}

int main() 
{
    std::cout << "=== ゲームプロジェクト構造のベストプラクティス ===" << std::endl;
    
    MyGame game;
    
    try 
    {
        game.initialize();
        game.run();
        game.shutdown();
    }
    catch (const std::exception& e) 
    {
        Utils::Logger::log(Utils::LogLevel::Error, std::string("例外が発生しました: ") + e.what());
        game.shutdown();
        return -1;
    }
    
    return 0;
}

/*
    まとめ - プロジェクト構造のベストプラクティス:
    
    1. ディレクトリ構造:
       - 機能別にディレクトリを分割
       - ヘッダファイルと実装ファイルを適切に分離
       - アセットファイルは専用ディレクトリに配置
    
    2. 名前空間の使用:
       - ディレクトリ構造と名前空間を対応させる
       - 名前の衝突を防ぐ
       - コードの整理と理解しやすさの向上
    
    3. 依存関係の管理:
       - 循環依存を避ける
       - インターフェースと実装の分離
       - 前方宣言の活用
    
    4. 設計パターンの活用:
       - Singletonパターン（Engine）
       - Factoryパターン（ゲームオブジェクト生成）
       - Observerパターン（イベントシステム）
    
    5. アセット管理:
       - パスの一元管理
       - リソースの適切な読み込み/解放
       - メモリ効率の考慮
    
    6. ログとデバッグ:
       - 統一されたログシステム
       - レベル別のログ出力
       - デバッグ情報の適切な管理
    
    7. ビルドシステム:
       - CMakeやMakefileの活用
       - 外部ライブラリの管理
       - プラットフォーム別ビルドの対応
    
    この構造により、大規模なゲームプロジェクトでも
    保守性と拡張性を保ちながら開発を進められます。
*/