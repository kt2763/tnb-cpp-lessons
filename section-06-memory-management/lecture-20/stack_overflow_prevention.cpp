// スタックオーバーフロー防止技術
// ゲーム開発でのスタックメモリ管理と最適化

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <functional>
#include <thread>
#include <exception>
#include <cassert>
#include <csetjmp>
#include <csignal>

#ifdef _WIN32
#include <windows.h>
#include <excpt.h>
#elif defined(__unix__) || defined(__APPLE__)
#include <sys/resource.h>
#include <unistd.h>
#endif

using namespace std;
using namespace chrono;

// スタック使用量監視クラス
class StackMonitor
{
private:
    static thread_local void* stackBase;
    static thread_local size_t stackSize;
    static thread_local size_t maxUsage;
    
public:
    static void initialize()
    {
#ifdef _WIN32
        // Windows でのスタック情報取得
        NT_TIB* tib = (NT_TIB*)NtCurrentTeb();
        stackBase = tib->StackBase;
        stackSize = (char*)tib->StackBase - (char*)tib->StackLimit;
#elif defined(__unix__) || defined(__APPLE__)
        // Unix系でのスタック情報取得
        struct rlimit limit;
        getrlimit(RLIMIT_STACK, &limit);
        stackSize = limit.rlim_cur;
        
        // 近似的なスタックベース（実際の取得は複雑）
        char localVar;
        stackBase = &localVar;
#endif
        maxUsage = 0;
        cout << "スタック監視を初期化 (サイズ: " << stackSize << " bytes)\n";
    }
    
    static size_t getCurrentUsage()
    {
        char currentPos;
        size_t usage = abs((char*)stackBase - &currentPos);
        maxUsage = max(maxUsage, usage);
        return usage;
    }
    
    static size_t getMaxUsage()
    {
        return maxUsage;
    }
    
    static double getUsagePercentage()
    {
        return static_cast<double>(getCurrentUsage()) / stackSize * 100.0;
    }
    
    static void printStatus()
    {
        size_t current = getCurrentUsage();
        cout << "スタック使用量: " << current << " / " << stackSize 
             << " bytes (" << fixed << setprecision(1) << getUsagePercentage() << "%)\n";
        cout << "最大使用量: " << maxUsage << " bytes\n";
    }
    
    static bool isNearLimit(double threshold = 80.0)
    {
        return getUsagePercentage() > threshold;
    }
};

// スレッドローカル変数の定義
thread_local void* StackMonitor::stackBase = nullptr;
thread_local size_t StackMonitor::stackSize = 8 * 1024 * 1024; // デフォルト8MB
thread_local size_t StackMonitor::maxUsage = 0;

// スタックオーバーフロー検出とハンドリング
class StackOverflowDetector
{
private:
    static jmp_buf recoveryPoint;
    static bool inRecovery;
    
public:
    static void setupSignalHandler()
    {
#ifdef _WIN32
        // Windows では構造化例外処理を使用
        SetUnhandledExceptionFilter(windowsExceptionHandler);
#elif defined(__unix__) || defined(__APPLE__)
        // Unix系ではシグナルハンドラを使用
        signal(SIGSEGV, signalHandler);
        signal(SIGBUS, signalHandler);
#endif
        cout << "スタックオーバーフロー検出器を設定\n";
    }
    
    static bool setRecoveryPoint()
    {
        inRecovery = false;
        int result = setjmp(recoveryPoint);
        if (result != 0)
        {
            cout << "スタックオーバーフローから回復しました\n";
            return true; // オーバーフローから回復
        }
        return false; // 正常実行
    }
    
