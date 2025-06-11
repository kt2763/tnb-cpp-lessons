/*
 * テンプレート第3講: テンプレート特殊化
 * 
 * テンプレート特殊化は、特定の型に対して特別な実装を提供する機能です。
 * ゲーム開発では、特定のデータ型に最適化された処理や
 * 異なる動作が必要な場合に使用されます。
 */

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

// 基本テンプレート: ゲームオブジェクトの情報表示
template<typename T>
class GameObjectInfo
{
public:
    static void display(const T& obj)
    {
        std::cout << "汎用オブジェクト: " << obj << std::endl;
    }
    
    static std::string getType()
    {
        return "Unknown";
    }
    
    static void performAction(const T& obj)
    {
        std::cout << "汎用アクション: " << obj << std::endl;
    }
};

// 完全特殊化: int型（プレイヤーレベル）に対する特殊化
template<>
class GameObjectInfo<int>
{
public:
    static void display(const int& level)
    {
        std::cout << "プレイヤーレベル: " << level;
        if (level >= 50)
        {
            std::cout << " (上級者)";
        }
        else if (level >= 25)
        {
            std::cout << " (中級者)";
        }
        else
        {
            std::cout << " (初心者)";
        }
        std::cout << std::endl;
    }
    
    static std::string getType()
    {
        return "PlayerLevel";
    }
    
    static void performAction(const int& level)
    {
        std::cout << "レベル " << level << " でスキル習得チェック中..." << std::endl;
    }
};

// 完全特殊化: string型（プレイヤー名）に対する特殊化
template<>
class GameObjectInfo<std::string>
{
public:
    static void display(const std::string& name)
    {
        std::cout << "勇者の名前: \"" << name << "\"";
        if (name.length() > 10)
        {
            std::cout << " (長い名前)";
        }
        std::cout << std::endl;
    }
    
    static std::string getType()
    {
        return "PlayerName";
    }
    
    static void performAction(const std::string& name)
    {
        std::cout << name << " が冒険に出発しました！" << std::endl;
    }
};

// 部分特殊化の例: ポインタ型に対する特殊化
template<typename T>
class GameObjectInfo<T*>
{
public:
    static void display(T* const& ptr)
    {
        if (ptr)
        {
            std::cout << "ポインタオブジェクト (有効): ";
            GameObjectInfo<T>::display(*ptr);
        }
        else
        {
            std::cout << "ポインタオブジェクト: nullptr" << std::endl;
        }
    }
    
    static std::string getType()
    {
        return "Pointer<" + GameObjectInfo<T>::getType() + ">";
    }
    
    static void performAction(T* const& ptr)
    {
        if (ptr)
        {
            std::cout << "ポインタ経由でアクション実行:" << std::endl;
            GameObjectInfo<T>::performAction(*ptr);
        }
        else
        {
            std::cout << "nullptrのためアクション実行不可" << std::endl;
        }
    }
};

// 関数テンプレートの特殊化例
template<typename T>
T calculateDamage(T baseDamage, T multiplier)
{
    return baseDamage * multiplier;
}

// double型に対する特殊化（より精密な計算）
template<>
double calculateDamage<double>(double baseDamage, double multiplier)
{
    // より複雑な計算ロジック
    double result = baseDamage * multiplier;
    // 小数点以下2桁で丸める
    return std::round(result * 100.0) / 100.0;
}

// string型に対する特殊化（ダメージ文字列の生成）
template<>
std::string calculateDamage<std::string>(std::string baseDamage, std::string multiplier)
{
    return "ダメージ計算: " + baseDamage + " × " + multiplier;
}

// ゲーム用の汎用コンテナ
template<typename T>
class GameContainer
{
private:
    std::vector<T> items;
    
public:
    void add(const T& item)
    {
        items.push_back(item);
        std::cout << "アイテム追加: " << item << std::endl;
    }
    
    void displayAll() const
    {
        std::cout << "=== 全アイテム ===" << std::endl;
        for (const auto& item : items)
        {
            GameObjectInfo<T>::display(item);
        }
    }
    
    size_t size() const
    {
        return items.size();
    }
};

// bool型に対する特殊化（フラグ管理）
template<>
class GameContainer<bool>
{
private:
    std::vector<std::pair<std::string, bool>> flags;
    
public:
    void add(const bool& flag)
    {
        std::string flagName = "フラグ" + std::to_string(flags.size() + 1);
        flags.emplace_back(flagName, flag);
        std::cout << "フラグ追加: " << flagName << " = " << (flag ? "ON" : "OFF") << std::endl;
    }
    
