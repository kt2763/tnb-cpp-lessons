#include <iostream>
#include <string>
using namespace std;

int main()
{
    string name = "Tanabe";
    printf("%s\n", name.c_str()); // C言語のprintfと連携 - 内部的にconst char*に変換される
}
