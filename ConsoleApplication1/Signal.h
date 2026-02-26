#pragma once
#include <iostream>

/**
 * @brief Класс для представления и математической обработки одномерных сигналов.
 * @details Хранит динамический массив отсчетов сигнала и предоставляет
 * базовые операции, такие как сложение, умножение на скаляр и конкатенация.
 */
class Signal {
private:
    double* values; /**< Указатель на динамический массив значений (отсчетов) сигнала */
    int size;       /**< Количество отсчетов в сигнале */

public:
    /**
     * @brief Конструктор инициализации сигнала заданного размера.
     * @details Выделяет память под массив и заполняет его нулями.
     * @param n Размер создаваемого сигнала (количество отсчетов).
     */
    Signal(int n) : size(n) {
        values = new double[n]; // выделение памяти под массив
        for (int i = 0; i < n; i++) {
            values[i] = 0.0; // заполнение массива нулями
        }
    }

    /**
     * @brief Конструктор копирования.
     * @param other Оригинальный объект сигнала, копия которого создается.
     */
    Signal(const Signal& other) : size(other.size) {
        values = (size > 0) ? new double[size] : nullptr;
        for (int i = 0; i < size; i++) {
            values[i] = other.values[i];
        }
    }

    /**
     * @brief Оператор присваивания.
     * @param other Объект сигнала для присваивания.
     * @return Ссылка на текущий измененный объект.
     */
    Signal& operator=(const Signal& other) {
        if (this == &other) return *this; // проверка на самоприсваивание
        delete[] values; // освобождение старой памяти
        size = other.size;
        values = (size > 0) ? new double[size] : nullptr;
        for (int i = 0; i < size; i++) {
            values[i] = other.values[i];
        }
        return *this;
    }

    /**
     * @brief Деструктор. Освобождает выделенную память.
     */
    ~Signal() {
        delete[] values;
    }

    /**
     * @brief Устанавливает значение конкретного отсчета сигнала.
     * @param index Индекс отсчета (начиная с 0).
     * @param val Новое значение.
     */
    void setValue(int index, double val) {
        if (index >= 0 && index < size) {
            values[index] = val;
        }
    }

    /**
     * @brief Получает значение конкретного отсчета сигнала.
     * @param index Индекс отсчета.
     * @return Значение сигнала по заданному индексу или 0.0, если индекс вне границ.
     */
    double getValue(int index) const {
        if (index >= 0 && index < size) {
            return values[index];
        }
        return 0.0;
    }

    /**
     * @brief Получает текущий размер сигнала.
     * @return Количество отсчетов.
     */
    int getSize() const {
        return size;
    }

    /**
     * @brief Выводит значения сигнала в стандартный поток вывода (консоль).
     */
    void print() const {
        for (int i = 0; i < size; i++)
            std::cout << values[i] << " ";
        std::cout << std::endl;
    }

    // ===== перегрузка операторов =====

    /**
     * @brief Оператор сложения двух сигналов.
     * @details Если сигналы разной длины, размер результата равен максимальной длине.
     * Недостающие отсчеты короткого сигнала считаются равными 0.
     * @param other Второй сигнал для сложения.
     * @return Новый объект сигнала, содержащий покомпонентную сумму.
     */
    Signal operator+(const Signal& other) const {
        int maxSize = (size > other.size) ? size : other.size;
        Signal result(maxSize);
        for (int i = 0; i < maxSize; i++) {
            double val1 = (i < size) ? values[i] : 0.0;
            double val2 = (i < other.size) ? other.values[i] : 0.0;
            result.values[i] = val1 + val2;
        }
        return result;
    }

    /**
     * @brief Оператор умножения сигнала на скаляр (число).
     * @param scalar Действительное число для умножения.
     * @return Новый масштабированный сигнал.
     */
    Signal operator*(double scalar) const {
        Signal result(size);
        for (int i = 0; i < size; i++) {
            result.values[i] = values[i] * scalar;
        }
        return result;
    }

    /**
     * @brief Конкатенация (склейка) двух сигналов.
     * @param other Сигнал, который будет присоединен в конец текущего.
     * @return Новый сигнал, содержащий элементы обоих сигналов последовательно.
     */
    Signal concat(const Signal& other) const {
        Signal result(size + other.size);
        for (int i = 0; i < size; i++) {
            result.values[i] = values[i];
        }
        for (int i = 0; i < other.size; i++) {
            result.values[size + i] = other.values[i];
        }
        return result;
    }
};