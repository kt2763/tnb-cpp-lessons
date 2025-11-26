#include <iostream>
#include <print>
using namespace std;

int main()
{
    cout << "Hello, World!\n";
    cout << "数値: " << 42 << " / 小数: " << 3.14 << "\n";
    cout << "改行＋フラッシュ" << endl;

    // std::print()
    print("Hello World!");
    string name{"Tanabe"};
    print("Hello, {}", name);
}
