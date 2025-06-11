/*
 * テンプレート第4講: テンプレートパラメータの種類
 * 
 * テンプレートパラメータには、型パラメータ、非型パラメータ、
 * テンプレートテンプレートパラメータの3種類があります。
 * ゲーム開発では、コンパイル時の最適化と柔軟性の向上に活用されます。
 */

#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <memory>

// 1. 型パラメータ（Type Parameters）
template<typename T, typename U = int>
class GamePair
{
private:
    T first;
    U second;
    
public:
    GamePair(const T& f, const U& s) : first(f), second(s) {}
    
    void display() const
    {
        std::cout << "ペア: (" << first << ", " << second << ")" << std::endl;
    }
    
    T getFirst() const { return first; }
    U getSecond() const { return second; }
};

// 2. 非型パラメータ（Non-type Parameters）
template<typename T, size_t Size>
class FixedGameArray
{
private:
    std::array<T, Size> data;
    size_t currentSize;
    
public:
    FixedGameArray() : currentSize(0) {}
    
    bool add(const T& item)
    {
        if (currentSize >= Size)
        {
            std::cout << "配列が満杯です！ (最大: " << Size << ")" << std::endl;
            return false;
        }
        
        data[currentSize++] = item;
        std::cout << "追加: " << item << " (使用領域: " << currentSize << "/" << Size << ")" << std::endl;
        return true;
    }
    
    void displayAll() const
    {
        std::cout << "=== 固定サイズ配列 (サイズ: " << Size << ") ===" << std::endl;
        for (size_t i = 0; i < currentSize; ++i)
        {
            std::cout << "[" << i << "] " << data[i] << std::endl;
        }
    }
    
    constexpr size_t maxSize() const { return Size; }
    size_t size() const { return currentSize; }
};

// 非型パラメータでゲーム設定を定義
template<int MaxHP, int MaxMP, bool HasMagic = true>
class GameCharacter
{
private:
    int currentHP;
    int currentMP;
    std::string name;
    
public:
    GameCharacter(const std::string& n) : name(n), currentHP(MaxHP), currentMP(MaxMP) {}
    
    void displayStatus() const
    {
        std::cout << "=== " << name << " のステータス ===" << std::endl;
        std::cout << "HP: " << currentHP << "/" << MaxHP << std::endl;
        if constexpr (HasMagic)
        {
            std::cout << "MP: " << currentMP << "/" << MaxMP << std::endl;
        }
        else
        {
            std::cout << "魔法は使用できません" << std::endl;
        }
    }
    
    void heal(int amount)
    {
        currentHP = std::min(currentHP + amount, MaxHP);
        std::cout << name << " は " << amount << " HP回復しました" << std::endl;
    }
    
    template<bool Magic = HasMagic>
    typename std::enable_if<Magic, void>::type
    castSpell(int cost)
    {
        if (currentMP >= cost)
        {
            currentMP -= cost;
            std::cout << name << " は魔法を使用しました (MP消費: " << cost << ")" << std::endl;
        }
        else
        {
            std::cout << name << " のMPが不足しています" << std::endl;
        }
    }
    
    constexpr int getMaxHP() const { return MaxHP; }
    constexpr int getMaxMP() const { return MaxMP; }
    constexpr bool canUseMagic() const { return HasMagic; }
};

// 3. テンプレートテンプレートパラメータ
template<typename T, template<typename> class Container = std::vector>
class GameInventory
{
private:
    Container<T> items;
    
public:
    void addItem(const T& item)
    {
        items.push_back(item);
        std::cout << "アイテム追加: " << item << std::endl;
    }
    
    void displayItems() const
    {
        std::cout << "=== インベントリ ===" << std::endl;
        size_t index = 1;
        for (const auto& item : items)
        {
            std::cout << index++ << ". " << item << std::endl;
        }
    }
    
    size_t size() const
    {
        return items.size();
    }
    
    const Container<T>& getContainer() const
    {
        return items;
    }
};

// カスタムコンテナの例
template<typename T>
class SimpleList
{
private:
    std::vector<T> data;
    
public:
    void push_back(const T& item) { data.push_back(item); }
    size_t size() const { return data.size(); }
    
    typename std::vector<T>::const_iterator begin() const { return data.begin(); }
    typename std::vector<T>::const_iterator end() const { return data.end(); }
};

// 複数のテンプレートパラメータを組み合わせた例
template<typename ValueType, size_t Capacity, template<typename> class Storage = std::vector>
class AdvancedGameContainer
{
private:
    Storage<ValueType> storage;
    
public:
    bool add(const ValueType& value)
    {
        if (storage.size() >= Capacity)
        {
            std::cout << "容量超過！ (最大: " << Capacity << ")" << std::endl;
            return false;
        }
        
        storage.push_back(value);
        std::cout << "追加成功: " << value << " (容量: " << storage.size() << "/" << Capacity << ")" << std::endl;
        return true;
    }
    
    void displayAll() const
    {
        std::cout << "=== 高度なコンテナ (容量: " << Capacity << ") ===" << std::endl;
        size_t index = 1;
        for (const auto& item : storage)
        {
            std::cout << index++ << ". " << item << std::endl;
        }
    }
    
    constexpr size_t maxCapacity() const { return Capacity; }
    size_t currentSize() const { return storage.size(); }
};

