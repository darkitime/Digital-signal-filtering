import tkinter as tk
from tkinter import ttk, messagebox
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt
import ctypes
import os
import math
import random

# --- 1. ЗАГРУЗКА DLL ---
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
# Добавляем IIR
lib.addIIR.argtypes = [
    ctypes.c_void_p, 
    ctypes.c_char_p, 
    ctypes.POINTER(ctypes.c_double), ctypes.c_int, # b
    ctypes.POINTER(ctypes.c_double), ctypes.c_int  # a
]

if hasattr(lib, 'processSignal'):
    lib.processSignal.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_double), 
                                  ctypes.POINTER(ctypes.c_double), ctypes.c_int]

def to_c_double_array(data):
    return (ctypes.c_double * len(data))(*data)

# --- 2. КЛАСС ПРИЛОЖЕНИЯ ---
class AdvancedFilterApp:
    def __init__(self, root):
        self.root = root
        self.root.title("DSP Лаборатория: FIR и IIR")
        self.root.geometry("1100x700")

        # Панель управления (слева)
        control_frame = tk.Frame(root, padx=15, pady=15, width=350, bg="#f0f0f0")
        control_frame.pack(side=tk.LEFT, fill=tk.Y)

        # === Секция 1: Генератор Сигнала ===
        tk.Label(control_frame, text="1. Генератор сигнала", bg="#f0f0f0", font=("Arial", 11, "bold")).pack(anchor="w", pady=(0, 5))
        
        tk.Label(control_frame, text="Тип сигнала:", bg="#f0f0f0").pack(anchor="w")
        self.signal_type = ttk.Combobox(control_frame, values=["Синусоида с шумом", "Ступенька (Step)", "Импульс"])
        self.signal_type.current(0)
        self.signal_type.pack(fill=tk.X, pady=(0, 10))

        tk.Label(control_frame, text="Частота (для синуса):", bg="#f0f0f0").pack(anchor="w")
        self.entry_freq = tk.Entry(control_frame)
        self.entry_freq.insert(0, "0.05")
        self.entry_freq.pack(fill=tk.X, pady=(0, 15))

        # === Секция 2: Настройка Фильтра ===
        tk.Label(control_frame, text="2. Параметры Фильтра", bg="#f0f0f0", font=("Arial", 11, "bold")).pack(anchor="w", pady=(0, 5))

        tk.Label(control_frame, text="Тип фильтра:", bg="#f0f0f0").pack(anchor="w")
        self.filter_type = ttk.Combobox(control_frame, values=["FIR (КИХ)", "IIR (БИХ)"])
        self.filter_type.current(0)
        self.filter_type.bind("<<ComboboxSelected>>", self.toggle_iir_fields)
        self.filter_type.pack(fill=tk.X, pady=(0, 10))

        # Коэффициенты B (Числитель)
        tk.Label(control_frame, text="Коэффициенты B (вход):\n(для FIR это просто coeffs)", bg="#f0f0f0").pack(anchor="w")
        self.entry_b = tk.Entry(control_frame)
        self.entry_b.insert(0, "0.2, 0.2, 0.2, 0.2, 0.2") # Пример FIR
        self.entry_b.pack(fill=tk.X, pady=(0, 10))

        # Коэффициенты A (Знаменатель) - только для IIR
        self.lbl_a = tk.Label(control_frame, text="Коэффициенты A (обратная связь):\n(Первый должен быть 1.0)", bg="#f0f0f0")
        self.entry_a = tk.Entry(control_frame)
        self.entry_a.insert(0, "1.0, -0.9") # Пример простого IIR
        
        # Скрываем A по умолчанию (так как выбран FIR)
        # (Логика скрытия реализована в toggle_iir_fields)

        # Кнопка
        self.btn_run = tk.Button(control_frame, text="РАССЧИТАТЬ", 
                                 bg="#2196F3", fg="white", font=("Arial", 12, "bold"),
                                 command=self.run_processing)
        self.btn_run.pack(fill=tk.X, pady=30)

        # График
        plot_frame = tk.Frame(root)
        plot_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True)
        self.fig, self.ax = plt.subplots(figsize=(5, 4), dpi=100)
        self.canvas = FigureCanvasTkAgg(self.fig, master=plot_frame)
        self.canvas.draw()
        self.canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=True)

        # Инициализация видимости полей
        self.toggle_iir_fields()

    def toggle_iir_fields(self, event=None):
        """Скрывает или показывает поле коэффициентов A"""
        if self.filter_type.get() == "IIR (БИХ)":
            self.lbl_a.pack(anchor="w")
            self.entry_a.pack(fill=tk.X, pady=(0, 10))
            # Предлагаем дефолтные значения для IIR
            if self.entry_b.get().startswith("0.2"): 
                self.entry_b.delete(0, tk.END); self.entry_b.insert(0, "0.1")
        else:
            self.lbl_a.pack_forget()
            self.entry_a.pack_forget()
            # Предлагаем дефолтные значения для FIR
            if self.entry_b.get() == "0.1":
                self.entry_b.delete(0, tk.END); self.entry_b.insert(0, "0.2, 0.2, 0.2, 0.2, 0.2")

    def run_processing(self):
        try:
            # 1. Генерация сигнала
            N = 300
            input_data = []
            sig_type = self.signal_type.get()
            freq = float(self.entry_freq.get())

            for i in range(N):
                val = 0.0
                if sig_type == "Синусоида с шумом":
                    val = math.sin(i * freq) * 3.0 + random.uniform(-0.5, 0.5)
                elif sig_type == "Ступенька (Step)":
                    val = 1.0 if i > 50 else 0.0
                    val += random.uniform(-0.05, 0.05) # Чуть шума
                elif sig_type == "Импульс":
                    val = 5.0 if i == 50 else 0.0
                input_data.append(val)

            # 2. Подготовка C++
            system = lib.createSystem()
            
            filt_type = self.filter_type.get()
            b_str = self.entry_b.get()
            b_coeffs = [float(x) for x in b_str.split(',')]
            c_b = to_c_double_array(b_coeffs)

            if filt_type == "FIR (КИХ)":
                lib.addFIR(system, b"MyFilter", c_b, len(b_coeffs))
            else:
                # IIR
                a_str = self.entry_a.get()
                a_coeffs = [float(x) for x in a_str.split(',')]
                c_a = to_c_double_array(a_coeffs)
                lib.addIIR(system, b"MyFilter", c_b, len(b_coeffs), c_a, len(a_coeffs))

            # 3. Расчет
            c_input = to_c_double_array(input_data)
            output_data = [0.0] * N
            c_output = to_c_double_array(output_data)

            if hasattr(lib, 'processSignal'):
                lib.processSignal(system, b"MyFilter", c_input, c_output, N)
            else:
                lib.computeBlock.restype = ctypes.c_double
                lib.resetAll(system)
                for i in range(N):
                    c_output[i] = lib.computeBlock(system, b"MyFilter", ctypes.c_double(input_data[i]))

            lib.destroySystem(system)

            # 4. Визуализация
            self.ax.clear()
            self.ax.plot(input_data, label='Вход', color='#CCCCCC', linestyle='--')
            self.ax.plot(list(c_output), label='Выход', color='#FF5722', linewidth=2)
            self.ax.set_title(f"Результат: {filt_type} | Сигнал: {sig_type}")
            self.ax.legend()
            self.ax.grid(True, linestyle=':', alpha=0.6)
            self.canvas.draw()

        except Exception as e:
            messagebox.showerror("Ошибка", f"Некорректные данные!\n{e}")

if __name__ == "__main__":
    root = tk.Tk()
    app = AdvancedFilterApp(root)
    root.mainloop()