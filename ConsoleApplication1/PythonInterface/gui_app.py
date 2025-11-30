import tkinter as tk
from tkinter import messagebox
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt
import ctypes
import os
import math
import random


# 1. ЗАГРУЗКА И НАСТРОЙКА DLL

dll_name = "ConsoleApplication1.dll"
if not os.path.exists(dll_name):
    messagebox.showerror("Ошибка", f"Файл {dll_name} не найден!")
    exit(1)

try:
    lib = ctypes.CDLL(os.path.abspath(dll_name))
except OSError as e:
    messagebox.showerror("Ошибка загрузки DLL", str(e))
    exit(1)

# Настройка типов
lib.createSystem.restype = ctypes.c_void_p
lib.destroySystem.argtypes = [ctypes.c_void_p]
lib.addFIR.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_double), ctypes.c_int]
if hasattr(lib, 'processSignal'):
    lib.processSignal.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_double), 
                                  ctypes.POINTER(ctypes.c_double), ctypes.c_int]

# Вспомогательная функция для массивов
def to_c_double_array(data):
    return (ctypes.c_double * len(data))(*data)


# 2. КЛАСС ГРАФИЧЕСКОГО ИНТЕРФЕЙСА

class FilterApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Digital Filter Lab")
        self.root.geometry("1000x600")

        # --- Левая панель (Настройки) ---
        control_frame = tk.Frame(root, padx=10, pady=10, width=300)
        control_frame.pack(side=tk.LEFT, fill=tk.Y)

        # Метка и поле для частоты
        tk.Label(control_frame, text="Частота сигнала (Гц):", font=("Arial", 10, "bold")).pack(anchor="w", pady=(0, 5))
        self.entry_freq = tk.Entry(control_frame)
        self.entry_freq.insert(0, "0.1")
        self.entry_freq.pack(fill=tk.X, pady=(0, 15))

        # Метка и поле для шума
        tk.Label(control_frame, text="Уровень шума (амплитуда):", font=("Arial", 10, "bold")).pack(anchor="w", pady=(0, 5))
        self.entry_noise = tk.Entry(control_frame)
        self.entry_noise.insert(0, "1.5")
        self.entry_noise.pack(fill=tk.X, pady=(0, 15))

        # Метка и поле для коэффициентов
        tk.Label(control_frame, text="Коэффициенты FIR фильтра\n(через запятую):", font=("Arial", 10, "bold")).pack(anchor="w", pady=(0, 5))
        self.entry_coeffs = tk.Entry(control_frame)
        # Пример: скользящее среднее по 5 точкам
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

        # Создаем фигуру Matplotlib
        self.fig, self.ax = plt.subplots(figsize=(5, 4), dpi=100)
        self.canvas = FigureCanvasTkAgg(self.fig, master=plot_frame)
        self.canvas.draw()
        self.canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=True)

    def run_processing(self):
        try:
            # 1. Считываем данные из полей ввода
            freq = float(self.entry_freq.get())
            noise_level = float(self.entry_noise.get())
            
            coeffs_str = self.entry_coeffs.get()
            coeffs = [float(x.strip()) for x in coeffs_str.split(',')]
            
            # 2. Генерируем сигнал в Python
            N = 200
            input_data = []
            for i in range(N):
                val = math.sin(i * freq) * 5.0 + random.uniform(-noise_level, noise_level)
                input_data.append(val)
            
            # 3. Работаем с C++ DLL
            # Создаем систему
            system = lib.createSystem()
            
            # Добавляем фильтр с пользовательскими коэффициентами
            c_coeffs = to_c_double_array(coeffs)
            lib.addFIR(system, b"UserFilter", c_coeffs, len(coeffs))
            
            # Подготовка массивов
            c_input = to_c_double_array(input_data)
            output_data = [0.0] * N
            c_output = to_c_double_array(output_data)
            
            # Вызов расчета
            if hasattr(lib, 'processSignal'):
                lib.processSignal(system, b"UserFilter", c_input, c_output, N)
            else:
                # Fallback для медленного метода, если processSignal не экспортирован
                lib.computeBlock.restype = ctypes.c_double
                lib.resetAll(system)
                for i in range(N):
                    c_output[i] = lib.computeBlock(system, b"UserFilter", ctypes.c_double(input_data[i]))
            
            # Удаляем систему (важно для очистки памяти в C++)
            lib.destroySystem(system)
            
            # 4. Рисуем результат
            self.ax.clear()
            self.ax.plot(input_data, label='Входной (Шум)', color='lightgray', alpha=0.7)
            self.ax.plot(list(c_output), label='Выход (Фильтр)', color='red', linewidth=2)
            self.ax.set_title("Результат обработки C++")
            self.ax.grid(True)
            self.ax.legend()
            
            # Обновляем холст
            self.canvas.draw()

        except ValueError:
            messagebox.showerror("Ошибка ввода", "Проверьте, что во всех полях введены числа.\nКоэффициенты должны быть разделены запятой.")
        except Exception as e:
            messagebox.showerror("Ошибка", f"Произошла ошибка: {e}")


# 3. ТОЧКА ВХОДА

if __name__ == "__main__":
    root = tk.Tk()
    app = FilterApp(root)
    root.mainloop()