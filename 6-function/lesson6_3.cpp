#include <iostream>
using namespace std;

double calcBMI(double height, double weight)
{
    double height_m = height / 100.0;
    return weight / (height_m * height_m);
}

int main()
{
    double bmi = calcBMI(170, 65);
    cout << "BMI = " << bmi << "\n";
}
