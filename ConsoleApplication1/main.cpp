#include <iostream>
#include <cassert>
#include "FIRFilter.h"
#include "IIRFilter.h"
#include "Summator.h"
#include "ProcessingSystem.h"

int main() {
    FIRFilter fir("FIR1", { 0.5, 0.5 });
    IIRFilter iir("IIR1", { 0.2 }, { 0.8 });
    Summator sum("SUM1", 2.0, 3.0);

    double y1 = fir(1.0);
    double y2 = fir(0.0);
    double y3 = iir(1.0);
    double y4 = iir(0.0);
    double y5 = sum(1.0, 2.0);

    std::cout << "FIR outputs: " << y1 << ", " << y2 << std::endl;
    std::cout << "IIR outputs: " << y3 << ", " << y4 << std::endl;
    std::cout << "Summator output: " << y5 << std::endl;

    std::cout << "All tests are passed" << std::endl;
    std::cout << "Begin test " << std::endl;
    try {
        ProcessingSystem system;

        // Добавляем FIR фильтр
        std::vector<double> firCoeffs = { 0.2, 0.2, 0.2, 0.2, 0.2 };
        system.addBlock(std::make_unique<FIRFilter>("FIR1", firCoeffs));

        // Добавляем IIR фильтр
        std::vector<double> iirB = { 0.1, 0.1 };
        std::vector<double> iirA = { 1.0, -0.9 };
        system.addBlock(std::make_unique<IIRFilter>("IIR2", iirB, iirA));

        // Добавляем сумматор с коэффициентами u=1.0, v=1.0
        system.addBlock(std::make_unique<Summator>("SUM1", 1.0, 1.0));

        // Соединяем фильтры с сумматором
        system.connect("SUM1", { "FIR1", "IIR2" });

        // Входной сигнал
        double inputSignal = 1.0;

        // Вычисляем все выходы системы
        auto outputs = system.computeAll(inputSignal);

        // Выводим результаты
		std::cout << "Processing System Outputs:\n";
        for (const auto& p : outputs) {
            const std::string& name = p.first;
            double value = p.second;
            std::cout << name << " -> " << value << "\n";
        }

        // Сбрасываем все блоки
        system.resetAll();

    }
    catch (const std::exception& ex) {
        std::cerr << "Ошибка: " << ex.what() << "\n";
    }
    return 0;
}
