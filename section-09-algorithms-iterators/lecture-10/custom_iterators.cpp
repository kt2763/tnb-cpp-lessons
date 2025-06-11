/*
 * C++モダンコース - セクション9: アルゴリズムとイテレータ
 * レクチャー10: カスタムイテレータとアダプタ
 * 
 * このレクチャーでは、独自のイテレータとイテレータアダプタの作成方法を学びます
 * ゲーム開発での特殊なデータ構造やアクセスパターンに対応するイテレータを実装します
 */

#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <string>
#include <iomanip>

// ゲームタイルのデータ構造
struct GameTile
{
    int x, y;
    char symbol;
    bool is_walkable;
    
    GameTile(int px, int py, char s, bool walkable)
        : x(px), y(py), symbol(s), is_walkable(walkable) {}
    
    void display() const
    {
        std::cout << "(" << x << "," << y << ")" << symbol 
                  << (is_walkable ? "○" : "×");
    }
};

// 2Dゲームマップクラス
class GameMap
{
private:
    std::vector<std::vector<GameTile>> tiles;
    int width, height;
    
public:
    GameMap(int w, int h) : width(w), height(h)
    {
        tiles.resize(height);
        for (int y = 0; y < height; ++y)
        {
            tiles[y].reserve(width);
            for (int x = 0; x < width; ++x)
            {
                char symbol = '.';
                bool walkable = true;
                
                // 境界は壁
                if (x == 0 || x == width-1 || y == 0 || y == height-1)
                {
                    symbol = '#';
                    walkable = false;
                }
                // ランダムに障害物を配置
                else if ((x + y) % 7 == 0)
                {
                    symbol = 'X';
                    walkable = false;
                }
                
                tiles[y].emplace_back(x, y, symbol, walkable);
            }
        }
    }
    
    // カスタム行イテレータ
    class RowIterator
    {
    private:
        std::vector<GameTile>* row_ptr;
        int index;
        
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = GameTile;
        using difference_type = std::ptrdiff_t;
        using pointer = GameTile*;
        using reference = GameTile&;
        
        RowIterator(std::vector<GameTile>* row, int idx) : row_ptr(row), index(idx) {}
        
        reference operator*() { return (*row_ptr)[index]; }
        pointer operator->() { return &(*row_ptr)[index]; }
        
        RowIterator& operator++() { ++index; return *this; }
        RowIterator operator++(int) { RowIterator temp = *this; ++index; return temp; }
        
        RowIterator& operator--() { --index; return *this; }
        RowIterator operator--(int) { RowIterator temp = *this; --index; return temp; }
        
        RowIterator& operator+=(difference_type n) { index += n; return *this; }
        RowIterator& operator-=(difference_type n) { index -= n; return *this; }
        
        RowIterator operator+(difference_type n) const { return RowIterator(row_ptr, index + n); }
        RowIterator operator-(difference_type n) const { return RowIterator(row_ptr, index - n); }
        
        difference_type operator-(const RowIterator& other) const { return index - other.index; }
        
        bool operator==(const RowIterator& other) const { return index == other.index; }
        bool operator!=(const RowIterator& other) const { return index != other.index; }
        bool operator<(const RowIterator& other) const { return index < other.index; }
        bool operator>(const RowIterator& other) const { return index > other.index; }
        bool operator<=(const RowIterator& other) const { return index <= other.index; }
        bool operator>=(const RowIterator& other) const { return index >= other.index; }
        
        reference operator[](difference_type n) { return (*row_ptr)[index + n]; }
    };
    
    // 歩行可能タイル専用イテレータ
    class WalkableIterator
    {
    private:
        const GameMap* map_ptr;
        int current_x, current_y;
        
        void find_next_walkable()
        {
            while (current_y < map_ptr->height)
            {
                while (current_x < map_ptr->width)
                {
                    if (map_ptr->tiles[current_y][current_x].is_walkable)
                        return;
                    ++current_x;
                }
                current_x = 0;
                ++current_y;
            }
        }
        
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = GameTile;
        using difference_type = std::ptrdiff_t;
        using pointer = const GameTile*;
        using reference = const GameTile&;
        
        WalkableIterator(const GameMap* map, int x = 0, int y = 0)
            : map_ptr(map), current_x(x), current_y(y)
        {
            find_next_walkable();
        }
        
        reference operator*() const
        {
            return map_ptr->tiles[current_y][current_x];
        }
        
