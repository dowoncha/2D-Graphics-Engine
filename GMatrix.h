#pragma once

#include <vector>
//#include <initializer_list>
#include <iostream>

template <typename T>
class GMatrix
{
public:
  GMatrix(unsigned rows, unsigned cols);
  GMatrix(unsigned rows, unsigned cols, std::vector<T> Elements);
  GMatrix(unsigned rows, unsigned cols, std::initializer_list<T> Elements);

  /* Set all values in matrix to zero*/
  void SetZero();
  /* Transpose the Matrix */
  void Transpose();

  /* Matrix copy operator overload */
  GMatrix<T>& operator = (const GMatrix<T>& a);

  /* Acesss Methods to get the i, j elements */
  T&        operator() (unsigned i, unsigned j);
  const T&  operator() (unsigned i, unsigned j) const;

  /* Acess Matrix element linearly */
  T&        operator() (unsigned i);
  const T&  operator() (unsigned i) const;

  //SIDENOTE: Saw this being used on isocpp idk if this is valid
  /* Thrown when sizes don't match for an operation*/
  class SizeMismatchViolation {};
  /* Apparently this is allowed to throw an object on violation*/
  class BoundsViolation {};

  /* Number of rows and columns in the matrix */
  unsigned NumRows() const { return nRows; }
  unsigned NumCols() const { return nCols; };

  /* Calculate the dot product from two vectors, they must be the same size */
  static T DotProduct(const std::vector<T>& a, const std::vector<T>& b);

  /* Returns a vector of the elements in the specified row or column*/
  std::vector<T> GetRow(unsigned row) const;
  std::vector<T> GetCol(unsigned col) const;

  //void ConcatIn(const GMatrix& a);
  GMatrix<T>& ConcatIn(const GMatrix& a); //Difference b/w this and above?
  //GMatrix ConcatOut(const GMatrix& a);

  /* Equality operators*/
  bool operator == (const GMatrix<T>& a) const;
  bool operator != (const GMatrix<T>& a) const;

  /* CHECK: Apparently passing by value on left is better performance for chaining? a + b + c*/
  /* Binary arithmetic overloads, declared as friend for performance? */
  GMatrix<T> operator+(const GMatrix<T>& a) const;
  //GMatrix<T> operator*(const GMatrix<T>& a) const;
  GMatrix<T> operator*(const T& scalar) const;

  /* Overloads of arithmetic assignment operators */
  GMatrix<T>& operator+=(const GMatrix<T>& a);
  GMatrix<T>& operator*=(const T& a);
  GMatrix<T>& operator/=(const T& a);
private:
  unsigned nRows;
  unsigned nCols;
protected:
  //This is protected so derived classes can manipulate the Matrix directly without worry
  std::vector<T> Matrix;
};

template<typename T>
inline T& GMatrix<T>::operator() (unsigned i, unsigned j)
{
  if (i >= nRows || j >= nCols) throw BoundsViolation();
  return Matrix.at(i * nRows + j);
}

template<typename T>
inline const T& GMatrix<T>::operator() (unsigned i, unsigned j) const
{
  if (i >= nRows || j >= nCols) throw BoundsViolation();

  return Matrix.at(i * nRows + j);
}

template<typename T>
inline T& GMatrix<T>::operator() (unsigned i)
{
  if (i >= Matrix.size()) throw BoundsViolation();
  return Matrix.at(i);
}

template<typename T>
inline const T& GMatrix<T>::operator() (unsigned i) const
{
  if (i >= Matrix.size()) throw BoundsViolation();
  return Matrix.at(i);
}

template<typename T>
inline bool GMatrix<T>::operator==(const GMatrix<T>& a) const
{
    return nRows == a.nCols && nCols == a.nCols && Matrix == a.Matrix;
}

template<typename T>
inline bool GMatrix<T>::operator!=(const GMatrix<T>& a) const
{
    return !(*this == a);
}
