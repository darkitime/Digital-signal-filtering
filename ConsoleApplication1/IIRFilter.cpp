#include "IIRFilter.h"
#include <cassert>

IIRFilter::IIRFilter(const std::string& nm,
    const std::vector<double>& bcoef,
    const std::vector<double>& acoef)
    : Block(nm), b(bcoef), a(acoef),
    xbuf(bcoef.size(), 0.0),
    ybuf(acoef.size(), 0.0) {
}

double IIRFilter::process(const std::vector<double>& inputs) {
	assert(inputs.size() == 1); // фильтр принимает 1 вход
	double x_t = inputs[0]; // текущее входное значение

    // обновляем буфер входов
    for (size_t i = xbuf.size() - 1; i > 0; --i)
        xbuf[i] = xbuf[i - 1];
	xbuf[0] = x_t; // новое значение в буфере

    // вычисляем выход
	double y = 0.0; // текущее выходное значение
    for (size_t i = 0; i < b.size(); ++i)
		y += b[i] * xbuf[i]; // b[i] * x[t-i]
    for (size_t j = 0; j < a.size(); ++j)
		y += a[j] * ybuf[j]; // a[j] * y[t-j]

    // обновляем буфер выходов
    for (size_t i = ybuf.size() - 1; i > 0; --i)
        ybuf[i] = ybuf[i - 1]; 
    if (!ybuf.empty()) ybuf[0] = y;

    return y;
}

double IIRFilter::operator()(double x_t) {
    return process({ x_t }); // вызов process с одиночным значением
}

void IIRFilter::reset() {
    std::fill(xbuf.begin(), xbuf.end(), 0.0);
	std::fill(ybuf.begin(), ybuf.end(), 0.0); // сброс буфера выходных значений
}

