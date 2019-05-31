#include <iostream>

int main(){
    int number1;

    std::cout << "First intger: ";
    std::cin >> number1;

    int number2;
    int sum;

    std::cout << "second integer: ";
    std::cin >> number2;
    sum = number1 + number2;
    std::cout << "Sum is: " << sum << std::endl;
}