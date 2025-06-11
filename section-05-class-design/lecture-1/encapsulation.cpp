// 第1講: カプセル化
// データと操作の隠蔽によるオブジェクトの整合性と安全性を学ぶ

#include <iostream>
#include <string>
#include <vector>

// === 悪い例：カプセル化されていないクラス ===
class BadPlayer 
{
public:  // 全てpublicなため、外部から自由に変更可能
    std::string name;
    int hp;
    int maxHp;
    int level;
    int experience;
    
    void displayStatus() 
    {
        std::cout << "名前: " << name << ", HP: " << hp << "/" << maxHp 
                  << ", レベル: " << level << ", 経験値: " << experience << std::endl;
    }
};

// === 良い例：適切にカプセル化されたクラス ===
class Player 
{
private:
    std::string name;
    int hp;
    int maxHp;
    int level;
    int experience;
    int gold;

public:
    // コンストラクタ
    Player(const std::string& playerName) 
        : name(playerName), hp(100), maxHp(100), level(1), experience(0), gold(500)
    {
        // 初期化時に整合性を保証
        validateStats();
    }
    
    // === 読み取り専用アクセス（ゲッター） ===
    const std::string& getName() const { return name; }
    int getHp() const { return hp; }
    int getMaxHp() const { return maxHp; }
    int getLevel() const { return level; }
    int getExperience() const { return experience; }
    int getGold() const { return gold; }
    
    // === 制御された書き込みアクセス（メソッド経由） ===
    bool setName(const std::string& newName) 
    {
        if (newName.empty()) 
        {
            std::cout << "エラー: 空の名前は設定できません" << std::endl;
            return false;
        }
        
        if (newName.length() > 20) 
        {
            std::cout << "エラー: 名前は20文字以下にしてください" << std::endl;
            return false;
        }
        
        name = newName;
        std::cout << "名前を「" << name << "」に変更しました" << std::endl;
        return true;
    }
    
    bool takeDamage(int damage) 
    {
        if (damage < 0) 
        {
            std::cout << "エラー: ダメージは0以上の値である必要があります" << std::endl;
            return false;
        }
        
        hp -= damage;
        if (hp < 0) 
        {
            hp = 0;
        }
        
        std::cout << name << "は" << damage << "のダメージを受けた！（HP: " << hp << "/" << maxHp << "）" << std::endl;
        
        if (hp == 0) 
        {
            std::cout << name << "は倒れてしまった..." << std::endl;
        }
        
        return true;
    }
    
    bool heal(int amount) 
    {
        if (amount < 0) 
        {
            std::cout << "エラー: 回復量は0以上の値である必要があります" << std::endl;
            return false;
        }
        
        if (hp == 0) 
        {
            std::cout << "エラー: 戦闘不能状態では回復できません" << std::endl;
            return false;
        }
        
        int oldHp = hp;
        hp += amount;
        if (hp > maxHp) 
        {
            hp = maxHp;
        }
        
        int actualHealing = hp - oldHp;
        std::cout << name << "は" << actualHealing << "回復した！（HP: " << hp << "/" << maxHp << "）" << std::endl;
        
        return true;
    }
    
    bool spendGold(int amount) 
    {
        if (amount < 0) 
        {
            std::cout << "エラー: 使用金額は0以上である必要があります" << std::endl;
            return false;
        }
        
        if (gold < amount) 
        {
            std::cout << "エラー: ゴールドが不足しています（所持金: " << gold << "G）" << std::endl;
            return false;
        }
        
        gold -= amount;
        std::cout << amount << "Gを使用しました（残金: " << gold << "G）" << std::endl;
        return true;
    }
    
    void earnGold(int amount) 
    {
        if (amount < 0) 
        {
            std::cout << "エラー: 獲得金額は0以上である必要があります" << std::endl;
            return;
        }
        
        gold += amount;
        std::cout << amount << "Gを獲得しました！（所持金: " << gold << "G）" << std::endl;
    }
    
    void gainExperience(int exp) 
    {
        if (exp < 0) 
        {
            std::cout << "エラー: 経験値は0以上である必要があります" << std::endl;
            return;
        }
        
        experience += exp;
        std::cout << name << "は" << exp << "の経験値を獲得！" << std::endl;
        
        // レベルアップ判定
        checkLevelUp();
    }
    
