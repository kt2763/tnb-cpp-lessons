// リフレクションと内省
// 実行時に型情報を取得し、動的にオブジェクトを操作する技術

#include <iostream>
#include <typeinfo>
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <any>
#include <variant>

// 基本的なリフレクション情報
template<typename T>
class TypeInfo
{
public:
    static const char* name() { return typeid(T).name(); }
    static size_t size() { return sizeof(T); }
    static bool is_pointer() { return std::is_pointer_v<T>; }
    static bool is_class() { return std::is_class_v<T>; }
    static bool is_trivial() { return std::is_trivial_v<T>; }
    
    static void printInfo()
    {
        std::cout << "型名: " << name() << std::endl;
        std::cout << "サイズ: " << size() << " bytes" << std::endl;
        std::cout << "ポインタ型: " << (is_pointer() ? "Yes" : "No") << std::endl;
        std::cout << "クラス型: " << (is_class() ? "Yes" : "No") << std::endl;
        std::cout << "POD型: " << (is_trivial() ? "Yes" : "No") << std::endl;
    }
};

// プロパティシステム
class Property
{
private:
    std::string name_;
    std::any value_;
    std::function<std::string()> toString_;
    
public:
    template<typename T>
    Property(const std::string& name, const T& value)
        : name_(name), value_(value)
    {
        toString_ = [value]() { 
            if constexpr (std::is_arithmetic_v<T>)
            {
                return std::to_string(value);
            }
            else if constexpr (std::is_same_v<T, std::string>)
            {
                return value;
            }
            else
            {
                return std::string("Custom Type");
            }
        };
    }
    
    const std::string& getName() const { return name_; }
    
    template<typename T>
    T getValue() const
    {
        return std::any_cast<T>(value_);
    }
    
    template<typename T>
    void setValue(const T& newValue)
    {
        value_ = newValue;
    }
    
    std::string toString() const { return toString_(); }
};

// ゲームオブジェクトの基底クラス
class GameObject
{
protected:
    std::string name_;
    std::map<std::string, Property> properties_;
    
public:
    GameObject(const std::string& name) : name_(name) {}
    virtual ~GameObject() = default;
    
    const std::string& getName() const { return name_; }
    
    template<typename T>
    void setProperty(const std::string& propName, const T& value)
    {
        properties_[propName] = Property(propName, value);
    }
    
    template<typename T>
    T getProperty(const std::string& propName) const
    {
        auto it = properties_.find(propName);
        if (it != properties_.end())
        {
            return it->second.getValue<T>();
        }
        throw std::runtime_error("Property not found: " + propName);
    }
    
    bool hasProperty(const std::string& propName) const
    {
        return properties_.find(propName) != properties_.end();
    }
    
    void printProperties() const
    {
        std::cout << name_ << " のプロパティ:" << std::endl;
        for (const auto& [name, prop] : properties_)
        {
            std::cout << "  " << name << ": " << prop.toString() << std::endl;
        }
    }
    
    virtual std::string getTypeName() const = 0;
};

// プレイヤークラス
class Player : public GameObject
{
public:
    Player(const std::string& name) : GameObject(name)
    {
        setProperty("health", 100);
        setProperty("mana", 50);
        setProperty("level", 1);
        setProperty("experience", 0);
    }
    
    std::string getTypeName() const override { return "Player"; }
    
    void levelUp()
    {
        int currentLevel = getProperty<int>("level");
        setProperty("level", currentLevel + 1);
        setProperty("health", 100 + currentLevel * 10);
        setProperty("mana", 50 + currentLevel * 5);
        std::cout << name_ << " がレベルアップ！ Lv." << (currentLevel + 1) << std::endl;
    }
};

// 敵クラス
class Enemy : public GameObject
{
public:
    Enemy(const std::string& name, const std::string& type) : GameObject(name)
    {
        setProperty("type", type);
        setProperty("health", 80);
        setProperty("attackPower", 15);
        setProperty("defense", 5);
        setProperty("aggressive", true);
    }
    