        pointer operator->() const
        {
            return &map_ptr->tiles[current_y][current_x];
        }
        
        WalkableIterator& operator++()
        {
            ++current_x;
            find_next_walkable();
            return *this;
        }
        
        WalkableIterator operator++(int)
        {
            WalkableIterator temp = *this;
            ++(*this);
            return temp;
        }
        
        bool operator==(const WalkableIterator& other) const
        {
            return current_x == other.current_x && current_y == other.current_y;
        }
        
        bool operator!=(const WalkableIterator& other) const
        {
            return !(*this == other);
        }
    };
    
    RowIterator row_begin(int y) { return RowIterator(&tiles[y], 0); }
    RowIterator row_end(int y) { return RowIterator(&tiles[y], width); }
    
    WalkableIterator walkable_begin() const { return WalkableIterator(this, 0, 0); }
    WalkableIterator walkable_end() const { return WalkableIterator(this, 0, height); }
    
    void display() const
    {
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                std::cout << tiles[y][x].symbol;
            }
            std::cout << std::endl;
        }
    }
    
    GameTile& get_tile(int x, int y) { return tiles[y][x]; }
    const GameTile& get_tile(int x, int y) const { return tiles[y][x]; }
    
    int get_width() const { return width; }
    int get_height() const { return height; }
};

