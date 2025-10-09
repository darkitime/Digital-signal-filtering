#include "FIRFilter.h"
#include <cassert>

FIRFilter::FIRFilter(const std::string& nm, const std::vector<double>& coefficients)
	: Block(nm), b(coefficients), xbuf(coefficients.size(), 0.0) {
	assert(!b.empty() && "Coefficients vector must not be empty");
}

double FIRFilter::process(const std::vector<double>& inputs) {
	assert(inputs.size() == 1); // фильтр принимает 1 вход 
	double x_t = inputs[0]; // текущее входное значение

	//сдвиг буфера: xbuf[0] <- x_t, xbuf[1] <- x[t-1], ...
	for (size_t i = xbuf.size() - 1; i > 0; --i) {
		xbuf[i] = xbuf[i - 1];
	}
	xbuf[0] = x_t; // новое значение в буфере

	//вычисление выходного значения y[t] = Σ b[i] * x[t-i]
	double y = 0.0;
	for (size_t i = 0; i < b.size(); ++i) {
		y += b[i] * xbuf[i]; // b[i] * x[t-i]
	}
	return y;
}

double FIRFilter::operator()(double x_t) {
	return process({ x_t }); // вызов process с одиночным значением
}

void FIRFilter::reset() {
	std::fill(xbuf.begin(), xbuf.end(), 0.0); // сброс буфера входных значений
}

