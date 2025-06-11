// エラーハンドリング戦略
// 例外、エラーコード、std::optional、std::expectedを使った包括的なエラー処理

#include <iostream>
#include <optional>
#include <variant>
#include <expected> // C++23
#include <system_error>
#include <string>
#include <vector>
#include <functional>
#include <cassert>

// C++23のstd::expectedがまだ利用できない場合の簡易実装
#if __cplusplus < 202302L
template<typename T, typename E>
class expected
{
private:
    std::variant<T, E> data;
    
public:
    expected(const T& value) : data(value) {}
    expected(T&& value) : data(std::move(value)) {}
    
    template<typename... Args>
    static expected<T, E> make_error(Args&&... args)
    {
        expected<T, E> result;
        result.data = E(std::forward<Args>(args)...);
        return result;
    }
    
    bool has_value() const { return std::holds_alternative<T>(data); }
    T& value() { return std::get<T>(data); }
    const T& value() const { return std::get<T>(data); }
    E& error() { return std::get<E>(data); }
    const E& error() const { return std::get<E>(data); }
    
    explicit operator bool() const { return has_value(); }
    
private:
    expected() = default;
};
#else
using std::expected;
#endif

// エラーコード定義
enum class GameErrorCode
{
    Success = 0,
    InvalidParameter,
    ResourceNotFound,
    InsufficientResources,
    OperationFailed,
    NetworkError,
    SaveLoadError
};

// エラーカテゴリ
class GameErrorCategory : public std::error_category
{
public:
    const char* name() const noexcept override
    {
        return "GameError";
    }
    
    std::string message(int code) const override
    {
        switch (static_cast<GameErrorCode>(code))
        {
            case GameErrorCode::Success:
                return "成功";
            case GameErrorCode::InvalidParameter:
                return "無効なパラメータ";
            case GameErrorCode::ResourceNotFound:
                return "リソースが見つかりません";
            case GameErrorCode::InsufficientResources:
                return "リソースが不足しています";
            case GameErrorCode::OperationFailed:
                return "操作に失敗しました";
            case GameErrorCode::NetworkError:
                return "ネットワークエラー";
            case GameErrorCode::SaveLoadError:
                return "セーブ/ロードエラー";
            default:
                return "不明なエラー";
        }
    }
};

const GameErrorCategory& getGameErrorCategory()
{
    static GameErrorCategory category;
    return category;
}

std::error_code make_error_code(GameErrorCode code)
{
    return std::error_code(static_cast<int>(code), getGameErrorCategory());
}

// 戦略1: 従来のエラーコード方式
class TraditionalErrorHandler
{
public:
    struct ItemData
    {
        std::string name;
        int quantity;
        int value;
    };
    
    // エラーコードを返す関数
    GameErrorCode loadItem(const std::string& itemId, ItemData& outData)
    {
        if (itemId.empty())
        {
            return GameErrorCode::InvalidParameter;
        }
        
        // アイテムデータの検索（シミュレーション）
        if (itemId == "sword")
        {
            outData = {"鉄の剣", 1, 100};
            return GameErrorCode::Success;
        }
        else if (itemId == "potion")
        {
            outData = {"ポーション", 5, 50};
            return GameErrorCode::Success;
        }
        
        return GameErrorCode::ResourceNotFound;
    }
    
    // boolとエラーメッセージ
    bool saveGame(const std::string& filename, std::string& errorMsg)
    {
        if (filename.empty())
        {
            errorMsg = "ファイル名が空です";
            return false;
        }
        
        // セーブ処理（シミュレーション）
        if (filename.find(".sav") == std::string::npos)
        {
            errorMsg = "無効なファイル形式です";
            return false;
        }
        
        std::cout << "ゲームを " << filename << " に保存しました" << std::endl;
        return true;
    }
};

// 戦略2: std::optionalを使った方式
class OptionalErrorHandler
{
public:
    struct PlayerData
    {
        std::string name;
        int level;
        int experience;
    };
    
