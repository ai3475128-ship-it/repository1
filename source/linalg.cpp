#include "linalg.h"
#include <algorithm>
#include <cmath>
#include <limits>
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

        void require_not_empty(const Matrix& m) // операции над пустой матрицей не имеют смысла
        {
            if (m.empty())
                throw std::runtime_error("Matrix: matrix is empty");
        }

        Matrix identity(std::size_t n) // единичная матрица n x n
        {
            Matrix result(n, n); // заполнена нулями
            for (std::size_t i = 0; i < n; ++i)
                result(i, i) = 1;
            return result;
        }

        // размер матрицы rows * columns без переполнения size_t
        std::size_t checked_size(std::size_t rows, std::size_t columns)
        {
            if (columns != 0 && rows > std::numeric_limits<std::size_t>::max() / columns)
                throw std::runtime_error("Matrix: size is too large");
            return rows * columns;
        }
    }

    Matrix::Matrix(std::size_t rows):
        Matrix(rows, 1) {

    }

    Matrix::Matrix(std::size_t rows, std::size_t columns):
    m_ptr(new double[checked_size(rows, columns)]()), //выделяем память и заполняем нулями; checked_size бросит исключение при переполнении
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
            Matrix tmp(other); // новая память выделяется во временном объекте
            swap(tmp);         // забираем её, а старая уйдёт вместе с tmp
            return *this;
        }

        m_rows = other.m_rows; // памяти хватает: переиспользуем массив
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
        const std::size_t n = checked_size(rows, columns); // при переполнении бросит исключение, матрица не изменится
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

    double& Matrix::operator()(std::size_t row, std::size_t column)
    {
        if (row >= m_rows || column >= m_columns) // для пустой матрицы условие тоже истинно
            throw std::runtime_error("Matrix: index out of range");
        return m_ptr[row * m_columns + column];
    }

    const double& Matrix::operator()(std::size_t row, std::size_t column) const
    {
        if (row >= m_rows || column >= m_columns)
            throw std::runtime_error("Matrix: index out of range");
        return m_ptr[row * m_columns + column];
    }

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
        require_not_empty(m);
        return m;
    }

    Matrix operator-(const Matrix& m)
    {
        require_not_empty(m);
        Matrix result(m);
        for (double& x : result)
            x = -x;
        return result;
    }

    Matrix& Matrix::operator+=(const Matrix& other)
    {
        require_not_empty(*this);
        require_not_empty(other);
        require_same_size(*this, other); // сначала проверки, потом изменение данных
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
        require_not_empty(*this);
        require_not_empty(other);
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

    Matrix& Matrix::operator*=(double k)
    {
        require_not_empty(*this);
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
        require_not_empty(a);
        require_not_empty(b);
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

    double Matrix::norm() const
    {
        require_not_empty(*this);
        double sum = 0;
        for (double x : *this)
            sum += x * x; // сумма квадратов всех элементов
        return std::sqrt(sum);
    }

    double Matrix::trace() const
    {
        require_not_empty(*this);
        if (m_rows != m_columns)
            throw std::runtime_error("Matrix: trace requires a square matrix");

        double sum = 0;
        for (std::size_t i = 0; i < m_rows; ++i)
            sum += raw(i, i); // сумма элементов главной диагонали
        return sum;
    }

    std::size_t Matrix::eliminate_forward(double& scale) noexcept
    {
        scale = 1.0; // произведение ведущих элементов, на которые мы делили строки
        std::size_t swaps = 0;
        std::size_t row = 0; // текущая строка, в которую ставим ведущий элемент

        for (std::size_t col = 0; col < m_columns && row < m_rows; ++col)
        {
            // выбираем строку с наибольшим по модулю элементом в столбце (точность вычислений)
            std::size_t pivot = row;
            for (std::size_t i = row + 1; i < m_rows; ++i)
                if (std::fabs(raw(i, col)) > std::fabs(raw(pivot, col)))
                    pivot = i;

            if (std::fabs(raw(pivot, col)) < EPS)
                continue; // весь столбец нулевой, переходим к следующему

            if (pivot != row)
            {
                for (std::size_t j = 0; j < m_columns; ++j)
                    std::swap(raw(row, j), raw(pivot, j));
                ++swaps;
            }

            // нормировка: делим строку на ведущий элемент, он становится единицей
            const double lead = raw(row, col);
            scale *= lead; // запоминаем делитель, чтобы определитель можно было восстановить
            raw(row, col) = 1.0;
            for (std::size_t j = col + 1; j < m_columns; ++j)
                raw(row, j) /= lead;

            // обнуляем элементы под ведущим (ведущий равен 1, множитель = сам элемент)
            for (std::size_t i = row + 1; i < m_rows; ++i)
            {
                const double factor = raw(i, col);
                raw(i, col) = 0;
                for (std::size_t j = col + 1; j < m_columns; ++j)
                    raw(i, j) -= factor * raw(row, j);
            }
            ++row;
        }
        return swaps;
    }

    bool Matrix::is_unit_upper_triangular() const noexcept
    {
        const std::size_t n = std::min(m_rows, m_columns);
        for (std::size_t i = 0; i < n; ++i)
        {
            if (std::fabs(raw(i, i) - 1.0) > EPS)
                return false; // на диагонали не единица
            for (std::size_t j = 0; j < i; ++j)
                if (std::fabs(raw(i, j)) > EPS)
                    return false; // под диагональю не ноль
        }
        for (std::size_t i = n; i < m_rows; ++i) // лишние строки ниже должны быть нулевыми
            for (std::size_t j = 0; j < m_columns; ++j)
                if (std::fabs(raw(i, j)) > EPS)
                    return false;
        return true;
    }

    Matrix& Matrix::gauss_forward()
    {
        require_not_empty(*this);
        double scale;
        eliminate_forward(scale);
        return *this; // меняем саму матрицу и возвращаем её, а не копию
    }

    double Matrix::det() const
    {
        require_not_empty(*this);
        if (m_rows != m_columns)
            throw std::runtime_error("Matrix: determinant requires a square matrix");

        Matrix tmp(*this); // метод const, поэтому работаем с копией
        double scale;
        const std::size_t swaps = tmp.eliminate_forward(scale);

        // нормировка делила строки на ведущие элементы, поэтому умножаем обратно (scale);
        // каждая перестановка строк меняет знак
        double result = (swaps % 2 == 0) ? scale : -scale;
        for (std::size_t i = 0; i < m_rows; ++i)
            result *= tmp.raw(i, i); // диагональ: единицы (или ноль у вырожденной матрицы)
        return result;
    }

    std::size_t Matrix::rank() const
    {
        require_not_empty(*this);
        Matrix tmp(*this); // метод const, поэтому работаем с копией
        double scale;
        tmp.eliminate_forward(scale);

        std::size_t result = 0;
        for (std::size_t i = 0; i < tmp.m_rows; ++i)
            for (std::size_t j = 0; j < tmp.m_columns; ++j)
                if (std::fabs(tmp.raw(i, j)) > EPS) // строка ненулевая: считаем и переходим к следующей
                {
                    ++result;
                    break;
                }
        return result; // ранг = число ненулевых строк ступенчатой матрицы
    }

    Matrix& Matrix::gauss_backward()
    {
        require_not_empty(*this);

        if (!is_unit_upper_triangular()) // если ещё не приведена, сначала делаем прямой ход
        {
            double scale;
            eliminate_forward(scale);
        }

        // идём снизу вверх; ведущие элементы уже равны 1
        for (std::size_t row = m_rows; row-- > 0;)
        {
            std::size_t lead = 0; // ведущий (первый ненулевой) столбец строки
            while (lead < m_columns && std::fabs(raw(row, lead)) < EPS)
                ++lead;
            if (lead == m_columns)
                continue; // нулевая строка

            for (std::size_t i = 0; i < row; ++i) // обнуляем элементы над ведущим
            {
                const double factor = raw(i, lead);
                raw(i, lead) = 0;
                for (std::size_t j = lead + 1; j < m_columns; ++j)
                    raw(i, j) -= factor * raw(row, j);
            }
        }
        return *this;
    }

    Matrix concatenate(const Matrix& a, const Matrix& b)
    {
        require_not_empty(a);
        require_not_empty(b);
        if (a.rows() != b.rows())
            throw std::runtime_error("Matrix: concatenate requires equal number of rows");

        Matrix result(a.rows(), a.columns() + b.columns());
        for (std::size_t i = 0; i < a.rows(); ++i)
        {
            for (std::size_t j = 0; j < a.columns(); ++j)
                result(i, j) = a(i, j); // левая часть
            for (std::size_t j = 0; j < b.columns(); ++j)
                result(i, a.columns() + j) = b(i, j); // правая часть
        }
        return result;
    }

    Matrix transpose(const Matrix& m)
    {
        require_not_empty(m);
        Matrix result(m.columns(), m.rows());
        for (std::size_t i = 0; i < m.rows(); ++i)
            for (std::size_t j = 0; j < m.columns(); ++j)
                result(j, i) = m(i, j); // строки и столбцы меняются местами
        return result;
    }

    Matrix invert(const Matrix& m)
    {
        require_not_empty(m);
        if (m.rows() != m.columns())
            throw std::runtime_error("Matrix: inverse requires a square matrix");

        const std::size_t n = m.rows();
        Matrix aug = concatenate(m, identity(n)); // расширенная матрица [A | E]
        aug.gauss_forward();

        for (std::size_t i = 0; i < n; ++i)
            if (std::fabs(aug(i, i)) < EPS)
                throw std::runtime_error("Matrix: matrix is singular"); // нулевой элемент на диагонали: обратной нет

        aug.gauss_backward(); // слева получается E, справа A^-1

        Matrix result(n, n);
        for (std::size_t i = 0; i < n; ++i)
            for (std::size_t j = 0; j < n; ++j)
                result(i, j) = aug(i, n + j);
        return result;
    }

    Matrix power(const Matrix& m, int n)
    {
        require_not_empty(m);
        if (m.rows() != m.columns())
            throw std::runtime_error("Matrix: power requires a square matrix");

        Matrix base = (n < 0) ? invert(m) : m; // отрицательная степень = степень обратной матрицы
        long long e = n;
        if (e < 0)
            e = -e;

        Matrix result = identity(m.rows()); // A^0 = E
        while (e > 0) // быстрое возведение в степень
        {
            if (e % 2 == 1)
                result *= base;
            base *= base;
            e /= 2;
        }
        return result;
    }

    Matrix solve(const Matrix& A, const Matrix& f)
    {
        require_not_empty(A);
        require_not_empty(f);
        if (A.rows() != A.columns())
            throw std::runtime_error("Matrix: solve requires a square matrix A");
        if (A.rows() != f.rows())
            throw std::runtime_error("Matrix: A and f must have equal number of rows");

        const std::size_t n = A.rows();
        Matrix aug = concatenate(A, f); // расширенная матрица [A | f]
        aug.gauss_forward();

        for (std::size_t i = 0; i < n; ++i)
            if (std::fabs(aug(i, i)) < EPS)
                throw std::runtime_error("Matrix: system has no unique solution");

        aug.gauss_backward(); // слева E, справа решение

        Matrix result(n, f.columns());
        for (std::size_t i = 0; i < n; ++i)
            for (std::size_t j = 0; j < f.columns(); ++j)
                result(i, j) = aug(i, n + j);
        return result;
    }
}