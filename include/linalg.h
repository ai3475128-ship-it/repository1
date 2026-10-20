#pragma once
#include <cstddef>

//определяем класс в linalg
namespace linalg {
    class Matrix {
    private:
        double *m_ptr = nullptr;
        std :: size_t m_rows = 0;
        std :: size_t m_columns = 0;
        std:: size_t m_capacity =0; // m_capacity>=m_columns*m_rows (памяти хотя бы столько же, сколько элементов в матрице
        

    };
}