    std::string getTypeName() const override { return "Enemy"; }
};

// アイテムクラス
class Item : public GameObject
{
public:
    Item(const std::string& name, const std::string& category) : GameObject(name)
    {
        setProperty("category", category);
        setProperty("value", 100);
        setProperty("weight", 1.0f);
        setProperty("stackable", false);
    }
    
    std::string getTypeName() const override { return "Item"; }
};

// オブジェクトファクトリ
class GameObjectFactory
{
private:
    using CreateFunc = std::function<std::unique_ptr<GameObject>(const std::string&)>;
    std::map<std::string, CreateFunc> creators_;
    
public:
    template<typename T>
    void registerType(const std::string& typeName)
    {
        creators_[typeName] = [](const std::string& name) {
            if constexpr (std::is_same_v<T, Player>)
            {
                return std::make_unique<Player>(name);
            }
            else if constexpr (std::is_same_v<T, Enemy>)
            {
                return std::make_unique<Enemy>(name, "Generic");
            }
            else if constexpr (std::is_same_v<T, Item>)
            {
                return std::make_unique<Item>(name, "Misc");
            }
            return std::unique_ptr<GameObject>{};
        };
    }
    
    std::unique_ptr<GameObject> create(const std::string& typeName, const std::string& name)
    {
        auto it = creators_.find(typeName);
        if (it != creators_.end())
        {
            return it->second(name);
        }
        return nullptr;
    }
    
    std::vector<std::string> getRegisteredTypes() const
    {
        std::vector<std::string> types;
        for (const auto& [type, _] : creators_)
        {
            types.push_back(type);
        }
        return types;
    }
};

// プロパティの動的操作
class PropertyEditor
{
public:
    static void editProperty(GameObject& obj, const std::string& propName)
    {
        if (!obj.hasProperty(propName))
        {
            std::cout << "プロパティ '" << propName << "' が見つかりません" << std::endl;
            return;
        }
        
        std::cout << obj.getName() << " の " << propName << " を編集中..." << std::endl;
        
        // 型に応じた編集処理をシミュレート
        try
        {
            // 整数として試行
            try
            {
                int currentValue = obj.getProperty<int>(propName);
                std::cout << "現在の値（int）: " << currentValue << std::endl;
                obj.setProperty(propName, currentValue + 10);
                std::cout << "新しい値: " << obj.getProperty<int>(propName) << std::endl;
                return;
            }
            catch (...)
            {
                // 文字列として試行
                try
                {
                    std::string currentValue = obj.getProperty<std::string>(propName);
                    std::cout << "現在の値（string）: " << currentValue << std::endl;
                    obj.setProperty(propName, currentValue + "_modified");
                    std::cout << "新しい値: " << obj.getProperty<std::string>(propName) << std::endl;
                    return;
                }
                catch (...)
                {
                    // 浮動小数点として試行
                    try
                    {
                        float currentValue = obj.getProperty<float>(propName);
                        std::cout << "現在の値（float）: " << currentValue << std::endl;
                        obj.setProperty(propName, currentValue * 1.5f);
                        std::cout << "新しい値: " << obj.getProperty<float>(propName) << std::endl;
                        return;
                    }
                    catch (...)
                    {
                        // ブール値として試行
                        try
                        {
                            bool currentValue = obj.getProperty<bool>(propName);
                            std::cout << "現在の値（bool）: " << currentValue << std::endl;
                            obj.setProperty(propName, !currentValue);
                            std::cout << "新しい値: " << obj.getProperty<bool>(propName) << std::endl;
                            return;
                        }
                        catch (...)
                        {
                            std::cout << "サポートされていない型です" << std::endl;
                        }
                    }
                }
            }
        }
        catch (const std::exception& e)
        {
            std::cout << "エラー: " << e.what() << std::endl;
        }
    }
};

