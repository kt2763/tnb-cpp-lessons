// 第3講: 名前空間の実践的な使用
// 大規模なゲームプロジェクトでの名前空間設計とベストプラクティス

#include <iostream>
#include <string>
#include <vector>
#include <map>

// === ゲーム全体の名前空間 ===
namespace MyGame 
{
    const std::string GAME_TITLE = "Epic Adventure";
    const std::string VERSION = "1.0.0";
    
    // === グラフィック関連の名前空間 ===
    namespace Graphics 
    {
        struct Color 
        {
            int r, g, b, a;
            
            Color(int red = 255, int green = 255, int blue = 255, int alpha = 255)
                : r(red), g(green), b(blue), a(alpha) {}
        };
        
        class Texture 
        {
        private:
            std::string filename;
            int width, height;
            
        public:
            Texture(const std::string& file, int w, int h)
                : filename(file), width(w), height(h) 
            {
                std::cout << "テクスチャを読み込み: " << filename << std::endl;
            }
            
            void render(int x, int y) const 
            {
                std::cout << "テクスチャを描画: (" << x << ", " << y << ") " << filename << std::endl;
            }
        };
        
        namespace UI 
        {
            class Button 
            {
            private:
                std::string text;
                int x, y, width, height;
                
            public:
                Button(const std::string& buttonText, int posX, int posY, int w, int h)
                    : text(buttonText), x(posX), y(posY), width(w), height(h) {}
                
                void draw() const 
                {
                    std::cout << "ボタン描画: \"" << text << "\" at (" << x << ", " << y << ")" << std::endl;
                }
                
                bool isClicked(int mouseX, int mouseY) const 
                {
                    return mouseX >= x && mouseX < x + width &&
                           mouseY >= y && mouseY < y + height;
                }
            };
            
            class Menu 
            {
            private:
                std::vector<Button> buttons;
                
            public:
                void addButton(const Button& button) 
                {
                    buttons.push_back(button);
                }
                
                void render() const 
                {
                    std::cout << "=== メニュー描画 ===" << std::endl;
                    for (const auto& button : buttons) 
                    {
                        button.draw();
                    }
                }
                
                int checkButtonClick(int x, int y) const 
                {
                    for (size_t i = 0; i < buttons.size(); i++) 
                    {
                        if (buttons[i].isClicked(x, y)) 
                        {
                            return static_cast<int>(i);
                        }
                    }
                    return -1;
                }
            };
        }
    }
    
    // === オーディオ関連の名前空間 ===
    namespace Audio 
    {
        enum class SoundType 
        {
            SFX,
            BGM,
            Voice
        };
        
        class Sound 
        {
        private:
            std::string filename;
            SoundType type;
            float volume;
            
        public:
            Sound(const std::string& file, SoundType soundType)
                : filename(file), type(soundType), volume(1.0f) 
            {
                std::cout << "サウンドを読み込み: " << filename << std::endl;
            }
            
            void play() const 
            {
                std::string typeStr;
                switch (type) 
                {
                    case SoundType::SFX: typeStr = "効果音"; break;
                    case SoundType::BGM: typeStr = "BGM"; break;
                    case SoundType::Voice: typeStr = "ボイス"; break;
                }
                std::cout << typeStr << "を再生: " << filename 
                          << " (音量: " << volume << ")" << std::endl;
            }
            
            void setVolume(float vol) 
            {
                volume = vol;
            }
        };
        
        class AudioManager 
        {
        private:
            std::map<std::string, Sound> sounds;
            
        public:
            void loadSound(const std::string& name, const std::string& filename, SoundType type) 
            {
                sounds.emplace(name, Sound(filename, type));
            }
            
            void playSound(const std::string& name) 
            {
                auto it = sounds.find(name);
                if (it != sounds.end()) 
                {
                    it->second.play();
                }
                else 
                {
                    std::cout << "サウンドが見つかりません: " << name << std::endl;
                }
            }
        };
    }
    
    // === ゲームプレイ関連の名前空間 ===
    namespace Gameplay 
    {
        class Player 
        {
        private:
            std::string name;
            int level;
            int hp, maxHp;
            int x, y;  // 位置
            
        public:
            Player(const std::string& playerName) 
                : name(playerName), level(1), hp(100), maxHp(100), x(0), y(0) {}
            
            void move(int deltaX, int deltaY) 
            {
                x += deltaX;
                y += deltaY;
                std::cout << name << "が移動: (" << x << ", " << y << ")" << std::endl;
            }
            
            void attack() 
            {
                std::cout << name << "が攻撃した！" << std::endl;
            }
            
            const std::string& getName() const { return name; }
            int getX() const { return x; }
            int getY() const { return y; }
        };
        
        namespace Combat 
        {
            enum class DamageType 
            {
                Physical,
                Magical,
                True
            };
            
            struct DamageInfo 
            {
                int amount;
                DamageType type;
                bool isCritical;
                
                DamageInfo(int dmg, DamageType dmgType, bool critical = false)
                    : amount(dmg), type(dmgType), isCritical(critical) {}
            };
            
