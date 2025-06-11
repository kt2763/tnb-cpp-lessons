// 第4講: 演算子オーバーロード
// クラスに対して自然な演算を可能にする演算子オーバーロードを学ぶ

#include <iostream>
#include <string>
#include <vector>

// === 基本的な演算子オーバーロード（Vector3Dクラス） ===
class Vector3D 
{
private:
    double x, y, z;

public:
    Vector3D(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}
    
    // === 算術演算子 ===
    Vector3D operator+(const Vector3D& other) const 
    {
        return Vector3D(x + other.x, y + other.y, z + other.z);
    }
    
    Vector3D operator-(const Vector3D& other) const 
    {
        return Vector3D(x - other.x, y - other.y, z - other.z);
    }
    
    Vector3D operator*(double scalar) const 
    {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }
    
    Vector3D operator/(double scalar) const 
    {
        if (scalar != 0) 
        {
            return Vector3D(x / scalar, y / scalar, z / scalar);
        }
        return *this;
    }
    
    // === 複合代入演算子 ===
    Vector3D& operator+=(const Vector3D& other) 
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    
    Vector3D& operator*=(double scalar) 
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    
    // === 比較演算子 ===
    bool operator==(const Vector3D& other) const 
    {
        const double epsilon = 1e-9;
        return (std::abs(x - other.x) < epsilon && 
                std::abs(y - other.y) < epsilon && 
                std::abs(z - other.z) < epsilon);
    }
    
    bool operator!=(const Vector3D& other) const 
    {
        return !(*this == other);
    }
    
    // === 添字演算子 ===
    double& operator[](int index) 
    {
        switch (index) 
        {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: throw std::out_of_range("Index out of range");
        }
    }
    
    const double& operator[](int index) const 
    {
        switch (index) 
        {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: throw std::out_of_range("Index out of range");
        }
    }
    
    // === 出力演算子（フレンド関数） ===
    friend std::ostream& operator<<(std::ostream& os, const Vector3D& vec) 
    {
        os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return os;
    }
    
    // === 入力演算子（フレンド関数） ===
    friend std::istream& operator>>(std::istream& is, Vector3D& vec) 
    {
        is >> vec.x >> vec.y >> vec.z;
        return is;
    }
    
    // ユーティリティメソッド
    double length() const 
    {
        return std::sqrt(x * x + y * y + z * z);
    }
    
    Vector3D normalize() const 
    {
        double len = length();
        if (len > 0) 
        {
            return *this / len;
        }
        return Vector3D();
    }
};

// === ゲーム通貨クラス（金額の計算） ===
class GameCurrency 
{
private:
    long long amount;  // 最小単位（例：銅貨）で管理

public:
    explicit GameCurrency(long long value = 0) : amount(value) {}
    
    // === 算術演算子 ===
    GameCurrency operator+(const GameCurrency& other) const 
    {
        return GameCurrency(amount + other.amount);
    }
    
    GameCurrency operator-(const GameCurrency& other) const 
    {
        return GameCurrency(amount - other.amount);
    }
    
    GameCurrency operator*(int multiplier) const 
    {
        return GameCurrency(amount * multiplier);
    }
    
    GameCurrency operator*(double multiplier) const 
    {
        return GameCurrency(static_cast<long long>(amount * multiplier));
    }
    
    // === 複合代入演算子 ===
    GameCurrency& operator+=(const GameCurrency& other) 
    {
        amount += other.amount;
        return *this;
    }
    
    GameCurrency& operator-=(const GameCurrency& other) 
    {
        amount -= other.amount;
        return *this;
    }
    
    // === 比較演算子 ===
    bool operator<(const GameCurrency& other) const 
    {
        return amount < other.amount;
    }
    
    bool operator<=(const GameCurrency& other) const 
    {
        return amount <= other.amount;
    }
    
    bool operator>(const GameCurrency& other) const 
    {
        return amount > other.amount;
    }
    
    bool operator>=(const GameCurrency& other) const 
    {
        return amount >= other.amount;
    }
    
    bool operator==(const GameCurrency& other) const 
    {
        return amount == other.amount;
    }
    
    bool operator!=(const GameCurrency& other) const 
    {
        return amount != other.amount;
    }
    
    // === 単項演算子 ===
    GameCurrency operator-() const 
    {
        return GameCurrency(-amount);
    }
    
    GameCurrency& operator++()  // 前置インクリメント
    {
        ++amount;
        return *this;
    }
    
    GameCurrency operator++(int)  // 後置インクリメント
    {
        GameCurrency temp(*this);
        amount++;
        return temp;
    }
    
