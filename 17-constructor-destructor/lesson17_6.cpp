#include <iostream>
using namespace std;

class FileManager
{
public:
    string filename;

    FileManager(string name) : filename(name)
    {
        cout << filename << "を開きました" << endl;
        // ファイルを開く処理(イメージ)
    }

    ~FileManager()
    {
        cout << filename << "を閉じました" << endl;
        // ファイルを閉じる処理(イメージ)
    }
};

int main()
{
    FileManager file("save_data.txt");
    // スコープを抜けると自動的にファイルが閉じられる
}
