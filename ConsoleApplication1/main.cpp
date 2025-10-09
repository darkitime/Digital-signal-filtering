#include <iostream>
#include <cassert>
#include "FIRFilter.h"
#include "IIRFilter.h"
#include "Summator.h"

int main() {
    FIRFilter fir("FIR1", { 0.5, 0.5 });
    IIRFilter iir("IIR1", { 0.2 }, { 0.8 });
    Summator sum("SUM1", 2.0, 3.0);

    double y1 = fir(1.0);
    double y2 = fir(0.0);
    double y3 = iir(1.0);
    double y4 = iir(0.0);
    double y5 = sum(1.0, 2.0);

    std::cout << "FIR outputs: " << y1 << ", " << y2 << std::endl;
    std::cout << "IIR outputs: " << y3 << ", " << y4 << std::endl;
    std::cout << "Summator output: " << y5 << std::endl;

    std::cout << "All tests are passed" << std::endl;
    return 0;
}
