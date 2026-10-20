#include "linalg.h"

namespace linalg {
    Matrix::~Matrix() {
        delete[] m_ptr;
    }
}