    // 値が存在しない可能性がある場合
    std::optional<PlayerData> findPlayer(int playerId)
    {
        // プレイヤー検索（シミュレーション）
        if (playerId == 1001)
        {
            return PlayerData{"勇者", 25, 15000};
        }
        else if (playerId == 1002)
        {
            return PlayerData{"魔法使い", 23, 13500};
        }
        
        return std::nullopt; // プレイヤーが見つからない
    }
    
    // 計算結果が無効な場合
    std::optional<int> calculateDamage(int attack, int defense)
    {
        if (attack < 0 || defense < 0)
        {
            return std::nullopt; // 無効なパラメータ
        }
        
        int damage = attack - defense / 2;
        if (damage < 0)
        {
            damage = 0;
        }
        
        return damage;
    }
};

// 戦略3: std::expectedを使った方式（C++23）
class ExpectedErrorHandler
{
public:
    struct Quest
    {
        std::string name;
        int requiredLevel;
        int reward;
    };
    
    // 成功または詳細なエラー情報
    expected<Quest, std::string> loadQuest(const std::string& questId)
    {
        if (questId.empty())
        {
            return expected<Quest, std::string>::make_error("クエストIDが空です");
        }
        
        if (questId == "main_01")
        {
            return Quest{"魔王討伐", 30, 10000};
        }
        else if (questId == "side_01")
        {
            return Quest{"薬草採取", 5, 100};
        }
        
        return expected<Quest, std::string>::make_error(
            "クエストID '" + questId + "' が見つかりません");
    }
    
    // 複雑なエラー情報
    struct CraftError
    {
        GameErrorCode code;
        std::string message;
        std::vector<std::string> missingMaterials;
    };
    
    expected<std::string, CraftError> craftItem(
        const std::string& recipe,
        const std::vector<std::string>& materials)
    {
        if (recipe == "強化ポーション")
        {
            std::vector<std::string> required = {"ポーション", "魔法の粉"};
            std::vector<std::string> missing;
            
            for (const auto& req : required)
            {
                if (std::find(materials.begin(), materials.end(), req) == materials.end())
                {
                    missing.push_back(req);
                }
            }
            
            if (!missing.empty())
            {
                return expected<std::string, CraftError>::make_error(
                    CraftError{
                        GameErrorCode::InsufficientResources,
                        "材料が不足しています",
                        missing
                    });
            }
            
            return "強化ポーションを作成しました！";
        }
        
        return expected<std::string, CraftError>::make_error(
            CraftError{
                GameErrorCode::ResourceNotFound,
                "レシピが見つかりません",
                {}
            });
    }
};

// 戦略4: 例外を使った方式
class ExceptionErrorHandler
{
public:
    class GameException : public std::exception
    {
    private:
        std::string message;
        GameErrorCode code;
        
    public:
        GameException(const std::string& msg, GameErrorCode c)
            : message(msg), code(c) {}
            
        const char* what() const noexcept override
        {
            return message.c_str();
        }
        
        GameErrorCode getCode() const noexcept { return code; }
    };
    
    // リソースを確実に管理する必要がある場合
    void executeSkill(const std::string& skillName, int manaCost)
    {
        if (skillName.empty())
        {
            throw GameException("スキル名が指定されていません", 
                              GameErrorCode::InvalidParameter);
        }
        
        // マナチェック（シミュレーション）
        int currentMana = 50;
        if (currentMana < manaCost)
        {
            throw GameException("マナが不足しています", 
                              GameErrorCode::InsufficientResources);
        }
        
        std::cout << skillName << " を発動！（マナ消費: " 
                  << manaCost << "）" << std::endl;
    }
};