    void displayStatus() const 
    {
        std::cout << "=== " << name << "のステータス ===" << std::endl;
        std::cout << "レベル: " << level << std::endl;
        std::cout << "HP: " << hp << "/" << maxHp << std::endl;
        std::cout << "経験値: " << experience << std::endl;
        std::cout << "所持金: " << gold << "G" << std::endl;
        std::cout << "生存状態: " << (isAlive() ? "生存" : "戦闘不能") << std::endl;
        std::cout << "===============================" << std::endl;
    }
    
    bool isAlive() const 
    {
        return hp > 0;
    }
    
    // デバッグ用（開発時のみ使用）
    #ifdef DEBUG
    void debugSetHp(int newHp) 
    {
        hp = newHp;
        std::cout << "[DEBUG] HPを" << hp << "に設定しました" << std::endl;
    }
    #endif

private:
    // === プライベートメソッド（内部処理） ===
    void validateStats() 
    {
        // ステータスの整合性をチェック
        if (hp < 0) hp = 0;
        if (hp > maxHp) hp = maxHp;
        if (level < 1) level = 1;
        if (experience < 0) experience = 0;
        if (gold < 0) gold = 0;
    }
    
    void checkLevelUp() 
    {
        int expNeeded = level * 100;
        while (experience >= expNeeded) 
        {
            levelUp();
            expNeeded = level * 100;
        }
    }
    
    void levelUp() 
    {
        experience -= level * 100;
        level++;
        
        // ステータス向上
        int hpIncrease = 15 + level;  // レベルが上がるほど上昇量増加
        int oldMaxHp = maxHp;
        
        maxHp += hpIncrease;
        hp += hpIncrease;  // レベルアップ時は最大HP分回復
        
        std::cout << "★ レベルアップ！ " << name << "はレベル" << level << "になった！" << std::endl;
        std::cout << "  最大HPが" << hpIncrease << "上昇！（" << oldMaxHp << " → " << maxHp << "）" << std::endl;
        
        // レベルアップボーナス
        int goldBonus = level * 10;
        gold += goldBonus;
        std::cout << "  ボーナスとして" << goldBonus << "Gを獲得！" << std::endl;
    }
};

// === インベントリクラス（カプセル化の実例） ===
class Inventory 
{
private:
    std::vector<std::string> items;
    int maxCapacity;
    int currentWeight;
    int maxWeight;

public:
    Inventory(int capacity = 20, int weightLimit = 100) 
        : maxCapacity(capacity), currentWeight(0), maxWeight(weightLimit) {}
    
    bool addItem(const std::string& item, int weight = 1) 
    {
        // 容量チェック
        if (items.size() >= maxCapacity) 
        {
            std::cout << "エラー: インベントリが満杯です（" << items.size() << "/" << maxCapacity << "）" << std::endl;
            return false;
        }
        
        // 重量チェック
        if (currentWeight + weight > maxWeight) 
        {
            std::cout << "エラー: 重量制限を超えています（" << currentWeight + weight << "/" << maxWeight << "）" << std::endl;
            return false;
        }
        
        items.push_back(item);
        currentWeight += weight;
        
        std::cout << "「" << item << "」をインベントリに追加しました" << std::endl;
        return true;
    }
    
    bool removeItem(const std::string& item) 
    {
        for (auto it = items.begin(); it != items.end(); ++it) 
        {
            if (*it == item) 
            {
                items.erase(it);
                currentWeight -= 1;  // 簡略化：全アイテム重量1
                std::cout << "「" << item << "」をインベントリから削除しました" << std::endl;
                return true;
            }
        }
        
        std::cout << "エラー: 「" << item << "」は見つかりませんでした" << std::endl;
        return false;
    }
    
    bool hasItem(const std::string& item) const 
    {
        for (const auto& inventoryItem : items) 
        {
            if (inventoryItem == item) 
            {
                return true;
            }
        }
        return false;
    }
    
    void displayInventory() const 
    {
        std::cout << "=== インベントリ（" << items.size() << "/" << maxCapacity 
                  << "）重量：" << currentWeight << "/" << maxWeight << " ===" << std::endl;
        
        if (items.empty()) 
        {
            std::cout << "（空）" << std::endl;
        }
        else 
        {
            for (size_t i = 0; i < items.size(); ++i) 
            {
                std::cout << i + 1 << ". " << items[i] << std::endl;
            }
        }
        std::cout << "======================================" << std::endl;
    }
    
