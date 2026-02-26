#pragma once
#include "Block.h"
#include <vector>

/**
 * @brief Класс, реализующий КИХ-фильтр (Finite Impulse Response Filter).
 * * @details Фильтр с конечной импульсной характеристикой. Наследуется от базового класса Block.
 * Выходной сигнал вычисляется как взвешенная сумма текущего и предыдущих входных значений.
 */
class FIRFilter : public Block {
private:
    std::vector<double> b;    /**< Коэффициенты фильтра b0 ... bN */
    std::vector<double> xbuf; /**< Буфер входных значений (x[t] ... x[t-N]) */

public:
    /**
     * @brief Конструктор КИХ-фильтра.
     * @param nm Имя фильтра.
     * @param coefficients Вектор коэффициентов фильтра (числителя).
     */
    FIRFilter(const std::string& nm, const std::vector<double>& coefficients);

    /**
     * @brief Обработка вектора входных данных.
     * @param inputs Вектор входных данных. Ожидается, что размер вектора равен 1 (текущий отсчет).
     * @return Вычисленное отфильтрованное значение.
     */
    double process(const std::vector<double>& inputs) override;

    /**
     * @brief Сброс состояния фильтра.
     * @details Обнуляет буфер истории входных значений (xbuf).
     */
    void reset() override;

    /**
     * @brief Перегрузка оператора вызова функции "()".
     * @details Позволяет использовать объект фильтра как функцию для обработки одиночных значений.
     * @param x_t Текущее значение входного сигнала.
     * @return Отфильтрованное значение.
     */
    double operator()(double x_t);
};

