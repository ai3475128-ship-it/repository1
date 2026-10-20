# linalg

Библиотека линейной алгебры: динамический размер класса `Matrix` из `double` элементов

## Build
    cmake -B build // запуск программы cmake, указываем на папку build
    cmake --build build // сборка проекта, используя файлы из папки build

Результат — статическая библиотека `liblinalg.a` в папке `build`.

## Подключение

Библиотека предназначена для подключения как git-подмодуль:

    git submodule add <https://git.miem.hse.ru/ivanov_lab_1/matrix-lib.git> linalg

В `CMakeLists.txt` проекта-потребителя:

    add_subdirectory(linalg)
    target_link_libraries(<лаба> PRIVATE linalg)
