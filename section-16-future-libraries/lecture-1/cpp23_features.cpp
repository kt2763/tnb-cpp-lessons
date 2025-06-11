// C++23の新機能
// 次世代C++標準の新機能とゲーム開発での活用

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <ranges>
#include <algorithm>
#include <format>
#include <memory>
#include <functional>
#include <type_traits>

// 注意：この実装はC++23機能の概念実装です
// 一部の機能は実際のコンパイラではまだ利用できない可能性があります

// =======================================
// C++23の概要
// =======================================

namespace cpp23_overview
{
    void explainCpp23Features()
    {
        std::cout << "=== C++23の新機能概要 ===" << std::endl;
        
        std::cout << "\n1. 主要な新機能カテゴリ" << std::endl;
        std::cout << "   - std::expected: エラーハンドリングの改善" << std::endl;
        std::cout << "   - std::flat_map/flat_set: 効率的な連想コンテナ" << std::endl;
        std::cout << "   - std::mdspan: 多次元配列ビュー" << std::endl;
        std::cout << "   - Ranges機能の拡張" << std::endl;
        std::cout << "   - constexpr の拡張" << std::endl;
        std::cout << "   - コンセプトの改善" << std::endl;
        
        std::cout << "\n2. ゲーム開発への影響" << std::endl;
        std::cout << "   - エラーハンドリングの簡素化" << std::endl;
        std::cout << "   - パフォーマンスの向上" << std::endl;
        std::cout << "   - 数値計算の効率化" << std::endl;
        std::cout << "   - より表現力豊かなコード" << std::endl;
        
        std::cout << "\n3. 採用タイムライン" << std::endl;
        std::cout << "   - 標準化: 2023年" << std::endl;
        std::cout << "   - 主要コンパイラ対応: 2024-2025年" << std::endl;
        std::cout << "   - 実用段階: 2025-2026年" << std::endl;
    }
}

// =======================================
// std::expected の概念実装
// =======================================

namespace expected_concept
{
    // std::expected の概念実装（実際のstd::expectedが利用可能になるまで）
    template<typename T, typename E>
    class expected
    {
    private:
        union
        {
            T value_;
            E error_;
        };
        bool has_value_;
        
    public:
        // コンストラクタ
        expected(const T& value) : value_(value), has_value_(true) {}
        expected(const E& error) : error_(error), has_value_(false) {}
        
        // デストラクタ
        ~expected()
        {
            if (has_value_)
            {
                value_.~T();
            }
            else
            {
                error_.~E();
            }
        }
        
        // コピー/ムーブ
        expected(const expected& other) : has_value_(other.has_value_)
        {
            if (has_value_)
            {
                new (&value_) T(other.value_);
            }
            else
            {
                new (&error_) E(other.error_);
            }
        }
        
        expected(expected&& other) noexcept : has_value_(other.has_value_)
        {
            if (has_value_)
            {
                new (&value_) T(std::move(other.value_));
            }
            else
            {
                new (&error_) E(std::move(other.error_));
            }
        }
        
        // 状態チェック
        bool has_value() const noexcept { return has_value_; }
        explicit operator bool() const noexcept { return has_value_; }
        
        // 値の取得
        const T& value() const&
        {
            if (!has_value_)
            {
                throw std::runtime_error("expected has no value");
            }
            return value_;
        }
        
        T& value() &
        {
            if (!has_value_)
            {
                throw std::runtime_error("expected has no value");
            }
            return value_;
        }
        
        const T& operator*() const& { return value(); }
        T& operator*() & { return value(); }
        
        // エラーの取得
        const E& error() const&
        {
            if (has_value_)
            {
                throw std::runtime_error("expected has value, not error");
            }
            return error_;
        }
        
        // 値またはデフォルト
        template<typename U>
        T value_or(U&& default_value) const&
        {
            return has_value_ ? value_ : static_cast<T>(std::forward<U>(default_value));
        }
        
