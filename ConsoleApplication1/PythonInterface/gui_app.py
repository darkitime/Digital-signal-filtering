"""
Модуль графического интерфейса «Digital Filter Lab».

Этот модуль реализует интерактивную лабораторию для тестирования цифровых фильтров.
Пользователь может настраивать параметры генерируемого сигнала (частоту и шум) 
и задавать произвольные коэффициенты КИХ-фильтра. Обработка данных выполняется 
в высокопроизводительном ядре на C++ через DLL.
"""

import tkinter as tk
from tkinter import messagebox
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt
import ctypes
import os
import math
import random
import sys

def get_resource_path(relative_path):
    """ Получает абсолютный путь к ресурсу, работает для обычной разработки и для PyInstaller """
    if hasattr(sys, '_MEIPASS'):
        return os.path.join(sys._MEIPASS, relative_path)
    return os.path.abspath(os.path.join(".", relative_path))

# --- 1. БЕЗОПАСНАЯ ЗАГРУЗКА DLL ---
lib = None

# Если мы НЕ собираем документацию Sphinx, пробуем загрузить DLL
if os.environ.get('SPHINX_BUILD') != '1':
    dll_name = "ConsoleApplication1.dll"
    dll_path = get_resource_path(dll_name)

    if not os.path.exists(dll_path):
        # Если запускаем не через GUI, выводим в консоль
        print(f"Ошибка: {dll_path} не найден.")
    else:
        try:
            lib = ctypes.CDLL(dll_path)
            
            # Настройка типов (обязательно внутри try или сразу после)
            lib.createSystem.restype = ctypes.c_void_p
            lib.destroySystem.argtypes = [ctypes.c_void_p]
            lib.addFIR.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_double), ctypes.c_int]
            
            if hasattr(lib, 'processSignal'):
                lib.processSignal.argtypes = [
                    ctypes.c_void_p, ctypes.c_char_p, 
                    ctypes.POINTER(ctypes.c_double), 
                    ctypes.POINTER(ctypes.c_double), ctypes.c_int
                ]
        except Exception as e:
            print(f"Критическая ошибка загрузки DLL: {e}")

def to_c_double_array(data):
    """
    Преобразует список Python в массив C-типа double.

    :param data: Список числовых значений для конвертации.
    :type data: list[float]
    :return: Массив ctypes, совместимый с функциями C++.
    :rtype: ctypes.Array
    """
    return (ctypes.c_double * len(data))(*data)


# --- 2. КЛАСС ГРАФИЧЕСКОГО ИНТЕРФЕЙСА ---

