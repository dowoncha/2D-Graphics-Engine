#pragma once

#include <vector>
#include <iostream>

template <typename T>
class GMatrix
{
public:
  GMatrix(unsigned rows, unsigned cols);
  GMatrix(unsigned rows, unsigned cols, const std::vector<T> values);

  /* Matrix copy over */
  GMatrix<T>& operator=(const GMatrix<T>& a);

  //Acesss Methods to get the i, j elements
  T&        operator() (unsigned i, unsigned j);
  const T&  operator() (unsigned i, unsigned j) const;

  /* If i want to access the matrix linearly */
  T&        operator() (unsigned i);
  const T&  operator() (unsigned i) const;

  /* Thrown when sizes don't match for an operation*/
  class SizeMismatchViolation {};

  /* Apparently this is allowed to throw an object on violation*/
  class BoundsViolation {};

  //Number of rows and columns in the matrix
  unsigned NumRows() const { return nRows; }
  unsigned NumCols() const { return nCols; };

  std::vector<T> Data() const { return Matrix; }

  void save();
  void restore();

  /* Calculate the dot product from two vectors, they must be the same size*/
  static T DotProduct(const std::vector<T>& a, const std::vector<T>& b);

  /* Returns a vector of the elements in the specified row or column*/
  std::vector<T> GetRow(unsigned row) const;
  std::vector<T> GetCol(unsigned col) const;

  /* Apparently passing by value on left is better performance for chaining? a + b + c*/
  /* Binary arithmetic overloads, declared as friend for performance? */
  GMatrix<T> operator+(const GMatrix<T>& a) const;
  GMatrix<T> operator*(const GMatrix<T>& a) const;
  GMatrix<T> operator*(const T& scalar) const;

  /* Overloads of assignment operators */
  GMatrix<T>& operator+=(const GMatrix<T>& b);
  GMatrix<T>& operator*=(const T& b);
  GMatrix<T>& operator*=(const GMatrix<T>& b);
private:
  std::vector<T> Matrix;
  unsigned nRows;
  unsigned nCols;
};

template<typename T>
inline T& GMatrix<T>::operator() (unsigned i, unsigned j)
{
  if (i >= nRows || j >= nCols) throw BoundsViolation();
  return Matrix[i * nRows + j];
}

template<typename T>
inline const T& GMatrix<T>::operator() (unsigned i, unsigned j) const
{
  if (i >= nRows || j >= nCols) throw BoundsViolation();
  return Matrix[i * nRows + j];
}

template<typename T>
inline T& GMatrix<T>::operator() (unsigned i)
{
  if (i >= Matrix.size()) throw BoundsViolation();
  return Matrix[i];
}

template<typename T>
inline const T& GMatrix<T>::operator() (unsigned i) const
{
  if (i >= Matrix.size()) throw BoundsViolation();
  return Matrix[i];
}