    void displayAll() const
    {
        std::cout << "=== ゲームフラグ一覧 ===" << std::endl;
        for (const auto& [name, value] : flags)
        {
            std::cout << name << ": " << (value ? "有効" : "無効") << std::endl;
        }
    }
    
    size_t size() const
    {
        return flags.size();
    }
    
    void toggleFlag(size_t index)
    {
        if (index < flags.size())
        {
            flags[index].second = !flags[index].second;
            std::cout << flags[index].first << " を " 
                      << (flags[index].second ? "ON" : "OFF") << " に切り替えました" << std::endl;
        }
    }
};

int main()
{
    std::cout << "=== テンプレート特殊化のデモ ===" << std::endl;
    
    // 汎用テンプレートの使用
    std::cout << "\n1. 汎用テンプレートの使用:" << std::endl;
    GameObjectInfo<double>::display(123.45);
    std::cout << "型: " << GameObjectInfo<double>::getType() << std::endl;
    GameObjectInfo<double>::performAction(123.45);
    
    // int型特殊化の使用
    std::cout << "\n2. int型特殊化（プレイヤーレベル）:" << std::endl;
    GameObjectInfo<int>::display(15);
    GameObjectInfo<int>::display(35);
    GameObjectInfo<int>::display(60);
    std::cout << "型: " << GameObjectInfo<int>::getType() << std::endl;
    GameObjectInfo<int>::performAction(25);
    
    // string型特殊化の使用
    std::cout << "\n3. string型特殊化（プレイヤー名）:" << std::endl;
    GameObjectInfo<std::string>::display("アリス");
    GameObjectInfo<std::string>::display("非常に長いプレイヤー名前");
    std::cout << "型: " << GameObjectInfo<std::string>::getType() << std::endl;
    GameObjectInfo<std::string>::performAction("勇者ボブ");
    
    // ポインタ型特殊化の使用
    std::cout << "\n4. ポインタ型特殊化:" << std::endl;
    int level = 42;
    int* levelPtr = &level;
    int* nullPtr = nullptr;
    
    GameObjectInfo<int*>::display(levelPtr);
    GameObjectInfo<int*>::display(nullPtr);
    std::cout << "型: " << GameObjectInfo<int*>::getType() << std::endl;
    GameObjectInfo<int*>::performAction(levelPtr);
    GameObjectInfo<int*>::performAction(nullPtr);
    
    // 関数テンプレート特殊化の使用
    std::cout << "\n5. 関数テンプレート特殊化:" << std::endl;
    
    // 汎用テンプレート（int）
    int intDamage = calculateDamage(50, 2);
    std::cout << "整数ダメージ: " << intDamage << std::endl;
    
    // double型特殊化
    double doubleDamage = calculateDamage(75.456, 1.25);
    std::cout << "小数ダメージ: " << doubleDamage << std::endl;
    
    // string型特殊化
    std::string stringDamage = calculateDamage(std::string("100"), std::string("1.5"));
    std::cout << stringDamage << std::endl;
    
    // コンテナの使用例
    std::cout << "\n6. コンテナの特殊化:" << std::endl;
    
    // 汎用コンテナ（string）
    GameContainer<std::string> weaponContainer;
    weaponContainer.add("鉄の剣");
    weaponContainer.add("炎の杖");
    weaponContainer.displayAll();
    
    // bool型特殊化コンテナ
    std::cout << "\n7. bool型特殊化コンテナ（フラグ管理）:" << std::endl;
    GameContainer<bool> flagContainer;
    flagContainer.add(true);
    flagContainer.add(false);
    flagContainer.add(true);
    flagContainer.displayAll();
    
    std::cout << "\nフラグ操作:" << std::endl;
    flagContainer.toggleFlag(1);
    flagContainer.displayAll();
    
    return 0;
}

/*
 * 学習ポイント:
 * 1. template<> で完全特殊化を定義
 * 2. template<typename T> class Name<T*> で部分特殊化
 * 3. 特殊化により特定の型に最適化された実装を提供
 * 4. 関数テンプレートも特殊化可能
 * 5. 特殊化はより具体的なマッチングが優先される
 * 6. ゲーム開発では型ごとの最適化でパフォーマンス向上
 * 7. 特殊化を使って型安全性と使いやすさを両立
 */