        // モナド的操作
        template<typename F>
        auto and_then(F&& f) const
        {
            using Result = std::invoke_result_t<F, const T&>;
            
            if (has_value_)
            {
                return std::invoke(std::forward<F>(f), value_);
            }
            else
            {
                return Result(error_);
            }
        }
        
        template<typename F>
        auto transform(F&& f) const
        {
            using U = std::invoke_result_t<F, const T&>;
            using Result = expected<U, E>;
            
            if (has_value_)
            {
                return Result(std::invoke(std::forward<F>(f), value_));
            }
            else
            {
                return Result(error_);
            }
        }
    };
    
    // ヘルパー関数
    template<typename E>
    struct unexpected
    {
        E error;
        explicit unexpected(const E& e) : error(e) {}
        explicit unexpected(E&& e) : error(std::move(e)) {}
    };
    
    template<typename E>
    auto make_unexpected(E&& error)
    {
        return unexpected<std::decay_t<E>>(std::forward<E>(error));
    }
    
    // ゲーム開発での使用例
    enum class GameError
    {
        FileNotFound,
        InvalidFormat,
        OutOfMemory,
        NetworkError,
        GraphicsError
    };
    
    std::string errorToString(GameError error)
    {
        switch (error)
        {
            case GameError::FileNotFound: return "ファイルが見つかりません";
            case GameError::InvalidFormat: return "無効なフォーマット";
            case GameError::OutOfMemory: return "メモリ不足";
            case GameError::NetworkError: return "ネットワークエラー";
            case GameError::GraphicsError: return "グラフィックスエラー";
            default: return "不明なエラー";
        }
    }
    
    // リソース読み込み関数
    expected<std::string, GameError> loadTextFile(const std::string& filename)
    {
        std::cout << "ファイル読み込み試行: " << filename << std::endl;
        
        // ファイル名に応じてシミュレート
        if (filename.find("missing") != std::string::npos)
        {
            return make_unexpected(GameError::FileNotFound);
        }
        
        if (filename.find("corrupt") != std::string::npos)
        {
            return make_unexpected(GameError::InvalidFormat);
        }
        
        return std::string("ファイル内容: " + filename);
    }
    
    // 設定の解析
    expected<int, GameError> parseInteger(const std::string& content)
    {
        std::cout << "整数解析試行: " << content << std::endl;
        
        if (content.find("invalid") != std::string::npos)
        {
            return make_unexpected(GameError::InvalidFormat);
        }
        
        return 42;  // サンプル値
    }
    
    // チェーン処理の例
    expected<int, GameError> loadAndParseConfig(const std::string& filename)
    {
        return loadTextFile(filename)
            .and_then([](const std::string& content) {
                return parseInteger(content);
            });
    }
    
    void demonstrateExpected()
    {
        std::cout << "\n=== std::expected のデモンストレーション ===" << std::endl;
        
        // 成功ケース
        auto result1 = loadTextFile("config.txt");
        if (result1)
        {
            std::cout << "成功: " << *result1 << std::endl;
        }
        
        // 失敗ケース
        auto result2 = loadTextFile("missing.txt");
        if (!result2)
        {
            std::cout << "失敗: " << errorToString(result2.error()) << std::endl;
        }
        
        // チェーン処理
        auto result3 = loadAndParseConfig("config.txt");
        if (result3)
        {
            std::cout << "設定値: " << *result3 << std::endl;
        }
        else
        {
            std::cout << "設定読み込み失敗: " << errorToString(result3.error()) << std::endl;
        }
        
        // 変換処理
        auto result4 = loadTextFile("data.txt")
            .transform([](const std::string& content) {
                return content.length();
            });
        
        if (result4)
        {
            std::cout << "ファイルサイズ: " << *result4 << " 文字" << std::endl;
        }
        
        // デフォルト値の使用
        auto result5 = loadTextFile("missing.txt");
        std::string content = result5.value_or("デフォルトコンテンツ");
        std::cout << "取得したコンテンツ: " << content << std::endl;
    }
}

