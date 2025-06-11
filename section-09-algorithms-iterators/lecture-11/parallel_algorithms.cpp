/*
 * C++モダンコース - セクション9: アルゴリズムとイテレータ
 * レクチャー11: 並列アルゴリズム（C++17）
 * 
 * このレクチャーでは、C++17で導入された並列実行ポリシーを学びます
 * ゲーム開発での大量データ処理、並列計算、パフォーマンス向上を実践します
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <execution>
#include <chrono>
#include <random>
#include <string>
#include <iomanip>
#include <thread>

// ゲームエンティティのデータ構造
struct GameEntity
{
    int entity_id;
    float x, y, z;  // 位置
    float health;
    float damage;
    bool is_active;
    
    GameEntity(int id, float px, float py, float pz, float hp, float dmg, bool active = true)
        : entity_id(id), x(px), y(py), z(pz), health(hp), damage(dmg), is_active(active) {}
    
    void update_physics(float delta_time)
    {
        // 重い計算をシミュレート
        for (int i = 0; i < 1000; ++i)
        {
            x += 0.001f * delta_time;
            y += 0.001f * delta_time;
        }
    }
    
    float distance_from_origin() const
    {
        return std::sqrt(x*x + y*y + z*z);
    }
    
    void apply_damage(float amount)
    {
        health -= amount;
        if (health <= 0.0f)
        {
            is_active = false;
            health = 0.0f;
        }
    }
};

// パフォーマンス測定ヘルパー
template<typename Func>
auto measure_time(Func&& func) -> std::chrono::milliseconds
{
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
}

void demonstrateParallelSort()
{
    std::cout << "\n=== 並列ソートの実例 ===" << std::endl;
    
    // 大量のゲームスコアデータを生成
    const size_t data_size = 1000000;
    std::vector<int> scores(data_size);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> score_dist(1000, 10000);
    
    // ランダムスコアを生成
    std::generate(scores.begin(), scores.end(),
        [&]() { return score_dist(gen); });
    
    std::cout << "生成されたスコア数: " << scores.size() << std::endl;
    std::cout << "サンプル (最初の10個): ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << scores[i] << " ";
    }
    std::cout << "..." << std::endl;
    
    // 1. 逐次ソート
    auto sequential_scores = scores;
    auto seq_time = measure_time([&]() 
    {
        std::sort(sequential_scores.begin(), sequential_scores.end());
    });
    
    std::cout << "\n逐次ソート時間: " << seq_time.count() << "ms" << std::endl;
    
    // 2. 並列ソート
    auto parallel_scores = scores;
    auto par_time = measure_time([&]() 
    {
        std::sort(std::execution::par, parallel_scores.begin(), parallel_scores.end());
    });
    
    std::cout << "並列ソート時間: " << par_time.count() << "ms" << std::endl;
    
    if (seq_time.count() > 0)
    {
        double speedup = static_cast<double>(seq_time.count()) / par_time.count();
        std::cout << "速度向上: " << std::fixed << std::setprecision(2) << speedup << "倍" << std::endl;
    }
    
    // 結果の検証
    bool results_match = std::equal(sequential_scores.begin(), sequential_scores.end(), 
                                   parallel_scores.begin());
    std::cout << "結果の一致: " << (results_match ? "はい" : "いいえ") << std::endl;
    
    std::cout << "ソート後サンプル: ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << parallel_scores[i] << " ";
    }
    std::cout << "..." << std::endl;
}

void demonstrateParallelForEach()
{
    std::cout << "\n=== 並列for_eachの実例 ===" << std::endl;
    
    // ゲームエンティティの大量生成
    const size_t entity_count = 100000;
    std::vector<GameEntity> entities;
    entities.reserve(entity_count);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> pos_dist(-100.0f, 100.0f);
    std::uniform_real_distribution<float> health_dist(50.0f, 200.0f);
    std::uniform_real_distribution<float> damage_dist(10.0f, 50.0f);
    
    for (size_t i = 0; i < entity_count; ++i)
    {
        entities.emplace_back(
            static_cast<int>(i),
            pos_dist(gen), pos_dist(gen), pos_dist(gen),
            health_dist(gen), damage_dist(gen)
        );
    }
    
    std::cout << "生成されたエンティティ数: " << entities.size() << std::endl;
    
    const float delta_time = 0.016f; // 60FPS想定
    
    // 1. 逐次物理更新
    auto entities_seq = entities;
    auto seq_physics_time = measure_time([&]() 
    {
        std::for_each(entities_seq.begin(), entities_seq.end(),
            [delta_time](GameEntity& entity) 
            {
                if (entity.is_active)
                {
                    entity.update_physics(delta_time);
                }
            });
    });
    
    std::cout << "\n逐次物理更新時間: " << seq_physics_time.count() << "ms" << std::endl;
    
    // 2. 並列物理更新
    auto entities_par = entities;
    auto par_physics_time = measure_time([&]() 
    {
        std::for_each(std::execution::par_unseq, entities_par.begin(), entities_par.end(),
            [delta_time](GameEntity& entity) 
            {
                if (entity.is_active)
                {
                    entity.update_physics(delta_time);
                }
            });
    });
    
    std::cout << "並列物理更新時間: " << par_physics_time.count() << "ms" << std::endl;
    
    if (seq_physics_time.count() > 0)
    {
        double physics_speedup = static_cast<double>(seq_physics_time.count()) / par_physics_time.count();
        std::cout << "物理更新速度向上: " << std::fixed << std::setprecision(2) << physics_speedup << "倍" << std::endl;
    }
    
    // 3. 並列ダメージ適用
    std::cout << "\n並列ダメージ処理:" << std::endl;
    const float area_damage = 25.0f;
    
    auto damage_time = measure_time([&]() 
    {
        std::for_each(std::execution::par, entities_par.begin(), entities_par.end(),
            [area_damage](GameEntity& entity) 
            {
                if (entity.is_active && entity.distance_from_origin() < 50.0f)
                {
                    entity.apply_damage(area_damage);
                }
            });
    });
    
    std::cout << "並列ダメージ処理時間: " << damage_time.count() << "ms" << std::endl;
    
    // アクティブエンティティ数をカウント
    auto active_count = std::count_if(std::execution::par, entities_par.begin(), entities_par.end(),
        [](const GameEntity& entity) { return entity.is_active; });
    
    std::cout << "生存エンティティ数: " << active_count << " / " << entity_count << std::endl;
}

void demonstrateParallelTransform()
{
    std::cout << "\n=== 並列transformの実例 ===" << std::endl;
    
    // プレイヤーの基本ステータス
    std::vector<int> base_attack = {45, 52, 38, 61, 49, 58, 43, 67, 55, 42};
    std::vector<float> level_multipliers = {1.0f, 1.2f, 0.8f, 1.5f, 1.1f, 1.3f, 0.9f, 1.6f, 1.4f, 1.0f};
    
    // 大量データのシミュレーション
    const size_t player_count = 1000000;
    std::vector<int> all_base_attacks(player_count);
    std::vector<float> all_multipliers(player_count);
    std::vector<int> final_attacks(player_count);
    
    // データ初期化
    for (size_t i = 0; i < player_count; ++i)
    {
        all_base_attacks[i] = base_attack[i % base_attack.size()];
        all_multipliers[i] = level_multipliers[i % level_multipliers.size()];
    }
    
    std::cout << "プレイヤー数: " << player_count << std::endl;
    std::cout << "基本攻撃力サンプル: ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << all_base_attacks[i] << " ";
    }
    std::cout << std::endl;
    
    // 1. 逐次変換
    auto seq_transform_time = measure_time([&]() 
    {
        std::transform(all_base_attacks.begin(), all_base_attacks.end(),
                      all_multipliers.begin(), final_attacks.begin(),
                      [](int base_atk, float multiplier) 
                      {
                          return static_cast<int>(base_atk * multiplier);
                      });
    });
    
    std::cout << "\n逐次transform時間: " << seq_transform_time.count() << "ms" << std::endl;
    
    // 結果のコピーを保存
    auto sequential_result = final_attacks;
    
    // 2. 並列変換
    auto par_transform_time = measure_time([&]() 
    {
        std::transform(std::execution::par_unseq,
                      all_base_attacks.begin(), all_base_attacks.end(),
                      all_multipliers.begin(), final_attacks.begin(),
                      [](int base_atk, float multiplier) 
                      {
                          return static_cast<int>(base_atk * multiplier);
                      });
    });
    
    std::cout << "並列transform時間: " << par_transform_time.count() << "ms" << std::endl;
    
    if (seq_transform_time.count() > 0)
    {
        double transform_speedup = static_cast<double>(seq_transform_time.count()) / par_transform_time.count();
        std::cout << "transform速度向上: " << std::fixed << std::setprecision(2) << transform_speedup << "倍" << std::endl;
    }
    
    // 結果の一致確認
    bool transform_match = std::equal(sequential_result.begin(), sequential_result.end(), 
                                     final_attacks.begin());
    std::cout << "結果の一致: " << (transform_match ? "はい" : "いいえ") << std::endl;
    
    std::cout << "最終攻撃力サンプル: ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << final_attacks[i] << " ";
    }
    std::cout << std::endl;
}

void demonstrateParallelReduction()
{
    std::cout << "\n=== 並列リダクション（reduce/transform_reduce）の実例 ===" << std::endl;
    
    // ゲーム内の経済データ
    const size_t transaction_count = 10000000;
    std::vector<int> transaction_amounts(transaction_count);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> amount_dist(-1000, 5000); // 負の値は支出
    
    // トランザクションデータ生成
    std::generate(std::execution::par, transaction_amounts.begin(), transaction_amounts.end(),
        [&]() { return amount_dist(gen); });
    
    std::cout << "トランザクション数: " << transaction_count << std::endl;
    std::cout << "サンプルデータ: ";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << transaction_amounts[i] << " ";
    }
    std::cout << "..." << std::endl;
    
    // 1. 逐次合計計算
    auto seq_reduce_time = measure_time([&]() 
    {
        int sequential_total = std::accumulate(transaction_amounts.begin(), transaction_amounts.end(), 0);
        std::cout << "\n逐次合計: " << sequential_total << std::endl;
    });
    
    std::cout << "逐次reduce時間: " << seq_reduce_time.count() << "ms" << std::endl;
    
    // 2. 並列合計計算
    int parallel_total;
    auto par_reduce_time = measure_time([&]() 
    {
        parallel_total = std::reduce(std::execution::par, 
                                   transaction_amounts.begin(), transaction_amounts.end(), 0);
    });
    
    std::cout << "並列合計: " << parallel_total << std::endl;
    std::cout << "並列reduce時間: " << par_reduce_time.count() << "ms" << std::endl;
    
    if (seq_reduce_time.count() > 0)
    {
        double reduce_speedup = static_cast<double>(seq_reduce_time.count()) / par_reduce_time.count();
        std::cout << "reduce速度向上: " << std::fixed << std::setprecision(2) << reduce_speedup << "倍" << std::endl;
    }
    
    // 3. 変換リダクション（税金計算）
    std::cout << "\n税金計算（正の値のみ10%）:" << std::endl;
    
    auto tax_time = measure_time([&]() 
    {
        int total_tax = std::transform_reduce(
            std::execution::par,
            transaction_amounts.begin(), transaction_amounts.end(),
            0,  // 初期値
            std::plus<>(),  // リダクション演算
            [](int amount) 
            {  // 変換関数
                return amount > 0 ? static_cast<int>(amount * 0.1) : 0;
            }
        );
        std::cout << "総税額: " << total_tax << std::endl;
    });
    
    std::cout << "税金計算時間: " << tax_time.count() << "ms" << std::endl;
    
    // 4. 収益分析
    auto profit_analysis_time = measure_time([&]() 
    {
        // 収入の合計
        int total_income = std::transform_reduce(
            std::execution::par,
            transaction_amounts.begin(), transaction_amounts.end(),
            0,
            std::plus<>(),
            [](int amount) { return amount > 0 ? amount : 0; }
        );
        
        // 支出の合計
        int total_expense = std::transform_reduce(
            std::execution::par,
            transaction_amounts.begin(), transaction_amounts.end(),
            0,
            std::plus<>(),
            [](int amount) { return amount < 0 ? -amount : 0; }
        );
        
        std::cout << "総収入: " << total_income << std::endl;
        std::cout << "総支出: " << total_expense << std::endl;
        std::cout << "純利益: " << (total_income - total_expense) << std::endl;
    });
    
    std::cout << "収益分析時間: " << profit_analysis_time.count() << "ms" << std::endl;
}

void demonstrateExecutionPolicies()
{
    std::cout << "\n=== 実行ポリシーの比較 ===" << std::endl;
    
    // テストデータの準備
    const size_t data_size = 1000000;
    std::vector<int> test_data(data_size);
    
    std::iota(test_data.begin(), test_data.end(), 1);
    std::random_shuffle(test_data.begin(), test_data.end());
    
    std::cout << "テストデータサイズ: " << data_size << std::endl;
    
    // 各実行ポリシーでのソート性能比較
    std::cout << "\nソート性能比較:" << std::endl;
    
    // 1. sequenced_policy
    auto seq_data = test_data;
    auto seq_policy_time = measure_time([&]() 
    {
        std::sort(std::execution::seq, seq_data.begin(), seq_data.end());
    });
    std::cout << "std::execution::seq: " << seq_policy_time.count() << "ms" << std::endl;
    
    // 2. parallel_policy
    auto par_data = test_data;
    auto par_policy_time = measure_time([&]() 
    {
        std::sort(std::execution::par, par_data.begin(), par_data.end());
    });
    std::cout << "std::execution::par: " << par_policy_time.count() << "ms" << std::endl;
    
    // 3. parallel_unsequenced_policy
    auto par_unseq_data = test_data;
    auto par_unseq_time = measure_time([&]() 
    {
        std::sort(std::execution::par_unseq, par_unseq_data.begin(), par_unseq_data.end());
    });
    std::cout << "std::execution::par_unseq: " << par_unseq_time.count() << "ms" << std::endl;
    
    // 性能分析
    std::cout << "\n性能分析:" << std::endl;
    if (seq_policy_time.count() > 0)
    {
        double par_speedup = static_cast<double>(seq_policy_time.count()) / par_policy_time.count();
        double par_unseq_speedup = static_cast<double>(seq_policy_time.count()) / par_unseq_time.count();
        
        std::cout << "並列化による速度向上: " << std::fixed << std::setprecision(2) << par_speedup << "倍" << std::endl;
        std::cout << "並列+ベクトル化による速度向上: " << std::fixed << std::setprecision(2) << par_unseq_speedup << "倍" << std::endl;
    }
    
    // CPU情報
    std::cout << "\nシステム情報:" << std::endl;
    std::cout << "論理CPU数: " << std::thread::hardware_concurrency() << std::endl;
}

void demonstrateGameScenarios()
{
    std::cout << "\n=== ゲーム開発での実践的な並列処理 ===" << std::endl;
    
    // 1. マップ生成の並列化
    std::cout << "1. 地形生成の並列化:" << std::endl;
    
    const size_t map_size = 1000;
    std::vector<std::vector<float>> heightmap(map_size, std::vector<float>(map_size));
    
    auto terrain_time = measure_time([&]() 
    {
        // 各行を並列で処理
        std::for_each(std::execution::par, heightmap.begin(), heightmap.end(),
            [map_size](std::vector<float>& row) 
            {
                for (size_t x = 0; x < map_size; ++x)
                {
                    // 複雑な地形生成アルゴリズムをシミュレート
                    float height = 0.0f;
                    for (int octave = 0; octave < 8; ++octave)
                    {
                        height += std::sin(x * 0.01f * (1 << octave)) * (1.0f / (1 << octave));
                    }
                    row[x] = height;
                }
            });
    });
    
    std::cout << "地形生成時間: " << terrain_time.count() << "ms" << std::endl;
    
    // 2. AIの意思決定並列化
    std::cout << "\n2. AI意思決定の並列化:" << std::endl;
    
    struct AIAgent
    {
        int agent_id;
        float x, y;
        float health;
        int decision;
        
        AIAgent(int id, float px, float py, float hp) 
            : agent_id(id), x(px), y(py), health(hp), decision(0) {}
        
        void make_decision()
        {
            // 複雑なAI計算をシミュレート
            decision = 0;
            for (int i = 0; i < 1000; ++i)
            {
                decision += static_cast<int>(std::sin(x + y + i) * 100);
            }
            decision = decision % 4; // 4つの行動から選択
        }
    };
    
    const size_t agent_count = 10000;
    std::vector<AIAgent> ai_agents;
    ai_agents.reserve(agent_count);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> pos_dist(0.0f, 1000.0f);
    std::uniform_real_distribution<float> health_dist(50.0f, 100.0f);
    
    for (size_t i = 0; i < agent_count; ++i)
    {
        ai_agents.emplace_back(static_cast<int>(i), pos_dist(gen), pos_dist(gen), health_dist(gen));
    }
    
    auto ai_time = measure_time([&]() 
    {
        std::for_each(std::execution::par, ai_agents.begin(), ai_agents.end(),
            [](AIAgent& agent) 
            {
                agent.make_decision();
            });
    });
    
    std::cout << "AI意思決定時間(" << agent_count << "エージェント): " << ai_time.count() << "ms" << std::endl;
    
    // 意思決定の分布を表示
    std::vector<int> decision_counts(4, 0);
    for (const auto& agent : ai_agents)
    {
        decision_counts[agent.decision]++;
    }
    
    std::cout << "意思決定分布: ";
    for (size_t i = 0; i < decision_counts.size(); ++i)
    {
        std::cout << "行動" << i << ":" << decision_counts[i] << " ";
    }
    std::cout << std::endl;
    
    // 3. 粒子システムの並列更新
    std::cout << "\n3. 粒子システムの並列化:" << std::endl;
    
    struct Particle
    {
        float x, y, z;
        float vx, vy, vz;
        float life;
        
        Particle() : x(0), y(0), z(0), vx(0), vy(0), vz(0), life(1.0f) {}
        
        void update(float dt)
        {
            x += vx * dt;
            y += vy * dt;
            z += vz * dt;
            life -= dt * 0.1f;
            
            // 重力の適用
            vy -= 9.8f * dt;
        }
    };
    
    const size_t particle_count = 100000;
    std::vector<Particle> particles(particle_count);
    
    // 粒子の初期化
    std::for_each(std::execution::par, particles.begin(), particles.end(),
        [&gen](Particle& p) mutable 
        {
            std::uniform_real_distribution<float> init_dist(-10.0f, 10.0f);
            p.x = init_dist(gen);
            p.y = init_dist(gen);
            p.z = init_dist(gen);
            p.vx = init_dist(gen);
            p.vy = init_dist(gen);
            p.vz = init_dist(gen);
        });
    
    const float delta_time = 0.016f;
    auto particle_time = measure_time([&]() 
    {
        std::for_each(std::execution::par_unseq, particles.begin(), particles.end(),
            [delta_time](Particle& p) 
            {
                p.update(delta_time);
            });
    });
    
    std::cout << "粒子更新時間(" << particle_count << "粒子): " << particle_time.count() << "ms" << std::endl;
    
    // アクティブな粒子数をカウント
    auto active_particles = std::count_if(std::execution::par, particles.begin(), particles.end(),
        [](const Particle& p) { return p.life > 0.0f; });
    
    std::cout << "アクティブ粒子数: " << active_particles << " / " << particle_count << std::endl;
}

int main()
{
    std::cout << "C++アルゴリズムとイテレータ - レクチャー11: 並列アルゴリズム（C++17）" << std::endl;
    std::cout << "=================================================================" << std::endl;
    
    // 各並列アルゴリズムの実演
    demonstrateParallelSort();
    demonstrateParallelForEach();
    demonstrateParallelTransform();
    demonstrateParallelReduction();
    demonstrateExecutionPolicies();
    demonstrateGameScenarios();
    
    std::cout << "\n=== まとめ ===" << std::endl;
    std::cout << "1. 実行ポリシー:" << std::endl;
    std::cout << "   - std::execution::seq: 逐次実行" << std::endl;
    std::cout << "   - std::execution::par: 並列実行" << std::endl;
    std::cout << "   - std::execution::par_unseq: 並列+ベクトル化実行" << std::endl;
    std::cout << "2. 並列対応アルゴリズム:" << std::endl;
    std::cout << "   - sort, for_each, transform, reduce, find など" << std::endl;
    std::cout << "3. ゲーム開発での活用場面:" << std::endl;
    std::cout << "   - 大量エンティティの更新" << std::endl;
    std::cout << "   - 地形生成・粒子システム" << std::endl;
    std::cout << "   - AI計算・物理演算" << std::endl;
    std::cout << "注意: 並列化にはオーバーヘッドがあるため、小さなデータでは逆効果の場合も" << std::endl;
    
    return 0;
}