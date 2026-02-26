"""
Графический интерфейс системы обработки сигналов.

Этот модуль предоставляет оконное приложение на базе библиотеки Tkinter.
Оно позволяет пользователю интерактивно выбирать тип сигнала, настраивать параметры 
КИХ и БИХ фильтров и в реальном времени визуализировать результаты обработки 
через встроенные графики Matplotlib. Вся вычислительная нагрузка передается в C++ DLL.
"""

import tkinter as tk
from tkinter import ttk, messagebox
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt
import ctypes
import os
import math
import random

def to_c_double_array(lst):
    """
    Конвертирует Python-список в C-массив типа double для передачи в DLL.

    :param lst: Список чисел.
    :type lst: list
    :return: Массив ctypes.c_double.
    """
    return (ctypes.c_double * len(lst))(*lst)

# --- 1. БЕЗОПАСНАЯ ЗАГРУЗКА DLL ---
lib = None

# Проверяем, не читает ли нас сейчас Sphinx
if os.environ.get('SPHINX_BUILD') != '1':
    
    dll_name = "ConsoleApplication1.dll"
    if not os.path.exists(dll_name):
        messagebox.showerror("Ошибка", f"Файл {dll_name} не найден!")
        exit(1)

    try:
        lib = ctypes.CDLL(os.path.abspath(dll_name))
    except OSError as e:
        messagebox.showerror("Ошибка загрузки DLL", str(e))
        exit(1)

    # Типы аргументов
    lib.createSystem.restype = ctypes.c_void_p
    lib.destroySystem.argtypes = [ctypes.c_void_p]
    lib.addFIR.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_double), ctypes.c_int]
    lib.addIIR.argtypes = [
        ctypes.c_void_p, ctypes.c_char_p, 
        ctypes.POINTER(ctypes.c_double), ctypes.c_int,
        ctypes.POINTER(ctypes.c_double), ctypes.c_int
    ]
    lib.processSignal.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double), ctypes.c_int]

    try:
        lib.getLastError.restype = ctypes.c_char_p
    except AttributeError:
        pass

def check_for_error():
    """
    Проверяет наличие исключений, перехваченных внутри C++ DLL.
    """
    if lib and hasattr(lib, 'getLastError'):
        err_ptr = lib.getLastError()
        if err_ptr:
            raise Exception(err_ptr.decode('utf-8'))

class SignalApp:
    """
    Главный класс графического приложения (GUI).

    Управляет жизненным циклом интерфейса Tkinter, обрабатывает события кнопок 
    и связывает логику пользовательского ввода с вызовами функций библиотеки C++.
    Включает в себя интеграцию графиков Matplotlib непосредственно в окно приложения.
    """

    def __init__(self, root):
        """
        Инициализирует интерфейс пользователя, создает графики и элементы управления.

        :param root: Корневой виджет (главное окно) Tkinter.
        :type root: tk.Tk
        """
        self.root = root
        self.root.title("Продвинутая обработка сигналов (C++ Core)")
        
        # Переменные интерфейса
        self.signal_type_var = tk.StringVar(value="Синус + Шум")
        self.filter_type_var = tk.StringVar(value="FIR (Скользящее среднее)")
        
        self.setup_ui()
        self.system = lib.createSystem()

    def setup_ui(self):
        """
        Создает и размещает все визуальные элементы управления на форме (кнопки, списки, канвас графика).
        """
        control_frame = ttk.Frame(self.root, padding=10)
        control_frame.pack(side=tk.LEFT, fill=tk.Y)

        ttk.Label(control_frame, text="Тип сигнала:").pack(anchor=tk.W, pady=5)
        ttk.Combobox(control_frame, textvariable=self.signal_type_var, 
                     values=["Синус + Шум", "Меандр + Шум", "Только шум"]).pack(fill=tk.X)

        ttk.Label(control_frame, text="Тип фильтра:").pack(anchor=tk.W, pady=5)
        ttk.Combobox(control_frame, textvariable=self.filter_type_var, 
                     values=["FIR (Скользящее среднее)", "IIR (Рекурсивный сглаживающий)"]).pack(fill=tk.X)

        ttk.Button(control_frame, text="Сгенерировать и Обработать", command=self.process_data).pack(pady=20, fill=tk.X)

        # Зона графика
        self.fig, self.ax = plt.subplots(figsize=(8, 5))
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.root)
        self.canvas.get_tk_widget().pack(side=tk.RIGHT, fill=tk.BOTH, expand=True)

    def process_data(self):
        """
        Основной метод обработки, вызываемый при нажатии кнопки в интерфейсе.

        Выполняет генерацию входного массива данных в зависимости от выбора пользователя, 
        настраивает выбранный фильтр в C++ системе (КИХ или БИХ), 
        пропускает массив через DLL и обновляет график Matplotlib результатами.

        :raises Exception: Перехватывает ошибки от DLL и показывает их пользователю в диалоговом окне.
        """
        try:
            # Сброс старой системы и создание новой для чистоты эксперимента
            lib.destroySystem(self.system)
            self.system = lib.createSystem()

            filt_type = self.filter_type_var.get()
            if "FIR" in filt_type:
                coeffs = [0.2, 0.2, 0.2, 0.2, 0.2]
                lib.addFIR(self.system, b"MyFilter", to_c_double_array(coeffs), len(coeffs))
            else:
                b_coeffs = [0.1, 0.1]
                a_coeffs = [1.0, -0.8]
                lib.addIIR(self.system, b"MyFilter", to_c_double_array(b_coeffs), len(b_coeffs), to_c_double_array(a_coeffs), len(a_coeffs))

            N = 200
            input_data = []
            sig_type = self.signal_type_var.get()
            
            for i in range(N):
                noise = random.uniform(-2.0, 2.0)
                if "Синус" in sig_type:
                    val = math.sin(i * 0.1) * 5.0 + noise
                elif "Меандр" in sig_type:
                    val = 5.0 if (i // 20) % 2 == 0 else -5.0
                    val += noise
                else:
                    val = noise
                input_data.append(val)

            c_input = to_c_double_array(input_data)
            output_data = [0.0] * N
            c_output = to_c_double_array(output_data)

            if hasattr(lib, 'processSignal'):
                lib.processSignal(self.system, b"MyFilter", c_input, c_output, N)
                check_for_error()
            else:
                lib.computeBlock.restype = ctypes.c_double
                try:
                    lib.resetAll(self.system)
                except:
                    pass
                for i in range(N):
                    c_output[i] = lib.computeBlock(self.system, b"MyFilter", ctypes.c_double(input_data[i]))
                    check_for_error()

            # Визуализация
            self.ax.clear()
            self.ax.plot(input_data, label='Вход', color='#CCCCCC', linestyle='--')
            self.ax.plot(list(c_output), label='Выход', color='#FF5722', linewidth=2)
            self.ax.set_title(f"Результат: {filt_type} | Сигнал: {sig_type}")
            self.ax.legend()
            self.ax.grid(True, linestyle=':', alpha=0.6)
            self.canvas.draw()

        except Exception as e:
            messagebox.showerror("Ошибка выполнения", f"Внутренняя ошибка библиотеки:\n{e}")

    def __del__(self):
        """
        Деструктор класса. Корректно очищает память системы в C++.
        """
        if hasattr(self, 'system') and self.system:
            lib.destroySystem(self.system)

if __name__ == "__main__":
    root = tk.Tk()
    app = SignalApp(root)
    root.mainloop()