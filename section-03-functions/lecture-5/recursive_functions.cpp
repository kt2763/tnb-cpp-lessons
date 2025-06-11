// 第5講: 再帰関数
// 自分自身を呼び出す再帰関数の仕組みと使い方を学ぶ

#include <iostream>
#include <string>
#include <vector>
#include <map>

// === 基本的な再帰関数 ===
// 階乗計算（n!）
int factorial(int n) 
{
    // ベースケース（終了条件）
    if (n <= 1) 
    {
        return 1;
    }
    
    // 再帰呼び出し
    return n * factorial(n - 1);
}

// フィボナッチ数列
int fibonacci(int n) 
{
    // ベースケース
    if (n <= 1) 
    {
        return n;
    }
    
    // 再帰呼び出し
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// === ゲーム開発での再帰関数の活用 ===
// ダンジョンの階層探索
struct DungeonFloor 
{
    int floorNumber;
    bool hasTreasure;
    bool hasStairs;
    std::string description;
};

bool exploreDungeon(int currentFloor, int targetFloor, bool& treasureFound) 
{
    std::cout << "階層 " << currentFloor << " を探索中..." << std::endl;
    
    // ベースケース：目標階層に到達
    if (currentFloor == targetFloor) 
    {
        std::cout << "目標階層に到達！ボスと遭遇！" << std::endl;
        return true;
    }
    
    // この階層の処理
    if (currentFloor == 3) 
    {
        treasureFound = true;
        std::cout << "  宝箱を発見！" << std::endl;
    }
    
    // 次の階層へ（再帰呼び出し）
    std::cout << "  階段を発見。次の階層へ..." << std::endl;
    return exploreDungeon(currentFloor + 1, targetFloor, treasureFound);
}

// スキルツリーの探索
struct SkillNode 
{
    std::string name;
    int requiredPoints;
    bool isUnlocked;
    std::vector<SkillNode*> children;
};

int countUnlockedSkills(SkillNode* node) 
{
    if (node == nullptr) 
    {
        return 0;
    }
    
    int count = 0;
    
    // 現在のノードがアンロックされているか
    if (node->isUnlocked) 
    {
        count = 1;
        std::cout << "  習得済み: " << node->name << std::endl;
    }
    
    // 子ノードを再帰的に探索
    for (SkillNode* child : node->children) 
    {
        count += countUnlockedSkills(child);
    }
    
    return count;
}

// インベントリの総価値計算（ネストしたコンテナ）
struct InventoryItem 
{
    std::string name;
    int value;
    std::vector<InventoryItem> contents;  // バッグや箱の中身
};

int calculateTotalValue(const InventoryItem& item) 
{
    int totalValue = item.value;
    
    // アイテムの中身も再帰的に計算
    for (const InventoryItem& content : item.contents) 
    {
        totalValue += calculateTotalValue(content);
    }
    
    return totalValue;
}

// 敵AIの行動決定木
struct AIDecisionNode 
{
    std::string condition;
    std::string action;
    AIDecisionNode* trueBranch;
    AIDecisionNode* falseBranch;
    
    AIDecisionNode(std::string cond, std::string act = "") 
        : condition(cond), action(act), trueBranch(nullptr), falseBranch(nullptr) {}
};

std::string makeDecision(AIDecisionNode* node, int playerHp, int enemyHp, int distance) 
{
    if (node == nullptr) 
    {
        return "待機";
    }
    
    // 葉ノード（アクション）に到達
    if (!node->action.empty()) 
    {
        return node->action;
    }
    
    // 条件を評価
    bool conditionResult = false;
    
    if (node->condition == "プレイヤーHP低い？") 
    {
        conditionResult = (playerHp < 30);
    }
    else if (node->condition == "自分のHP低い？") 
    {
        conditionResult = (enemyHp < 30);
    }
    else if (node->condition == "距離近い？") 
    {
        conditionResult = (distance < 5);
    }
    
    // 条件に応じて次のノードへ（再帰）
    if (conditionResult) 
    {
        return makeDecision(node->trueBranch, playerHp, enemyHp, distance);
    }
    else 
    {
        return makeDecision(node->falseBranch, playerHp, enemyHp, distance);
    }
}

// コンボ判定（パターンマッチング）
bool checkCombo(const std::vector<char>& input, const std::vector<char>& pattern, int index = 0) 
{
    // ベースケース：パターンの終端に到達
    if (index >= pattern.size()) 
    {
        return true;
    }
    
    // 入力が足りない
    if (index >= input.size()) 
    {
        return false;
    }
    
    // 現在の位置が一致するかチェック
    if (input[index] == pattern[index]) 
    {
        // 次の位置をチェック（再帰）
        return checkCombo(input, pattern, index + 1);
    }
    
    return false;
}

// クエスト達成条件の再帰的チェック
struct QuestCondition 
{
    std::string type;  // "AND", "OR", "ITEM", "KILL", etc.
    std::string target;
    int requiredCount;
    int currentCount;
    std::vector<QuestCondition*> subConditions;
};

bool checkQuestComplete(QuestCondition* condition) 
{
    if (condition == nullptr) 
    {
        return true;
    }
    
    // 基本条件のチェック
    if (condition->type == "ITEM" || condition->type == "KILL") 
    {
        return condition->currentCount >= condition->requiredCount;
    }
    
    // 複合条件（AND/OR）
    if (condition->type == "AND") 
    {
        for (QuestCondition* sub : condition->subConditions) 
        {
            if (!checkQuestComplete(sub)) 
            {
                return false;
            }
        }
        return true;
    }
    else if (condition->type == "OR") 
    {
        for (QuestCondition* sub : condition->subConditions) 
        {
            if (checkQuestComplete(sub)) 
            {
                return true;
            }
        }
        return false;
    }
    
    return false;
}

// メモ化を使った効率的な再帰（フィボナッチの改善版）
std::map<int, int> fibMemo;

int fibonacciMemo(int n) 
{
    // ベースケース
    if (n <= 1) 
    {
        return n;
    }
    
    // メモに既に計算結果がある場合
    if (fibMemo.find(n) != fibMemo.end()) 
    {
        return fibMemo[n];
    }
    
    // 計算してメモに保存
    fibMemo[n] = fibonacciMemo(n - 1) + fibonacciMemo(n - 2);
    return fibMemo[n];
}

int main() 
{
    std::cout << "=== 基本的な再帰関数 ===" << std::endl;
    
    // 階乗
    int n = 5;
    std::cout << n << "! = " << factorial(n) << std::endl;
    
    // フィボナッチ数列
    std::cout << "\nフィボナッチ数列（最初の10項）:" << std::endl;
    for (int i = 0; i < 10; i++) 
    {
        std::cout << fibonacci(i) << " ";
    }
    std::cout << std::endl;
    
    std::cout << "\n=== ゲーム開発での実践例 ===" << std::endl;
    
    // ダンジョン探索
    bool foundTreasure = false;
    std::cout << "ダンジョン探索開始！" << std::endl;
    exploreDungeon(1, 5, foundTreasure);
    if (foundTreasure) 
    {
        std::cout << "探索完了：宝を手に入れた！" << std::endl;
    }
    
    // スキルツリー
    std::cout << "\nスキルツリーの確認:" << std::endl;
    
    // スキルツリーの構築
    SkillNode* root = new SkillNode{"火魔法", 0, true};
    SkillNode* fire2 = new SkillNode{"ファイラ", 5, true};
    SkillNode* fire3 = new SkillNode{"ファイガ", 10, false};
    SkillNode* meteor = new SkillNode{"メテオ", 15, false};
    
    root->children.push_back(fire2);
    fire2->children.push_back(fire3);
    fire2->children.push_back(meteor);
    
    int unlockedCount = countUnlockedSkills(root);
    std::cout << "習得済みスキル数: " << unlockedCount << std::endl;
    
    // インベントリの価値計算
    std::cout << "\nインベントリの総価値計算:" << std::endl;
    
    InventoryItem bag{"冒険者のバッグ", 100};
    bag.contents.push_back({"ポーション", 50});
    bag.contents.push_back({"エリクサー", 200});
    
    InventoryItem treasureBox{"宝箱", 0};
    treasureBox.contents.push_back({"金貨", 1000});
    treasureBox.contents.push_back({"宝石", 500});
    
    bag.contents.push_back(treasureBox);
    
    int totalValue = calculateTotalValue(bag);
    std::cout << "総価値: " << totalValue << "G" << std::endl;
    
    // AI決定木
    std::cout << "\n敵AIの行動決定:" << std::endl;
    
    // 決定木の構築
    AIDecisionNode* aiRoot = new AIDecisionNode("プレイヤーHP低い？");
    AIDecisionNode* aggressive = new AIDecisionNode("距離近い？");
    AIDecisionNode* defensive = new AIDecisionNode("自分のHP低い？");
    
    aiRoot->trueBranch = aggressive;
    aiRoot->falseBranch = defensive;
    
    aggressive->trueBranch = new AIDecisionNode("", "必殺技");
    aggressive->falseBranch = new AIDecisionNode("", "接近");
    
    defensive->trueBranch = new AIDecisionNode("", "回復");
    defensive->falseBranch = new AIDecisionNode("", "通常攻撃");
    
    // 様々な状況での決定
    std::cout << "状況1 - " << makeDecision(aiRoot, 20, 80, 3) << std::endl;
    std::cout << "状況2 - " << makeDecision(aiRoot, 80, 20, 10) << std::endl;
    std::cout << "状況3 - " << makeDecision(aiRoot, 50, 50, 2) << std::endl;
    
    // コンボ判定
    std::cout << "\nコンボ入力判定:" << std::endl;
    std::vector<char> playerInput = {'A', 'A', 'B', 'A'};
    std::vector<char> comboPattern1 = {'A', 'A', 'B'};
    std::vector<char> comboPattern2 = {'A', 'B', 'A'};
    
    if (checkCombo(playerInput, comboPattern1)) 
    {
        std::cout << "火炎斬り発動！" << std::endl;
    }
    
    // メモ化の効果
    std::cout << "\nメモ化による高速化:" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    int fib40 = fibonacci(40);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    start = std::chrono::high_resolution_clock::now();
    int fib40Memo = fibonacciMemo(40);
    end = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "通常版: " << fib40 << " (" << duration1.count() << "ms)" << std::endl;
    std::cout << "メモ化版: " << fib40Memo << " (" << duration2.count() << "μs)" << std::endl;
    
    // メモリ解放
    delete fire3;
    delete meteor;
    delete fire2;
    delete root;
    delete aggressive->trueBranch;
    delete aggressive->falseBranch;
    delete defensive->trueBranch;
    delete defensive->falseBranch;
    delete aggressive;
    delete defensive;
    delete aiRoot;
    
    return 0;
}

/*
    まとめ:
    - 再帰関数は自分自身を呼び出す関数
    - 必ずベースケース（終了条件）が必要
    - 複雑な問題を簡潔に表現できる
    - スタックオーバーフローに注意
    
    ゲーム開発での活用：
    - ダンジョンやマップの探索
    - スキルツリーの処理
    - 階層構造を持つデータの処理
    - AI決定木の評価
    - パターンマッチング
    - クエスト条件の評価
    
    注意点：
    - 深い再帰はスタックオーバーフローの原因
    - 重複計算が多い場合はメモ化を検討
    - 単純なループで書ける場合は再帰を避ける
    - デバッグが難しくなることがある
    
    最適化テクニック：
    - メモ化（計算結果をキャッシュ）
    - 末尾再帰の最適化
    - 反復的な実装への書き換え
*/