// C++20モジュールの基礎
// 従来のヘッダーファイルシステムから新しいモジュールシステムへ

#include <iostream>
#include <string>
#include <vector>
#include <memory>

// 注意：このファイルはモジュールの概念を説明するためのものです
// 実際のモジュール実装には対応するコンパイラとビルドシステムが必要です

/*
==== 従来のヘッダーファイルシステムの問題 ====

1. プリプロセッサの問題：
   - #include はテキスト置換
   - 同じヘッダーの重複インクルード
   - マクロ汚染の問題

2. コンパイル時間の問題：
   - 大きなヘッダーファイルの重複処理
   - テンプレートの重複インスタンス化
   - 依存関係の連鎖

3. 名前空間汚染：
   - グローバル名前空間の汚染
   - マクロによる予期しない名前の衝突

4. ODR（One Definition Rule）違反のリスク
*/

// =======================================
// 従来のヘッダーファイル方式の例
// =======================================

// 従来の方式：ヘッダーファイル + 実装ファイル
namespace traditional_approach
{
    // game_engine.h 相当
    class GameEngine
    {
    private:
        std::string title_;
        bool running_;
        
    public:
        GameEngine(const std::string& title);
        ~GameEngine();
        
        void initialize();
        void update();
        void render();
        void shutdown();
        
        bool isRunning() const { return running_; }
        const std::string& getTitle() const { return title_; }
    };
    
    // ユーティリティ関数
    void logMessage(const std::string& message);
    double calculateDistance(double x1, double y1, double x2, double y2);
    
    // game_engine.cpp 相当の実装
    GameEngine::GameEngine(const std::string& title) : title_(title), running_(false)
    {
        std::cout << "ゲームエンジン作成: " << title_ << std::endl;
    }
    
    GameEngine::~GameEngine()
    {
        std::cout << "ゲームエンジン破棄: " << title_ << std::endl;
    }
    
    void GameEngine::initialize()
    {
        std::cout << "ゲームエンジン初期化..." << std::endl;
        running_ = true;
    }
    
    void GameEngine::update()
    {
        if (running_)
        {
            // ゲームロジック更新
        }
    }
    
    void GameEngine::render()
    {
        if (running_)
        {
            // レンダリング処理
        }
    }
    
    void GameEngine::shutdown()
    {
        std::cout << "ゲームエンジン終了..." << std::endl;
        running_ = false;
    }
    
    void logMessage(const std::string& message)
    {
        std::cout << "[LOG] " << message << std::endl;
    }
    
    double calculateDistance(double x1, double y1, double x2, double y2)
    {
        double dx = x2 - x1;
        double dy = y2 - y1;
        return std::sqrt(dx * dx + dy * dy);
    }
}

// =======================================
// モジュールシステムの概念説明
// =======================================

/*
==== C++20 モジュールの利点 ====

1. 高速コンパイル：
   - モジュールは一度だけコンパイル
   - BMI（Binary Module Interface）の活用
   - プリプロセッサの負荷軽減

2. 強力なカプセル化：
   - exportキーワードによる明示的な公開
   - 内部実装の完全な隠蔽
   - マクロ汚染の防止

3. 依存関係の明確化：
   - importによる明示的な依存関係
   - 循環依存の検出
   - より良いエラーメッセージ

4. セマンティクスの改善：
   - ODR違反の防止
   - テンプレートの適切な管理
   - 名前空間の自動管理
*/

// =======================================
// モジュール構文の説明（疑似コード）
// =======================================