    // === 出力演算子 ===
    friend std::ostream& operator<<(std::ostream& os, const GameCurrency& currency) 
    {
        long long gold = currency.amount / 10000;
        long long silver = (currency.amount % 10000) / 100;
        long long copper = currency.amount % 100;
        
        if (gold > 0) 
        {
            os << gold << "G ";
        }
        if (silver > 0) 
        {
            os << silver << "S ";
        }
        if (copper > 0 || currency.amount == 0) 
        {
            os << copper << "C";
        }
        
        return os;
    }
    
    long long getValue() const { return amount; }
    
    bool canAfford(const GameCurrency& cost) const 
    {
        return amount >= cost.amount;
    }
};

// === ゲームアイテムクラス（比較とソート） ===
class GameItem 
{
private:
    std::string name;
    int rarity;  // 1-5（レア度）
    GameCurrency price;

public:
    GameItem(const std::string& itemName, int itemRarity, long long itemPrice) 
        : name(itemName), rarity(itemRarity), price(itemPrice) {}
    
    // === 比較演算子（レア度でソート） ===
    bool operator<(const GameItem& other) const 
    {
        if (rarity != other.rarity) 
        {
            return rarity < other.rarity;
        }
        return price.getValue() < other.price.getValue();
    }
    
    bool operator>(const GameItem& other) const 
    {
        return other < *this;
    }
    
    bool operator==(const GameItem& other) const 
    {
        return name == other.name && rarity == other.rarity;
    }
    
    // === 出力演算子 ===
    friend std::ostream& operator<<(std::ostream& os, const GameItem& item) 
    {
        os << "[★" << item.rarity << "] " << item.name << " (" << item.price << ")";
        return os;
    }
    
    const std::string& getName() const { return name; }
    int getRarity() const { return rarity; }
    const GameCurrency& getPrice() const { return price; }
};

// === プレイヤーインベントリクラス（添字演算子） ===
class PlayerInventory 
{
private:
    std::vector<GameItem> items;
    int maxCapacity;

public:
    PlayerInventory(int capacity = 20) : maxCapacity(capacity) {}
    
    // === 添字演算子 ===
    GameItem& operator[](int index) 
    {
        if (index < 0 || index >= static_cast<int>(items.size())) 
        {
            throw std::out_of_range("インベントリインデックスが範囲外です");
        }
        return items[index];
    }
    
    const GameItem& operator[](int index) const 
    {
        if (index < 0 || index >= static_cast<int>(items.size())) 
        {
            throw std::out_of_range("インベントリインデックスが範囲外です");
        }
        return items[index];
    }
    
    // === 複合代入演算子（アイテム追加） ===
    PlayerInventory& operator+=(const GameItem& item) 
    {
        if (items.size() < maxCapacity) 
        {
            items.push_back(item);
            std::cout << "「" << item.getName() << "」をインベントリに追加しました" << std::endl;
        }
        else 
        {
            std::cout << "インベントリが満杯です！" << std::endl;
        }
        return *this;
    }
    
    // === 関数呼び出し演算子 ===
    void operator()(const std::string& itemName) 
    {
        for (const auto& item : items) 
        {
            if (item.getName() == itemName) 
            {
                std::cout << "「" << itemName << "」を使用しました！" << std::endl;
                return;
            }
        }
        std::cout << "「" << itemName << "」は見つかりませんでした" << std::endl;
    }
    
    // === 出力演算子 ===
    friend std::ostream& operator<<(std::ostream& os, const PlayerInventory& inventory) 
    {
        os << "=== インベントリ (" << inventory.items.size() 
           << "/" << inventory.maxCapacity << ") ===" << std::endl;
        
        for (size_t i = 0; i < inventory.items.size(); ++i) 
        {
            os << i + 1 << ". " << inventory.items[i] << std::endl;
        }
        
        return os;
    }
    
    size_t size() const { return items.size(); }
    bool empty() const { return items.empty(); }
};