    int getItemCount() const { return items.size(); }
    int getCapacity() const { return maxCapacity; }
    int getCurrentWeight() const { return currentWeight; }
    int getMaxWeight() const { return maxWeight; }
    bool isFull() const { return items.size() >= maxCapacity; }
};

void demonstrateBadEncapsulation() 
{
    std::cout << "\n=== 悪い例：カプセル化なし ===" << std::endl;
    
    BadPlayer badPlayer;
    badPlayer.name = "悪い例";
    badPlayer.hp = 100;
    badPlayer.maxHp = 100;
    badPlayer.level = 1;
    badPlayer.experience = 0;
    
    badPlayer.displayStatus();
    
    // 問題：外部から直接変更でき、不正な状態にできてしまう
    badPlayer.hp = -50;         // 負のHP
    badPlayer.maxHp = 0;        // 最大HPが0
    badPlayer.level = -1;       // 負のレベル
    badPlayer.experience = -100;// 負の経験値
    
    std::cout << "\n外部から不正な値を設定した結果：" << std::endl;
    badPlayer.displayStatus();  // 不正な状態が表示される
}

void demonstrateGoodEncapsulation() 
{
    std::cout << "\n=== 良い例：適切なカプセル化 ===" << std::endl;
    
    Player goodPlayer("勇者");
    goodPlayer.displayStatus();
    
    std::cout << "\n不正な操作の試行：" << std::endl;
    
    // これらの操作は全て適切にエラーハンドリングされる
    goodPlayer.setName("");           // 空の名前
    goodPlayer.takeDamage(-10);       // 負のダメージ
    goodPlayer.heal(-5);              // 負の回復量
    goodPlayer.spendGold(1000);       // 所持金以上の支払い
    goodPlayer.gainExperience(-50);   // 負の経験値
    
    std::cout << "\n正常な操作：" << std::endl;
    goodPlayer.takeDamage(30);
    goodPlayer.heal(20);
    goodPlayer.spendGold(100);
    goodPlayer.earnGold(200);
    goodPlayer.gainExperience(150);  // レベルアップが発生
    
    goodPlayer.displayStatus();
}

int main() 
{
    std::cout << "=== カプセル化の重要性 ===" << std::endl;
    
    // カプセル化されていない問題のあるクラス
    demonstrateBadEncapsulation();
    
    // 適切にカプセル化されたクラス
    demonstrateGoodEncapsulation();
    
    std::cout << "\n=== インベントリシステムのカプセル化例 ===" << std::endl;
    
    Inventory playerInventory(5, 50);  // 容量5、重量制限50
    
    // 正常なアイテム追加
    playerInventory.addItem("ポーション", 2);
    playerInventory.addItem("薬草", 1);
    playerInventory.addItem("魔法の石", 5);
    playerInventory.displayInventory();
    
    // 制限のテスト
    playerInventory.addItem("重い武器", 50);  // 重量制限オーバー
    playerInventory.addItem("軽い剣", 10);
    playerInventory.addItem("盾", 15);
    playerInventory.addItem("鎧", 20);     // 容量制限到達
    playerInventory.addItem("兜", 5);      // 容量オーバー
    
    playerInventory.displayInventory();
    
    // アイテム検索と削除
    if (playerInventory.hasItem("薬草")) 
    {
        std::cout << "薬草を使用します" << std::endl;
        playerInventory.removeItem("薬草");
    }
    
    playerInventory.displayInventory();
    
    return 0;
}

/*
    まとめ - カプセル化の重要性:
    
    1. データの整合性保護:
       - 不正な値の設定を防ぐ
       - オブジェクトの状態を常に有効に保つ
       - ビジネスルールの強制
    
    2. インターフェースの明確化:
       - 外部から使用すべきメソッドが明確
       - 内部実装の詳細を隠蔽
       - 使いやすいAPIの提供
    
    3. 保守性の向上:
       - 内部実装の変更が外部に影響しない
       - バグの原因を特定しやすい
       - コードの理解が容易
    
    4. 拡張性の確保:
       - 新機能の追加が容易
       - 既存コードへの影響を最小化
       - テストしやすい構造
    
    ゲーム開発での活用:
    - プレイヤーステータスの管理
    - インベントリシステム
    - セーブデータの整合性
    - ゲームバランスの調整
    - チートの防止
    
    カプセル化のベストプラクティス:
    - メンバ変数はprivateに
    - 必要に応じてゲッター/セッターを提供
    - 入力値の検証を必ず行う
    - エラーハンドリングを適切に実装
    - constの正しい使用
*/