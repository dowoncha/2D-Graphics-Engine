#include "GMatrix.h"

template<typename T>
GMatrix<T>::GMatrix(unsigned rows, unsigned cols)
  : Matrix(rows)
{
  for (unsigned i = 0; i < nrows; ++i)
    Matrix[i].resize(cols);
}

template<typename T>
void GMatrix<T>::save()
{

}

template<typename T>
void GMatrix<T>::restore()
{

}

template<typename T>
GMatrix<T> GMatrix<T>::operator+(const GMatrix<T>& b) const
{
  GMatrix matrix();
}

template<typename T>
GMatrix<T> GMatrix<T>::operator*(const T& b) const
{

}

template<typename T>
GMatrix<T> GMatrix<T>::operator*(const GMatrix<T>& b) const
{

}

template<typename T>
GMatrix<T>& GMatrix<T>::operator*=(const GMatrix<T>& b)
{

}
