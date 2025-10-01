#include <iostream>
#include <string>
using namespace std;

int main()
{
    string text = "C++ Programming";

    cout << text.size() << endl;        // 文字列の大きさ
    cout << text.length() << endl;      // 文字列の長さ - 実はsize()と同じ
    cout << text.capacity() << endl;    // 確保しているバッファの容量
    cout << text.max_size() << endl;    // 文字列の最大容量 - バイト数
    cout << text.empty() << endl;       // 文字列が空なら1、そうでなければ0
    cout << text.substr(4, 11) << endl; // 文字列の4番目から11文字分
    cout << text.find("Pro") << endl;   // 文字列の中から"Pro"を探し、見つかった位置を返す

    cout << text.at(0) << endl; // 文字列の0番目の文字 - 推奨
    cout << text[0] << endl;    // 文字列の0番目の文字 - 推奨しない
}
