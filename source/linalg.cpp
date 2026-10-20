#include "linalg.h"

namespace linalg {
    Matrix::Matrix(std::size_t rows) :
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
}

