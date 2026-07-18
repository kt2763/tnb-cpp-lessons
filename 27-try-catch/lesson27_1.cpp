#include <iostream>
#include <string>
#include <fstream>

// ❌ エラーコードアプローチ：チェックが必要
bool load_file_errorcode(const std::string& path, std::string& out_content)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        return false; // エラーを戻り値で伝える
    }
    out_content.assign(std::istreambuf_iterator<char>(file),
                       std::istreambuf_iterator<char>());
    return true;
}

// ✅ 例外アプローチ：戻り値で本来の値だけ返せる
std::string load_file_exception(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        throw std::runtime_error("ファイルを開けませんでした: " + path);
    }
    return { std::istreambuf_iterator<char>(file),
             std::istreambuf_iterator<char>() };
}

int main()
{
    // エラーコード版：戻り値チェックが必要
    std::string content;
    if (!load_file_errorcode("data.txt", content))
    {
        std::cerr << "ファイル読み込み失敗" << std::endl;
    }

    // 例外版：try/catch でまとめて処理
    try
    {
        std::string text = load_file_exception("data.txt");
        std::cout << "読み込み成功: " << text.size() << "文字" << std::endl;
    }
    catch (const std::runtime_error& e)
    {
        // what()はエラー理由を出力してくれる
        std::cerr << "エラー: " << e.what() << std::endl;
    }

    return 0;
}