void demonstrateCustomIterators()
{
    std::cout << "\n=== カスタムイテレータの実例 ===" << std::endl;
    
    // ゲームマップの作成
    GameMap map(10, 6);
    
    std::cout << "ゲームマップ:" << std::endl;
    map.display();
    
    // 1. 行イテレータの使用
    std::cout << "\n行イテレータ - 3行目のタイル情報:" << std::endl;
    int row = 2;
    for (auto it = map.row_begin(row); it != map.row_end(row); ++it)
    {
        it->display();
        std::cout << " ";
    }
    std::cout << std::endl;
    
    // 2. 歩行可能タイルのみを反復
    std::cout << "\n歩行可能タイルの座標:" << std::endl;
    int walkable_count = 0;
    for (auto it = map.walkable_begin(); it != map.walkable_end(); ++it)
    {
        std::cout << "(" << it->x << "," << it->y << ") ";
        ++walkable_count;
        if (walkable_count % 8 == 0) std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "歩行可能タイル数: " << walkable_count << std::endl;
    
    // 3. STLアルゴリズムとの組み合わせ
    std::cout << "\n歩行可能タイルでのアルゴリズム適用:" << std::endl;
    
    // 歩行可能タイル数をカウント
    int count = std::distance(map.walkable_begin(), map.walkable_end());
    std::cout << "std::distance による歩行可能タイル数: " << count << std::endl;
    
    // 特定座標のタイルを検索
    auto target_tile = std::find_if(map.walkable_begin(), map.walkable_end(),
        [](const GameTile& tile) 
        {
            return tile.x == 5 && tile.y == 3;
        });
    
    if (target_tile != map.walkable_end())
    {
        std::cout << "座標(5,3)のタイルが見つかりました: ";
        target_tile->display();
        std::cout << std::endl;
    }
}

// フィルタリングイテレータアダプタ
template<typename Iterator, typename Predicate>
class FilterIterator
{
private:
    Iterator current_;
    Iterator end_;
    Predicate predicate_;
    
    void advance_to_valid()
    {
        while (current_ != end_ && !predicate_(*current_))
        {
            ++current_;
        }
    }
    
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename std::iterator_traits<Iterator>::value_type;
    using difference_type = typename std::iterator_traits<Iterator>::difference_type;
    using pointer = typename std::iterator_traits<Iterator>::pointer;
    using reference = typename std::iterator_traits<Iterator>::reference;
    
    FilterIterator(Iterator current, Iterator end, Predicate pred)
        : current_(current), end_(end), predicate_(pred)
    {
        advance_to_valid();
    }
    
    reference operator*() { return *current_; }
    pointer operator->() { return current_.operator->(); }
    
    FilterIterator& operator++()
    {
        ++current_;
        advance_to_valid();
        return *this;
    }
    
    FilterIterator operator++(int)
    {
        FilterIterator temp = *this;
        ++(*this);
        return temp;
    }
    
    bool operator==(const FilterIterator& other) const
    {
        return current_ == other.current_;
    }
    
    bool operator!=(const FilterIterator& other) const
    {
        return !(*this == other);
    }
};

// フィルタリングイテレータのヘルパー関数
template<typename Iterator, typename Predicate>
FilterIterator<Iterator, Predicate> make_filter_iterator(Iterator current, Iterator end, Predicate pred)
{
    return FilterIterator<Iterator, Predicate>(current, end, pred);
}

void demonstrateFilterIterator()
{
    std::cout << "\n=== フィルタリングイテレータアダプタ ===" << std::endl;
    
    // プレイヤーのスコアデータ
    std::vector<int> scores = {1200, 450, 1800, 320, 2100, 890, 1650, 1400, 750, 1950};
    
    std::cout << "全スコア: ";
    for (int score : scores)
    {
        std::cout << score << " ";
    }
    std::cout << std::endl;
    
    // 1. 高得点（1000以上）のみをフィルタリング
    auto high_score_begin = make_filter_iterator(scores.begin(), scores.end(),
        [](int score) { return score >= 1000; });
    auto high_score_end = make_filter_iterator(scores.end(), scores.end(),
        [](int score) { return score >= 1000; });
    
    std::cout << "高得点（1000以上）: ";
    for (auto it = high_score_begin; it != high_score_end; ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 2. STLアルゴリズムとの組み合わせ
    int high_score_count = std::distance(high_score_begin, high_score_end);
    std::cout << "高得点の数: " << high_score_count << std::endl;
    
    // 3. ゲームアイテムでのフィルタリング
    struct GameItem
    {
        std::string name;
        int level;
        std::string rarity;
        bool is_equipped;
        
        GameItem(const std::string& n, int lv, const std::string& r, bool equipped)
            : name(n), level(lv), rarity(r), is_equipped(equipped) {}
    };
    
    std::vector<GameItem> items = 
    {
        {"木の剣", 1, "コモン", false},
        {"鉄の剣", 5, "コモン", true},
        {"魔法の剣", 10, "レア", false},
        {"ドラゴンソード", 15, "エピック", false},
        {"布の帽子", 1, "コモン", false},
        {"魔法の帽子", 8, "レア", true},
        {"神聖な冠", 20, "レジェンダリー", false}
    };
    
    std::cout << "\n装備中のアイテム:" << std::endl;
    auto equipped_begin = make_filter_iterator(items.begin(), items.end(),
        [](const GameItem& item) { return item.is_equipped; });
    auto equipped_end = make_filter_iterator(items.end(), items.end(),
        [](const GameItem& item) { return item.is_equipped; });
    
    for (auto it = equipped_begin; it != equipped_end; ++it)
    {
        std::cout << "- " << it->name << " (Lv." << it->level << ")" << std::endl;
    }
    
    std::cout << "\nレアアイテム:" << std::endl;
    auto rare_begin = make_filter_iterator(items.begin(), items.end(),
        [](const GameItem& item) { return item.rarity == "レア" || item.rarity == "エピック" || item.rarity == "レジェンダリー"; });
    auto rare_end = make_filter_iterator(items.end(), items.end(),
        [](const GameItem& item) { return item.rarity == "レア" || item.rarity == "エピック" || item.rarity == "レジェンダリー"; });
    
    for (auto it = rare_begin; it != rare_end; ++it)
    {
        std::cout << "- " << it->name << " (" << it->rarity << ")" << std::endl;
    }
}

// 変換イテレータアダプタ
template<typename Iterator, typename Transformer>
class TransformIterator
{
private:
    Iterator current_;
    Transformer transformer_;
    
public:
    using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
    using value_type = decltype(std::declval<Transformer>()(*std::declval<Iterator>()));
    using difference_type = typename std::iterator_traits<Iterator>::difference_type;
    using pointer = value_type*;
    using reference = value_type;
    
    TransformIterator(Iterator current, Transformer transformer)
        : current_(current), transformer_(transformer) {}
    
    value_type operator*() { return transformer_(*current_); }
    
    TransformIterator& operator++()
    {
        ++current_;
        return *this;
    }
    
    TransformIterator operator++(int)
    {
        TransformIterator temp = *this;
        ++current_;
        return temp;
    }
    
    bool operator==(const TransformIterator& other) const
    {
        return current_ == other.current_;
    }
    
    bool operator!=(const TransformIterator& other) const
    {
        return !(*this == other);
    }
    
    // ランダムアクセスイテレータのサポート
    TransformIterator& operator+=(difference_type n)
    {
        current_ += n;
        return *this;
    }
    
    TransformIterator operator+(difference_type n) const
    {
        return TransformIterator(current_ + n, transformer_);
    }
    
    difference_type operator-(const TransformIterator& other) const
    {
        return current_ - other.current_;
    }
};

template<typename Iterator, typename Transformer>
TransformIterator<Iterator, Transformer> make_transform_iterator(Iterator iter, Transformer transformer)
{
    return TransformIterator<Iterator, Transformer>(iter, transformer);
}

void demonstrateTransformIterator()
{
    std::cout << "\n=== 変換イテレータアダプタ ===" << std::endl;
    
    // プレイヤーの基本経験値
    std::vector<int> base_experience = {1000, 1500, 800, 1200, 1800, 950, 1350, 1600};
    
    std::cout << "基本経験値: ";
    for (int exp : base_experience)
    {
        std::cout << exp << " ";
    }
    std::cout << std::endl;
    
    // 1. ボーナス経験値（1.5倍）に変換
    auto bonus_begin = make_transform_iterator(base_experience.begin(),
        [](int exp) { return static_cast<int>(exp * 1.5); });
    auto bonus_end = make_transform_iterator(base_experience.end(),
        [](int exp) { return static_cast<int>(exp * 1.5); });
    
    std::cout << "ボーナス経験値: ";
    for (auto it = bonus_begin; it != bonus_end; ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 2. 経験値をレベルに変換
    auto level_begin = make_transform_iterator(base_experience.begin(),
        [](int exp) { return exp / 100; }); // 100 EXPごとに1レベル
    auto level_end = make_transform_iterator(base_experience.end(),
        [](int exp) { return exp / 100; });
    
    std::cout << "対応レベル: ";
    for (auto it = level_begin; it != level_end; ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 3. STLアルゴリズムとの組み合わせ
    int total_bonus_exp = std::accumulate(bonus_begin, bonus_end, 0);
    std::cout << "総ボーナス経験値: " << total_bonus_exp << std::endl;
    
    int max_level = *std::max_element(level_begin, level_end);
    std::cout << "最高レベル: " << max_level << std::endl;
}

// カウンティングイテレータ
class CountingIterator
{
private:
    int value_;
    
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = int;
    using difference_type = std::ptrdiff_t;
    using pointer = int*;
    using reference = int;
    
    explicit CountingIterator(int start_value = 0) : value_(start_value) {}
    
    int operator*() const { return value_; }
    
    CountingIterator& operator++() { ++value_; return *this; }
    CountingIterator operator++(int) { CountingIterator temp = *this; ++value_; return temp; }
    
    CountingIterator& operator--() { --value_; return *this; }
    CountingIterator operator--(int) { CountingIterator temp = *this; --value_; return temp; }
    
    CountingIterator& operator+=(difference_type n) { value_ += n; return *this; }
    CountingIterator& operator-=(difference_type n) { value_ -= n; return *this; }
    
    CountingIterator operator+(difference_type n) const { return CountingIterator(value_ + n); }
    CountingIterator operator-(difference_type n) const { return CountingIterator(value_ - n); }
    
    difference_type operator-(const CountingIterator& other) const { return value_ - other.value_; }
    
    bool operator==(const CountingIterator& other) const { return value_ == other.value_; }
    bool operator!=(const CountingIterator& other) const { return value_ != other.value_; }
    bool operator<(const CountingIterator& other) const { return value_ < other.value_; }
    bool operator>(const CountingIterator& other) const { return value_ > other.value_; }
    bool operator<=(const CountingIterator& other) const { return value_ <= other.value_; }
    bool operator>=(const CountingIterator& other) const { return value_ >= other.value_; }
    
    int operator[](difference_type n) const { return value_ + n; }
};

void demonstrateCountingIterator()
{
    std::cout << "\n=== カウンティングイテレータ ===" << std::endl;
    
    // 1. 連続した数値の生成
    CountingIterator start(1);
    CountingIterator end(11);
    
    std::cout << "1から10までの数値: ";
    for (auto it = start; it != end; ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 2. ゲームレベルの生成
    std::cout << "プレイヤーレベル進行 (レベル5〜15): ";
    CountingIterator level_start(5);
    CountingIterator level_end(16);
    
    std::for_each(level_start, level_end,
        [](int level) 
        {
            int required_exp = level * level * 100; // レベルの2乗 × 100
            std::cout << "Lv." << level << "(" << required_exp << "EXP) ";
        });
    std::cout << std::endl;
    
    // 3. インデックスとしての使用
    std::vector<std::string> items = {"剣", "盾", "薬", "鍵", "宝石"};
    
    std::cout << "アイテムリスト:" << std::endl;
    CountingIterator index_start(0);
    CountingIterator index_end(items.size());
    
    std::for_each(index_start, index_end,
        [&items](int index) 
        {
            std::cout << index + 1 << ". " << items[index] << std::endl;
        });
}

void demonstrateIteratorCombination()
{
    std::cout << "\n=== イテレータの組み合わせ活用 ===" << std::endl;
    
    // ゲームスコアデータ
    std::vector<int> raw_scores = {850, 1200, 400, 1500, 300, 1800, 750, 1100, 950, 1350};
    
    std::cout << "元のスコア: ";
    for (int score : raw_scores)
    {
        std::cout << score << " ";
    }
    std::cout << std::endl;
    
    // 1. フィルタリング → 変換の組み合わせ
    // 1000以上のスコアをフィルタリングしてからボーナス適用
    auto high_scores_begin = make_filter_iterator(raw_scores.begin(), raw_scores.end(),
        [](int score) { return score >= 1000; });
    auto high_scores_end = make_filter_iterator(raw_scores.end(), raw_scores.end(),
        [](int score) { return score >= 1000; });
    
    std::vector<int> filtered_scores(high_scores_begin, high_scores_end);
    
    std::cout << "高得点のみ: ";
    for (int score : filtered_scores)
    {
        std::cout << score << " ";
    }
    std::cout << std::endl;
    
    auto bonus_scores_begin = make_transform_iterator(filtered_scores.begin(),
        [](int score) { return static_cast<int>(score * 1.2); }); // 20%ボーナス
    auto bonus_scores_end = make_transform_iterator(filtered_scores.end(),
        [](int score) { return static_cast<int>(score * 1.2); });
    
    std::cout << "ボーナス適用後: ";
    for (auto it = bonus_scores_begin; it != bonus_scores_end; ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 2. ランキング生成
    CountingIterator rank_start(1);
    CountingIterator rank_end(filtered_scores.size() + 1);
    
    std::cout << "\nハイスコアランキング:" << std::endl;
    
    // スコアを降順ソート
    std::vector<int> sorted_scores(filtered_scores);
    std::sort(sorted_scores.rbegin(), sorted_scores.rend());
    
    auto ranking_scores = make_transform_iterator(sorted_scores.begin(),
        [](int score) { return static_cast<int>(score * 1.2); });
    
    auto rank_it = rank_start;
    auto score_it = ranking_scores;
    
    for (; rank_it != rank_end; ++rank_it, ++score_it)
    {
        std::cout << "第" << *rank_it << "位: " << *score_it << "ポイント" << std::endl;
    }
    
    std::cout << "\n総合統計:" << std::endl;
    int total_bonus = std::accumulate(bonus_scores_begin, bonus_scores_end, 0);
    std::cout << "高得点者のボーナス合計: " << total_bonus << std::endl;
    
    int max_bonus = *std::max_element(bonus_scores_begin, bonus_scores_end);
    std::cout << "最高ボーナススコア: " << max_bonus << std::endl;
}

int main()
{
    std::cout << "C++アルゴリズムとイテレータ - レクチャー10: カスタムイテレータとアダプタ" << std::endl;
    std::cout << "=================================================================" << std::endl;
    
    // 各カスタムイテレータの実演
    demonstrateCustomIterators();
    demonstrateFilterIterator();
    demonstrateTransformIterator();
    demonstrateCountingIterator();
    demonstrateIteratorCombination();
    
    std::cout << "\n=== まとめ ===" << std::endl;
    std::cout << "1. カスタムイテレータ: 特殊なデータ構造への対応" << std::endl;
    std::cout << "   - 必要な演算子のオーバーロード" << std::endl;
    std::cout << "   - iterator_traitsの対応" << std::endl;
    std::cout << "2. フィルタリングイテレータ: 条件に合う要素のみを反復" << std::endl;
    std::cout << "3. 変換イテレータ: 要素を変換しながら反復" << std::endl;
    std::cout << "4. カウンティングイテレータ: 連続した値の生成" << std::endl;
    std::cout << "5. イテレータの組み合わせ: 複雑な処理の実現" << std::endl;
    std::cout << "ゲーム開発では、特殊なデータアクセスパターンに対応可能" << std::endl;
    
    return 0;
}