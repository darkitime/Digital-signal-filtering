#pragma once

/**
 * @file api.h
 * @brief Экспортируемый C-совместимый интерфейс (API) библиотеки.
 * @details Предоставляет набор функций-оберток для работы с C++ классом ProcessingSystem
 * из других языков программирования (например, C#, Python, C) через динамическую библиотеку (DLL).
 */

 /*
  * Макрос API_EXPORT скрывает специфичный для Windows синтаксис
  * и решает проблему парсинга функций генератором документации Doxygen.
  */
#ifndef API_EXPORT
#if defined(_WIN32) || defined(_WIN64)
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * @brief Создает новый экземпляр ProcessingSystem.
     * @return Указатель на созданную систему (непрозрачный указатель void*).
     */
    API_EXPORT void* createSystem();

    /**
     * @brief Уничтожает экземпляр системы и освобождает память.
     * @param systemPtr Указатель на систему, полученный через createSystem().
     */
    API_EXPORT void destroySystem(void* systemPtr);

    /**
     * @brief Добавляет КИХ-фильтр (FIR) в систему.
     * @param systemPtr Указатель на систему.
     * @param name Уникальное имя создаваемого блока.
     * @param coeffs Массив коэффициентов фильтра.
     * @param n Количество коэффициентов (размер массива).
     */
    API_EXPORT void addFIR(void* systemPtr, const char* name, const double* coeffs, int n);

    /**
     * @brief Добавляет БИХ-фильтр (IIR) в систему.
     * @param systemPtr Указатель на систему.
     * @param name Уникальное имя создаваемого блока.
     * @param b Массив коэффициентов числителя (прямые связи).
     * @param nB Размер массива b.
     * @param a Массив коэффициентов знаменателя (обратные связи).
     * @param nA Размер массива a.
     */
    API_EXPORT void addIIR(void* systemPtr, const char* name, const double* b, int nB, const double* a, int nA);

    /**
     * @brief Добавляет блок сумматора в систему.
     * @param systemPtr Указатель на систему.
     * @param name Уникальное имя сумматора.
     * @param u Весовой коэффициент для первого входа.
     * @param v Весовой коэффициент для второго входа.
     */
    API_EXPORT void addSummator(void* systemPtr, const char* name, double u, double v);

    /**
     * @brief Соединяет выходные порты нескольких блоков с входом целевого блока.
     * @param systemPtr Указатель на систему.
     * @param outputBlock Имя блока, который принимает сигналы.
     * @param sourceBlocks Массив строк (имен блоков), из которых берутся сигналы.
     * @param nSources Количество блоков-источников.
     */
    API_EXPORT void connect(void* systemPtr, const char* outputBlock, const char** sourceBlocks, int nSources);

    /**
     * @brief Вычисляет текущий выходной сигнал конкретного блока.
     * @param systemPtr Указатель на систему.
     * @param blockName Имя целевого блока для вычисления.
     * @param input Значение внешнего входного сигнала для всей системы.
     * @return Результат обработки (выходное значение).
     */
    API_EXPORT double computeBlock(void* systemPtr, const char* blockName, double input);

    /**
     * @brief Сбрасывает состояние буферов памяти всех блоков в системе.
     * @param systemPtr Указатель на систему.
     */
    API_EXPORT void resetAll(void* systemPtr);

    /**
     * @brief Обрабатывает целый массив данных (сигнал) через заданный блок.
     * @param systemPtr Указатель на систему.
     * @param blockName Имя целевого выходного блока.
     * @param input Указатель на массив входных отсчетов.
     * @param output Указатель на выделенный массив для записи результата (должен быть размера length).
     * @param length Количество элементов в массивах.
     */
    API_EXPORT void processSignal(void* systemPtr, const char* blockName, const double* input, double* output, int length);

    /**
     * @brief Получает текст последней перехваченной ошибки (Exception).
     * @details Если функции API сталкиваются с C++ исключениями, они сохраняются во внутренний буфер.
     * @return Указатель на C-строку с текстом ошибки или nullptr, если ошибок не было.
     */
    API_EXPORT const char* getLastError();

#ifdef __cplusplus
}
#endif