    static void triggerRecovery()
    {
        if (!inRecovery)
        {
            inRecovery = true;
            longjmp(recoveryPoint, 1);
        }
    }
    
private:
#ifdef _WIN32
    static LONG WINAPI windowsExceptionHandler(EXCEPTION_POINTERS* exceptionInfo)
    {
        if (exceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW)
        {
            cout << "Windows: スタックオーバーフローを検出\n";
            triggerRecovery();
            return EXCEPTION_EXECUTE_HANDLER;
        }
        return EXCEPTION_CONTINUE_SEARCH;
    }
#elif defined(__unix__) || defined(__APPLE__)
    static void signalHandler(int signal)
    {
        cout << "Unix: スタックオーバーフロー（シグナル " << signal << "）を検出\n";
        triggerRecovery();
    }
#endif
};

// 静的変数の定義
jmp_buf StackOverflowDetector::recoveryPoint;
bool StackOverflowDetector::inRecovery = false;

// 危険な再帰関数の例
namespace DangerousRecursion
{
    // 末尾再帰最適化されない再帰
    int badFactorial(int n)
    {
        StackMonitor::getCurrentUsage(); // 使用量更新
        
        if (StackMonitor::isNearLimit())
        {
            cout << "スタック使用量が限界に近づいています！\n";
            StackMonitor::printStatus();
            throw runtime_error("Stack near overflow");
        }
        
        if (n <= 1) return 1;
        
        // 局所変数を大量に作成（スタック圧迫）
        char buffer[1024]; // 1KB のローカル配列
        buffer[0] = 'x';   // 最適化防止
        
        return n * badFactorial(n - 1);
    }
    
    // 相互再帰での無限ループ
    void functionA(int depth);
    void functionB(int depth);
    
    void functionA(int depth)
    {
        cout << "Function A (depth: " << depth << ")\n";
        StackMonitor::getCurrentUsage();
        
        if (StackMonitor::isNearLimit(70.0))
        {
            cout << "相互再帰でスタック限界に接近\n";
            return;
        }
        
        char largeArray[2048]; // 2KB のローカル配列
        largeArray[0] = 'A';
        
        functionB(depth + 1);
    }
    
    void functionB(int depth)
    {
        cout << "Function B (depth: " << depth << ")\n";
        StackMonitor::getCurrentUsage();
        
        if (StackMonitor::isNearLimit(70.0))
        {
            cout << "相互再帰でスタック限界に接近\n";
            return;
        }
        
        char largeArray[2048]; // 2KB のローカル配列
        largeArray[0] = 'B';
        
        functionA(depth + 1);
    }
}

// 安全な再帰実装
namespace SafeRecursion
{
    // 末尾再帰最適化対応の階乗
    int safeFactorial(int n, int accumulator = 1)
    {
        if (n <= 1) return accumulator;
        return safeFactorial(n - 1, n * accumulator);
    }
    
    // 反復版への変換
    int iterativeFactorial(int n)
    {
        int result = 1;
        for (int i = 2; i <= n; ++i)
        {
            result *= i;
        }
        return result;
    }
    
    // スタック深度制限付き再帰
    int limitedDepthRecursion(int n, int maxDepth = 1000, int currentDepth = 0)
    {
        if (currentDepth >= maxDepth)
        {
            cout << "最大再帰深度 " << maxDepth << " に到達\n";
            throw runtime_error("Maximum recursion depth exceeded");
        }
        
        if (n <= 1) return 1;
        return n * limitedDepthRecursion(n - 1, maxDepth, currentDepth + 1);
    }
    
    // ヒープ使用の明示的スタック
    template<typename T>
    class ExplicitStack
    {
    private:
        vector<T> stack;
        
    public:
        void push(const T& value)
        {
            stack.push_back(value);
        }
        
        T pop()
        {
            if (stack.empty())
            {
                throw runtime_error("Stack underflow");
            }
            T value = stack.back();
            stack.pop_back();
            return value;
        }
        
        bool empty() const { return stack.empty(); }
        size_t size() const { return stack.size(); }
    };
    
    // 明示的スタックを使用した反復的な深度優先探索
    struct TreeNode
    {
        int value;
        vector<unique_ptr<TreeNode>> children;
        
