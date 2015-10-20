#pragma once

#include <vector>

template <typename T>
class GMatrix
{
public:
  GMatrix(unsigned rows, unsigned cols);

  //Acesss Methods to get the i, j elements
  T&        operator() (unsigned i, unsigned j);
  const T&  operator() (unsigned i, unsigned j) const;

  /* Apparently this is allowed to throw an object on violation*/
  class BoundsViolation{};

  //Number of rows and columns in the matrix
  unsigned GetRows() const;
  unsigned GetCols() const;

  void save();
  void restore();

  GMatrix<T> operator*(const T& scalar) const;
  GMatrix<T> operator*(const GMatrix<T>& b) const;
  GMatrix<T>& operator*=(const GMatrix<T>& b);
private:
  std::vector<std::vector<T>> Matrix;
};

template<typename T>
inline unsigned GMatrix<T>::GetRows() const
{  return Matrix.size(); }

template<typename T>
inline unsigned GMatrix<T>::GetCols() const
{  return Matrix[0].size(); }

template<typename T>
inline T& GMatrix<T>::operator() (unsigned i, unsigned j)
{
  if (i >= GetRows() || j >= GetCols()) throw BoundsViolation();
  return Data[i][j];
}

template<typename T>
inline const T& GMatrix<T>::operator() (unsigned i, unsigned j) const
{
  if (i >= GetRows() || j >= GetCols()) throw BoundsViolation();
  return Data[i][j];
}
