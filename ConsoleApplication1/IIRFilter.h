#pragma once
#include "Block.h"
#include <vector>

/**
 * @brief Класс, реализующий БИХ-фильтр (Infinite Impulse Response Filter).
 * * @details Фильтр с бесконечной импульсной характеристикой. Наследуется от класса Block.
 * Выходное значение зависит не только от текущих и прошлых входов, но и от прошлых выходов (обратная связь).
 */
class IIRFilter : public Block {
private:
    std::vector<double> b;    /**< Коэффициенты числителя b0 ... bN */
    std::vector<double> a;    /**< Коэффициенты знаменателя a0 ... aM */
    std::vector<double> xbuf; /**< Буфер последних входных значений */
    std::vector<double> ybuf; /**< Буфер последних выходных значений */

public:
    /**
     * @brief Конструктор БИХ-фильтра.
     * @param nm Имя фильтра.
     * @param bcoef Вектор коэффициентов прямой связи (числитель передаточной функции).
     * @param acoef Вектор коэффициентов обратной связи (знаменатель передаточной функции).
     */
    IIRFilter(const std::string& nm, const std::vector<double>& bcoef, const std::vector<double>& acoef);

    /**
     * @brief Обработка вектора входных данных.
     * @param inputs Вектор входных данных. Ожидается размер, равный 1.
     * @return Вычисленное отфильтрованное значение.
     */
    double process(const std::vector<double>& inputs) override;

    /**
     * @brief Сброс состояния фильтра.
     * @details Обнуляет буферы истории входных и выходных значений.
     */
    void reset() override;

    /**
     * @brief Перегрузка оператора вызова функции "()".
     * @param x_t Одиночное входное значение.
     * @return Отфильтрованное значение.
     */
    double operator()(double x_t);
};


