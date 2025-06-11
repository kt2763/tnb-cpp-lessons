// 第7講: 制御フローのネストと組み合わせ
// 複雑な制御構造の組み合わせとベストプラクティスを学ぶ

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

int main() 
{
    std::cout << "=== ネストした制御構造 ===" << std::endl;
    
    // if文とfor文の組み合わせ
    std::vector<int> playerScores = {850, 920, 780, 950, 890};
    int highScoreThreshold = 900;
    int bonusPoints = 100;
    
    std::cout << "スコア評価システム:" << std::endl;
    for (int i = 0; i < playerScores.size(); i++) 
    {
        std::cout << "プレイヤー" << i + 1 << ": " << playerScores[i] << "点 ";
        
        if (playerScores[i] >= highScoreThreshold) 
        {
            playerScores[i] += bonusPoints;
            std::cout << "→ ハイスコアボーナス! 新スコア: " << playerScores[i] << "点";
        }
        else 
        {
            std::cout << "→ 通常スコア";
        }
        std::cout << std::endl;
    }
    
    // switch文とfor文の組み合わせ
    enum class ItemType { Weapon, Armor, Consumable, KeyItem };
    
    struct Item 
    {
        std::string name;
        ItemType type;
        int value;
    };
    
    std::vector<Item> inventory = {
        {"鋼の剣", ItemType::Weapon, 500},
        {"ポーション", ItemType::Consumable, 50},
        {"魔法の鍵", ItemType::KeyItem, 0},
        {"鉄の鎧", ItemType::Armor, 300},
        {"エリクサー", ItemType::Consumable, 200}
    };
    
    std::cout << "\nアイテム分類:" << std::endl;
    int totalValue = 0;
    
    for (const Item& item : inventory) 
    {
        std::cout << item.name << " - ";
        
        switch (item.type) 
        {
            case ItemType::Weapon:
                std::cout << "武器";
                totalValue += item.value;
                break;
                
            case ItemType::Armor:
                std::cout << "防具";
                totalValue += item.value;
                break;
                
            case ItemType::Consumable:
                std::cout << "消耗品";
                totalValue += item.value / 2;  // 消耗品は半額評価
                break;
                
            case ItemType::KeyItem:
                std::cout << "重要アイテム（売却不可）";
                break;
        }
        
        if (item.type != ItemType::KeyItem) 
        {
            std::cout << " (価値: " << item.value << "G)";
        }
        std::cout << std::endl;
    }
    
    std::cout << "売却可能アイテムの総価値: " << totalValue << "G" << std::endl;
    
    std::cout << "\n=== ゲーム開発での実践例 ===" << std::endl;
    
    // 戦闘システムの複雑な制御
    struct Combatant 
    {
        std::string name;
        int hp;
        int maxHp;
        int attack;
        int speed;
        bool isPlayer;
        bool isDefending;
        std::vector<std::string> statusEffects;
    };
    
    std::vector<Combatant> battlefield = {
        {"勇者", 100, 100, 50, 80, true, false, {}},
        {"魔法使い", 70, 70, 70, 60, true, false, {"魔力増強"}},
        {"ゴブリン", 40, 40, 30, 70, false, false, {}},
        {"オーガ", 80, 80, 60, 40, false, false, {"激怒"}}
    };
    
    // 行動順の決定（速度順にソート）
    std::sort(battlefield.begin(), battlefield.end(), 
              [](const Combatant& a, const Combatant& b) { return a.speed > b.speed; });
    
    std::cout << "戦闘ターン処理:" << std::endl;
    
    for (Combatant& actor : battlefield) 
    {
        if (actor.hp <= 0) 
        {
            continue;  // 戦闘不能者はスキップ
        }
        
        std::cout << "\n" << actor.name << "の行動:" << std::endl;
        
        // ステータス効果の処理
        for (const std::string& effect : actor.statusEffects) 
        {
            if (effect == "毒") 
            {
                int poisonDamage = 5;
                actor.hp -= poisonDamage;
                std::cout << "  毒のダメージ: -" << poisonDamage << "HP" << std::endl;
                
                if (actor.hp <= 0) 
                {
                    std::cout << "  " << actor.name << "は倒れた！" << std::endl;
                    continue;
                }
            }
            else if (effect == "魔力増強") 
            {
                std::cout << "  魔力増強中（攻撃力1.5倍）" << std::endl;
            }
            else if (effect == "激怒") 
            {
                std::cout << "  激怒状態（攻撃力2倍、防御力半減）" << std::endl;
            }
        }
        
        // ターゲット選択
        Combatant* target = nullptr;
        
        if (actor.isPlayer) 
        {
            // プレイヤーは敵をターゲット
            for (Combatant& enemy : battlefield) 
            {
                if (!enemy.isPlayer && enemy.hp > 0) 
                {
                    target = &enemy;
                    break;
                }
            }
        }
        else 
        {
            // 敵はプレイヤーをターゲット
            for (Combatant& player : battlefield) 
            {
                if (player.isPlayer && player.hp > 0) 
                {
                    if (!player.isDefending || target == nullptr) 
                    {
                        target = &player;
                        if (!player.isDefending) break;  // 防御していないキャラを優先
                    }
                }
            }
        }
        
        // 攻撃処理
        if (target) 
        {
            int damage = actor.attack;
            
            // ステータス効果によるダメージ補正
            for (const std::string& effect : actor.statusEffects) 
            {
                if (effect == "魔力増強") damage = damage * 1.5;
                if (effect == "激怒") damage = damage * 2;
            }
            
            // 防御によるダメージ軽減
            if (target->isDefending) 
            {
                damage = damage / 2;
                std::cout << "  " << target->name << "は防御中！" << std::endl;
            }
            
            target->hp -= damage;
            std::cout << "  " << actor.name << "の攻撃！ " 
                      << target->name << "に" << damage << "のダメージ！" << std::endl;
            
            if (target->hp <= 0) 
            {
                std::cout << "  " << target->name << "を倒した！" << std::endl;
            }
        }
    }
    
    // クエスト進行システム
    std::map<std::string, bool> questFlags = {
        {"talked_to_elder", true},
        {"defeated_goblins", true},
        {"found_artifact", false},
        {"returned_to_elder", false}
    };
    
    int questStage = 0;
    
    std::cout << "\nクエスト進行チェック:" << std::endl;
    
    if (questFlags["talked_to_elder"]) 
    {
        questStage = 1;
        std::cout << "ステージ1: 長老と話した ✓" << std::endl;
        
        if (questFlags["defeated_goblins"]) 
        {
            questStage = 2;
            std::cout << "ステージ2: ゴブリンを倒した ✓" << std::endl;
            
            if (questFlags["found_artifact"]) 
            {
                questStage = 3;
                std::cout << "ステージ3: アーティファクトを発見した ✓" << std::endl;
                
                if (questFlags["returned_to_elder"]) 
                {
                    questStage = 4;
                    std::cout << "ステージ4: 長老に報告した ✓" << std::endl;
                    std::cout << "クエスト完了！" << std::endl;
                }
                else 
                {
                    std::cout << "次の目標: 長老のもとへ戻る" << std::endl;
                }
            }
            else 
            {
                std::cout << "次の目標: 洞窟でアーティファクトを探す" << std::endl;
            }
        }
        else 
        {
            std::cout << "次の目標: ゴブリンを5体倒す" << std::endl;
        }
    }
    else 
    {
        std::cout << "次の目標: 村の長老と話す" << std::endl;
    }
    
    // ネストレベルを減らすリファクタリング例
    std::cout << "\n=== リファクタリング例 ===" << std::endl;
    
    // 早期returnを使った可読性の向上
    auto checkQuestProgress = [](const std::map<std::string, bool>& flags) -> int 
    {
        if (!flags.at("talked_to_elder")) return 0;
        if (!flags.at("defeated_goblins")) return 1;
        if (!flags.at("found_artifact")) return 2;
        if (!flags.at("returned_to_elder")) return 3;
        return 4;  // 完了
    };
    
    int progress = checkQuestProgress(questFlags);
    std::vector<std::string> questMessages = {
        "村の長老と話す",
        "ゴブリンを5体倒す",
        "洞窟でアーティファクトを探す",
        "長老のもとへ戻る",
        "クエスト完了！"
    };
    
    std::cout << "現在の進行状況: " << questMessages[progress] << std::endl;
    
    return 0;
}

/*
    まとめ:
    - 制御構造のネストは避けられないが、深すぎると可読性が低下
    - 早期return、continue、関数分割でネストレベルを減らせる
    - 複雑な条件は、意味のある変数名や関数に抽出する
    - ゲーム開発では、戦闘システムやクエスト管理で複雑な制御が必要
    - switch文とif文、for文とif文など、適切な組み合わせを選ぶ
    - ステートマシンやデザインパターンで、より整理された実装も可能
    - コードの可読性と保守性を常に意識する
*/