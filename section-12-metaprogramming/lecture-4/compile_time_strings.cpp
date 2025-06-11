// コンパイル時文字列処理
// 文字列をコンパイル時に操作し、効率的なコードを生成

#include <iostream>
#include <array>
#include <string_view>
#include <algorithm>
#include <utility>

// 基本的なコンパイル時文字列
template<size_t N>
struct CompileTimeString
{
    char data[N];
    
    constexpr CompileTimeString() : data{} {}
    
    constexpr CompileTimeString(const char (&str)[N])
    {
        for (size_t i = 0; i < N; ++i)
        {
            data[i] = str[i];
        }
    }
    
    constexpr size_t size() const { return N - 1; }  // null終端を除く
    constexpr const char* c_str() const { return data; }
    constexpr operator std::string_view() const { return {data, size()}; }
};

// C++17 deduction guide
template<size_t N>
CompileTimeString(const char (&)[N]) -> CompileTimeString<N>;

// 文字列リテラルからの型生成
template<CompileTimeString str>
struct StringConstant
{
    static constexpr auto value = str;
    static constexpr size_t length = str.size();
};

// コンパイル時文字列操作
template<size_t N, size_t M>
constexpr auto concatenate(const CompileTimeString<N>& s1, const CompileTimeString<M>& s2)
{
    CompileTimeString<N + M - 1> result;
    
    size_t idx = 0;
    // 最初の文字列をコピー（null終端を除く）
    for (size_t i = 0; i < N - 1; ++i)
    {
        result.data[idx++] = s1.data[i];
    }
    
    // 二番目の文字列をコピー（null終端を含む）
    for (size_t i = 0; i < M; ++i)
    {
        result.data[idx++] = s2.data[i];
    }
    
    return result;
}

// ハッシュ値のコンパイル時計算
constexpr uint32_t fnv1a_32(std::string_view str)
{
    uint32_t hash = 2166136261u;
    for (char c : str)
    {
        hash ^= static_cast<uint32_t>(c);
        hash *= 16777619u;
    }
    return hash;
}

// ゲームコマンドのコンパイル時マッピング
template<CompileTimeString cmd>
struct GameCommand
{
    static constexpr auto name = cmd;
    static constexpr uint32_t hash = fnv1a_32(cmd);
};

// コマンドの定義
using AttackCommand = GameCommand<"attack">;
using DefendCommand = GameCommand<"defend">;
using HealCommand = GameCommand<"heal">;
using RunCommand = GameCommand<"run">;

// スイッチ文での使用
void executeCommand(uint32_t commandHash)
{
    switch (commandHash)
    {
        case AttackCommand::hash:
            std::cout << "攻撃を実行！" << std::endl;
            break;
        case DefendCommand::hash:
            std::cout << "防御態勢！" << std::endl;
            break;
        case HealCommand::hash:
            std::cout << "回復魔法を使用！" << std::endl;
            break;
        case RunCommand::hash:
            std::cout << "逃走を試みる！" << std::endl;
            break;
        default:
            std::cout << "不明なコマンド" << std::endl;
    }
}

// より高度な文字列クラス
template<size_t Capacity>
class StaticString
{
private:
    std::array<char, Capacity + 1> buffer;
    size_t length_;
    
public:
    constexpr StaticString() : buffer{}, length_(0) {}
    
    constexpr StaticString(const char* str) : buffer{}, length_(0)
    {
        while (str[length_] && length_ < Capacity)
        {
            buffer[length_] = str[length_];
            ++length_;
        }
        buffer[length_] = '\0';
    }
    
    constexpr size_t size() const { return length_; }
    constexpr size_t capacity() const { return Capacity; }
    constexpr const char* c_str() const { return buffer.data(); }
    
    // 文字列操作
    constexpr StaticString& append(const char* str)
    {
        size_t i = 0;
        while (str[i] && length_ < Capacity)
        {
            buffer[length_++] = str[i++];
        }
        buffer[length_] = '\0';
        return *this;
    }
    
