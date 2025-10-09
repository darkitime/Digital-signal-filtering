#pragma once
#include "Block.h"
#include <vector>

// КИХ-фильтр (Finite Impulse Response Filter)
class FIRFilter : public Block {
    private:
		std::vector<double> b; // Коэффициенты фильтра b0 ... bN
		std::vector<double> xbuf; //Буфер входных значений (x[t]...x[t-N])

public:
	FIRFilter(const std::string& nm, const std::vector<double>& coefficients); // Конструктор с параметрами имени и коэффициентов
	
	double process(const std::vector<double>& inputs) override; // Переопределение метода обработки входных данных
	void reset() override; // Переопределение метода сброса внутренних буферов

	double operator()(double x_t); // Перегрузка оператора вызова для удобной обработки одиночного значения

};

