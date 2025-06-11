/**
 * @file generic_lambdas.cpp
 * @brief ジェネリックラムダ（C++14）
 * 
 * C++14で導入されたジェネリックラムダは、autoパラメータを使用して
 * テンプレートのように動作するラムダ式を作成できます。
 * ゲーム開発では、異なるタイプのデータを扱う汎用的な処理に便利です。
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <variant>
#include <type_traits>
#include <functional>

int main()
{
    std::cout << "=== ジェネリックラムダの基礎 ===\n";
    
    // 1. 基本的なジェネリックラムダ
    auto printer = [](const auto& value)
    {
        std::cout << "値: " << value << "\n";
    };
    
    // 異なるタイプで使用可能
    printer(42);              // int
    printer(3.14);            // double
    printer("ハローワールド");    // const char*
    printer(std::string("ゲーム")); // std::string
    
    // 2. ゲームデータの汎用的な最大値検索
    std::cout << "\n=== 汎用的な最大値検索 ===\n";
    
    auto findMax = [](const auto& container)
    {
        if (container.empty())
        {
            return typename std::decay_t<decltype(container)>::value_type{};
        }
        
        auto maxElement = *std::max_element(container.begin(), container.end());
        return maxElement;
    };
    
    std::vector<int> playerScores = {1200, 850, 2100, 1800, 950};
    std::vector<double> damageValues = {45.5, 120.8, 89.2, 156.7, 78.1};
    std::vector<std::string> playerNames = {"タロウ", "ハナコ", "ジロウ", "サクラ"};
    
    std::cout << "最高スコア: " << findMax(playerScores) << "\n";
    std::cout << "最大ダメージ: " << findMax(damageValues) << "\n";
    std::cout << "名前の最大値: " << findMax(playerNames) << "\n";
    
    // 3. 汎用的なフィルタリング関数
    std::cout << "\n=== 汎用的なフィルタリング ===\n";
    
    auto filterBy = [](const auto& container, auto predicate)
    {
        std::vector<typename std::decay_t<decltype(container)>::value_type> result;
        
        std::copy_if(container.begin(), container.end(), 
                     std::back_inserter(result), predicate);
        
        return result;
    };
    
    // 高スコアプレイヤーをフィルタリング
    auto highScores = filterBy(playerScores, [](const auto& score)
    {
        return score > 1000;
    });
    
    std::cout << "高スコア（1000以上）: ";
    for (const auto& score : highScores)
    {
        std::cout << score << " ";
    }
    std::cout << "\n";
    
    // 高ダメージをフィルタリング
    auto highDamage = filterBy(damageValues, [](const auto& damage)
    {
        return damage > 100.0;
    });
    
    std::cout << "高ダメージ（100以上）: ";
    for (const auto& damage : highDamage)
    {
        std::cout << damage << " ";
    }
    std::cout << "\n";
    
    // 4. ゲームリソースの汎用的な管理システム
    std::cout << "\n=== リソース管理システム ===\n";
    
    // リソースマネージャーのジェネリックラムダ
    auto resourceManager = [](auto& resources, const auto& resourceId, auto operation)
    {
        auto it = resources.find(resourceId);
        if (it != resources.end())
        {
            operation(it->second);
            return true;
        }
        else
        {
            std::cout << "リソース '" << resourceId << "' が見つかりません\n";
            return false;
        }
    };
    
    // 異なるタイプのリソースコンテナ
    std::map<std::string, int> playerStats = 
    {
        {"HP", 100},
        {"MP", 50},
        {"ATK", 75},
        {"DEF", 60}
    };
    
    std::map<std::string, std::string> playerEquipment = 
    {
        {"武器", "鉄の剣"},
        {"防具", "革の鎧"},
        {"アクセサリー", "魔力の指輪"}
    };
    
    // ステータスの更新
    resourceManager(playerStats, "HP", [](auto& hp)
    {
        hp -= 20;
        std::cout << "HPが20減少しました。現在のHP: " << hp << "\n";
    });
    
    resourceManager(playerStats, "MP", [](auto& mp)
    {
        mp += 10;
        std::cout << "MPが10回復しました。現在のMP: " << mp << "\n";
    });
    
    // 装備の変更
    resourceManager(playerEquipment, "武器", [](auto& weapon)
    {
        std::string oldWeapon = weapon;
        weapon = "ミスリルの剣";
        std::cout << "武器を '" << oldWeapon << "' から '" << weapon << "' に変更\n";
    });
    
    // 5. ゲームイベントの汎用的なハンドル
    std::cout << "\n=== イベントシステム ===\n";
    
    // 汎用的なイベントハンドラー
    auto eventHandler = [](const auto& eventData, auto... handlers)
    {
        std::cout << "イベント発生: ";
        
        // fold expression (C++17) を使用して全ハンドラーを実行
        (handlers(eventData), ...);
    };
    
    // 異なるタイプのイベントデータ
    struct LevelUpEvent
    {
        std::string playerName;
        int newLevel;
        int bonusPoints;
    };
    
    struct ItemFoundEvent
    {
        std::string itemName;
        int rarity;
        int value;
    };
    
    // レベルアップイベントの処理
    LevelUpEvent levelUp = {"タロウ", 15, 100};
    
    eventHandler(levelUp,
        [](const auto& event)
        {
            std::cout << event.playerName << "がLv." << event.newLevel << "にレベルアップ！\n";
        },
        [](const auto& event)
        {
            std::cout << "ボーナスポイント +" << event.bonusPoints << " 獲得！\n";
        },
        [](const auto& event)
        {
            if (event.newLevel % 5 == 0)
            {
                std::cout << "特別スキルをアンロック！\n";
            }
        });
    
    // アイテム発見イベントの処理
    ItemFoundEvent itemFound = {"ドラゴンの卵", 5, 10000};
    
    eventHandler(itemFound,
        [](const auto& event)
        {
            std::cout << "レアアイテム '" << event.itemName << "' を発見！\n";
        },
        [](const auto& event)
        {
            std::cout << "レア度: " << event.rarity << ", 価値: " << event.value << "\n";
        },
        [](const auto& event)
        {
            if (event.rarity >= 4)
            {
                std::cout << "ギルドに通知しましょう！\n";
            }
        });
    
    // 6. 汎用的なゲーヤニメーションシステム
    std::cout << "\n=== アニメーションシステム ===\n";
    
    // 汎用的な補間関数
    auto lerp = [](const auto& start, const auto& end, double t)
    {
        return start + (end - start) * t;
    };
    
    // 汎用的なアニメーション関数
    auto animate = [&lerp](const auto& startValue, const auto& endValue, 
                          int frames, auto updateCallback)
    {
        std::cout << "アニメーション開始: " << startValue << " → " << endValue << "\n";
        
        for (int i = 0; i <= frames; ++i)
        {
            double t = static_cast<double>(i) / frames;
            auto currentValue = lerp(startValue, endValue, t);
            updateCallback(i, currentValue);
        }
        
        std::cout << "アニメーション完了\n";
    };
    
    // 異なるタイプのアニメーション
    
    // 数値アニメーション（HPバー）
    animate(0, 100, 5, [](int frame, const auto& value)
    {
        std::cout << "Frame " << frame << ": HP = " << static_cast<int>(value) << "\n";
    });
    
    std::cout << "\n";
    
    // 小数点アニメーション（キャラクターの位置）
    animate(0.0, 10.0, 4, [](int frame, const auto& value)
    {
        std::cout << "Frame " << frame << ": Position X = " << value << "\n";
    });
    
    // 7. 汎用的なゲームコンフィグシステム
    std::cout << "\n=== コンフィグシステム ===\n";
    
    // 汎用的な設定管理
    auto configManager = [](auto& configMap, const auto& key, auto defaultValue, auto validator)
    {
        auto it = configMap.find(key);
        if (it != configMap.end())
        {
            if (validator(it->second))
            {
                return it->second;
            }
            else
            {
                std::cout << "無効な設定値: " << key << ", デフォルトを使用\n";
                return defaultValue;
            }
        }
        else
        {
            std::cout << "設定が見つかりません: " << key << ", デフォルトを使用\n";
            return defaultValue;
        }
    };
    
    // ゲーム設定
    std::map<std::string, int> gameSettings = 
    {
        {"volume", 80},
        {"difficulty", 2},
        {"maxPlayers", -1} // 無効な値
    };
    
    std::map<std::string, double> graphicsSettings = 
    {
        {"brightness", 0.8},
        {"contrast", 1.2}
    };
    
    // 設定の取得とバリデーション
    int volume = configManager(gameSettings, "volume", 50, [](const auto& value)
    {
        return value >= 0 && value <= 100;
    });
    
    int difficulty = configManager(gameSettings, "difficulty", 1, [](const auto& value)
    {
        return value >= 1 && value <= 3;
    });
    
    int maxPlayers = configManager(gameSettings, "maxPlayers", 4, [](const auto& value)
    {
        return value > 0 && value <= 8;
    });
    
    double brightness = configManager(graphicsSettings, "brightness", 1.0, [](const auto& value)
    {
        return value >= 0.1 && value <= 2.0;
    });
    
    std::cout << "音量: " << volume << "\n";
    std::cout << "難易度: " << difficulty << "\n";
    std::cout << "最大プレイヤー数: " << maxPlayers << "\n";
    std::cout << "明度: " << brightness << "\n";
    
    return 0;
}

/**
 * まとめ：
 * - ジェネリックラムダは auto パラメータでテンプレートのように動作
 * - 異なるタイプのデータを扱う汎用的な処理を実現
 * - コードの再利用性と保守性が向上
 * - ゲームのリソース管理、イベント処理、アニメーションに活用
 * - コンパイル時に型が決定されるためパフォーマンスも良好
 * - C++17のfold expressionと組み合わせてより強力に
 */