/**
 * @file recursive_advanced.cpp
 * @brief 再帰ラムダと高度なテクニック
 * 
 * ラムダ式での再帰処理や高度なテクニックを学びます。
 * ゲーム開発では、ツリー構造のデータや複雑なアルゴリズムの実装に使用します。
 */

#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <map>
#include <memory>
#include <algorithm>
#include <random>
#include <cmath>

int main()
{
    std::cout << "=== 再帰ラムダの基礎 ===\n";
    
    // 1. std::functionを使用した再帰ラムダ
    std::function<int(int)> factorial;
    factorial = [&factorial](int n) -> int
    {
        if (n <= 1)
            return 1;
        return n * factorial(n - 1);
    };
    
    std::cout << "5! = " << factorial(5) << "\n";
    std::cout << "7! = " << factorial(7) << "\n";
    
    // 2. ゲームツリーのノード構造
    struct GameNode
    {
        std::string name;
        int value;
        std::vector<std::shared_ptr<GameNode>> children;
        
        GameNode(const std::string& n, int v) : name(n), value(v) {}
    };
    
    // ゲームツリーの作成（スキルツリーのような構造）
    auto root = std::make_shared<GameNode>("ルート", 0);
    
    auto warrior = std::make_shared<GameNode>("戦士", 10);
    auto mage = std::make_shared<GameNode>("魔法使い", 10);
    auto archer = std::make_shared<GameNode>("弓手", 10);
    
    // 戦士のスキル
    warrior->children.push_back(std::make_shared<GameNode>("剣術", 20));
    warrior->children.push_back(std::make_shared<GameNode>("防御術", 15));
    warrior->children[0]->children.push_back(std::make_shared<GameNode>("必殺技", 50));
    
    // 魔法使いのスキル
    mage->children.push_back(std::make_shared<GameNode>("火炎魔法", 30));
    mage->children.push_back(std::make_shared<GameNode>("氷魔法", 25));
    mage->children[0]->children.push_back(std::make_shared<GameNode>("メテオ", 80));
    
    root->children = {warrior, mage, archer};
    
    std::cout << "\n=== スキルツリーの処理 ===\n";
    
    // 3. ツリーの再帰的な探索ラムダ
    std::function<void(std::shared_ptr<GameNode>, int)> traverseTree;
    traverseTree = [&traverseTree](std::shared_ptr<GameNode> node, int depth)
    {
        if (!node) return;
        
        // インデントで階層を表現
        for (int i = 0; i < depth; ++i)
        {
            std::cout << "  ";
        }
        
        std::cout << node->name << " (コスト: " << node->value << ")\n";
        
        // 子ノードを再帰的に処理
        for (const auto& child : node->children)
        {
            traverseTree(child, depth + 1);
        }
    };
    
    traverseTree(root, 0);
    
    // 4. ツリーの合計値計算
    std::function<int(std::shared_ptr<GameNode>)> calculateTotalCost;
    calculateTotalCost = [&calculateTotalCost](std::shared_ptr<GameNode> node) -> int
    {
        if (!node) return 0;
        
        int total = node->value;
        for (const auto& child : node->children)
        {
            total += calculateTotalCost(child);
        }
        return total;
    };
    
    int totalCost = calculateTotalCost(root);
    std::cout << "\nスキルツリーの合計コスト: " << totalCost << "\n";
    
    // 5. 条件付き検索
    std::function<std::shared_ptr<GameNode>(std::shared_ptr<GameNode>, const std::string&)> findSkill;
    findSkill = [&findSkill](std::shared_ptr<GameNode> node, const std::string& skillName) -> std::shared_ptr<GameNode>
    {
        if (!node) return nullptr;
        
        if (node->name == skillName)
        {
            return node;
        }
        
        for (const auto& child : node->children)
        {
            auto result = findSkill(child, skillName);
            if (result)
            {
                return result;
            }
        }
        
        return nullptr;
    };
    
    auto foundSkill = findSkill(root, "必殺技");
    if (foundSkill)
    {
        std::cout << "\nスキル '必殺技' が見つかりました。コスト: " << foundSkill->value << "\n";
    }
    
    // 6. メモ化を使用した最適化ラムダ
    std::cout << "\n=== メモ化を使用した最適化 ===\n";
    
    // メモ化付きフィボナッチ数列
    std::map<int, long long> fibCache;
    std::function<long long(int)> fibonacciMemo;
    
    fibonacciMemo = [&fibCache, &fibonacciMemo](int n) -> long long
    {
        if (n <= 1) return n;
        
        // キャッシュにあるかチェック
        auto it = fibCache.find(n);
        if (it != fibCache.end())
        {
            return it->second;
        }
        
        // 計算してキャッシュに保存
        long long result = fibonacciMemo(n - 1) + fibonacciMemo(n - 2);
        fibCache[n] = result;
        return result;
    };
    
    // 大きな数でも高速に計算
    std::cout << "Fibonacci(40) = " << fibonacciMemo(40) << "\n";
    std::cout << "Fibonacci(50) = " << fibonacciMemo(50) << "\n";
    
    // 7. ゲームAIの意思決定ツリー
    std::cout << "\n=== AI意思決定システム ===\n";
    
    struct AIDecision
    {
        std::string action;
        int priority;
        std::function<bool()> condition;
        std::vector<std::shared_ptr<AIDecision>> alternatives;
    };
    
    // ゲーム状態
    int playerDistance = 5;
    int aiHealth = 60;
    int aiMana = 30;
    bool hasPotion = true;
    
    // AIの意思決定ツリーを構築
    auto attackDecision = std::make_shared<AIDecision>();
    attackDecision->action = "攻撃";
    attackDecision->priority = 80;
    attackDecision->condition = [&](){ return playerDistance <= 3 && aiHealth > 30; };
    
    auto magicDecision = std::make_shared<AIDecision>();
    magicDecision->action = "魔法攻撃";
    magicDecision->priority = 90;
    magicDecision->condition = [&](){ return playerDistance <= 5 && aiMana >= 20; };
    
    auto healDecision = std::make_shared<AIDecision>();
    healDecision->action = "回復";
    healDecision->priority = 100;
    healDecision->condition = [&](){ return aiHealth < 30 && hasPotion; };
    
    auto fleeDecision = std::make_shared<AIDecision>();
    fleeDecision->action = "逃走";
    fleeDecision->priority = 60;
    fleeDecision->condition = [&](){ return aiHealth < 20; };
    
    auto patrolDecision = std::make_shared<AIDecision>();
    patrolDecision->action = "巡回";
    patrolDecision->priority = 10;
    patrolDecision->condition = [&](){ return true; }; // デフォルト動作
    
    // ツリー構造を作成
    std::vector<std::shared_ptr<AIDecision>> decisions = 
    {
        healDecision, magicDecision, attackDecision, fleeDecision, patrolDecision
    };
    
    // AIの意思決定アルゴリズム
    std::function<std::string(const std::vector<std::shared_ptr<AIDecision>>&)> makeDecision;
    makeDecision = [&makeDecision](const std::vector<std::shared_ptr<AIDecision>>& options) -> std::string
    {
        // 優先度でソート
        auto sortedOptions = options;
        std::sort(sortedOptions.begin(), sortedOptions.end(), 
            [](const auto& a, const auto& b)
            {
                return a->priority > b->priority;
            });
        
        // 条件を満たす最初のオプションを選択
        for (const auto& option : sortedOptions)
        {
            if (option->condition())
            {
                return option->action;
            }
        }
        
        return "何もしない";
    };
    
    // 異なるシナリオでテスト
    std::cout << "シナリオ 1: プレイヤー近い、AIヘルス低い\n";
    std::cout << "AIの選択: " << makeDecision(decisions) << "\n";
    
    // 状態変更
    aiHealth = 80;
    playerDistance = 8;
    std::cout << "\nシナリオ 2: プレイヤー遠い、AIヘルス高い\n";
    std::cout << "AIの選択: " << makeDecision(decisions) << "\n";
    
    // 8. 高階関数ラムダ（ラムダを返すラムダ）
    std::cout << "\n=== 高階関数ラムダ ===\n";
    
    // ゲームバランシング用のカスタマイズ可能なダメージ計算関数を作成
    auto createDamageCalculator = [](double baseDamage, double critMultiplier, double levelBonus)
    {
        return [baseDamage, critMultiplier, levelBonus](int attackerLevel, bool isCritical, double defenseRatio)
        {
            double damage = baseDamage + (attackerLevel * levelBonus);
            if (isCritical)
            {
                damage *= critMultiplier;
            }
            damage *= (1.0 - defenseRatio);
            return std::max(1.0, damage); // 最低1ダメージ
        };
    };
    
    // 異なるキャラクター用のダメージ計算関数を作成
    auto warriorDamage = createDamageCalculator(50.0, 2.0, 3.0);
    auto mageDamage = createDamageCalculator(70.0, 2.5, 2.0);
    auto archerDamage = createDamageCalculator(40.0, 3.0, 2.5);
    
    int level = 10;
    bool critical = true;
    double defense = 0.2; // 20%防御
    
    std::cout << "レベル" << level << ", クリティカル, 防御" << (defense * 100) << "%\n";
    std::cout << "戦士のダメージ: " << static_cast<int>(warriorDamage(level, critical, defense)) << "\n";
    std::cout << "魔法使いのダメージ: " << static_cast<int>(mageDamage(level, critical, defense)) << "\n";
    std::cout << "弓手のダメージ: " << static_cast<int>(archerDamage(level, critical, defense)) << "\n";
    
    // 9. ラムダでの状態マシン実装
    std::cout << "\n=== 状態マシン ===\n";
    
    enum class GameState { Menu, Playing, Paused, GameOver };
    
    GameState currentState = GameState::Menu;
    
    // 状態遷移ロジックをラムダで実装
    std::map<GameState, std::function<GameState(const std::string&)>> stateHandlers;
    
    stateHandlers[GameState::Menu] = [](const std::string& input) -> GameState
    {
        std::cout << "メニュー状態: " << input << "\n";
        if (input == "start")
        {
            std::cout << "ゲーム開始！\n";
            return GameState::Playing;
        }
        return GameState::Menu;
    };
    
    stateHandlers[GameState::Playing] = [](const std::string& input) -> GameState
    {
        std::cout << "ゲーム中: " << input << "\n";
        if (input == "pause")
        {
            std::cout << "ゲーム一時停止\n";
            return GameState::Paused;
        }
        else if (input == "die")
        {
            std::cout << "ゲームオーバー！\n";
            return GameState::GameOver;
        }
        return GameState::Playing;
    };
    
    stateHandlers[GameState::Paused] = [](const std::string& input) -> GameState
    {
        std::cout << "一時停止中: " << input << "\n";
        if (input == "resume")
        {
            std::cout << "ゲーム再開\n";
            return GameState::Playing;
        }
        else if (input == "quit")
        {
            std::cout << "メニューに戻る\n";
            return GameState::Menu;
        }
        return GameState::Paused;
    };
    
    stateHandlers[GameState::GameOver] = [](const std::string& input) -> GameState
    {
        std::cout << "ゲームオーバー: " << input << "\n";
        if (input == "restart")
        {
            std::cout << "ゲームを再開始\n";
            return GameState::Playing;
        }
        else if (input == "menu")
        {
            std::cout << "メニューに戻る\n";
            return GameState::Menu;
        }
        return GameState::GameOver;
    };
    
    // 状態マシンのシミュレーション
    std::vector<std::string> inputs = {"start", "pause", "resume", "die", "restart", "pause", "quit"};
    
    for (const auto& input : inputs)
    {
        currentState = stateHandlers[currentState](input);
    }
    
    return 0;
}

/**
 * まとめ：
 * - std::functionで再帰ラムダを実現可能
 * - ツリー構造のデータ処理に再帰ラムダが有用
 * - メモ化でパフォーマンスを大幅改善可能
 * - 高階関数でカスタマイズ可能なロジックを作成
 * - 状態マシンやAIの意思決定システムに応用可能
 * - ゲームの複雑なロジックを簡潔に表現できる
 */