namespace module_syntax_explanation
{
    void explainModuleSyntax()
    {
        std::cout << "\n=== モジュール構文の説明 ===" << std::endl;
        
        std::cout << "1. モジュール宣言:" << std::endl;
        std::cout << "   export module my_module;" << std::endl;
        std::cout << "   - ファイルがモジュールであることを宣言" << std::endl;
        std::cout << "   - exportは他のモジュールから利用可能にする" << std::endl;
        
        std::cout << "\n2. モジュールインポート:" << std::endl;
        std::cout << "   import my_module;" << std::endl;
        std::cout << "   - 他のモジュールを使用する" << std::endl;
        std::cout << "   - #includeの代替" << std::endl;
        
        std::cout << "\n3. 標準ライブラリのインポート:" << std::endl;
        std::cout << "   import <iostream>;" << std::endl;
        std::cout << "   import <vector>;" << std::endl;
        std::cout << "   - 標準ライブラリもモジュールとして利用" << std::endl;
        
        std::cout << "\n4. エクスポート宣言:" << std::endl;
        std::cout << "   export class MyClass { ... };" << std::endl;
        std::cout << "   export void myFunction();" << std::endl;
        std::cout << "   export namespace my_namespace { ... }" << std::endl;
        std::cout << "   - モジュール外部に公開する要素を明示" << std::endl;
        
        std::cout << "\n5. プライベート実装:" << std::endl;
        std::cout << "   class InternalClass { ... };  // export なし" << std::endl;
        std::cout << "   void helperFunction() { ... }  // export なし" << std::endl;
        std::cout << "   - exportされていない要素は外部から見えない" << std::endl;
    }
}

// =======================================
// モジュールの利用パターン（概念的な例）
// =======================================

namespace module_usage_patterns
{
    // パターン1: 基本的なクラスのエクスポート
    class Player  // 実際のモジュールでは export class Player
    {
    private:
        std::string name_;
        int level_;
        int experience_;
        
    public:
        Player(const std::string& name) : name_(name), level_(1), experience_(0)
        {
            std::cout << "プレイヤー作成: " << name_ << std::endl;
        }
        
        void gainExperience(int exp)
        {
            experience_ += exp;
            std::cout << name_ << " が " << exp << " の経験値を獲得" << std::endl;
            
            // レベルアップ判定
            while (experience_ >= level_ * 100)
            {
                experience_ -= level_ * 100;
                level_++;
                std::cout << name_ << " がレベル " << level_ << " にアップ！" << std::endl;
            }
        }
        
        const std::string& getName() const { return name_; }
        int getLevel() const { return level_; }
        int getExperience() const { return experience_; }
    };
    
    // パターン2: 関数のエクスポート
    int calculateDamage(int attack, int defense)  // export int calculateDamage
    {
        int damage = std::max(1, attack - defense / 2);
        std::cout << "ダメージ計算: 攻撃力" << attack << " - 防御力" << defense << " = " << damage << std::endl;
        return damage;
    }
    
    // パターン3: 定数のエクスポート
    const int MAX_PLAYERS = 4;           // export const int MAX_PLAYERS
    const double GRAVITY = 9.81;         // export const double GRAVITY
    const std::string GAME_VERSION = "1.0.0";  // export const std::string GAME_VERSION
    
    // パターン4: テンプレートのエクスポート
    template<typename T>
    class GameContainer  // export template<typename T> class GameContainer
    {
    private:
        std::vector<T> items_;
        
    public:
        void add(const T& item)
        {
            items_.push_back(item);
            std::cout << "アイテム追加: 総数 " << items_.size() << std::endl;
        }
        
        void remove(size_t index)
        {
            if (index < items_.size())
            {
                items_.erase(items_.begin() + index);
                std::cout << "アイテム削除: 総数 " << items_.size() << std::endl;
            }
        }
        
        const T& get(size_t index) const
        {
            return items_.at(index);
        }
        
        size_t size() const { return items_.size(); }
        bool empty() const { return items_.empty(); }
    };
    
    // パターン5: 名前空間のエクスポート
    namespace graphics  // export namespace graphics
    {
        struct Color
        {
            float r, g, b, a;
            Color(float red, float green, float blue, float alpha = 1.0f)
                : r(red), g(green), b(blue), a(alpha) {}
        };
        
