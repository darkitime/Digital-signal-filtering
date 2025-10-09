#pragma once
#include <iostream>

//! описание сигнала
class Signal {
private:
	double* values; //!< указатель на массив значений сигнала
	int size; //!< текущий размер массива
public:
	//! конструктор массива, заполняющегося нулями
	Signal(int n) : size(n) {
		values = new double[n]; // выделение памяти под массив
		for (int i = 0; i < n; i++) {
			values[i] = 0.0; // заполнение массива нулями
		}
	}
	//! конструктор копирования
	Signal(const Signal& other) : size(other.size) {
		values = (size > 0) ? new double[size] : nullptr; // выделение памяти под массив
		for (int i = 0; i < size; i++) {
			values[i] = other.values[i]; // копирование значений
		}
	}
	//! оператор присваивания
	Signal& operator=(const Signal& other) {
		if (this == &other) return *this; // проверка на самоприсваивание
		delete[] values; // освобождение старой памяти
		size = other.size; // копирование размера
		values = (size > 0) ? new double[size] : nullptr; // выделение памяти под новый массив
		for (int i = 0; i < size; i++) {
			values[i] = other.values[i]; // копирование значений
		}
		return *this; // возврат текущего объекта
	}

	//! деструктор для освобождения памяти
	~Signal() {
		delete[] values; // освобождение памяти
	}
	//! установка значения сигнала по индексу
	void setValue(int index, double x) {
		if (index >= 0 && index < size) {
			values[index] = x; // установка значения
		}
		if (index < 0 || index >= size) {
			printf("Error: Index out of bounds\n"); // обработка ошибки выхода за границы
		}

	}
	//! получение значения сигнала по индексу
	double getValue(int index) const {
		if (index >= 0 && index < size) {
			return values[index]; // возврат значения
		}
		if (index < 0 || index >= size) {
			printf("Error: Index out of bounds\n"); // обработка ошибки выхода за границы
			return 0.0; // возврат значения по умолчанию
		}
	}
	//! получить размер сигнала
	double getSize() const {
		return size; // возврат текущего размера
	}
	//! вывод сигнала на экран
	void print() const {
		for (int i = 0; i < size; i++) 
			std::cout << values[i] << " "; // вывод значений
		std::cout << std::endl; // переход на новую строку
	}
	// ===== перегрузка операторов =====

	// сложение сигналов
	Signal operator+(const Signal& other) const {
		int maxSize = (size > other.size) ? size : other.size; // определение максимального размера
		Signal result(maxSize); // создание результирующего сигнала
		for (int i = 0; i < maxSize; i++) {
			double val1 = (i < size) ? values[i] : 0.0; // значение первого сигнала или 0
			double val2 = (i < other.size) ? other.values[i] : 0.0; // значение второго сигнала или 0
			result.values[i] = val1 + val2; // сложение значений
		}
		return result; // возврат результирующего сигнала
	}

	//умножение на действительное число
	Signal operator*(double scalar) const {
		Signal result(size); // создание результирующего сигнала
		for (int i = 0; i < size; i++) {
			result.values[i] = values[i] * scalar; // умножение значения на скаляр
		}
		return result; // возврат результирующего сигнала
	}

	//конкатенация сигналов (склейка)
	Signal concat(const Signal& other) const {
		Signal result(size + other.size); // создание результирующего сигнала
		for (int i = 0; i < size; i++) {
			result.values[i] = values[i]; // копирование значений первого сигнала
		}
		for (int i = 0; i < other.size; i++) {
			result.values[size + i] = other.values[i]; // копирование значений второго сигнала
		}
		return result; // возврат результирующего сигнала
	}
	

};