// シリアライゼーション
class Serializer
{
public:
    static std::string serialize(const GameObject& obj)
    {
        std::string result = "Object: " + obj.getName() + "\n";
        result += "Type: " + obj.getTypeName() + "\n";
        result += "Properties:\n";
        
        // プロパティにアクセスするためのハック
        // 実際の実装では、プロパティのイテレータが必要
        return result;
    }
    
    template<typename T>
    static std::string serializeType()
    {
        std::string result;
        result += "Type Name: " + std::string(typeid(T).name()) + "\n";
        result += "Size: " + std::to_string(sizeof(T)) + "\n";
        result += "Is Trivial: " + std::to_string(std::is_trivial_v<T>) + "\n";
        return result;
    }
};

// 型の検査システム
class TypeInspector
{
public:
    template<typename T>
    static void inspect(const T& obj)
    {
        std::cout << "=== 型検査結果 ===" << std::endl;
        TypeInfo<T>::printInfo();
        
        if constexpr (std::is_base_of_v<GameObject, T>)
        {
            std::cout << "ゲームオブジェクトです" << std::endl;
            obj.printProperties();
        }
        else
        {
            std::cout << "プリミティブ型またはカスタム型です" << std::endl;
        }
    }
    
    static void inspectPolymorphic(const GameObject& obj)
    {
        std::cout << "=== ポリモーフィック検査 ===" << std::endl;
        std::cout << "実際の型: " << obj.getTypeName() << std::endl;
        std::cout << "型ID: " << typeid(obj).name() << std::endl;
        obj.printProperties();
    }
};

int main()
{
    std::cout << "=== リフレクションと内省 ===" << std::endl;
    
    // 1. 基本的な型情報
    std::cout << "\n--- 型情報の取得 ---" << std::endl;
    TypeInspector::inspect(42);
    std::cout << std::endl;
    TypeInspector::inspect(std::string("Hello"));
    
    // 2. ゲームオブジェクトの作成と操作
    std::cout << "\n--- ゲームオブジェクト ---" << std::endl;
    Player hero("勇者");
    Enemy goblin("ゴブリン", "Small");
    Item sword("鉄の剣", "Weapon");
    
    TypeInspector::inspect(hero);
    std::cout << std::endl;
    TypeInspector::inspectPolymorphic(goblin);
    
    // 3. 動的プロパティ操作
    std::cout << "\n--- プロパティ編集 ---" << std::endl;
    PropertyEditor::editProperty(hero, "health");
    PropertyEditor::editProperty(hero, "level");
    PropertyEditor::editProperty(goblin, "type");
    PropertyEditor::editProperty(sword, "weight");
    
    hero.printProperties();
    
    // 4. ファクトリシステム
    std::cout << "\n--- オブジェクトファクトリ ---" << std::endl;
    GameObjectFactory factory;
    factory.registerType<Player>("Player");
    factory.registerType<Enemy>("Enemy");
    factory.registerType<Item>("Item");
    
    std::cout << "登録された型:" << std::endl;
    for (const auto& type : factory.getRegisteredTypes())
    {
        std::cout << "- " << type << std::endl;
    }
    
    auto dynamicPlayer = factory.create("Player", "動的プレイヤー");
    if (dynamicPlayer)
    {
        TypeInspector::inspectPolymorphic(*dynamicPlayer);
    }
    
    // 5. シリアライゼーション
    std::cout << "\n--- シリアライゼーション ---" << std::endl;
    std::cout << Serializer::serializeType<Player>() << std::endl;
    std::cout << Serializer::serializeType<int>() << std::endl;
    
    // 6. レベルアップシステム
    std::cout << "\n--- レベルアップ ---" << std::endl;
    hero.levelUp();
    hero.printProperties();
    
    return 0;
}

// まとめ：
// - typeidやtype_traitsで実行時型情報を取得
// - プロパティシステムで動的な属性管理
// - ファクトリパターンで型名からオブジェクト生成
// - シリアライゼーションや動的編集を実現
// - ゲーム開発でのデータ駆動設計に有効