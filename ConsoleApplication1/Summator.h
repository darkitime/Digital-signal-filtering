#pragma once
#include "Block.h"

// Сумматор: y = u * x1 + v * x2
class Summator : public Block {
private:
    double u, v;

public:
    Summator(const std::string& nm, double uu, double vv);

	double process(const std::vector<double>& inputs) override; // Переопределение метода обработки входных данных
	void reset() override; // Переопределение метода сброса внутренних буферов

	double operator()(double x1, double x2); // Перегрузка оператора вызова для удобной обработки
};
