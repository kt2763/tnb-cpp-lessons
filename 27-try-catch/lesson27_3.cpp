#include <iostream>
#include <stdexcept>
#include <string>

// ゲーム固有のエラークラス
class GameError : public std::runtime_error
{
public:
    explicit GameError(const std::string& message)
        : std::runtime_error("[GameError] " + message)
    {}
};

class SaveDataCorruptedError : public GameError
{
public:
    explicit SaveDataCorruptedError(const std::string& save_path)
        : GameError("セーブデータが壊れています: " + save_path)
        , save_path_(save_path)
    {}

    const std::string& save_path() const { return save_path_; }

private:
    std::string save_path_;
};

// セーブデータを読み込む（失敗すると例外を投げる）
void load_save_data(const std::string& path)
{
    // 実際にはファイル読み込みなどを行う
    bool is_corrupted = true; // 仮のフラグ

    if (is_corrupted)
    {
        throw SaveDataCorruptedError(path);
    }
}

int main()
{
    try
    {
        load_save_data("/saves/slot1.dat");
    }
    catch (const SaveDataCorruptedError& e)
    {
        // 具体的な型でキャッチ：詳細情報にアクセスできる
        std::cerr << e.what() << std::endl;
        std::cerr << "パス: " << e.save_path() << std::endl;
        std::cerr << "→ 新規ゲームで起動します" << std::endl;
    }
    catch (const GameError& e)
    {
        // ゲーム系エラーをまとめてキャッチ
        std::cerr << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        // その他の標準例外
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
