#pragma once
#include <string>
#include <vector>

// Абстрактный базовый класс "блок обработки сигналов"
class Block {
protected:
	std::string name; //!< имя блока
public:
	explicit Block(const std::string& nm) : name(nm) {} // конструктор с параметром имени
	virtual ~Block() = default; // виртуальный деструктор для корректного удаления производных классов

	std::string getName() const { return name; } //!< получение имени блока

	//Абстрактный метод обработки входных данных
	virtual double process(const std::vector<double>& input) = 0;

	//Сброс внутренних буфферов (например, для фильтров)
	virtual void reset() = 0;
};
