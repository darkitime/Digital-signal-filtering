#pragma once
#include "Block.h"
#include <vector>

// БИХ-фильтр (Infinite Impulse Response Filter)
class IIRFilter : public Block {
private:
	std::vector<double> b; // Коэффициенты числителя b0 ... bN
	std::vector<double> a; // Коэффициенты знаменателя a0 ... aM
	std::vector<double> xbuf; // Последние входы
	std::vector<double> ybuf; // Последние выходы

public:
    IIRFilter(const std::string& nm,
        const std::vector<double>& bcoef,
		const std::vector<double>& acoef); // Конструктор с параметрами имени и коэффициентов

	double process(const std::vector<double>& inputs) override; // Переопределение метода обработки входных данных
	void reset() override; // Переопределение метода сброса внутренних буферов

	double operator()(double x_t); // Перегрузка оператора вызова для удобной обработки одиночного значения
};