        TreeNode(int val) : value(val) {}
        
        void addChild(int val)
        {
            children.push_back(make_unique<TreeNode>(val));
        }
    };
    
    void iterativeDFS(TreeNode* root)
    {
        if (!root) return;
        
        ExplicitStack<TreeNode*> nodeStack;
        nodeStack.push(root);
        
        cout << "反復的DFS: ";
        while (!nodeStack.empty())
        {
            TreeNode* current = nodeStack.pop();
            cout << current->value << " ";
            
            // 子ノードを逆順でスタックに追加（左から処理するため）
            for (auto it = current->children.rbegin(); it != current->children.rend(); ++it)
            {
                nodeStack.push(it->get());
            }
        }
        cout << "\n";
    }
    
    // 比較用の再帰的DFS
    void recursiveDFS(TreeNode* node, int depth = 0)
    {
        if (!node) return;
        
        if (depth > 100)
        {
            cout << "\n深度制限に到達: " << depth;
            return;
        }
        
        cout << node->value << " ";
        for (const auto& child : node->children)
        {
            recursiveDFS(child.get(), depth + 1);
        }
    }
}

// ゲーム開発でのスタック最適化例
namespace GameStackOptimization
{
    // ゲームエンティティの階層処理
    class GameEntity
    {
    public:
        string name;
        vector<unique_ptr<GameEntity>> children;
        bool active;
        
        GameEntity(const string& n) : name(n), active(true) {}
        
        void addChild(const string& childName)
        {
            children.push_back(make_unique<GameEntity>(childName));
        }
        
        // 再帰的更新（危険）
        void recursiveUpdate(float deltaTime, int depth = 0)
        {
            if (depth > 50)
            {
                cout << "エンティティ階層が深すぎます (depth: " << depth << ")\n";
                return;
            }
            
            if (!active) return;
            
            // 大きなローカル変数（スタック圧迫の原因）
            float transformMatrix[16];
            for (int i = 0; i < 16; ++i)
            {
                transformMatrix[i] = static_cast<float>(i) * deltaTime;
            }
            
            // 子エンティティの更新
            for (auto& child : children)
            {
                child->recursiveUpdate(deltaTime, depth + 1);
            }
        }
        
        // 反復的更新（安全）
        void iterativeUpdate(float deltaTime)
        {
            struct UpdateTask
            {
                GameEntity* entity;
                float deltaTime;
            };
            
            SafeRecursion::ExplicitStack<UpdateTask> taskStack;
            taskStack.push({this, deltaTime});
            
            while (!taskStack.empty())
            {
                UpdateTask task = taskStack.pop();
                GameEntity* entity = task.entity;
                
                if (!entity || !entity->active) continue;
                
                // 変換行列計算（ヒープ上で実行）
                auto transformMatrix = make_unique<float[]>(16);
                for (int i = 0; i < 16; ++i)
                {
                    transformMatrix[i] = static_cast<float>(i) * task.deltaTime;
                }
                
                // 子エンティティをタスクとして追加
                for (auto& child : entity->children)
                {
                    taskStack.push({child.get(), task.deltaTime});
                }
            }
        }
        
        void printHierarchy(int depth = 0) const
        {
            string indent(depth * 2, ' ');
            cout << indent << "- " << name << "\n";
            
            for (const auto& child : children)
            {
                child->printHierarchy(depth + 1);
            }
        }
    };
    
