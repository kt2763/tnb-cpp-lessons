// 第2講: インクルードガードとプリプロセッサ
// 重複インクルードを防ぐ方法とプリプロセッサディレクティブを学ぶ

#include <iostream>
#include <string>

// === 従来のインクルードガード（#ifndef方式）===
/*
// game_config.h
#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

const int MAX_LEVEL = 99;
const int MAX_HP = 9999;
const double CRITICAL_RATE = 0.1;

#endif // GAME_CONFIG_H
*/

// === pragma once方式（現代的な方法）===
/*
// modern_config.h  
#pragma once

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int FPS = 60;
*/

// === 条件付きコンパイルの例 ===
// デバッグモードとリリースモードの切り替え

// #define DEBUG_MODE  // この行をコメントアウト/有効化でモード切り替え

#ifdef DEBUG_MODE
    #define DEBUG_LOG(msg) std::cout << "[DEBUG] " << msg << std::endl
    const bool IS_DEBUG = true;
#else
    #define DEBUG_LOG(msg) // リリース版では何もしない
    const bool IS_DEBUG = false;
#endif

// プラットフォーム別の処理
#ifdef _WIN32
    #define PLATFORM_NAME "Windows"
    #define PATH_SEPARATOR "\\"
#elif defined(__linux__)
    #define PLATFORM_NAME "Linux"
    #define PATH_SEPARATOR "/"
#elif defined(__APPLE__)
    #define PLATFORM_NAME "macOS"
    #define PATH_SEPARATOR "/"
#else
    #define PLATFORM_NAME "Unknown"
    #define PATH_SEPARATOR "/"
#endif

// バージョン管理
#define GAME_VERSION_MAJOR 1
#define GAME_VERSION_MINOR 2
#define GAME_VERSION_PATCH 0

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define GAME_VERSION TOSTRING(GAME_VERSION_MAJOR) "." TOSTRING(GAME_VERSION_MINOR) "." TOSTRING(GAME_VERSION_PATCH)

// 機能の有効/無効切り替え
#define ENABLE_SOUND
#define ENABLE_MULTIPLAYER
// #define ENABLE_CHEATS  // チート機能（通常は無効）

// === ゲーム設定クラス ===
class GameConfig 
{
public:
    static void displayInfo() 
    {
        std::cout << "=== ゲーム情報 ===" << std::endl;
        std::cout << "バージョン: " << GAME_VERSION << std::endl;
        std::cout << "プラットフォーム: " << PLATFORM_NAME << std::endl;
        std::cout << "デバッグモード: " << (IS_DEBUG ? "有効" : "無効") << std::endl;
        
        #ifdef ENABLE_SOUND
            std::cout << "サウンド: 有効" << std::endl;
        #else
            std::cout << "サウンド: 無効" << std::endl;
        #endif
        
        #ifdef ENABLE_MULTIPLAYER
            std::cout << "マルチプレイヤー: 有効" << std::endl;
        #else
            std::cout << "マルチプレイヤー: 無効" << std::endl;
        #endif
        
        #ifdef ENABLE_CHEATS
            std::cout << "チート機能: 有効" << std::endl;
        #else
            std::cout << "チート機能: 無効" << std::endl;
        #endif
    }
};

// === プラットフォーム固有の実装例 ===
class FileSystem 
{
public:
    static std::string getConfigPath() 
    {
        std::string basePath;
        
        #ifdef _WIN32
            basePath = "C:\\Users\\Username\\AppData\\Local\\MyGame";
        #elif defined(__APPLE__)
            basePath = "~/Library/Application Support/MyGame";
        #else
            basePath = "~/.config/mygame";
        #endif
        
        return basePath + PATH_SEPARATOR + "config.ini";
    }
    
    static void createSaveDirectory() 
    {
        DEBUG_LOG("セーブディレクトリを作成中...");
        
        #ifdef _WIN32
            DEBUG_LOG("Windows用のディレクトリ作成処理");
        #elif defined(__linux__)
            DEBUG_LOG("Linux用のディレクトリ作成処理");
        #elif defined(__APPLE__)
            DEBUG_LOG("macOS用のディレクトリ作成処理");
        #endif
    }
};

// === デバッグ専用機能 ===
class DebugSystem 
{
public:
    static void godMode(bool enable) 
    {
        #ifdef ENABLE_CHEATS
            if (enable) 
            {
                std::cout << "無敵モードを有効にしました" << std::endl;
            }
            else 
            {
                std::cout << "無敵モードを無効にしました" << std::endl;
            }
        #else
            std::cout << "チート機能は無効です" << std::endl;
        #endif
    }
    
