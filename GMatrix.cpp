#include "GMatrix.h"

/* Initialize rows and cols that are input, Set default value to row x cols*/
template<typename T>
GMatrix<T>::GMatrix(unsigned rows, unsigned cols)
  : nRows(rows), nCols(cols), Matrix(rows * cols, T(0.0))
{
}

template<typename T>
GMatrix<T>::GMatrix(unsigned rows, unsigned cols, std::initializer_list<T> Elements)
  : nRows(rows), nCols(cols), Matrix(Elements)
{
}

template<typename T>
GMatrix<T>::GMatrix(unsigned rows, unsigned cols, std::vector<T> Elements)
  : nRows(rows), nCols(cols), Matrix(Elements)
{
}

template<typename T>
void GMatrix<T>::SetZero()
{
  std::fill(Matrix.begin(), Matrix.end(), T(0.0));
}

/* Get a vector of the values in the input row*/
template<typename T>
std::vector<T> GMatrix<T>::GetRow(unsigned row) const
{
  if (row >= nRows) throw BoundsViolation();

  std::vector<T> Row;
  unsigned end = (row + 1) * nCols;
  for (unsigned i = row * nCols; i < end; ++i)
    Row.push_back(Matrix[i]);

  return Row;
}

template<typename T>
std::vector<T> GMatrix<T>::GetCol(unsigned col) const
{
  if (col >= nCols) throw BoundsViolation();

  std::vector<T> Col;
  for (unsigned i = col; i < Matrix.size(); i += nRows)
    Col.push_back(Matrix[i]);

  return Col;
}

template<typename T>
void GMatrix<T>::Transpose()
{
  if (nRows != nCols) return;

  /* Transpose the matrix*/
  for (unsigned i = 0; i < nRows - 2; ++i)
    for (unsigned j = i + 1; i < nRows - 1; ++j)
      std::swap(Matrix.at(i * nRows + j), Matrix.at(j * nRows + i));
}

template<typename T>
GMatrix<T> GMatrix<T>::operator+(const GMatrix<T>& a) const
{
  return GMatrix<T>(*this) += a;
}

template<typename T>
GMatrix<T> GMatrix<T>::operator*(const GMatrix<T>& a) const
{
  if (nCols != a.NumRows()) throw SizeMismatchViolation();

  /* Vector to hold final quantity. Should hold same amount*/
  std::vector<T> Product;
  for(unsigned i = 0; i < nRows; ++i)
  {
    auto row = GetRow(i);
    for (unsigned j = 0; j < a.NumCols(); ++j)
    {
      auto col = a.GetCol(j);
      Product.emplace_back(DotProduct(row, col));
    }
  }

  return GMatrix<T>(nRows, a.NumCols(), Product);
}

template<typename T>
GMatrix<T> GMatrix<T>::operator*(const T& scalar) const
{
  return GMatrix<T>(*this) *= scalar;
}

template<typename T>
GMatrix<T>& GMatrix<T>::operator+=(const GMatrix<T>& a)
{
  if (nRows != a.NumRows() || nCols != a.NumCols()) throw SizeMismatchViolation();

  for (unsigned i = 0; i < Matrix.size(); ++i)
    Matrix[i] += a(i);

  return *this;
}

template<typename T>
GMatrix<T>& GMatrix<T>::operator*=(const T& a)
{
  for (auto& Element: Matrix)
    Element *= a;

  return *this;
}

template<typename T>
GMatrix<T>& GMatrix<T>::operator/=(const T& a)
{
  for (auto& Element: Matrix)
    Element /= a;

  return *this;
}

/* Find the dot product of two vectors */
template<typename T>
T GMatrix<T>::DotProduct(const std::vector<T>& a, const std::vector<T>& b)
{
  if (a.size() != b.size()) throw SizeMismatchViolation();

  T product = 0;

  for (auto i = 0; i < a.size(); ++i)
    product += a[i] * b[i];

  return product;
}

template class GMatrix<float>;