    // シーングラフの処理
    void processSceneGraph()
    {
        cout << "\n=== ゲームエンティティの階層処理 ===\n";
        
        auto root = make_unique<GameEntity>("Root");
        
        // 深い階層を作成
        GameEntity* current = root.get();
        for (int i = 0; i < 20; ++i)
        {
            current->addChild("Entity_" + to_string(i));
            current = current->children[0].get();
            
            // いくつかの子エンティティを追加
            if (i % 3 == 0)
            {
                current->addChild("Child_A_" + to_string(i));
                current->addChild("Child_B_" + to_string(i));
            }
        }
        
        cout << "エンティティ階層:\n";
        root->printHierarchy();
        
        StackMonitor::initialize();
        
        cout << "\n再帰的更新:\n";
        StackMonitor::getCurrentUsage();
        root->recursiveUpdate(0.016f);
        StackMonitor::printStatus();
        
        cout << "\n反復的更新:\n";
        size_t beforeUsage = StackMonitor::getCurrentUsage();
        root->iterativeUpdate(0.016f);
        size_t afterUsage = StackMonitor::getCurrentUsage();
        cout << "スタック使用量変化: " << beforeUsage << " -> " << afterUsage << " bytes\n";
    }
    
    // パーサーのスタック最適化
    class GameScriptParser
    {
    private:
        struct ParseNode
        {
            string type;
            string value;
            vector<unique_ptr<ParseNode>> children;
        };
        
    public:
        // 再帰下降パーサー（危険）
        unique_ptr<ParseNode> recursiveParse(const vector<string>& tokens, size_t& index, int depth = 0)
        {
            if (depth > 100)
            {
                throw runtime_error("Parser depth limit exceeded");
            }
            
            if (index >= tokens.size()) return nullptr;
            
            auto node = make_unique<ParseNode>();
            node->type = "node";
            node->value = tokens[index++];
            
            // 大きなローカル変数
            char parserBuffer[1024];
            snprintf(parserBuffer, sizeof(parserBuffer), "parsing_%s", node->value.c_str());
            
            // 子ノードの解析
            while (index < tokens.size() && tokens[index] != ")")
            {
                if (auto child = recursiveParse(tokens, index, depth + 1))
                {
                    node->children.push_back(move(child));
                }
            }
            
            return node;
        }
        
        // 反復パーサー（安全）
        unique_ptr<ParseNode> iterativeParse(const vector<string>& tokens)
        {
            struct ParseContext
            {
                ParseNode* parent;
                size_t tokenIndex;
                bool completed;
            };
            
            SafeRecursion::ExplicitStack<ParseContext> parseStack;
            auto root = make_unique<ParseNode>();
            root->type = "root";
            
            parseStack.push({root.get(), 0, false});
            
            while (!parseStack.empty())
            {
                ParseContext context = parseStack.pop();
                
                if (context.tokenIndex >= tokens.size() || context.completed)
                {
                    continue;
                }
                
                auto node = make_unique<ParseNode>();
                node->type = "node";
                node->value = tokens[context.tokenIndex];
                
                ParseNode* nodePtr = node.get();
                context.parent->children.push_back(move(node));
                
                // 次のトークンの処理をスタックに追加
                if (context.tokenIndex + 1 < tokens.size())
                {
                    parseStack.push({nodePtr, context.tokenIndex + 1, false});
                }
            }
            
            return root;
        }
    };
}

// スタックオーバーフローのデモとテスト
void demonstrateStackOverflow()
{
    cout << "\n=== スタックオーバーフローデモ ===\n";
    
    StackMonitor::initialize();
    StackOverflowDetector::setupSignalHandler();
    
    cout << "\n1. 安全な再帰の例:\n";
    try
    {
        int result = SafeRecursion::safeFactorial(10);
        cout << "安全な階乗 10! = " << result << "\n";
        StackMonitor::printStatus();
    }
    catch (const exception& e)
    {
        cout << "例外: " << e.what() << "\n";
    }
    
    cout << "\n2. 深度制限付き再帰の例:\n";
    try
    {
        int result = SafeRecursion::limitedDepthRecursion(100, 50);
        cout << "制限付き再帰 結果: " << result << "\n";
    }
    catch (const exception& e)
    {
        cout << "例外: " << e.what() << "\n";
    }
    
    cout << "\n3. 危険な再帰の例（制限付き）:\n";
    if (StackOverflowDetector::setRecoveryPoint())
    {
        cout << "回復処理が実行されました\n";
        StackMonitor::printStatus();
    }
    else
    {
        try
        {
            DangerousRecursion::badFactorial(50);
        }
        catch (const exception& e)
        {
            cout << "例外をキャッチ: " << e.what() << "\n";
            StackMonitor::printStatus();
        }
    }
    
    cout << "\n4. 相互再帰の例:\n";
    DangerousRecursion::functionA(0);
    
    cout << "\n5. 明示的スタックを使用したDFS:\n";
    auto root = make_unique<SafeRecursion::TreeNode>(1);
    root->addChild(2);
    root->addChild(3);
    root->children[0]->addChild(4);
    root->children[0]->addChild(5);
    root->children[1]->addChild(6);
    
    cout << "再帰的DFS: ";
    SafeRecursion::recursiveDFS(root.get());
    cout << "\n";
    
    SafeRecursion::iterativeDFS(root.get());
}

