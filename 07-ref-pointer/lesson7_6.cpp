#include <iostream>
#include <memory>
using namespace std;

int main()
{
    // 非推奨（例示のため）
    int *p = new int(42);
    delete p;

    // 推奨
    auto up = make_unique<int>(42); // 自動解放
    auto sp = make_shared<int>(42); // 共有所有
}
