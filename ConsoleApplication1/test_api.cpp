#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include "api.h"

// Простой макрос для проверки
#define ASSERT_TRUE(condition, msg) \
    if (!(condition)) { \
        std::cerr << "FAIL: " << msg << std::endl; \
        std::cerr << "Last C++ Error: " << (getLastError() ? getLastError() : "None") << std::endl; \
        return 1; \
    } else { \
        std::cout << "OK: " << msg << std::endl; \
    }

int main() {
    std::cout << "=== Running API Tests ===" << std::endl;

    // Тест 1: Создание системы
    void* sys = createSystem();
    ASSERT_TRUE(sys != nullptr, "System creation");

    // Тест 2: Добавление FIR фильтра
    // Простой moving average: [0.5, 0.5]
    double coeffs[] = { 0.5, 0.5 };
    addFIR(sys, "Filter1", coeffs, 2);
    ASSERT_TRUE(getLastError() == nullptr, "Add FIR Filter");

    // Тест 3: Проверка вычислений (computeBlock)
    // Вход 10: 10 * 0.5 + 0 * 0.5 = 5
    double val1 = computeBlock(sys, "Filter1", 10.0);
    ASSERT_TRUE(std::abs(val1 - 5.0) < 1e-6, "FIR Step 1 calculation");

    // Вход 10: 10 * 0.5 + 10 * 0.5 = 10
    double val2 = computeBlock(sys, "Filter1", 10.0);
    ASSERT_TRUE(std::abs(val2 - 10.0) < 1e-6, "FIR Step 2 calculation");

    // Тест 4: Обработка массива (processSignal)
    // Сбросим систему
    resetAll(sys);

    double input[] = { 10.0, 10.0, 10.0 };
    double output[3] = { 0 };
    processSignal(sys, "Filter1", input, output, 3);

    ASSERT_TRUE(getLastError() == nullptr, "Process Signal array");
    ASSERT_TRUE(std::abs(output[0] - 5.0) < 1e-6, "Array Output[0]");
    ASSERT_TRUE(std::abs(output[1] - 10.0) < 1e-6, "Array Output[1]");
    ASSERT_TRUE(std::abs(output[2] - 10.0) < 1e-6, "Array Output[2]");

    // Тест 5: Обработка ошибок
    // Попробуем подключиться к несуществующему блоку
    const char* sources[] = { "GhostBlock" };
    connect(sys, "Filter1", sources, 1);

    const char* err = getLastError();
    ASSERT_TRUE(err != nullptr, "Error should occur for missing block");
    std::cout << "Expected error caught: " << err << std::endl;

    // Очистка
    destroySystem(sys);
    std::cout << "=== All Tests Passed ===" << std::endl;

    return 0;
}