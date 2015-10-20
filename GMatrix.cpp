#include "GMatrix.h"

template<typename T>
GMatrix<T>::GMatrix(unsigned rows, unsigned cols)
  : nRows(rows), nCols(cols), Matrix(rows * cols, T(0.0))
{
  std::cout << "Matrix created with size: " << Matrix.size() + '\n';
}

template<typename T>
GMatrix<T>::GMatrix(unsigned rows, unsigned cols, const std::vector<T> values)
  : nRows(rows), nCols(cols), Matrix(std::move(values))
  {
  }

template<typename T>
GMatrix<T>& GMatrix<T>::operator=(const GMatrix<T>& a)
{
  if (this != &a)
  {
    nRows = a.NumRows();
    nCols = a.NumCols();
    Matrix = a.Data();
  }

  return *this;
}

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
GMatrix<T> GMatrix<T>::operator+(const GMatrix<T>& a) const
{
  return GMatrix<T>(*this) += a;
}

template<typename T>
GMatrix<T> GMatrix<T>::operator*(const GMatrix<T>& a) const
{
  return GMatrix<T>(*this) += a;
}

template<typename T>
GMatrix<T> GMatrix<T>::operator*(const T& scalar) const
{
  return GMatrix<T>(*this) *= scalar;
}

template<typename T>
GMatrix<T>& GMatrix<T>::operator+=(const GMatrix<T>& b)
{
  if (nRows != b.NumRows() || nCols != b.NumCols()) throw SizeMismatchViolation();

  for (unsigned i = 0; i < Matrix.size(); ++i)
    Matrix[i] += b(i);

  return *this;
}

template<typename T>
GMatrix<T>& GMatrix<T>::operator*=(const T& b)
{
  for (auto& Element: Matrix)
    Element *= b;

  return *this;
}

template<typename T>
GMatrix<T>& GMatrix<T>::operator*= (const GMatrix<T>& b)
{
  if (nCols != b.NumRows()) throw SizeMismatchViolation();

  std::vector<T> Product;
  for(unsigned i = 0; i < nRows; ++i)
  {
    auto row = this->GetRow(i);
    for (unsigned j = 0; j < nCols; ++j)
    {
      auto col = b.GetCol(j);
      auto prod = DotProduct(row, col);
      Product.push_back(prod);
    }
  }

  nCols = b.NumCols();
  Matrix = b.Data();

  return *this;
}

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