// =======================================
// std::flat_map の概念実装
// =======================================

namespace flat_map_concept
{
    // std::flat_map の簡易実装
    template<typename Key, typename Value, typename Compare = std::less<Key>>
    class flat_map
    {
    private:
        std::vector<std::pair<Key, Value>> data_;
        Compare comp_;
        
        auto find_position(const Key& key)
        {
            return std::lower_bound(data_.begin(), data_.end(), key,
                [this](const auto& pair, const Key& k) {
                    return comp_(pair.first, k);
                });
        }
        
        auto find_position(const Key& key) const
        {
            return std::lower_bound(data_.begin(), data_.end(), key,
                [this](const auto& pair, const Key& k) {
                    return comp_(pair.first, k);
                });
        }
        
    public:
        using iterator = typename std::vector<std::pair<Key, Value>>::iterator;
        using const_iterator = typename std::vector<std::pair<Key, Value>>::const_iterator;
        
        // 挿入
        std::pair<iterator, bool> insert(const std::pair<Key, Value>& value)
        {
            auto pos = find_position(value.first);
            
            if (pos != data_.end() && !comp_(value.first, pos->first))
            {
                // 既に存在
                return {pos, false};
            }
            
            auto it = data_.insert(pos, value);
            return {it, true};
        }
        
        // 要素アクセス
        Value& operator[](const Key& key)
        {
            auto pos = find_position(key);
            
            if (pos == data_.end() || comp_(key, pos->first))
            {
                // 新しいキー
                pos = data_.insert(pos, {key, Value{}});
            }
            
            return pos->second;
        }
        
        // 検索
        iterator find(const Key& key)
        {
            auto pos = find_position(key);
            return (pos != data_.end() && !comp_(key, pos->first)) ? pos : data_.end();
        }
        
        const_iterator find(const Key& key) const
        {
            auto pos = find_position(key);
            return (pos != data_.end() && !comp_(key, pos->first)) ? pos : data_.end();
        }
        
        // 削除
        size_t erase(const Key& key)
        {
            auto it = find(key);
            if (it != data_.end())
            {
                data_.erase(it);
                return 1;
            }
            return 0;
        }
        
        // サイズ関連
        size_t size() const { return data_.size(); }
        bool empty() const { return data_.empty(); }
        
        // イテレータ
        iterator begin() { return data_.begin(); }
        iterator end() { return data_.end(); }
        const_iterator begin() const { return data_.begin(); }
        const_iterator end() const { return data_.end(); }
        
        // 容量の事前確保
        void reserve(size_t capacity)
        {
            data_.reserve(capacity);
        }
        
        // メモリ使用量の最適化
        void shrink_to_fit()
        {
            data_.shrink_to_fit();
        }
    };
    
    void demonstrateFlatMap()
    {
        std::cout << "\n=== std::flat_map のデモンストレーション ===" << std::endl;
        
        // ゲームのアイテム管理
        flat_map<std::string, int> inventory;
        
        // アイテムの追加
        inventory["剣"] = 1;
        inventory["盾"] = 1;
        inventory["薬草"] = 5;
        inventory["矢"] = 50;
        
        std::cout << "インベントリ内容:" << std::endl;
        for (const auto& [item, count] : inventory)
        {
            std::cout << "  " << item << ": " << count << "個" << std::endl;
        }
        
        // アイテムの検索
        auto it = inventory.find("薬草");
        if (it != inventory.end())
        {
            std::cout << "\n薬草を " << it->second << "個 所持" << std::endl;
        }
        
        // アイテムの使用
        inventory["薬草"] -= 1;
        std::cout << "薬草を1個使用。残り: " << inventory["薬草"] << "個" << std::endl;
        
        // パフォーマンステスト
        std::cout << "\nパフォーマンステスト:" << std::endl;
        
        flat_map<int, std::string> flatMap;
        std::map<int, std::string> stdMap;
        
        const int iterations = 10000;
        
        // 挿入テスト
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i)
        {
            flatMap[i] = "value" + std::to_string(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto flatTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i)
        {
            stdMap[i] = "value" + std::to_string(i);
        }
        end = std::chrono::high_resolution_clock::now();
        auto stdTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "挿入 " << iterations << " 要素:" << std::endl;
        std::cout << "  flat_map: " << flatTime.count() << " μs" << std::endl;
        std::cout << "  std::map: " << stdTime.count() << " μs" << std::endl;
        std::cout << "  flat_mapは " << (static_cast<double>(stdTime.count()) / flatTime.count()) 
                  << " 倍高速" << std::endl;
    }
}