int main() 
{
    std::cout << "=== 演算子オーバーロードの実例 ===" << std::endl;
    
    // === Vector3D の演算 ===
    std::cout << "\n--- 3Dベクトル演算 ---" << std::endl;
    
    Vector3D player_pos(10, 0, 5);
    Vector3D enemy_pos(8, 0, 7);
    Vector3D movement(1, 0, -1);
    
    std::cout << "プレイヤー位置: " << player_pos << std::endl;
    std::cout << "敵の位置: " << enemy_pos << std::endl;
    std::cout << "移動ベクトル: " << movement << std::endl;
    
    // ベクトル演算
    Vector3D distance = enemy_pos - player_pos;
    std::cout << "距離ベクトル: " << distance << std::endl;
    std::cout << "距離: " << distance.length() << std::endl;
    
    player_pos += movement * 2.0;
    std::cout << "移動後のプレイヤー位置: " << player_pos << std::endl;
    
    // 添字演算子
    std::cout << "X座標: " << player_pos[0] << std::endl;
    player_pos[1] = 5.0;  // Y座標を変更
    std::cout << "Y座標変更後: " << player_pos << std::endl;
    
    // === ゲーム通貨の演算 ===
    std::cout << "\n--- ゲーム通貨演算 ---" << std::endl;
    
    GameCurrency playerMoney(125350);  // 12G 53S 50C
    GameCurrency shopPrice(50075);     // 5G 0S 75C
    GameCurrency questReward(30000);   // 3G 0S 0C
    
    std::cout << "所持金: " << playerMoney << std::endl;
    std::cout << "商品価格: " << shopPrice << std::endl;
    std::cout << "クエスト報酬: " << questReward << std::endl;
    
    if (playerMoney >= shopPrice) 
    {
        playerMoney -= shopPrice;
        std::cout << "アイテムを購入しました！" << std::endl;
        std::cout << "残金: " << playerMoney << std::endl;
    }
    
    playerMoney += questReward;
    std::cout << "クエスト報酬獲得後: " << playerMoney << std::endl;
    
    // 税金計算（10%）
    GameCurrency tax = playerMoney * 0.1;
    std::cout << "税金（10%）: " << tax << std::endl;
    
    // === ゲームアイテムの比較とソート ===
    std::cout << "\n--- ゲームアイテム管理 ---" << std::endl;
    
    PlayerInventory inventory(10);
    
    // アイテムの追加
    inventory += GameItem("鉄の剣", 2, 15000);
    inventory += GameItem("炎の杖", 4, 80000);
    inventory += GameItem("革の鎧", 1, 5000);
    inventory += GameItem("ドラゴンの鱗", 5, 200000);
    inventory += GameItem("ポーション", 1, 500);
    
    std::cout << inventory << std::endl;
    
    // 添字演算子でアクセス
    try 
    {
        std::cout << "1番目のアイテム: " << inventory[0] << std::endl;
        std::cout << "3番目のアイテム: " << inventory[2] << std::endl;
    }
    catch (const std::out_of_range& e) 
    {
        std::cout << "エラー: " << e.what() << std::endl;
    }
    
    // 関数呼び出し演算子でアイテム使用
    inventory("ポーション");
    inventory("存在しないアイテム");
    
    // === カスタム演算子の実用例 ===
    std::cout << "\n--- 複合演算の例 ---" << std::endl;
    
    // 位置の更新（物理演算風）
    Vector3D velocity(2, 0, -1);
    Vector3D gravity(0, -9.8, 0);
    double deltaTime = 0.1;
    
    Vector3D newPosition = player_pos + velocity * deltaTime + gravity * (deltaTime * deltaTime) * 0.5;
    std::cout << "物理演算後の位置: " << newPosition << std::endl;
    
    // 通貨の複雑な計算
    GameCurrency basePrice(10000);  // 1G
    GameCurrency discountedPrice = basePrice * 0.8;  // 20%割引
    GameCurrency finalPrice = discountedPrice + GameCurrency(50);  // 手数料
    
    std::cout << "基本価格: " << basePrice << std::endl;
    std::cout << "割引価格: " << discountedPrice << std::endl;
    std::cout << "最終価格: " << finalPrice << std::endl;
    
    return 0;
}

/*
    まとめ:
    
    1. 算術演算子（+, -, *, /）:
       - 直感的な数学演算を提供
       - const メンバ関数として実装
       - 新しいオブジェクトを返す
    
    2. 複合代入演算子（+=, -=, *=, /=）:
       - 自身を変更して参照を返す
       - より効率的（一時オブジェクトなし）
    
    3. 比較演算子（==, !=, <, >, <=, >=）:
       - オブジェクトの順序付けやソートに使用
       - STLアルゴリズムとの連携
    
    4. 添字演算子（[]）:
       - 配列風のアクセスを提供
       - const版と非const版の両方を定義
    
    5. 入出力演算子（<<, >>）:
       - フレンド関数として実装
       - ストリームとの自然な連携
    
    6. 関数呼び出し演算子（()）:
       - オブジェクトを関数のように使用
       - ファンクタの実装
    
    7. 単項演算子（-, ++, --）:
       - 前置・後置の区別に注意
       - 適切な戻り値の型
    
    ベストプラクティス:
    - 直感的で自然な動作を実装
    - const correctness を守る
    - 例外安全性を考慮
    - 対応する演算子をセットで実装
    - フレンド関数の適切な使用
    
    ゲーム開発での活用:
    - 3Dベクトル・行列演算
    - ゲーム内通貨システム
    - 座標・位置計算
    - アイテム・インベントリ管理
    - 物理演算での数値計算
*/