    constexpr bool contains(char c) const
    {
        for (size_t i = 0; i < length_; ++i)
        {
            if (buffer[i] == c) return true;
        }
        return false;
    }
    
    constexpr StaticString substr(size_t pos, size_t len = Capacity) const
    {
        StaticString result;
        size_t end = std::min(pos + len, length_);
        for (size_t i = pos; i < end; ++i)
        {
            result.buffer[result.length_++] = buffer[i];
        }
        result.buffer[result.length_] = '\0';
        return result;
    }
};

// フォーマット文字列のコンパイル時検証
template<CompileTimeString fmt>
struct FormatString
{
    static constexpr auto format = fmt;
    
    static constexpr int countPlaceholders()
    {
        int count = 0;
        for (size_t i = 0; i < fmt.size() - 1; ++i)
        {
            if (fmt.data[i] == '{' && fmt.data[i + 1] == '}')
            {
                count++;
                i++;  // Skip '}'
            }
        }
        return count;
    }
    
    static constexpr int placeholderCount = countPlaceholders();
};

// ゲームメッセージのテンプレート
template<CompileTimeString msg>
class GameMessage
{
private:
    static constexpr auto messageTemplate = msg;
    
public:
    template<typename... Args>
    static void print(Args... args)
    {
        std::cout << "[ゲーム] ";
        printImpl(messageTemplate.c_str(), args...);
        std::cout << std::endl;
    }
    
private:
    static void printImpl(const char* str)
    {
        std::cout << str;
    }
    
    template<typename T, typename... Rest>
    static void printImpl(const char* str, T&& first, Rest&&... rest)
    {
        while (*str)
        {
            if (*str == '{' && *(str + 1) == '}')
            {
                std::cout << first;
                printImpl(str + 2, rest...);
                return;
            }
            std::cout << *str++;
        }
    }
};

// コンパイル時の文字列変換
template<CompileTimeString str>
struct ToUpperCase
{
    static constexpr auto convert()
    {
        CompileTimeString<str.size() + 1> result;
        for (size_t i = 0; i < str.size(); ++i)
        {
            if (str.data[i] >= 'a' && str.data[i] <= 'z')
            {
                result.data[i] = str.data[i] - 'a' + 'A';
            }
            else
            {
                result.data[i] = str.data[i];
            }
        }
        result.data[str.size()] = '\0';
        return result;
    }
    
    static constexpr auto value = convert();
};

// 国際化（i18n）のコンパイル時サポート
template<CompileTimeString key>
struct LocalizedString
{
    static constexpr uint32_t keyHash = fnv1a_32(key);
    
    static const char* get()
    {
        switch (keyHash)
        {
            case fnv1a_32("menu.start"):
                return "ゲームを開始";
            case fnv1a_32("menu.continue"):
                return "続きから";
            case fnv1a_32("menu.options"):
                return "オプション";
            case fnv1a_32("menu.quit"):
                return "終了";
            default:
                return "???";
        }
    }
};

// パフォーマンス最適化の例
class OptimizedLogger
{
private:
    template<size_t N>
    struct LogLevel
    {
        char name[N];
        uint32_t hash;
        
        constexpr LogLevel(const char (&str)[N]) 
            : name{}, hash(fnv1a_32({str, N - 1}))
        {
            for (size_t i = 0; i < N; ++i)
            {
                name[i] = str[i];
            }
        }
    };
    