class FilterApp:
    """
    Главный класс приложения «Digital Filter Lab».

    Создает окно Tkinter, управляет полями ввода параметров фильтрации
    и отвечает за визуализацию графиков Matplotlib.
    """

    def __init__(self, root):
        """
        Инициализация интерфейса: создание панелей управления и области графиков.

        :param root: Корневой объект Tkinter.
        :type root: tk.Tk
        """
        self.root = root
        self.root.title("Digital Filter Lab")
        self.root.geometry("1000x600")

        # --- Левая панель (Настройки) ---
        control_frame = tk.Frame(root, padx=10, pady=10, width=300)
        control_frame.pack(side=tk.LEFT, fill=tk.Y)

        # Поле частоты
        tk.Label(control_frame, text="Частота сигнала (Гц):", font=("Arial", 10, "bold")).pack(anchor="w", pady=(0, 5))
        self.entry_freq = tk.Entry(control_frame)
        self.entry_freq.insert(0, "0.1")
        self.entry_freq.pack(fill=tk.X, pady=(0, 15))

        # Поле шума
        tk.Label(control_frame, text="Уровень шума (амплитуда):", font=("Arial", 10, "bold")).pack(anchor="w", pady=(0, 5))
        self.entry_noise = tk.Entry(control_frame)
        self.entry_noise.insert(0, "1.5")
        self.entry_noise.pack(fill=tk.X, pady=(0, 15))

        # Поле коэффициентов
        tk.Label(control_frame, text="Коэффициенты FIR фильтра\n(через запятую):", font=("Arial", 10, "bold")).pack(anchor="w", pady=(0, 5))
        self.entry_coeffs = tk.Entry(control_frame)
        self.entry_coeffs.insert(0, "0.2, 0.2, 0.2, 0.2, 0.2") 
        self.entry_coeffs.pack(fill=tk.X, pady=(0, 15))
        
        # Кнопка запуска
        self.btn_run = tk.Button(control_frame, text="Рассчитать и построить", 
                                 bg="#4CAF50", fg="white", font=("Arial", 12, "bold"),
                                 command=self.run_processing)
        self.btn_run.pack(fill=tk.X, pady=20)

        # --- Правая панель (График) ---
        plot_frame = tk.Frame(root)
        plot_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True)

        self.fig, self.ax = plt.subplots(figsize=(5, 4), dpi=100)
        self.canvas = FigureCanvasTkAgg(self.fig, master=plot_frame)
        self.canvas.draw()
        self.canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=True)

    def run_processing(self):
        """
        Основной цикл обработки данных.

        Считывает параметры из GUI, генерирует сигнал в Python,
        вызывает функции C++ DLL для фильтрации и обновляет график.

        :raises ValueError: Если введены некорректные данные (не числа).
        :raises Exception: При ошибках взаимодействия с библиотекой или пустом объекте системы.
        """
        if lib is None:
            messagebox.showwarning("Внимание", "DLL не загружена. Проверьте путь к библиотеке.")
            return

        try:
            # 1. Считываем данные
            freq = float(self.entry_freq.get())
            noise_level = float(self.entry_noise.get())
            coeffs = [float(x.strip()) for x in self.entry_freq.get().split(',')] # Пример парсинга
            
            # (Примечание: в вашем коде была опечатка, исправляем на считывание из entry_coeffs)
            coeffs_str = self.entry_coeffs.get()
            coeffs = [float(x.strip()) for x in coeffs_str.split(',')]
            
            # 2. Генерация сигнала
            N = 200
            input_data = [math.sin(i * freq) * 5.0 + random.uniform(-noise_level, noise_level) for i in range(N)]
            
            # 3. Взаимодействие с DLL
            system = lib.createSystem()
            
            c_coeffs = to_c_double_array(coeffs)
            lib.addFIR(system, b"UserFilter", c_coeffs, len(coeffs))
            
            c_input = to_c_double_array(input_data)
            output_data = [0.0] * N
            c_output = to_c_double_array(output_data)
            
            if hasattr(lib, 'processSignal'):
                lib.processSignal(system, b"UserFilter", c_input, c_output, N)
            else:
                # Резервный метод поотсчетной обработки
                lib.computeBlock.restype = ctypes.c_double
                lib.resetAll(system)
                for i in range(N):
                    c_output[i] = lib.computeBlock(system, b"UserFilter", ctypes.c_double(input_data[i]))
            
            lib.destroySystem(system)
            
            # 4. Обновление графика
            self.ax.clear()
            self.ax.plot(input_data, label='Входной (Шум)', color='lightgray', alpha=0.7)
            self.ax.plot(list(c_output), label='Выход (Фильтр)', color='red', linewidth=2)
            self.ax.set_title("Результат обработки C++")
            self.ax.grid(True)
            self.ax.legend()
            self.canvas.draw()

        except ValueError:
            messagebox.showerror("Ошибка ввода", "Введите числовые значения. Коэффициенты разделяйте запятыми.")
        except Exception as e:
            messagebox.showerror("Ошибка", f"Критический сбой: {e}")


# --- 3. ТОЧКА ВХОДА ---

if __name__ == "__main__":
    root = tk.Tk()
    app = FilterApp(root)
    root.mainloop()

