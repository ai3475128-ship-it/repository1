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
        constexpr double EPS = 1e-9; // точность сравнения вещественных чисел (0.3*3 != 0.9)

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
        require_same_size(*this, other); // сначала проверка размеров, потом изменение данных
        for (std::size_t i = 0; i < size(); ++i)
            m_ptr[i] += other.m_ptr[i];
        return *this;
    }

    Matrix operator+(const Matrix& a, const Matrix& b)
    {
        Matrix result(a);
        result += b; // бинарный плюс через +=, логика не дублируется
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
        return m * k; // число слева сводим к числу справа
    }

    Matrix operator*(const Matrix& a, const Matrix& b)
    {
        if (a.columns() != b.rows())
            throw std::runtime_error("Matrix: incompatible sizes for multiplication");

        Matrix result(a.rows(), b.columns()); // заполнена нулями
        for (std::size_t i = 0; i < a.rows(); ++i)
            for (std::size_t k = 0; k < a.columns(); ++k)
                for (std::size_t j = 0; j < b.columns(); ++j)
                    result(i, j) += a(i, k) * b(k, j);
        return result;
    }

    Matrix& Matrix::operator*=(const Matrix& other)
    {
        Matrix result = *this * other; // размер результата может быть другим, считаем во временный объект
        swap(result);
        return *this;
    }

    bool operator==(const Matrix& a, const Matrix& b)
    {
        if (a.rows() != b.rows() || a.columns() != b.columns())
            return false;

        const double* pa = a.begin();
        const double* pb = b.begin();
        for (std::size_t i = 0; i < a.size(); ++i)
            if (std::fabs(pa[i] - pb[i]) > EPS)
                return false;
        return true;
    }

    bool operator!=(const Matrix& a, const Matrix& b)
    {
        return !(a == b);
    }

    double Matrix::norm() const noexcept
    {
        double sum = 0;
        for (double x : *this)
            sum += x * x; // сумма квадратов всех элементов
        return std::sqrt(sum);
    }

    double Matrix::trace() const
    {
        if (m_rows != m_columns)
            throw std::runtime_error("Matrix: trace requires a square matrix");

        double sum = 0;
        for (std::size_t i = 0; i < m_rows; ++i)
            sum += (*this)(i, i); // сумма элементов главной диагонали
        return sum;
    }

    std::size_t Matrix::eliminate_forward() noexcept
    {
        std::size_t swaps = 0;
        std::size_t row = 0; // текущая строка, в которую ставим ведущий элемент

        for (std::size_t col = 0; col < m_columns && row < m_rows; ++col)
        {
            // выбираем строку с наибольшим по модулю элементом в столбце (точность вычислений)
            std::size_t pivot = row;
            for (std::size_t i = row + 1; i < m_rows; ++i)
                if (std::fabs((*this)(i, col)) > std::fabs((*this)(pivot, col)))
                    pivot = i;

            if (std::fabs((*this)(pivot, col)) < EPS)
                continue; // весь столбец нулевой, переходим к следующему

            if (pivot != row)
            {
                for (std::size_t j = 0; j < m_columns; ++j)
                    std::swap((*this)(row, j), (*this)(pivot, j));
                ++swaps;
            }

            // обнуляем элементы под ведущим
            for (std::size_t i = row + 1; i < m_rows; ++i)
            {
                const double factor = (*this)(i, col) / (*this)(row, col);
                (*this)(i, col) = 0;
                for (std::size_t j = col + 1; j < m_columns; ++j)
                    (*this)(i, j) -= factor * (*this)(row, j);
            }
            ++row;
        }
        return swaps;
    }

    Matrix& Matrix::gauss_forward() noexcept
    {
        eliminate_forward();
        return *this; // меняем саму матрицу и возвращаем её, а не копию
    }

    double Matrix::det() const
    {
        if (m_rows != m_columns)
            throw std::runtime_error("Matrix: determinant requires a square matrix");

        Matrix tmp(*this); // метод const, поэтому работаем с копией
        const std::size_t swaps = tmp.eliminate_forward();

        double result = (swaps % 2 == 0) ? 1.0 : -1.0; // каждая перестановка строк меняет знак
        for (std::size_t i = 0; i < m_rows; ++i)
            result *= tmp(i, i); // определитель треугольной матрицы = произведение диагонали
        return result;
    }

    std::size_t Matrix::rank() const
    {
        Matrix tmp(*this); // метод const, поэтому работаем с копией
        tmp.eliminate_forward();

        std::size_t result = 0;
        for (std::size_t i = 0; i < tmp.m_rows; ++i)
            for (std::size_t j = 0; j < tmp.m_columns; ++j)
                if (std::fabs(tmp(i, j)) > EPS) // строка ненулевая: считаем и переходим к следующей
                {
                    ++result;
                    break;
                }
        return result; // ранг = число ненулевых строк ступенчатой матрицы
    }

    Matrix& Matrix::gauss_backward() noexcept
    {
        // идём снизу вверх; считаем, что матрица уже приведена к ступенчатому виду
        for (std::size_t row = m_rows; row-- > 0;)
        {
            std::size_t lead = 0; // ведущий (первый ненулевой) столбец строки
            while (lead < m_columns && std::fabs((*this)(row, lead)) < EPS)
                ++lead;
            if (lead == m_columns)
                continue; // нулевая строка

            const double pivot = (*this)(row, lead);
            for (std::size_t j = lead; j < m_columns; ++j)
                (*this)(row, j) /= pivot; // ведущий элемент становится единицей

            for (std::size_t i = 0; i < row; ++i) // обнуляем элементы над ведущим
            {
                const double factor = (*this)(i, lead);
                (*this)(i, lead) = 0;
                for (std::size_t j = lead + 1; j < m_columns; ++j)
                    (*this)(i, j) -= factor * (*this)(row, j);
            }
        }
        return *this;
    }
}