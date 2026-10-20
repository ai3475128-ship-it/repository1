#include "linalg.h"
#include <algorithm>
#include <cmath>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>


namespace linalg {
    namespace
    {
        void require_same_size(const Matrix& a, const Matrix& b)
        {
            if (a.rows() != b.rows() || a.columns() != b.columns())
                throw std::runtime_error("Matrix: sizes do not match");
        }
    }

    Matrix::Matrix(std::size_t rows):
        Matrix(rows, 1) {

    }

    Matrix::Matrix(std::size_t rows, std::size_t columns):
    m_ptr(new double[rows * columns]()), //выделяем память и заполняем нулями, а не мусором
    m_rows(rows),
    m_columns(columns), //сразу заполняем поля значениями, до входа в тело конструктора
    m_capacity(rows * columns) //выделяем необходимое количество
    {

    }

    Matrix::~Matrix() {
        delete[] m_ptr;
    }

    Matrix::Matrix(const Matrix& other): //выделяем сразу под актуальное количество элементов
    m_ptr(new double[other.size()]),
    m_rows(other.m_rows),
    m_columns(other.m_columns),
    m_capacity(other.size())
    { std::copy(other.begin(), other.end(), m_ptr); }

    Matrix::Matrix(Matrix&& other) noexcept
    : m_ptr(other.m_ptr)
    , m_rows(other.m_rows)
    , m_columns(other.m_columns)
    , m_capacity(other.m_capacity)
    {
        other.m_ptr = nullptr;
        other.m_rows = 0;
        other.m_columns = 0;
        other.m_capacity = 0;
    }
    //копируем поля и обнуляем источники

    Matrix::Matrix(std::initializer_list<double> list)
        : Matrix(list.size(), 1)
    {
        std::copy(list.begin(), list.end(), m_ptr);
    }

    Matrix::Matrix(std::initializer_list<std::initializer_list<double>> list)
        : Matrix(list.size(), list.size() == 0 ? 0 : list.begin()->size())
    {
        double* out = m_ptr;
        for (const auto& row : list)
        {
            if (row.size() != m_columns)
                throw std::runtime_error("Matrix: rows have different lengths");
            out = std::copy(row.begin(), row.end(), out);
        }
    }

    Matrix& Matrix::operator=(const Matrix& other)
    {
        if (this == &other)
            return *this;

        if (other.size() > m_capacity)
        {
            double* new_ptr = new double[other.size()];
            delete[] m_ptr;
            m_ptr = new_ptr;
            m_capacity = other.size();
        }

        m_rows = other.m_rows;
        m_columns = other.m_columns;
        std::copy(other.begin(), other.end(), m_ptr);
        return *this;
    }

    Matrix& Matrix::operator=(Matrix&& other) noexcept
    {
        if (this == &other)
            return *this;

        delete[] m_ptr;

        m_ptr = other.m_ptr;
        m_rows = other.m_rows;
        m_columns = other.m_columns;
        m_capacity = other.m_capacity;

        other.m_ptr = nullptr;
        other.m_rows = 0;
        other.m_columns = 0;
        other.m_capacity = 0;
        return *this;
    }

    void Matrix::reserve(std::size_t n)
    {
        if (n <= m_capacity)
            return;

        double* new_ptr = new double[n];
        std::copy(begin(), end(), new_ptr);
        delete[] m_ptr;
        m_ptr = new_ptr;
        m_capacity = n;
    }

    void Matrix::reshape(std::size_t rows, std::size_t columns) // меняем строки и столбцы, если помещается; если нет, перевыделяем память
    {
        const std::size_t n = rows * columns;
        if (n > m_capacity)
        {
            double* new_ptr = new double[n];
            delete[] m_ptr;
            m_ptr = new_ptr;
            m_capacity = n;
        }
        m_rows = rows;
        m_columns = columns;
    }

    void Matrix::shrink_to_fit() // выделяем память ровно на size() элементов, копируем элементы, очищаем старый, обновляем указатель и вместимость
    {
        const std::size_t n = size();
        if (n == m_capacity)
            return;

        double* new_ptr = n == 0 ? nullptr : new double[n];
        std::copy(begin(), end(), new_ptr);
        delete[] m_ptr;
        m_ptr = new_ptr;
        m_capacity = n;
    }

