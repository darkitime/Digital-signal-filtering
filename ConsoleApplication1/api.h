#pragma once

/**
 * @file api.h
 * @brief Экспортируемый C-совместимый интерфейс (API) библиотеки.
 * @details Предоставляет набор функций-оберток для работы с C++ классом ProcessingSystem
 * из других языков программирования (например, C#, Python, C) через динамическую библиотеку (DLL).
 */

extern "C" {

    /**
     * @brief Создает новый экземпляр ProcessingSystem.
     * @return Указатель на созданную систему (непрозрачный указатель void*).
     */
    __declspec(dllexport) void* createSystem();

    /**
     * @brief Уничтожает экземпляр системы и освобождает память.
     * @param systemPtr Указатель на систему, полученный через createSystem().
     */
    __declspec(dllexport) void destroySystem(void* systemPtr);

    /**
     * @brief Добавляет КИХ-фильтр (FIR) в систему.
     * @param systemPtr Указатель на систему.
     * @param name Уникальное имя создаваемого блока.
     * @param coeffs Массив коэффициентов фильтра.
     * @param n Количество коэффициентов (размер массива).
     */
    __declspec(dllexport) void addFIR(void* systemPtr, const char* name, const double* coeffs, int n);

    /**
     * @brief Добавляет БИХ-фильтр (IIR) в систему.
     * @param systemPtr Указатель на систему.
     * @param name Уникальное имя создаваемого блока.
     * @param b Массив коэффициентов числителя (прямые связи).
     * @param nB Размер массива b.
     * @param a Массив коэффициентов знаменателя (обратные связи).
     * @param nA Размер массива a.
     */
    __declspec(dllexport) void addIIR(void* systemPtr, const char* name, const double* b, int nB, const double* a, int nA);

    /**
     * @brief Добавляет блок сумматора в систему.
     * @param systemPtr Указатель на систему.
     * @param name Уникальное имя сумматора.
     * @param u Весовой коэффициент для первого входа.
     * @param v Весовой коэффициент для второго входа.
     */
    __declspec(dllexport) void addSummator(void* systemPtr, const char* name, double u, double v);

    /**
     * @brief Соединяет выходные порты нескольких блоков с входом целевого блока.
     * @param systemPtr Указатель на систему.
     * @param outputBlock Имя блока, который принимает сигналы.
     * @param sourceBlocks Массив строк (имен блоков), из которых берутся сигналы.
     * @param nSources Количество блоков-источников.
     */
    __declspec(dllexport) void connect(void* systemPtr, const char* outputBlock, const char** sourceBlocks, int nSources);

    /**
     * @brief Вычисляет текущий выходной сигнал конкретного блока.
     * @param systemPtr Указатель на систему.
     * @param blockName Имя целевого блока для вычисления.
     * @param input Значение внешнего входного сигнала для всей системы.
     * @return Результат обработки (выходное значение).
     */
    __declspec(dllexport) double computeBlock(void* systemPtr, const char* blockName, double input);

    /**
     * @brief Сбрасывает состояние буферов памяти всех блоков в системе.
     * @param systemPtr Указатель на систему.
     */
    __declspec(dllexport) void resetAll(void* systemPtr);

    /**
     * @brief Обрабатывает целый массив данных (сигнал) через заданный блок.
     * @param systemPtr Указатель на систему.
     * @param blockName Имя целевого выходного блока.
     * @param input Указатель на массив входных отсчетов.
     * @param output Указатель на выделенный массив для записи результата (должен быть размера length).
     * @param length Количество элементов в массивах.
     */
    __declspec(dllexport) void processSignal(void* systemPtr, const char* blockName, const double* input, double* output, int length);

    /**
     * @brief Получает текст последней перехваченной ошибки (Exception).
     * @details Если функции API сталкиваются с C++ исключениями, они сохраняются во внутренний буфер.
     * @return Указатель на C-строку с текстом ошибки или nullptr, если ошибок не было.
     */
    __declspec(dllexport) const char* getLastError();

}

