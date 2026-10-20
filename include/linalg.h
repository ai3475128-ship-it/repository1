#pragma once
#include <cstddef>
#include <initializer_list>

//определяем класс в linalg
namespace linalg {
    class Matrix {
    public:
        Matrix() noexcept = default;//дефолтный конструктор(компилятор сам сощдает пустую матрицу); noexcept=без исключений
        explicit Matrix(std::size_t rows); //explicit не позволяет случиться невозможным преобразованиям(число в матрицу)
        Matrix(std::size_t rows, std :: size_t columns);
        Matrix(const Matrix& other);
        Matrix(Matrix&& other) noexcept; //rvalue ссылка. привязана к временным объектам
        Matrix(std::initializer_list<double> list);
        Matrix(std::initializer_list<std::initializer_list<double>> list);
        Matrix& operator=(const Matrix& other);
        ~Matrix();

        bool empty() const noexcept { return m_rows == 0 || m_columns == 0; }
        std::size_t rows() const noexcept { return m_rows; }
        std::size_t columns() const noexcept { return m_columns; }
        std::size_t capacity() const noexcept { return m_capacity; } // сколько элементов возможно
        std::size_t size() const noexcept { return m_rows * m_columns; } // актальное количество элементов
        //публичной для доступности конструктора и деструктора для пользователя
        double *begin() noexcept {return m_ptr;}
        double *end() noexcept {return m_ptr + size();}
        const double *begin() const noexcept {return m_ptr;}
        const double *end() const noexcept {return m_ptr + size();}
        //const *double* нужен для const Matrix, где элементы менять нельзя
    private:
        double *m_ptr = nullptr;
        std :: size_t m_rows = 0;
        std :: size_t m_columns = 0;
        std:: size_t m_capacity =0; // m_capacity>=m_columns*m_rows (памяти хотя бы столько же, сколько элементов в матрице
        //поля скрыты от пользователя, работа только через методы. так класс следит за инвариантом(корректностью своих данных)

    };
}
