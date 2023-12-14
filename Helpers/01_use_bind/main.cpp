#include <functional>
#include <iostream>

void print_and_inc(int& num, const int val) {

    std::cout << num << " + " << val << std::endl; 
    num += val;
}


int main() {

    int x = 15;
    int val = 3;

    auto f1 = std::bind( print_and_inc, std::ref(x), val );

    f1();
    f1();

    return 0;
}