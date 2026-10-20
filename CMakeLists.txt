cmake_minimum_required(VERSION 3.15)
project(linalg LANGUAGES CXX)
#создаем статичную библиотеку, которая будт вшиваться в программу
add_library(linalg STATIC source/linalg.cpp)
#обозначаем, нде лежат заголовки: папка /include
target_include_directories(linalg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(linalg PUBLIC cxx_std_17)