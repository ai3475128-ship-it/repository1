#pragma once
#include <cstddef>

//определяем класс в linalg
namespace linalg {
    class Matrix {
    public:
        Matrix() noexcept = default;//дефолтный конструктор(компилятор сам сощдает пустую матрицу); noexcept=без исключений
        explicit Matrix(std::size_t rows); //explicit не позволяет случиться невозможным преобразованиям(число в матрицу)
        Matrix(std::size_t rows, std :: size_t columns);
        ~Matrix();

        bool empty() const noexcept { return m_rows == 0 || m_columns == 0; }
        std::size_t rows() const noexcept { return m_rows; }
        std::size_t columns() const noexcept { return m_columns; }
        std::size_t capacity() const noexcept { return m_capacity; }
        std::size_t size() const noexcept { return m_rows * m_columns; }
        //публичной для доступности конструктора и деструктора для пользователя
    private:
        double *m_ptr = nullptr;
        std :: size_t m_rows = 0;
        std :: size_t m_columns = 0;
        std:: size_t m_capacity =0; // m_capacity>=m_columns*m_rows (памяти хотя бы столько же, сколько элементов в матрице
        //поля скрыты от пользователя, работа только через методы. так класс следит за инвариантом(корректностью своих данных)

    };
}