int main()
{
    cout << "=== スタックオーバーフロー防止技術 ===\n";
    
    // スタックオーバーフローのデモとテスト
    demonstrateStackOverflow();
    
    // ゲーム開発でのスタック最適化例
    GameStackOptimization::processSceneGraph();
    
    cout << "\n=== スタックオーバーフローの原因 ===\n";
    cout << "1. 深すぎる再帰呼び出し\n";
    cout << "2. 大きなローカル変数（配列、構造体）\n";
    cout << "3. 相互再帰による無限ループ\n";
    cout << "4. 末尾再帰最適化の未適用\n";
    cout << "5. スレッドスタックサイズの不足\n";
    
    cout << "\n=== 防止技術 ===\n";
    cout << "1. 反復アルゴリズムへの変換\n";
    cout << "2. 明示的スタック（ヒープ使用）の活用\n";
    cout << "3. 末尾再帰最適化の適用\n";
    cout << "4. 再帰深度の制限\n";
    cout << "5. スタック使用量の監視\n";
    cout << "6. 大きなローカル変数のヒープ移動\n";
    cout << "7. スレッドスタックサイズの増加\n";
    
    cout << "\n=== ゲーム開発での具体例 ===\n";
    cout << "1. シーングラフの階層処理: 反復的な走査アルゴリズム\n";
    cout << "2. パーサー実装: 明示的パーサースタックの使用\n";
    cout << "3. AI の決定木: 深度制限と反復処理\n";
    cout << "4. パス検索: A*アルゴリズムでの明示的優先度キュー\n";
    cout << "5. 物理シミュレーション: 階層的衝突検出の最適化\n";
    
    cout << "\n=== 設計指針 ===\n";
    cout << "1. 再帰の必要性を慎重に検討\n";
    cout << "2. スタック使用量を予測・測定\n";
    cout << "3. クリティカルパスでは反復アルゴリズムを優先\n";
    cout << "4. 深度制限とエラーハンドリングの実装\n";
    cout << "5. プロファイリングによる最適化ポイントの特定\n";
    
    cout << "\n=== デバッグ技術 ===\n";
    cout << "1. スタック使用量の実行時監視\n";
    cout << "2. コールスタックの可視化\n";
    cout << "3. 自動的なスタック制限チェック\n";
    cout << "4. シグナル/例外ハンドラでの安全な回復\n";
    cout << "5. 単体テストでの境界値テスト\n";
    
    return 0;
}

// スタックオーバーフロー防止の重要なポイント：
// 1. ゲーム開発では特にリアルタイム性が重要で、スタックオーバーフローは致命的
// 2. 再帰的アルゴリズムは理解しやすいが、反復版への変換を常に検討する
// 3. 大きなローカル変数はヒープに移動するか、サイズを制限する
// 4. 深い階層構造を扱う場合は必ず深度制限を設ける
// 5. スタック使用量の監視とプロファイリングを定期的に実行する