#include "Summator.h"
#include <cassert>

Summator::Summator(const std::string& nm, double uu, double vv)
    : Block(nm), u(uu), v(vv) {
}

double Summator::process(const std::vector<double>& inputs) {
	assert(inputs.size() == 2); // —умматор принимает ровно 2 входа
	return u * inputs[0] + v * inputs[1]; // y = u * x1 + v * x2
}

double Summator::operator()(double x1, double x2) {
    return process({ x1, x2 }); // вызов process с двум€ значени€ми
}

void Summator::reset() {
    // —умматор не хранит состо€ние Ч ничего не делаем
}