    static constexpr LogLevel DEBUG{"DEBUG"};
    static constexpr LogLevel INFO{"INFO"};
    static constexpr LogLevel WARNING{"WARNING"};
    static constexpr LogLevel ERROR{"ERROR"};
    
public:
    template<CompileTimeString level, CompileTimeString msg>
    static void log()
    {
        constexpr uint32_t levelHash = fnv1a_32(level);
        
        // コンパイル時に不要なログレベルを除外
        if constexpr (levelHash == DEBUG.hash)
        {
            #ifdef DEBUG_BUILD
            std::cout << "[DEBUG] " << msg.c_str() << std::endl;
            #endif
        }
        else if constexpr (levelHash == fnv1a_32("INFO"))
        {
            std::cout << "[INFO] " << msg.c_str() << std::endl;
        }
        else if constexpr (levelHash == fnv1a_32("WARNING"))
        {
            std::cout << "[WARNING] " << msg.c_str() << std::endl;
        }
        else if constexpr (levelHash == fnv1a_32("ERROR"))
        {
            std::cerr << "[ERROR] " << msg.c_str() << std::endl;
        }
    }
};

int main()
{
    std::cout << "=== コンパイル時文字列処理 ===" << std::endl;
    
    // 1. 基本的な文字列
    std::cout << "\n--- 基本的なコンパイル時文字列 ---" << std::endl;
    constexpr CompileTimeString hello{"Hello"};
    constexpr CompileTimeString world{" World!"};
    constexpr auto greeting = concatenate(hello, world);
    std::cout << "結合結果: " << greeting.c_str() << std::endl;
    
    // 2. ハッシュ値計算
    std::cout << "\n--- コマンドハッシュ ---" << std::endl;
    std::cout << "attack hash: " << AttackCommand::hash << std::endl;
    std::cout << "defend hash: " << DefendCommand::hash << std::endl;
    
    // コマンド実行
    executeCommand(fnv1a_32("attack"));
    executeCommand(fnv1a_32("heal"));
    
    // 3. StaticString
    std::cout << "\n--- StaticString操作 ---" << std::endl;
    constexpr StaticString<32> playerName("勇者");
    constexpr auto title = []() {
        StaticString<64> s("伝説の");
        s.append("勇者");
        return s;
    }();
    std::cout << "タイトル: " << title.c_str() << std::endl;
    
    // 4. ゲームメッセージ
    std::cout << "\n--- ゲームメッセージ ---" << std::endl;
    GameMessage<"プレイヤー {} はレベル {} に上がった！">::print("太郎", 25);
    GameMessage<"{} に {} のダメージ！">::print("スライム", 42);
    
    // 5. 文字列変換
    std::cout << "\n--- 文字列変換 ---" << std::endl;
    constexpr auto upper = ToUpperCase<"hello world">::value;
    std::cout << "大文字変換: " << upper.c_str() << std::endl;
    
    // 6. 国際化
    std::cout << "\n--- 国際化 ---" << std::endl;
    std::cout << "menu.start: " << LocalizedString<"menu.start">::get() << std::endl;
    std::cout << "menu.continue: " << LocalizedString<"menu.continue">::get() << std::endl;
    std::cout << "menu.options: " << LocalizedString<"menu.options">::get() << std::endl;
    std::cout << "menu.quit: " << LocalizedString<"menu.quit">::get() << std::endl;
    
    // 7. 最適化されたログ
    std::cout << "\n--- 最適化ログ ---" << std::endl;
    OptimizedLogger::log<"INFO", "ゲームを開始しました">();
    OptimizedLogger::log<"WARNING", "メモリ使用量が高くなっています">();
    OptimizedLogger::log<"ERROR", "ファイルの読み込みに失敗">();
    
    // 8. フォーマット検証
    std::cout << "\n--- フォーマット検証 ---" << std::endl;
    using Format1 = FormatString<"HP: {} / {}">;
    using Format2 = FormatString<"アイテム「{}」を入手">;
    std::cout << "Format1のプレースホルダー数: " << Format1::placeholderCount << std::endl;
    std::cout << "Format2のプレースホルダー数: " << Format2::placeholderCount << std::endl;
    
    return 0;
}

// まとめ：
// - コンパイル時に文字列を操作して効率的なコードを生成
// - ハッシュ値の事前計算で実行時のオーバーヘッドを削減
// - テンプレートパラメータとして文字列を使用
// - コンパイル時の検証で実行時エラーを防止
// - 国際化やログなどの実用的な応用例