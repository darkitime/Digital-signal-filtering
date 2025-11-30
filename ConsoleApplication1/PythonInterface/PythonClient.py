import ctypes
import os
import math
import random
import matplotlib.pyplot as plt

# 1. ЗАГРУЗКА БИБЛИОТЕКИ

# Имя DLL.
dll_name = "ConsoleApplication1.dll"

# Получаем полный путь к DLL (чтобы Python точно её нашел)
dll_path = os.path.abspath(dll_name)

if not os.path.exists(dll_path):
    print(f"ОШИБКА: Файл {dll_name} не найден в папке {os.getcwd()}")
    exit(1)

try:
    # Загружаем библиотеку
    lib = ctypes.CDLL(dll_path)
    print(f"Библиотека {dll_name} успешно загружена.")
except OSError as e:
    print(f"ОШИБКА загрузки DLL: {e}")
    print("Проверка битности.")
    exit(1)


# 2. ОПИСАНИЕ ТИПОВ ФУНКЦИЙ (ARGTYPES)

# void* createSystem();
lib.createSystem.restype = ctypes.c_void_p

# void destroySystem(void* systemPtr);
lib.destroySystem.argtypes = [ctypes.c_void_p]

# void addFIR(void* systemPtr, const char* name, const double* coeffs, int n);
lib.addFIR.argtypes = [
    ctypes.c_void_p,
    ctypes.c_char_p,                 # Строка (имя)
    ctypes.POINTER(ctypes.c_double), # Массив коэффициентов
    ctypes.c_int
]

# void addIIR(void* systemPtr, const char* name, const double* b, int nB, const double* a, int nA);
lib.addIIR.argtypes = [
    ctypes.c_void_p,
    ctypes.c_char_p,
    ctypes.POINTER(ctypes.c_double), ctypes.c_int, # b
    ctypes.POINTER(ctypes.c_double), ctypes.c_int  # a
]

# void addSummator(void* systemPtr, const char* name, double u, double v);
lib.addSummator.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_double, ctypes.c_double]

# void connect(void* systemPtr, const char* outputBlock, const char** sourceBlocks, int nSources);
lib.connect.argtypes = [
    ctypes.c_void_p,
    ctypes.c_char_p,
    ctypes.POINTER(ctypes.c_char_p), # Массив строк (char**)
    ctypes.c_int
]

# void resetAll(void* systemPtr);
lib.resetAll.argtypes = [ctypes.c_void_p]

# void processSignal(void* systemPtr, const char* blockName, const double* input, double* output, int length);
lib.processSignal.argtypes = [
    ctypes.c_void_p,
    ctypes.c_char_p,
    ctypes.POINTER(ctypes.c_double), # Входной массив
    ctypes.POINTER(ctypes.c_double), # Выходной массив (куда писать)
    ctypes.c_int
]


# 3. ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ


def to_c_double_array(data):
    """Превращает список Python [1.0, 2.0] в массив C double"""
    return (ctypes.c_double * len(data))(*data)

def to_c_string_array(string_list):
    """Превращает список строк ['a', 'b'] в массив char** для функции connect"""
    # 1. Кодируем каждую строку в байты
    encoded = [s.encode('utf-8') for s in string_list]
    # 2. Создаем массив указателей (c_char_p)
    arr = (ctypes.c_char_p * len(string_list))()
    arr[:] = encoded
    return arr


# 4. ОСНОВНАЯ ЛОГИКА ПРОГРАММЫ

def main():
    # --- А. Создание системы ---
    system = lib.createSystem()
    print("Система создана.")

    try:
        # --- Б. Создание блоков ---
        
        # 1. Создадим простой FIR фильтр (скользящее среднее по 5 точкам)
        # Он сглаживает шум.
        fir_coeffs = [0.2, 0.2, 0.2, 0.2, 0.2]
        lib.addFIR(system, b"Filter1", to_c_double_array(fir_coeffs), len(fir_coeffs))
        print("Блок Filter1 добавлен.")

        # 2. Создадим еще один блок, например Сумматор (просто для примера связей)
        # Пусть он берет сигнал из Filter1 и умножает на 2 (если v=0)
        lib.addSummator(system, b"Sum1", 1.0, 1.0) # u=1, v=1 (просто пример)
        print("Блок Sum1 добавлен.")

        # --- В. Настройка связей ---
        
        # Соединим Sum1 так, чтобы он брал данные из Filter1
        # Sum1 зависит от ["Filter1"]
        sources = ["Filter1"]
        lib.connect(system, b"Sum1", to_c_string_array(sources), 1)
        print("Связь установлена: Sum1 -> Filter1")

        # --- Г. Подготовка данных ---
        
        N = 200 # Длина сигнала
        input_data = []
        # Генерируем синусоиду с сильным шумом
        for i in range(N):
            pure_signal = math.sin(i * 0.1) * 5.0
            noise = random.uniform(-2.0, 2.0)
            input_data.append(pure_signal + noise)
        
        # Массив для результата (заполнен нулями)
        output_data = [0.0] * N

        c_input = to_c_double_array(input_data)
        c_output = to_c_double_array(output_data)

        # --- Д. Обработка ---
        
        # Мы хотим получить результат на выходе Filter1 (сглаженный сигнал)
        print("Начинаем обработку сигнала...")
        lib.processSignal(system, b"Filter1", c_input, c_output, N)

        # Переводим результат из C-массива обратно в список Python
        result_list = list(c_output)

        # --- Е. Визуализация ---
        print("Построение графика...")
        plt.figure(figsize=(10, 6))
        
        plt.plot(input_data, label='Входной сигнал (с шумом)', color='lightgray', linestyle='-')
        plt.plot(result_list, label='Выход Filter1 (сглаженный)', color='red', linewidth=2)
        
        plt.title("Демонстрация цифровой фильтрации")
        plt.xlabel("Отсчеты")
        plt.ylabel("Амплитуда")
        plt.legend()
        plt.grid(True)
        plt.show()

    finally:
        # --- Ж. Очистка ---
        if system:
            lib.destroySystem(system)
            print("Память очищена.")

if __name__ == "__main__":
    main()