// デフォルトテンプレート引数の活用例
template<typename T, size_t DefaultSize = 10, bool EnableLogging = false>
class GameBuffer
{
private:
    std::array<T, DefaultSize> buffer;
    size_t pos;
    
    void log(const std::string& message) const
    {
        if constexpr (EnableLogging)
        {
            std::cout << "[LOG] " << message << std::endl;
        }
    }
    
public:
    GameBuffer() : pos(0)
    {
        log("バッファ初期化 (サイズ: " + std::to_string(DefaultSize) + ")");
    }
    
    bool push(const T& item)
    {
        if (pos >= DefaultSize)
        {
            log("バッファがフル");
            return false;
        }
        
        buffer[pos++] = item;
        log("アイテムをプッシュ: " + std::to_string(pos) + "/" + std::to_string(DefaultSize));
        return true;
    }
    
    void displayStatus() const
    {
        std::cout << "バッファ状態: " << pos << "/" << DefaultSize;
        if constexpr (EnableLogging)
        {
            std::cout << " (ログ有効)";
        }
        std::cout << std::endl;
    }
};

int main()
{
    std::cout << "=== テンプレートパラメータのデモ ===" << std::endl;
    
    // 1. 型パラメータの使用例
    std::cout << "\n1. 型パラメータ (GamePair):" << std::endl;
    GamePair<std::string, int> weaponPower("炎の剣", 150);
    GamePair<std::string, double> playerSpeed("移動速度", 12.5);
    GamePair<std::string> defaultPair("デフォルト値", 100); // Uはデフォルトでint
    
    weaponPower.display();
    playerSpeed.display();
    defaultPair.display();
    
    // 2. 非型パラメータの使用例
    std::cout << "\n2. 非型パラメータ (固定サイズ配列):" << std::endl;
    FixedGameArray<std::string, 3> weaponSlots;
    weaponSlots.add("鉄の剣");
    weaponSlots.add("炎の杖");
    weaponSlots.add("氷の弓");
    weaponSlots.add("雷の槍"); // 容量超過
    weaponSlots.displayAll();
    
    std::cout << "\n3. ゲームキャラクター (非型パラメータ):" << std::endl;
    GameCharacter<100, 50, true> mage("魔法使いアリス");
    GameCharacter<200, 0, false> warrior("戦士ボブ");
    
    mage.displayStatus();
    mage.heal(20);
    mage.castSpell(15);
    mage.displayStatus();
    
    warrior.displayStatus();
    warrior.heal(30);
    // warrior.castSpell(10); // コンパイルエラー: 魔法使用不可
    
    // 4. テンプレートテンプレートパラメータ
    std::cout << "\n4. テンプレートテンプレートパラメータ:" << std::endl;
    GameInventory<std::string> standardInventory; // デフォルトでstd::vector
    GameInventory<std::string, SimpleList> customInventory; // カスタムコンテナ
    
    standardInventory.addItem("回復ポーション");
    standardInventory.addItem("マナポーション");
    standardInventory.displayItems();
    
    customInventory.addItem("特別な薬");
    customInventory.addItem("レアアイテム");
    customInventory.displayItems();
    
    // 5. 複合的な例
    std::cout << "\n5. 複合的なテンプレートパラメータ:" << std::endl;
    AdvancedGameContainer<std::string, 2> advancedContainer;
    advancedContainer.add("アイテム1");
    advancedContainer.add("アイテム2");
    advancedContainer.add("アイテム3"); // 容量超過
    advancedContainer.displayAll();
    
    // 6. デフォルト引数の活用
    std::cout << "\n6. デフォルト引数の活用:" << std::endl;
    GameBuffer<int> defaultBuffer; // サイズ10、ログなし
    GameBuffer<int, 5> smallBuffer; // サイズ5、ログなし
    GameBuffer<int, 3, true> loggingBuffer; // サイズ3、ログ有り
    
    std::cout << "デフォルトバッファ:" << std::endl;
    defaultBuffer.push(1);
    defaultBuffer.push(2);
    defaultBuffer.displayStatus();
    
    std::cout << "\nログ有効バッファ:" << std::endl;
    loggingBuffer.push(100);
    loggingBuffer.push(200);
    loggingBuffer.displayStatus();
    
    // 7. コンパイル時定数の活用
    std::cout << "\n7. コンパイル時定数:" << std::endl;
    constexpr size_t maxItems = weaponSlots.maxSize();
    constexpr int maxHP = mage.getMaxHP();
    constexpr bool canCastMagic = mage.canUseMagic();
    
    std::cout << "武器スロット最大数: " << maxItems << std::endl;
    std::cout << "魔法使いの最大HP: " << maxHP << std::endl;
    std::cout << "魔法使用可能: " << (canCastMagic ? "はい" : "いいえ") << std::endl;
    
    return 0;
}

/*
 * 学習ポイント:
 * 1. 型パラメータ: template<typename T> - 任意の型を受け取る
 * 2. 非型パラメータ: template<int N> - コンパイル時定数を受け取る
 * 3. テンプレートテンプレートパラメータ: template<template<typename> class C>
 * 4. デフォルト引数でテンプレートの使いやすさ向上
 * 5. constexprとif constexprでコンパイル時分岐
 * 6. 非型パラメータはパフォーマンス最適化に有効
 * 7. ゲーム開発では設定値をテンプレートパラメータ化して最適化
 */