#include <iostream>

int main()
{
    double height_cm, weight_kg; // 身長、体重 ← ユーザー入力
    std::cout << "身長(cm)と体重(kg)を入力してね\n";
    std::cin >> height_cm >> weight_kg;

    double height_m = height_cm / 100.0;            // メートル変換
    double bmi = weight_kg / (height_m * height_m); // BMI換算の公式 体重÷身長×身長

    std::cout << "あなたのBMIは" << bmi << "です。\n";
    return 0;
}