    void Matrix::swap(Matrix& other) noexcept
    {
        std::swap(m_ptr, other.m_ptr);
        std::swap(m_rows, other.m_rows);
        std::swap(m_columns, other.m_columns);
        std::swap(m_capacity, other.m_capacity);
    }

    void swap(Matrix& a, Matrix& b) noexcept
    {
        a.swap(b);
    }
    //глобальная функция вызывает метод и не имеет доступа к private, однако может быть вызвана без объекта

    std::ostream& operator<<(std::ostream& os, const Matrix& m)
    {
        std::vector<std::string> cells(m.size());
        std::vector<std::size_t> widths(m.columns(), 0);

        for (std::size_t i = 0; i < m.rows(); ++i)
        {
            for (std::size_t j = 0; j < m.columns(); ++j)
            {
                std::ostringstream ss;
                ss << m(i, j);
                cells[i * m.columns() + j] = ss.str();
                widths[j] = std::max(widths[j], cells[i * m.columns() + j].size());
            }
        }

        for (std::size_t i = 0; i < m.rows(); ++i)
        {
            os << '|';
            for (std::size_t j = 0; j < m.columns(); ++j)
            {
                if (j != 0)
                    os << ' ';
                const std::string& cell = cells[i * m.columns() + j];
                os << std::string(widths[j] - cell.size(), ' ') << cell;
            }
            os << "|\n";
        }
        return os;
    }

    Matrix operator+(const Matrix& m)
    {
        return m;
    }

    Matrix operator-(const Matrix& m)
    {
        Matrix result(m);
        for (double& x : result)
            x = -x;
        return result;
    }

    Matrix& Matrix::operator+=(const Matrix& other)
    {
        require_same_size(*this, other);
        for (std::size_t i = 0; i < size(); ++i)
            m_ptr[i] += other.m_ptr[i];
        return *this;
    }

    Matrix operator+(const Matrix& a, const Matrix& b)
    {
        Matrix result(a);
        result += b;
        return result;
    }

    Matrix& Matrix::operator-=(const Matrix& other)
    {
        require_same_size(*this, other);
        for (std::size_t i = 0; i < size(); ++i)
            m_ptr[i] -= other.m_ptr[i];
        return *this;
    }

    Matrix operator-(const Matrix& a, const Matrix& b)
    {
        Matrix result(a);
        result -= b;
        return result;
    }

    Matrix& Matrix::operator*=(double k) noexcept
    {
        for (double& x : *this)
            x *= k;
        return *this;
    }

    Matrix operator*(const Matrix& m, double k)
    {
        Matrix result(m);
        result *= k;
        return result;
    }

    Matrix operator*(double k, const Matrix& m)
    {
        return m * k;
    }

    Matrix operator*(const Matrix& a, const Matrix& b)
    {
        if (a.columns() != b.rows())
            throw std::runtime_error("Matrix: incompatible sizes for multiplication");

        Matrix result(a.rows(), b.columns());
        for (std::size_t i = 0; i < a.rows(); ++i)
            for (std::size_t k = 0; k < a.columns(); ++k)
                for (std::size_t j = 0; j < b.columns(); ++j)
                    result(i, j) += a(i, k) * b(k, j);
        return result;
    }

    Matrix& Matrix::operator*=(const Matrix& other)
    {
        Matrix result = *this * other; // считаем во временный объект: размер результата может быть другим
        swap(result);
        return *this;
    }

    bool operator==(const Matrix& a, const Matrix& b)
    {
        if (a.rows() != b.rows() || a.columns() != b.columns())
            return false;

        constexpr double eps = 1e-9; // сравнение double только с точностью (0.3*3 != 0.9)
        const double* pa = a.begin();
        const double* pb = b.begin();
        for (std::size_t i = 0; i < a.size(); ++i)
            if (std::fabs(pa[i] - pb[i]) > eps)
                return false;
        return true;
    }

    bool operator!=(const Matrix& a, const Matrix& b)
    {
        return !(a == b);
    }
}