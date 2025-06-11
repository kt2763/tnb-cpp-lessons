/*
 * テンプレート第2講: クラステンプレートの基本
 * 
 * クラステンプレートは、異なる型で同じ構造を持つクラスを
 * 一つの定義で書けるようにする機能です。
 * ゲーム開発では、汎用的なコンテナやコンポーネントシステムで活用されます。
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>

// 基本的なクラステンプレート - ゲームアイテムスロット
template<typename T>
class GameSlot
{
private:
    T item;
    bool isEmpty;
    
public:
    // コンストラクタ
    GameSlot() : isEmpty(true) {}
    
    GameSlot(const T& newItem) : item(newItem), isEmpty(false) {}
    
    // アイテムをセット
    void setItem(const T& newItem)
    {
        item = newItem;
        isEmpty = false;
        std::cout << "アイテムをセットしました: " << item << std::endl;
    }
    
    // アイテムを取得
    T getItem() const
    {
        if (isEmpty)
        {
            throw std::runtime_error("スロットが空です！");
        }
        return item;
    }
    
    // スロットをクリア
    void clear()
    {
        isEmpty = true;
        std::cout << "スロットをクリアしました" << std::endl;
    }
    
    // 空かどうかチェック
    bool empty() const
    {
        return isEmpty;
    }
};

// より複雑な例: ゲーム用の汎用スタック
template<typename T>
class GameStack
{
private:
    std::vector<T> container;
    size_t maxSize;
    
public:
    explicit GameStack(size_t max = 10) : maxSize(max)
    {
        container.reserve(maxSize);
    }
    
    // アイテムをプッシュ
    bool push(const T& item)
    {
        if (container.size() >= maxSize)
        {
            std::cout << "スタックが満杯です！" << std::endl;
            return false;
        }
        
        container.push_back(item);
        std::cout << "追加: " << item << " (残り容量: " 
                  << (maxSize - container.size()) << ")" << std::endl;
        return true;
    }
    
    // アイテムをポップ
    T pop()
    {
        if (container.empty())
        {
            throw std::runtime_error("スタックが空です！");
        }
        
        T item = container.back();
        container.pop_back();
        std::cout << "取得: " << item << std::endl;
        return item;
    }
    
    // トップの要素を確認
    const T& top() const
    {
        if (container.empty())
        {
            throw std::runtime_error("スタックが空です！");
        }
        return container.back();
    }
    
    // 空かどうかチェック
    bool empty() const
    {
        return container.empty();
    }
    
    // サイズを取得
    size_t size() const
    {
        return container.size();
    }
    
    // 全要素を表示
    void display() const
    {
        std::cout << "=== スタック内容 (上から) ===" << std::endl;
        for (auto it = container.rbegin(); it != container.rend(); ++it)
        {
            std::cout << "  " << *it << std::endl;
        }
    }
};

// ゲーム用のキー・バリューペア
template<typename KeyType, typename ValueType>
class GameAttribute
{
private:
    KeyType key;
    ValueType value;
    
public:
    GameAttribute(const KeyType& k, const ValueType& v) : key(k), value(v) {}
    
    KeyType getKey() const { return key; }
    ValueType getValue() const { return value; }
    
    void setValue(const ValueType& newValue)
    {
        value = newValue;
        std::cout << key << " を " << value << " に更新しました" << std::endl;
    }
    
    void display() const
    {
        std::cout << key << ": " << value << std::endl;
    }
};

// 特殊化の例: boolに特化したゲームフラグ
template<>
class GameAttribute<std::string, bool>
{
private:
    std::string key;
    bool value;
    
public:
    GameAttribute(const std::string& k, bool v) : key(k), value(v) {}
    
    std::string getKey() const { return key; }
    bool getValue() const { return value; }
    
    void setValue(bool newValue)
    {
        value = newValue;
        std::cout << key << " を " << (value ? "ON" : "OFF") << " に切り替えました" << std::endl;
    }
    
    void toggle()
    {
        value = !value;
        std::cout << key << " を " << (value ? "ON" : "OFF") << " にトグルしました" << std::endl;
    }
    
    void display() const
    {
        std::cout << key << ": " << (value ? "有効" : "無効") << std::endl;
    }
};

int main()
{
    std::cout << "=== クラステンプレートのデモ ===" << std::endl;
    
    // 基本的なスロットの使用例
    std::cout << "\n1. ゲームスロットの基本操作:" << std::endl;
    GameSlot<std::string> weaponSlot;
    GameSlot<int> healthSlot(100);
    
    weaponSlot.setItem("炎の剣");
    std::cout << "装備中の武器: " << weaponSlot.getItem() << std::endl;
    
    std::cout << "現在のHP: " << healthSlot.getItem() << std::endl;
    
    // ゲームスタックの使用例
    std::cout << "\n2. アイテムスタックの操作:" << std::endl;
    GameStack<std::string> itemStack(5);
    
    itemStack.push("回復ポーション");
    itemStack.push("マナポーション");
    itemStack.push("解毒薬");
    
    itemStack.display();
    
    std::cout << "\nアイテム使用:" << std::endl;
    while (!itemStack.empty())
    {
        std::string item = itemStack.pop();
        std::cout << item << " を使用しました！" << std::endl;
    }
    
    // 数値スタックの例
    std::cout << "\n3. 数値スタック（経験値履歴）:" << std::endl;
    GameStack<int> expStack(3);
    expStack.push(150);
    expStack.push(200);
    expStack.push(175);
    
    std::cout << "最新の経験値獲得: " << expStack.top() << std::endl;
    expStack.display();
    
    // ゲーム属性の使用例
    std::cout << "\n4. ゲーム属性の管理:" << std::endl;
    GameAttribute<std::string, int> playerLevel("レベル", 25);
    GameAttribute<std::string, double> playerSpeed("移動速度", 12.5);
    GameAttribute<std::string, std::string> playerClass("職業", "戦士");
    
    playerLevel.display();
    playerSpeed.display();
    playerClass.display();
    
    std::cout << "\nレベルアップ！" << std::endl;
    playerLevel.setValue(26);
    
    // bool特殊化の使用例
    std::cout << "\n5. ゲームフラグの管理:" << std::endl;
    GameAttribute<std::string, bool> soundEnabled("サウンド", true);
    GameAttribute<std::string, bool> tutorialCompleted("チュートリアル完了", false);
    
    soundEnabled.display();
    tutorialCompleted.display();
    
    std::cout << "\n設定変更:" << std::endl;
    soundEnabled.toggle();
    tutorialCompleted.setValue(true);
    
    // 複数のスロットを管理
    std::cout << "\n6. 装備スロット管理:" << std::endl;
    GameSlot<std::string> helmet("鉄のヘルメット");
    GameSlot<std::string> armor("革の鎧");
    GameSlot<std::string> boots("運動靴");
    
    std::cout << "現在の装備:" << std::endl;
    std::cout << "頭: " << helmet.getItem() << std::endl;
    std::cout << "体: " << armor.getItem() << std::endl;
    std::cout << "足: " << boots.getItem() << std::endl;
    
    return 0;
}

/*
 * 学習ポイント:
 * 1. template<typename T> class ClassName でクラステンプレートを定義
 * 2. メンバ変数、メンバ関数でテンプレートパラメータを使用可能
 * 3. クラステンプレートの特殊化で特定の型に特別な実装を提供
 * 4. 複数のテンプレートパラメータを持つクラスも定義可能
 * 5. インスタンス化時に型を明示的に指定: GameSlot<string>
 * 6. ゲーム開発では汎用コンテナ、コンポーネントシステムで活用
 */