// =======================================
// std::mdspan の概念実装
// =======================================

namespace mdspan_concept
{
    // 簡易的なmdspan実装
    template<typename T, size_t... Extents>
    class mdspan
    {
    private:
        T* data_;
        static constexpr size_t total_size = (Extents * ...);
        
        template<size_t I, size_t... Is>
        constexpr size_t calculate_index(size_t index, size_t... indices) const
        {
            if constexpr (sizeof...(Is) == 0)
            {
                return index;
            }
            else
            {
                constexpr size_t stride = (Is * ...);
                return index * stride + calculate_index<Is...>(indices...);
            }
        }
        
    public:
        explicit mdspan(T* data) : data_(data) {}
        
        template<typename... Indices>
        T& operator()(Indices... indices)
        {
            static_assert(sizeof...(indices) == sizeof...(Extents));
            size_t index = calculate_index<Extents...>(indices...);
            return data_[index];
        }
        
        template<typename... Indices>
        const T& operator()(Indices... indices) const
        {
            static_assert(sizeof...(indices) == sizeof...(Extents));
            size_t index = calculate_index<Extents...>(indices...);
            return data_[index];
        }
        
        T* data() const { return data_; }
        constexpr size_t size() const { return total_size; }
        
        // 次元情報
        static constexpr size_t rank() { return sizeof...(Extents); }
        
        template<size_t I>
        static constexpr size_t extent()
        {
            static_assert(I < sizeof...(Extents));
            constexpr std::array<size_t, sizeof...(Extents)> extents = {Extents...};
            return extents[I];
        }
    };
    
