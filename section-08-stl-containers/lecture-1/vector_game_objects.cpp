/*
 * Lecture 1: std::vector - 動的配列とゲームオブジェクト管理
 * 
 * vectorは最も頻繁に使用されるSTLコンテナです。
 * ゲーム開発では、敵、アイテム、パーティクルなど
 * 動的に増減するオブジェクトの管理に最適です。
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

// ゲーム内の敵クラス
class Enemy
{
public:
    std::string name;
    int hp;
    int attackPower;
    
    Enemy(const std::string& n, int h, int ap)
        : name(n), hp(h), attackPower(ap)
    {
    }
};

// アイテムクラス
class Item
{
public:
    std::string name;
    int value;
    
    Item(const std::string& n, int v)
        : name(n), value(v)
    {
    }
};

int main()
{
    // 1. 基本的な使い方 - 敵の管理
    std::cout << "=== ベクターの基本操作 ===\n";
    
    std::vector<Enemy> enemies;  // 敵のリスト
    
    // 敵を追加（push_back）
    enemies.push_back(Enemy("スライム", 10, 5));
    enemies.push_back(Enemy("ゴブリン", 20, 8));
    enemies.push_back(Enemy("オーク", 30, 12));
    
    // C++11以降はemplace_backが効率的
    enemies.emplace_back("ドラゴン", 100, 50);
    
    std::cout << "敵の数: " << enemies.size() << "\n";
    
    // 2. アクセス方法
    std::cout << "\n=== 要素へのアクセス ===\n";
    
    // インデックスアクセス（境界チェックなし）
    std::cout << "最初の敵: " << enemies[0].name << "\n";
    
    // at()メソッド（境界チェックあり）
    try
    {
        std::cout << "2番目の敵: " << enemies.at(1).name << "\n";
    }
    catch (const std::out_of_range& e)
    {
        std::cout << "エラー: " << e.what() << "\n";
    }
    
    // イテレータを使った走査
    std::cout << "\n全ての敵:\n";
    for (const auto& enemy : enemies)
    {
        std::cout << "  " << enemy.name 
                  << " (HP: " << enemy.hp 
                  << ", 攻撃力: " << enemy.attackPower << ")\n";
    }
    
    // 3. 容量管理
    std::cout << "\n=== 容量管理 ===\n";
    
    std::vector<Item> inventory;
    inventory.reserve(50);  // 50個分の容量を予約
    
    std::cout << "インベントリのサイズ: " << inventory.size() << "\n";
    std::cout << "インベントリの容量: " << inventory.capacity() << "\n";
    
    // アイテムを追加
    inventory.emplace_back("ポーション", 50);
    inventory.emplace_back("エリクサー", 500);
    inventory.emplace_back("鉄の剣", 300);
    
    // 4. 削除操作
    std::cout << "\n=== 削除操作 ===\n";
    
    // HPが20以下の敵を削除（erase-removeイディオム）
    auto initialSize = enemies.size();
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const Enemy& e) { return e.hp <= 20; }),
        enemies.end()
    );
    
    std::cout << "弱い敵を削除: " << initialSize 
              << " -> " << enemies.size() << "\n";
    
    // 5. パフォーマンスを考慮した使用例
    std::cout << "\n=== パーティクルシステムの例 ===\n";
    
    struct Particle
    {
        float x, y;
        float vx, vy;
        float lifetime;
    };
    
    // 大量のパーティクルを管理
    std::vector<Particle> particles;
    particles.reserve(1000);  // 事前に容量確保で再配置を防ぐ
    
    // パーティクルを生成
    for (int i = 0; i < 100; ++i)
    {
        particles.push_back({0.0f, 0.0f, 
                           static_cast<float>(i), 
                           static_cast<float>(i * 2), 
                           1.0f});
    }
    
    // アクティブなパーティクルのみ保持
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const Particle& p) { return p.lifetime <= 0; }),
        particles.end()
    );
    
    std::cout << "アクティブなパーティクル数: " << particles.size() << "\n";
    
    // 6. 便利な操作
    std::cout << "\n=== その他の便利な操作 ===\n";
    
    std::vector<int> scores = {100, 250, 150, 300, 200};
    
    // ソート
    std::sort(scores.begin(), scores.end(), std::greater<int>());
    
    std::cout << "ハイスコアランキング:\n";
    for (size_t i = 0; i < scores.size(); ++i)
    {
        std::cout << "  " << (i + 1) << "位: " 
                  << scores[i] << "点\n";
    }
    
    // クリア
    particles.clear();  // 全要素削除（容量は保持）
    std::cout << "\nパーティクルをクリア後のサイズ: " 
              << particles.size() << "\n";
    std::cout << "容量: " << particles.capacity() << "\n";
    
    // shrink_to_fitで余分な容量を解放
    particles.shrink_to_fit();
    std::cout << "shrink_to_fit後の容量: " 
              << particles.capacity() << "\n";
    
    return 0;
}

/*
 * まとめ：
 * - vectorは動的配列で、ゲームオブジェクトの管理に最適
 * - push_back/emplace_backで要素を追加
 * - reserve()で事前に容量を確保してパフォーマンスを向上
 * - erase-removeイディオムで条件に合う要素を効率的に削除
 * - 連続したメモリ配置でキャッシュ効率が良い
 */