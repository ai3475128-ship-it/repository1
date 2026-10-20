#include "linalg.h"

#include <algorithm>

namespace linalg {
    Matrix::Matrix(std::size_t rows):
        Matrix(rows,1) {

    }
    Matrix::Matrix(std::size_t rows, std::size_t columns):
    m_ptr(new double[rows*columns]()),//выделяем память и заполняем нулями, а не мусором
    m_rows(rows),
    m_columns(columns), //сразу заполняем поля значениями, до входа в тело конмтруктора
    m_capacity(rows*columns)//выделяем необходимое количество
    {

    }
    Matrix::~Matrix() {
        delete[] m_ptr;
    }
    Matrix::Matrix(const Matrix& other)://выделяем сразу под актуальное количество элементов
    m_ptr(new double[other.size()]),
    m_rows(other.m_rows),
    m_columns(other.m_columns),
    m_capacity(other.size())
    {std :: copy(other.begin(), other.end(), m_ptr);}

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
    //копируем плоя и обнуляем источники
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
    void Matrix::reshape(std::size_t rows, std::size_t columns)
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

    void Matrix::shrink_to_fit()
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


}