    static void addGold(int amount) 
    {
        #ifdef ENABLE_CHEATS
            std::cout << "ゴールドを" << amount << "追加しました" << std::endl;
        #else
            std::cout << "チート機能は無効です" << std::endl;
        #endif
    }
};

// === マクロの実践的な使用例 ===
// アサートマクロ
#ifdef DEBUG_MODE
    #define ASSERT(condition, message) \
        if (!(condition)) { \
            std::cout << "アサート失敗: " << message << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl; \
            std::abort(); \
        }
#else
    #define ASSERT(condition, message) // リリース版では何もしない
#endif

// ログレベル
#define LOG_ERROR 0
#define LOG_WARNING 1
#define LOG_INFO 2
#define LOG_DEBUG 3

#ifndef LOG_LEVEL
    #ifdef DEBUG_MODE
        #define LOG_LEVEL LOG_DEBUG
    #else
        #define LOG_LEVEL LOG_WARNING
    #endif
#endif

#define LOG(level, message) \
    if (level <= LOG_LEVEL) { \
        std::cout << "[" << #level << "] " << message << std::endl; \
    }

// === サンプルゲームクラス ===
class Game 
{
private:
    int playerGold;
    bool isInvincible;
    
public:
    Game() : playerGold(100), isInvincible(false) {}
    
    void start() 
    {
        DEBUG_LOG("ゲーム開始");
        
        std::cout << "\n=== ゲーム開始 ===" << std::endl;
        std::cout << "所持金: " << playerGold << "G" << std::endl;
        
        // ファイルシステムの初期化
        FileSystem::createSaveDirectory();
        std::cout << "設定ファイルパス: " << FileSystem::getConfigPath() << std::endl;
        
        // チート機能のテスト
        DebugSystem::godMode(true);
        DebugSystem::addGold(1000);
        
        // アサートのテスト
        ASSERT(playerGold >= 0, "プレイヤーのゴールドが負の値です");
        
        // ログのテスト
        if (LOG_LEVEL >= LOG_ERROR) std::cout << "[LOG_ERROR] エラーレベルのログです" << std::endl;
        if (LOG_LEVEL >= LOG_WARNING) std::cout << "[LOG_WARNING] 警告レベルのログです" << std::endl;
        if (LOG_LEVEL >= LOG_INFO) std::cout << "[LOG_INFO] 情報レベルのログです" << std::endl;
        if (LOG_LEVEL >= LOG_DEBUG) std::cout << "[LOG_DEBUG] デバッグレベルのログです" << std::endl;
    }
    
    void processInput(char command) 
    {
        DEBUG_LOG("入力処理: " + std::string(1, command));
        
        switch (command) 
        {
            case 'g':
                #ifdef ENABLE_CHEATS
                    playerGold += 100;
                    std::cout << "ゴールドを100追加！現在: " << playerGold << "G" << std::endl;
                #else
                    std::cout << "無効なコマンドです" << std::endl;
                #endif
                break;
                
            case 'i':
                #ifdef ENABLE_CHEATS
                    isInvincible = !isInvincible;
                    std::cout << "無敵モード: " << (isInvincible ? "ON" : "OFF") << std::endl;
                #else
                    std::cout << "無効なコマンドです" << std::endl;
                #endif
                break;
                
            default:
                std::cout << "不明なコマンド: " << command << std::endl;
        }
    }
};

int main() 
{
    std::cout << "=== インクルードガードとプリプロセッサの実例 ===" << std::endl;
    
    // ゲーム設定情報の表示
    GameConfig::displayInfo();
    
    // ゲーム開始
    Game game;
    game.start();
    
    // コマンド入力シミュレーション
    std::cout << "\n=== コマンドテスト ===" << std::endl;
    game.processInput('g');  // ゴールド追加
    game.processInput('i');  // 無敵モード
    game.processInput('x');  // 無効なコマンド
    
    return 0;
}

/*
    まとめ:
    - インクルードガードで重複インクルードを防ぐ
    - #ifndef/#define/#endif または #pragma once を使用
    - 条件付きコンパイルでプラットフォーム別の処理
    - デバッグ/リリースビルドの切り替え
    - マクロによる機能の有効/無効制御
    - バージョン管理やログレベル制御
    
    実践的な活用：
    - プラットフォーム固有のコード分岐
    - デバッグ機能の制御
    - 開発版と製品版の機能切り替え
    - ログ出力の制御
    - チート機能の管理
    
    注意点：
    - マクロの多用は可読性を損なう
    - プリプロセッサはC++の型システムを無視
    - constexpr ifなど現代的な手法も検討
*/