        void setBackgroundColor(const Color& color)
        {
            std::cout << "背景色設定: RGB(" << color.r << ", " << color.g << ", " << color.b << ")" << std::endl;
        }
        
        void drawRectangle(float x, float y, float width, float height, const Color& color)
        {
            std::cout << "矩形描画: (" << x << ", " << y << ") " << width << "x" << height << std::endl;
        }
    }
    
    // 内部実装（エクスポートされない）
    namespace internal  // この名前空間はexportされない
    {
        void debugLog(const std::string& message)
        {
            std::cout << "[INTERNAL] " << message << std::endl;
        }
        
        class SecretImplementation
        {
            // 外部に公開したくない実装詳細
        };
    }
}

// =======================================
// モジュールとヘッダーファイルの比較デモ
// =======================================

namespace comparison_demo
{
    void demonstrateTraditionalApproach()
    {
        std::cout << "\n=== 従来のアプローチ ===" << std::endl;
        
        // 従来の方式でゲームエンジンを使用
        traditional_approach::GameEngine engine("サンプルゲーム");
        engine.initialize();
        
        traditional_approach::logMessage("ゲーム開始");
        
        for (int frame = 0; frame < 3; ++frame)
        {
            engine.update();
            engine.render();
        }
        
        traditional_approach::logMessage("ゲーム終了");
        engine.shutdown();
        
        // ユーティリティ関数の使用
        double distance = traditional_approach::calculateDistance(0, 0, 3, 4);
        std::cout << "距離: " << distance << std::endl;
    }
    
    void demonstrateModuleApproach()
    {
        std::cout << "\n=== モジュールアプローチ（概念的）===" << std::endl;
        
        // モジュール方式でのゲーム要素使用
        module_usage_patterns::Player player("勇者");
        player.gainExperience(150);
        player.gainExperience(200);
        
        // ダメージ計算
        int damage = module_usage_patterns::calculateDamage(50, 20);
        
        // ゲームコンテナの使用
        module_usage_patterns::GameContainer<std::string> inventory;
        inventory.add("剣");
        inventory.add("盾");
        inventory.add("薬草");
        
        std::cout << "インベントリサイズ: " << inventory.size() << std::endl;
        std::cout << "最初のアイテム: " << inventory.get(0) << std::endl;
        
        // グラフィックス名前空間の使用
        module_usage_patterns::graphics::Color red(1.0f, 0.0f, 0.0f);
        module_usage_patterns::graphics::setBackgroundColor(red);
        module_usage_patterns::graphics::drawRectangle(10, 20, 100, 50, red);
        
        // 定数の使用
        std::cout << "最大プレイヤー数: " << module_usage_patterns::MAX_PLAYERS << std::endl;
        std::cout << "ゲームバージョン: " << module_usage_patterns::GAME_VERSION << std::endl;
    }
}

// =======================================
// モジュールのコンパイルフロー説明
// =======================================

namespace compilation_flow
{
    void explainCompilationFlow()
    {
        std::cout << "\n=== モジュールのコンパイルフロー ===" << std::endl;
        
        std::cout << "1. モジュール実装単位（module implementation unit）" << std::endl;
        std::cout << "   - .cpp または .cppm ファイル" << std::endl;
        std::cout << "   - 'export module モジュール名;' で開始" << std::endl;
        std::cout << "   - BMI（Binary Module Interface）を生成" << std::endl;
        
        std::cout << "\n2. モジュールインターフェース単位（module interface unit）" << std::endl;
        std::cout << "   - 他のモジュールが利用できる部分" << std::endl;
        std::cout << "   - exportキーワードで公開要素を指定" << std::endl;
        std::cout << "   - 実装詳細は隠蔽される" << std::endl;
        
        std::cout << "\n3. コンパイル順序" << std::endl;
        std::cout << "   a) モジュール実装単位をコンパイル" << std::endl;
        std::cout << "   b) BMIファイル（.bmi等）が生成される" << std::endl;
        std::cout << "   c) 依存するコードがBMIを利用してコンパイル" << std::endl;
        std::cout << "   d) リンク時に実装がリンクされる" << std::endl;
        
        std::cout << "\n4. 従来との違い" << std::endl;
        std::cout << "   - プリプロセッサ処理が不要" << std::endl;
        std::cout << "   - テンプレートの重複インスタンス化を回避" << std::endl;
        std::cout << "   - 型情報の保持によるより良いエラーメッセージ" << std::endl;
    }
}