    void demonstrateMdspan()
    {
        std::cout << "\n=== std::mdspan のデモンストレーション ===" << std::endl;
        
        // 3Dテクスチャデータの管理
        constexpr size_t width = 4;
        constexpr size_t height = 4;
        constexpr size_t depth = 3;
        
        std::vector<float> textureData(width * height * depth);
        mdspan<float, width, height, depth> texture(textureData.data());
        
        // テクスチャデータの初期化
        std::cout << "3Dテクスチャ初期化 (" << width << "x" << height << "x" << depth << "):" << std::endl;
        
        for (size_t z = 0; z < depth; ++z)
        {
            for (size_t y = 0; y < height; ++y)
            {
                for (size_t x = 0; x < width; ++x)
                {
                    // グラデーションパターン
                    texture(x, y, z) = static_cast<float>(x + y + z) / (width + height + depth);
                }
            }
        }
        
        // データの表示
        for (size_t z = 0; z < depth; ++z)
        {
            std::cout << "レイヤー " << z << ":" << std::endl;
            for (size_t y = 0; y < height; ++y)
            {
                std::cout << "  ";
                for (size_t x = 0; x < width; ++x)
                {
                    std::cout << std::format("{:5.2f} ", texture(x, y, z));
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
        
        // ゲームの地形データ例
        constexpr size_t mapWidth = 8;
        constexpr size_t mapHeight = 8;
        
        std::vector<int> heightMap(mapWidth * mapHeight);
        mdspan<int, mapWidth, mapHeight> terrain(heightMap.data());
        
        std::cout << "地形マップの生成:" << std::endl;
        
        // 簡単な地形生成
        for (size_t y = 0; y < mapHeight; ++y)
        {
            for (size_t x = 0; x < mapWidth; ++x)
            {
                // 中央から距離に基づく高度
                float dx = static_cast<float>(x) - mapWidth / 2.0f;
                float dy = static_cast<float>(y) - mapHeight / 2.0f;
                float distance = std::sqrt(dx * dx + dy * dy);
                terrain(x, y) = static_cast<int>(10 - distance);
            }
        }
        
        // 地形の表示
        for (size_t y = 0; y < mapHeight; ++y)
        {
            for (size_t x = 0; x < mapWidth; ++x)
            {
                std::cout << std::format("{:3d} ", terrain(x, y));
            }
            std::cout << std::endl;
        }
    }
}

// =======================================
// 拡張されたRanges機能
// =======================================

namespace extended_ranges
{
    // ranges::to の概念実装
    template<typename Container>
    struct to_t
    {
        template<typename Range>
        Container operator()(Range&& range) const
        {
            Container result;
            if constexpr (requires { result.reserve(std::size(range)); })
            {
                result.reserve(std::size(range));
            }
            
            for (auto&& element : range)
            {
                result.insert(result.end(), std::forward<decltype(element)>(element));
            }
            
            return result;
        }
    };
    
    template<typename Container>
    constexpr to_t<Container> to{};
    
    // ゲームオブジェクトの例
    struct GameObject
    {
        int id;
        std::string name;
        float x, y;
        bool active;
        
        GameObject(int i, const std::string& n, float px, float py, bool a = true)
            : id(i), name(n), x(px), y(py), active(a) {}
    };
    
    void demonstrateExtendedRanges()
    {
        std::cout << "\n=== 拡張されたRanges機能のデモンストレーション ===" << std::endl;
        
        // ゲームオブジェクトの作成
        std::vector<GameObject> gameObjects = {
            {1, "プレイヤー", 10.0f, 20.0f, true},
            {2, "敵1", 50.0f, 30.0f, true},
            {3, "敵2", 80.0f, 40.0f, false},
            {4, "アイテム1", 25.0f, 15.0f, true},
            {5, "アイテム2", 75.0f, 35.0f, true},
            {6, "障害物", 60.0f, 50.0f, false}
        };
        
        std::cout << "全ゲームオブジェクト:" << std::endl;
        for (const auto& obj : gameObjects)
        {
            std::cout << std::format("  {}: {} ({:.1f}, {:.1f}) {}\n",
                obj.id, obj.name, obj.x, obj.y, obj.active ? "アクティブ" : "非アクティブ");
        }
        
        // アクティブなオブジェクトのみをフィルタリング
        auto activeObjects = gameObjects 
            | std::views::filter([](const GameObject& obj) { return obj.active; })
            | to<std::vector<GameObject>>;
        
        std::cout << "\nアクティブなオブジェクト:" << std::endl;
        for (const auto& obj : activeObjects)
        {
            std::cout << std::format("  {}: {} ({:.1f}, {:.1f})\n",
                obj.id, obj.name, obj.x, obj.y);
        }
        
        // オブジェクトIDのリストを取得
        auto activeIds = gameObjects
            | std::views::filter([](const GameObject& obj) { return obj.active; })
            | std::views::transform([](const GameObject& obj) { return obj.id; })
            | to<std::vector<int>>;
        
        std::cout << "\nアクティブなオブジェクトID: ";
        for (size_t i = 0; i < activeIds.size(); ++i)
        {
            if (i > 0) std::cout << ", ";
            std::cout << activeIds[i];
        }
        std::cout << std::endl;
        
        // 距離による並び替え（原点からの距離）
        auto sortedByDistance = gameObjects
            | std::views::filter([](const GameObject& obj) { return obj.active; })
            | to<std::vector<GameObject>>;
        
        std::ranges::sort(sortedByDistance, [](const GameObject& a, const GameObject& b) {
            float distA = std::sqrt(a.x * a.x + a.y * a.y);
            float distB = std::sqrt(b.x * b.x + b.y * b.y);
            return distA < distB;
        });
        
        std::cout << "\n原点からの距離順:" << std::endl;
        for (const auto& obj : sortedByDistance)
        {
            float distance = std::sqrt(obj.x * obj.x + obj.y * obj.y);
            std::cout << std::format("  {}: {} 距離 {:.1f}\n",
                obj.id, obj.name, distance);
        }
        
        // グループ化（名前の種類別）
        std::unordered_map<std::string, std::vector<GameObject>> groupedObjects;
        
        for (const auto& obj : gameObjects)
        {
            if (obj.active)
            {
                std::string category;
                if (obj.name.find("プレイヤー") != std::string::npos)
                {
                    category = "プレイヤー";
                }
                else if (obj.name.find("敵") != std::string::npos)
                {
                    category = "敵";
                }
                else if (obj.name.find("アイテム") != std::string::npos)
                {
                    category = "アイテム";
                }
                else
                {
                    category = "その他";
                }
                
                groupedObjects[category].push_back(obj);
            }
        }
        
        std::cout << "\nカテゴリ別グループ:" << std::endl;
        for (const auto& [category, objects] : groupedObjects)
        {
            std::cout << "  " << category << ": " << objects.size() << "個" << std::endl;
            for (const auto& obj : objects)
            {
                std::cout << std::format("    {}: {}\n", obj.id, obj.name);
            }
        }
    }
}

// =======================================
// constexpr の拡張
// =======================================

namespace constexpr_extensions
{
    // constexpr アルゴリズムの例
    constexpr int factorial(int n)
    {
        if (n <= 1) return 1;
        return n * factorial(n - 1);
    }
    
    constexpr std::array<int, 10> generateFactorials()
    {
        std::array<int, 10> result{};
        for (int i = 0; i < 10; ++i)
        {
            result[i] = factorial(i);
        }
        return result;
    }
    
    // constexpr での文字列処理
    constexpr bool isValidPlayerName(std::string_view name)
    {
        if (name.empty() || name.length() > 20)
        {
            return false;
        }
        
        // 英数字とアンダースコアのみ許可
        for (char c : name)
        {
            if (!((c >= 'a' && c <= 'z') || 
                  (c >= 'A' && c <= 'Z') || 
                  (c >= '0' && c <= '9') || 
                  c == '_'))
            {
                return false;
            }
        }
        
        return true;
    }
    
    // constexpr でのコンパイル時設定
    struct GameConfig
    {
        int maxPlayers;
        int maxEnemies;
        float worldSize;
        bool debugMode;
        
        constexpr GameConfig(int players, int enemies, float size, bool debug)
            : maxPlayers(players), maxEnemies(enemies), worldSize(size), debugMode(debug) {}
        
        constexpr bool isValid() const
        {
            return maxPlayers > 0 && maxPlayers <= 64 &&
                   maxEnemies >= 0 && maxEnemies <= 1000 &&
                   worldSize > 0.0f && worldSize <= 10000.0f;
        }
    };
    
    constexpr GameConfig createConfig(std::string_view difficulty)
    {
        if (difficulty == "easy")
        {
            return GameConfig(4, 50, 1000.0f, false);
        }
        else if (difficulty == "normal")
        {
            return GameConfig(4, 100, 2000.0f, false);
        }
        else if (difficulty == "hard")
        {
            return GameConfig(4, 200, 3000.0f, false);
        }
        else if (difficulty == "debug")
        {
            return GameConfig(1, 10, 500.0f, true);
        }
        else
        {
            return GameConfig(4, 100, 2000.0f, false);  // デフォルト
        }
    }
    
    void demonstrateConstexprExtensions()
    {
        std::cout << "\n=== constexpr 拡張のデモンストレーション ===" << std::endl;
        
        // コンパイル時計算
        constexpr auto factorials = generateFactorials();
        std::cout << "階乗（コンパイル時計算）:" << std::endl;
        for (size_t i = 0; i < factorials.size(); ++i)
        {
            std::cout << std::format("  {}! = {}\n", i, factorials[i]);
        }
        
        // プレイヤー名のバリデーション
        std::cout << "\nプレイヤー名バリデーション:" << std::endl;
        constexpr std::array<std::string_view, 5> testNames = {
            "Player1", "invalid-name", "TooLongPlayerNameThatExceedsLimit", "_underscore", "123"
        };
        
        for (const auto& name : testNames)
        {
            constexpr bool valid = isValidPlayerName(name);
            std::cout << std::format("  '{}': {}\n", name, valid ? "有効" : "無効");
        }
        
        // ゲーム設定
        std::cout << "\nゲーム設定（コンパイル時）:" << std::endl;
        constexpr auto easyConfig = createConfig("easy");
        constexpr auto hardConfig = createConfig("hard");
        constexpr auto debugConfig = createConfig("debug");
        
        std::cout << std::format("Easy - プレイヤー: {}, 敵: {}, ワールドサイズ: {:.1f}\n",
            easyConfig.maxPlayers, easyConfig.maxEnemies, easyConfig.worldSize);
        
        std::cout << std::format("Hard - プレイヤー: {}, 敵: {}, ワールドサイズ: {:.1f}\n",
            hardConfig.maxPlayers, hardConfig.maxEnemies, hardConfig.worldSize);
        
        std::cout << std::format("Debug - プレイヤー: {}, 敵: {}, デバッグ: {}\n",
            debugConfig.maxPlayers, debugConfig.maxEnemies, debugConfig.debugMode ? "有効" : "無効");
        
        // 設定の妥当性チェック
        static_assert(easyConfig.isValid(), "Easy設定が無効");
        static_assert(hardConfig.isValid(), "Hard設定が無効");
        static_assert(debugConfig.isValid(), "Debug設定が無効");
        
        std::cout << "全ての設定がコンパイル時に妥当性チェック済み" << std::endl;
    }
}

int main()
{
    std::cout << "=== C++23の新機能 ===" << std::endl;
    
    // 1. C++23の概要
    cpp23_overview::explainCpp23Features();
    
    // 2. std::expected
    expected_concept::demonstrateExpected();
    
    // 3. std::flat_map
    flat_map_concept::demonstrateFlatMap();
    
    // 4. std::mdspan
    mdspan_concept::demonstrateMdspan();
    
    // 5. 拡張されたRanges
    extended_ranges::demonstrateExtendedRanges();
    
    // 6. constexpr の拡張
    constexpr_extensions::demonstrateConstexprExtensions();
    
    std::cout << "\n=== C++23のゲーム開発への影響 ===" << std::endl;
    std::cout << "1. エラーハンドリング：std::expectedで例外のない安全なコード" << std::endl;
    std::cout << "2. パフォーマンス：flat_containerでメモリ効率向上" << std::endl;
    std::cout << "3. 数値計算：mdspanで多次元データの効率的処理" << std::endl;
    std::cout << "4. データ処理：拡張rangesでより表現力豊かなコード" << std::endl;
    std::cout << "5. コンパイル時最適化：constexpr拡張でより多くのコンパイル時計算" << std::endl;
    
    return 0;
}

// まとめ：
// - C++23はエラーハンドリング、パフォーマンス、表現力を大幅改善
// - std::expectedで例外を使わない安全なエラーハンドリング
// - flat_containerでメモリ効率の向上
// - mdspanで多次元配列の効率的な処理
// - ranges機能の拡張でより直感的なデータ処理
// - constexpr拡張でコンパイル時計算の可能性拡大
// - ゲーム開発では特にパフォーマンスと安全性の向上が期待される