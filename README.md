# SimpleHEXViewer

Простой HEX просмотрщик / Simple HEX Viewer

## Описание / Description

### Русский
Репозиторий содержит готовый исполняемый файл (.exe) и папку с исходным кодом.

**Структура исходного кода:**

**Заголовочные файлы (.h):**
- `constants.h` - заголовочный файл с константами, используемыми в программе
- `DPI.h` - заголовочный файл с функциями для управления DPI
- `HelpersWinAPI.h` - заголовочный файл с вспомогательными функциями WinAPI
- `OpenHexFile.h` - заголовочный файл с функцией открытия файлов
- `SearchStructure.h` - заголовочный файл со структурой для окна поиска
- `WindowLocalStruct.h` - заголовочный файл со структурой главного окна
- `WindowsProc.h` - заголовочный файл с обработчиками оконных сообщений

**Исходные файлы (.c):**
- `main.c` - главный файл с точкой входа и основной логикой приложения
- `HelpersWinAPI.c` - реализация вспомогательных функций WinAPI
- `OpenHexFile.c` - реализация функций для открытия и работы с HEX-файлами
- `WindowsProc.c` - реализация обработчиков оконных процедур и сообщений
- `DPI.c` - реализация функций для работы с DPI и масштабированием

### English
The repository contains a ready-to-run executable file (.exe) and a folder with source code.

**Source code structure:**

**Header files (.h):**
- `constants.h` - header file containing constants used in the program
- `DPI.h` - header file with functions for DPI management
- `HelpersWinAPI.h` - header file with auxiliary WinAPI functions
- `OpenHexFile.h` - header file with file opening function
- `SearchStructure.h` - header file with structure for search window
- `WindowLocalStruct.h` - header file with main window structure
- `WindowsProc.h` - header file with window message handlers

**Source files (.c):**
- `main.c` - main file with entry point and core application logic
- `HelpersWinAPI.c` - implementation of auxiliary WinAPI functions
- `OpenHexFile.c` - implementation of functions for opening and working with HEX files
- `WindowsProc.c` - implementation of window procedures and message handlers
- `DPI.c` - implementation of DPI and scaling functions