// =======================================
// モジュール利用のベストプラクティス
// =======================================

namespace best_practices
{
    void explainBestPractices()
    {
        std::cout << "\n=== モジュール利用のベストプラクティス ===" << std::endl;
        
        std::cout << "1. モジュール設計原則:" << std::endl;
        std::cout << "   - 単一責任の原則：一つのモジュールは一つの責任" << std::endl;
        std::cout << "   - 最小公開の原則：必要な部分のみexport" << std::endl;
        std::cout << "   - 安定したインターフェース：APIの変更を最小化" << std::endl;
        
        std::cout << "\n2. 命名規則:" << std::endl;
        std::cout << "   - モジュール名は小文字とアンダースコア" << std::endl;
        std::cout << "   - 階層的な名前: game.engine, game.graphics" << std::endl;
        std::cout << "   - 組織名の接頭辞: company.product.module" << std::endl;
        
        std::cout << "\n3. ファイル構成:" << std::endl;
        std::cout << "   - モジュールファイル: .cppm または .ixx（実装依存）" << std::endl;
        std::cout << "   - 実装ファイル: .cpp" << std::endl;
        std::cout << "   - ディレクトリ構造でモジュール階層を反映" << std::endl;
        
        std::cout << "\n4. 依存関係管理:" << std::endl;
        std::cout << "   - 循環依存の回避" << std::endl;
        std::cout << "   - 前方宣言の活用" << std::endl;
        std::cout << "   - 依存関係の最小化" << std::endl;
        
        std::cout << "\n5. 移行戦略:" << std::endl;
        std::cout << "   - 段階的な移行：重要でないモジュールから開始" << std::endl;
        std::cout << "   - ヘッダーファイルとの共存期間を設ける" << std::endl;
        std::cout << "   - テストの充実でリファクタリングリスクを軽減" << std::endl;
    }
}

int main()
{
    std::cout << "=== C++20モジュールの基礎 ===" << std::endl;
    
    // 1. モジュール構文の説明
    module_syntax_explanation::explainModuleSyntax();
    
    // 2. 従来のアプローチとモジュールアプローチの比較
    comparison_demo::demonstrateTraditionalApproach();
    comparison_demo::demonstrateModuleApproach();
    
    // 3. コンパイルフローの説明
    compilation_flow::explainCompilationFlow();
    
    // 4. ベストプラクティス
    best_practices::explainBestPractices();
    
    std::cout << "\n=== モジュールの利点まとめ ===" << std::endl;
    std::cout << "- コンパイル時間の大幅短縮" << std::endl;
    std::cout << "- 強力なカプセル化と情報隠蔽" << std::endl;
    std::cout << "- マクロ汚染の防止" << std::endl;
    std::cout << "- より良いエラーメッセージ" << std::endl;
    std::cout << "- 依存関係の明確化" << std::endl;
    std::cout << "- ODR違反の防止" << std::endl;
    
    return 0;
}

// まとめ：
// - C++20モジュールは従来のヘッダーファイルシステムの問題を解決
// - exportキーワードで公開要素を明示的に制御
// - importで依存関係を明確に記述
// - BMI（Binary Module Interface）により高速コンパイルを実現
// - ゲーム開発では大規模プロジェクトでの恩恵が特に大きい
// - 段階的な移行とベストプラクティスの遵守が成功の鍵