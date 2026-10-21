#pragma once
#include <cstddef>
#include <initializer_list>
#include <iosfwd>


//определяем класс в linalg
namespace linalg {
    class Matrix {
    public:
        Matrix() noexcept = default; //дефолтный конструктор (компилятор сам создаёт пустую матрицу); noexcept = без исключений
        explicit Matrix(std::size_t rows); //explicit не позволяет случиться невозможным преобразованиям (число в матрицу)
        Matrix(std::size_t rows, std::size_t columns);
        Matrix(const Matrix& other);
        Matrix(Matrix&& other) noexcept; //rvalue ссылка, привязана к временным объектам
        Matrix(std::initializer_list<double> list);
        Matrix(std::initializer_list<std::initializer_list<double>> list);
        Matrix& operator=(const Matrix& other);
        Matrix& operator=(Matrix&& other) noexcept;
        Matrix& operator+=(const Matrix& other);
        Matrix& operator-=(const Matrix& other);
        Matrix& operator*=(const Matrix& other);
        Matrix& operator*=(double k);
        ~Matrix();

        //публичной для доступности конструктора и деструктора для пользователя
        bool empty() const noexcept { return m_rows == 0 || m_columns == 0; }
        std::size_t rows() const noexcept { return m_rows; }
        std::size_t columns() const noexcept { return m_columns; }
        std::size_t capacity() const noexcept { return m_capacity; } // сколько элементов возможно
        std::size_t size() const noexcept { return m_rows * m_columns; } // актуальное количество элементов

        double* begin() noexcept { return m_ptr; }
        double* end() noexcept { return m_ptr + size(); }
        const double* begin() const noexcept { return m_ptr; }
        const double* end() const noexcept { return m_ptr + size(); }
        //const double* нужен для const Matrix, где элементы менять нельзя

        void reserve(std::size_t n);
        void reshape(std::size_t rows, std::size_t columns);
        void clear() noexcept { m_rows = 0; m_columns = 0; }
        void shrink_to_fit();
        void swap(Matrix& other) noexcept;

        double norm() const;          // норма Фробениуса, матрица не должна быть пустой
        double trace() const;         // след, только для квадратной матрицы
        double det() const;           // определитель, только для квадратной матрицы
        std::size_t rank() const;     // ранг
        Matrix& gauss_forward();      // прямой ход с нормировкой диагонали, меняет саму матрицу
        Matrix& gauss_backward();     // обратный ход, меняет саму матрицу

        // доступ с проверкой границ: выход за границы или пустая матрица -> std::runtime_error
        double& operator()(std::size_t row, std::size_t column);
        const double& operator()(std::size_t row, std::size_t column) const;

    private:
        //индексы верны
        double& raw(std::size_t row, std::size_t column) noexcept
        {
            return m_ptr[row * m_columns + column];
        }
        const double& raw(std::size_t row, std::size_t column) const noexcept
        {
            return m_ptr[row * m_columns + column];
        }

        // прямой ход с нормировкой; scale = произведение исходных ведущих элементов, возвращает число перестановок строк
        std::size_t eliminate_forward(double& scale) noexcept;
        bool is_unit_upper_triangular() const noexcept; // уже верхнетреугольная с единицами на диагонали?

        double* m_ptr = nullptr;
        std::size_t m_rows = 0;
        std::size_t m_columns = 0;
        std::size_t m_capacity = 0; // m_capacity >= m_columns * m_rows (памяти хотя бы столько же, сколько элементов в матрице)
        //поля скрыты от пользователя, работа только через методы. так класс следит за инвариантом (корректностью своих данных)
    };

    void swap(Matrix& a, Matrix& b) noexcept;
    std::ostream& operator<<(std::ostream& os, const Matrix& m);

    Matrix operator+(const Matrix& m);
    Matrix operator-(const Matrix& m);
    Matrix operator+(const Matrix& a, const Matrix& b);
    Matrix operator-(const Matrix& a, const Matrix& b);
    Matrix operator*(const Matrix& a, const Matrix& b);
    Matrix operator*(const Matrix& m, double k);
    Matrix operator*(double k, const Matrix& m);

    bool operator==(const Matrix& a, const Matrix& b);
    bool operator!=(const Matrix& a, const Matrix& b);

    // функции не меняют аргументы, а возвращают новые матрицы
    Matrix concatenate(const Matrix& a, const Matrix& b); // приписать b справа от a
    Matrix transpose(const Matrix& m);
    Matrix invert(const Matrix& m);
    Matrix power(const Matrix& m, int n);
    Matrix solve(const Matrix& A, const Matrix& f);       // решение A * x = f методом Гаусса
}