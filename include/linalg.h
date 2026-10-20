#pragma once
#include <cstddef>

//определяем класс в linalg
namespace linalg {
    class Matrix {
    public:
        Matrix() noexcept = default;//дефолтный конструктор(компилятор сам сощдает пустую матрицу); noexcept=без исключений
        ~Matrix();
        //публичной для доступности конструктора и деструктора для пользователя
    private:
        double *m_ptr = nullptr;
        std :: size_t m_rows = 0;
        std :: size_t m_columns = 0;
        std:: size_t m_capacity =0; // m_capacity>=m_columns*m_rows (памяти хотя бы столько же, сколько элементов в матрице
        //поля скрыты от пользователя, работа только через методы. так класс следит за инвариантом(корректностью своих данных)

    };
}
