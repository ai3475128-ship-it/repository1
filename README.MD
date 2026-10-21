# linalg

Библиотека линейной алгебры на C++17. Центральная сущность — класс `Matrix`
(матрица произвольного размера из элементов типа `double`) в пространстве имён `linalg`.

## Возможности

- Хранение в одном динамическом массиве с вместимостью (как у `std::vector`)
- Конструкторы: по умолчанию, по размеру, копирования, перемещения, из `std::initializer_list`
- Методы: `rows`, `columns`, `size`, `capacity`, `empty`, `reserve`, `reshape`, `clear`, `shrink_to_fit`, `swap`
- Доступ к элементам: `m(i, j)`, итераторы `begin`/`end`
- Вывод в поток с выравниванием по столбцам
- Арифметика: `+`, `-`, `*` (матрица и число), `+=`, `-=`, `*=`, `==`, `!=`
- Вычисления: `norm`, `trace`, `det`, `rank`, `gauss_forward`, `gauss_backward`
- Функции: `concatenate`, `transpose`, `invert`, `power`, `solve`
- Ошибки сообщаются исключениями `std::runtime_error`

## Сборка

Конфигурация проекта (генерирует файлы сборки в папку `build`):

    cmake -B build

Сборка библиотеки:

    cmake --build build

Результат — статическая библиотека `liblinalg.a` в папке `build`.

## Подключение

Библиотека предназначена для подключения как git-подмодуль:

    git submodule add <ссылка на репозиторий> linalg

В `CMakeLists.txt` проекта-потребителя:

    add_subdirectory(linalg)
    target_link_libraries(<ваша_цель> PRIVATE linalg)

## Пример

    #include "linalg.h"
    #include <iostream>

    int main()
    {
        linalg::Matrix a = { {2, 1}, {5, 3} };
        std::cout << linalg::invert(a);
    }