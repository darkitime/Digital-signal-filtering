import os
import sys

# Получаем точный путь к папке, где лежит сам файл conf.py
current_dir = os.path.dirname(os.path.abspath(__file__))

# Добавляем в зону видимости саму папку (если скрипты лежат рядом с conf.py)
sys.path.insert(0, current_dir)

# И добавляем папку на уровень выше (если скрипты лежат в корне PythonInterface)
parent_dir = os.path.abspath(os.path.join(current_dir, '..'))
sys.path.insert(0, parent_dir)

project = 'Digital signal filtering'
copyright = '2026, D'
author = 'D'
release = 'X'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ['sphinx.ext.autodoc',]

exclude_patterns = []

language = 'ru'

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
# Переменная-маячок для наших скриптов
os.environ['SPHINX_BUILD'] = '1'

# Игнорировать ошибки импорта тяжелых библиотек при сборке
autodoc_mock_imports = ["tkinter", "matplotlib"]