            class BattleCalculator 
            {
            public:
                static DamageInfo calculateDamage(int attack, int defense, DamageType type) 
                {
                    int damage = attack;
                    
                    switch (type) 
                    {
                        case DamageType::Physical:
                            damage = std::max(1, attack - defense);
                            break;
                        case DamageType::Magical:
                            damage = std::max(1, attack - defense / 2);
                            break;
                        case DamageType::True:
                            // 真ダメージは軽減されない
                            break;
                    }
                    
                    bool critical = (std::rand() % 100) < 10;  // 10%でクリティカル
                    if (critical) 
                    {
                        damage *= 2;
                    }
                    
                    return DamageInfo(damage, type, critical);
                }
            };
        }
    }
    
    // === エンジン関連の名前空間 ===
    namespace Engine 
    {
        class GameEngine 
        {
        private:
            bool isRunning;
            Graphics::UI::Menu mainMenu;
            Audio::AudioManager audioManager;
            
        public:
            GameEngine() : isRunning(false) 
            {
                initialize();
            }
            
            void initialize() 
            {
                std::cout << "=== " << GAME_TITLE << " v" << VERSION << " ===" << std::endl;
                std::cout << "ゲームエンジンを初期化しています..." << std::endl;
                
                // メニューの設定
                mainMenu.addButton(Graphics::UI::Button("ニューゲーム", 100, 100, 200, 50));
                mainMenu.addButton(Graphics::UI::Button("コンティニュー", 100, 170, 200, 50));
                mainMenu.addButton(Graphics::UI::Button("オプション", 100, 240, 200, 50));
                mainMenu.addButton(Graphics::UI::Button("終了", 100, 310, 200, 50));
                
                // サウンドの読み込み
                audioManager.loadSound("menu_bgm", "menu.ogg", Audio::SoundType::BGM);
                audioManager.loadSound("click", "click.wav", Audio::SoundType::SFX);
                audioManager.loadSound("attack", "sword.wav", Audio::SoundType::SFX);
                
                std::cout << "初期化完了！" << std::endl;
            }
            
            void run() 
            {
                isRunning = true;
                audioManager.playSound("menu_bgm");
                
                std::cout << "\n=== ゲーム開始 ===" << std::endl;
                mainMenu.render();
                
                // メニュー操作のシミュレーション
                std::cout << "\nマウスクリック: (150, 100)" << std::endl;
                int clickedButton = mainMenu.checkButtonClick(150, 100);
                
                if (clickedButton >= 0) 
                {
                    audioManager.playSound("click");
                    handleMenuClick(clickedButton);
                }
            }
            
        private:
            void handleMenuClick(int buttonIndex) 
            {
                switch (buttonIndex) 
                {
                    case 0:
                        std::cout << "ニューゲームを開始します" << std::endl;
                        startNewGame();
                        break;
                    case 1:
                        std::cout << "セーブデータを読み込みます" << std::endl;
                        break;
                    case 2:
                        std::cout << "オプション画面を開きます" << std::endl;
                        break;
                    case 3:
                        std::cout << "ゲームを終了します" << std::endl;
                        isRunning = false;
                        break;
                }
            }
            
            void startNewGame() 
            {
                std::cout << "\n=== 新しい冒険の始まり ===" << std::endl;
                
                Gameplay::Player hero("勇者");
                
                // 戦闘シミュレーション
                std::cout << "\n敵と遭遇！" << std::endl;
                audioManager.playSound("attack");
                
                auto damage = Gameplay::Combat::BattleCalculator::calculateDamage(50, 10, Gameplay::Combat::DamageType::Physical);
                
                std::cout << "与えたダメージ: " << damage.amount;
                if (damage.isCritical) 
                {
                    std::cout << " (クリティカル！)";
                }
                std::cout << std::endl;
            }
        };
    }
}

// === 名前空間エイリアス ===
namespace GFX = MyGame::Graphics;
namespace UI = MyGame::Graphics::UI;
namespace SFX = MyGame::Audio;

int main() 
{
    std::cout << "=== 名前空間の実践的な使用例 ===" << std::endl;
    
    // 名前空間を使った開発
    MyGame::Engine::GameEngine game;
    game.run();
    
    std::cout << "\n=== 名前空間エイリアスの使用 ===" << std::endl;
    
    // エイリアスを使用した簡潔な記述
    GFX::Color red(255, 0, 0);
    UI::Button testButton("テスト", 50, 50, 100, 30);
    testButton.draw();
    
    // 複数の名前空間の使用例
    SFX::Sound testSound("test.wav", SFX::SoundType::SFX);
    testSound.play();
    
    return 0;
}

/*
    まとめ:
    - 大規模プロジェクトでは階層的な名前空間設計が重要
    - 機能別に名前空間を分割（Graphics, Audio, Gameplay等）
    - ネストした名前空間で細かい分類も可能
    - 名前空間エイリアスで長い名前を短縮
    - 名前の衝突を防ぎ、コードの整理に有効
    
    ゲーム開発での名前空間設計例:
    - MyGame::Graphics::UI (UIコンポーネント)
    - MyGame::Audio::SFX (効果音)
    - MyGame::Gameplay::Combat (戦闘システム)
    - MyGame::Engine (エンジンコア)
    
    ベストプラクティス:
    - 意味のある名前空間名を使用
    - 深すぎるネストは避ける
    - usingディレクティブはローカルスコープで
    - ヘッダファイルではusing namespaceを避ける
    - エイリアスで可読性を向上
*/