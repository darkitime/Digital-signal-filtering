"""
Консольный клиент системы обработки сигналов.

Этот модуль демонстрирует базовое взаимодействие с C++ библиотекой (DLL) 
через модуль ctypes. Он генерирует тестовый зашумленный сигнал, 
пропускает его через КИХ-фильтр (FIR) и визуализирует результат 
с помощью библиотеки Matplotlib.
"""

import ctypes
import os
import math
import random
import matplotlib.pyplot as plt

def to_c_double_array(lst):
    """
    Преобразует стандартный Python-список в массив C-типа double.

    Эта функция необходима для корректной передачи массивов данных 
    из управляемого кода Python в неуправляемый код C++ (DLL).

    :param lst: Исходный список числовых значений (float или int).
    :type lst: list
    :return: Массив элементов типа c_double, готовый для передачи в DLL.
    :rtype: ctypes.Array
    """
    return (ctypes.c_double * len(lst))(*lst)

def main():
    """
    Главная функция выполнения скрипта.

    Выполняет следующие шаги:
    1. Загружает DLL-библиотеку `ConsoleApplication1.dll`.
    2. Инициализирует C-интерфейс (настраивает argtypes и restype).
    3. Создает экземпляр системы обработки и добавляет в него КИХ-фильтр.
    4. Генерирует синусоидальный сигнал с наложенным случайным шумом.
    5. Вызывает функцию DLL для обработки сигнала.
    6. Строит графики исходного и отфильтрованного сигналов.
    """
    # 1. ЗАГРУЗКА БИБЛИОТЕКИ
    dll_name = "ConsoleApplication1.dll"
    dll_path = os.path.abspath(dll_name)

    if not os.path.exists(dll_path):
        print(f"ОШИБКА: Файл {dll_name} не найден в папке {os.getcwd()}")
        return

    try:
        lib = ctypes.CDLL(dll_path)
        print(f"Библиотека {dll_name} успешно загружена.")
    except OSError as e:
        print(f"ОШИБКА загрузки DLL: {e}")
        return

    # 2. ОПИСАНИЕ ТИПОВ ФУНКЦИЙ (ARGTYPES)
    lib.createSystem.restype = ctypes.c_void_p
    lib.destroySystem.argtypes = [ctypes.c_void_p]
    lib.addFIR.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_double), ctypes.c_int]
    lib.processSignal.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double), ctypes.c_int]

    # Инициализация системы
    system = lib.createSystem()
    
    # Настройка фильтра (пример: скользящее среднее)
    coeffs = [0.2, 0.2, 0.2, 0.2, 0.2]
    c_coeffs = to_c_double_array(coeffs)
    lib.addFIR(system, b"Filter1", c_coeffs, len(coeffs))

    # Генерация сигнала
    N = 100
    input_data = []
    for i in range(N):
        pure_signal = math.sin(i * 0.1) * 5.0
        noise = random.uniform(-2.0, 2.0)
        input_data.append(pure_signal + noise)
    
    output_data = [0.0] * N
    c_input = to_c_double_array(input_data)
    c_output = to_c_double_array(output_data)

    # Обработка
    print("Начинаем обработку сигнала...")
    lib.processSignal(system, b"Filter1", c_input, c_output, N)
    result_list = list(c_output)

    # Очистка памяти C++
    lib.destroySystem(system)

    # Визуализация
    print("Построение графика...")
    plt.figure(figsize=(10, 6))
    plt.plot(input_data, label='Входной сигнал (с шумом)', color='lightgray', linestyle='-')
    plt.plot(result_list, label='Выход Filter1 (сглаженный)', color='red', linewidth=2)
    plt.title('Тестирование обработки сигнала (Python -> C++)')
    plt.legend()
    plt.grid(True)
    plt.show()

if __name__ == "__main__":
    main()