// 統合的なゲームシステム
class GameSystem
{
private:
    TraditionalErrorHandler traditional;
    OptionalErrorHandler optional;
    ExpectedErrorHandler expected;
    ExceptionErrorHandler exception;
    
public:
    void demonstrateErrorHandling()
    {
        // 1. 従来のエラーコード
        std::cout << "=== エラーコード方式 ===" << std::endl;
        TraditionalErrorHandler::ItemData itemData;
        GameErrorCode code = traditional.loadItem("sword", itemData);
        if (code == GameErrorCode::Success)
        {
            std::cout << "アイテム取得: " << itemData.name 
                     << " x" << itemData.quantity << std::endl;
        }
        else
        {
            std::cout << "エラー: " << make_error_code(code).message() << std::endl;
        }
        
        // 2. std::optional
        std::cout << "\n=== std::optional方式 ===" << std::endl;
        auto player = optional.findPlayer(1001);
        if (player)
        {
            std::cout << "プレイヤー発見: " << player->name 
                     << " (Lv." << player->level << ")" << std::endl;
        }
        else
        {
            std::cout << "プレイヤーが見つかりません" << std::endl;
        }
        
        // 3. std::expected
        std::cout << "\n=== std::expected方式 ===" << std::endl;
        auto questResult = expected.loadQuest("main_01");
        if (questResult)
        {
            std::cout << "クエスト: " << questResult.value().name 
                     << " (必要Lv." << questResult.value().requiredLevel << ")" << std::endl;
        }
        else
        {
            std::cout << "エラー: " << questResult.error() << std::endl;
        }
        
        // クラフトの例
        std::vector<std::string> materials = {"ポーション"};
        auto craftResult = expected.craftItem("強化ポーション", materials);
        if (!craftResult)
        {
            std::cout << "クラフト失敗: " << craftResult.error().message << std::endl;
            std::cout << "不足している材料:" << std::endl;
            for (const auto& mat : craftResult.error().missingMaterials)
            {
                std::cout << "- " << mat << std::endl;
            }
        }
        
        // 4. 例外
        std::cout << "\n=== 例外方式 ===" << std::endl;
        try
        {
            exception.executeSkill("ファイアボール", 30);
            exception.executeSkill("メテオ", 100); // マナ不足
        }
        catch (const ExceptionErrorHandler::GameException& e)
        {
            std::cout << "スキルエラー: " << e.what() << std::endl;
        }
    }
    
    // エラーハンドリング戦略の選択ガイド
    void printGuidelines()
    {
        std::cout << "\n=== エラーハンドリング戦略の選択ガイド ===" << std::endl;
        
        std::cout << "\n1. エラーコード:" << std::endl;
        std::cout << "   - C APIとの互換性が必要な場合" << std::endl;
        std::cout << "   - パフォーマンスが最重要な場合" << std::endl;
        std::cout << "   - 例外が使えない環境" << std::endl;
        
        std::cout << "\n2. std::optional:" << std::endl;
        std::cout << "   - 値が存在しない可能性がある場合" << std::endl;
        std::cout << "   - エラーの詳細が不要な場合" << std::endl;
        std::cout << "   - 検索や計算の結果を表現" << std::endl;
        
        std::cout << "\n3. std::expected:" << std::endl;
        std::cout << "   - 成功/失敗の両方の情報が必要な場合" << std::endl;
        std::cout << "   - エラーの詳細情報を保持したい場合" << std::endl;
        std::cout << "   - 関数型プログラミングスタイル" << std::endl;
        
        std::cout << "\n4. 例外:" << std::endl;
        std::cout << "   - エラーが稀で異常な状況の場合" << std::endl;
        std::cout << "   - リソース管理（RAII）が重要な場合" << std::endl;
        std::cout << "   - エラーを上位層に伝播させたい場合" << std::endl;
    }
};

int main()
{
    std::cout << "=== エラーハンドリング戦略 ===" << std::endl;
    
    GameSystem system;
    system.demonstrateErrorHandling();
    system.printGuidelines();
    
    return 0;
}

// まとめ：
// - エラーコード：軽量で予測可能、C互換
// - std::optional：値の有無を表現、シンプル
// - std::expected：成功/失敗の両方の情報を保持
// - 例外：異常系の処理、RAII、スタック巻き戻し
// - 状況に応じて適切な戦略を選択することが重要