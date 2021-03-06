/**
 * Copyright 2015 Dowon Cha
 * This template class is a 3x3 matrix. Encapsulates an array of size 9
 * Array holds the matrix linearly from row to column
 * Matrix is always default initialized to the identity matrix
 */

#pragma once

#include <assert.h>
#include <array>
#include "GPoint.h"

template<typename T>
class GMatrix
{
private:
  std::array<T, 9> Matrix{{T(1.0), T(0.0), T(0.0),
                           T(0.0), T(1.0), T(0.0),
                           T(0.0), T(0.0), T(1.0)
	                 }};
public:
  GMatrix() {};

  GMatrix(const T in[], int count)
  {
    assert(count <= 9);

    for (int i = 0; i < count; ++i) {
      Matrix[i] = in[i];
    }
  }

  GMatrix(std::array<T, 9> in) : Matrix(in) {}

  GMatrix(const GMatrix<T>& a) : Matrix(a.Matrix) {}  //Copy constructor

  GMatrix<T>& operator=(const GMatrix<T>& a)
  {
    if (this == &a) {
      return *this;
    }

    Matrix = a.Matrix;

    return *this;
  }

  static GMatrix<T> MakeTranslationMatrix(T x, T y)
  {
    GMatrix<T> Mat;
    Mat.setTranslation(x, y);

    return Mat;
  }

  static GMatrix<T> MakeScaleMatrix(T dx, T dy)
  {
    GMatrix<T> Mat;
    Mat.setScale(dx, dy);

    return Mat;
  }

  void setTranslation(T x, T y)
  {
    Matrix[2] = x;
    Matrix[5] = y;
  }

  void setScale(T dx, T dy)
  {
    Matrix[0] = dx;
    Matrix[4] = dy;
  }

  void setMatrix(const T newMat[], int count)
  {
    assert(count <= 9);

    for (int i = 0; i < count; ++i) {
      Matrix[i] = newMat[i];
    }
  }

  void convertPoint(GPoint& P) const      //Convert's input point by the current matrix
  {
    //Get Converted X Y Points using dotproduct
    T NewX = Matrix[0] * P.fX + Matrix[1] * P.fY + Matrix[2];
    T NewY = Matrix[3] * P.fX + Matrix[4] * P.fY + Matrix[5];

    P.set(NewX, NewY);
  }

  void convertPoint(T& x, T& y) const
  {
    T NewX = Matrix[0] * x + Matrix[1] * y + Matrix[2];
    T NewY = Matrix[3] * x + Matrix[4] * y + Matrix[5];

    x = NewX;
    y = NewY;
  }
  
  /* Multiply the input matrix by this matrix and return a new one*/
  GMatrix<T> operator*(const GMatrix<T>& In) const
  {
    std::array<T, 9> ConcatMat;

    int counter = 0;
    for (int i = 0; i < 9; i += 3)
    {
      for (int j = 0; j < 3; ++j, ++counter)
      {
        ConcatMat[counter] = Matrix[i] * In.Matrix[j] + Matrix[i+1] * In.Matrix[j+3] + Matrix[i+2] * In.Matrix[j+6];
      }
    }

    return GMatrix<T>(ConcatMat);
  }
  
  //Concat's two matrices together. Will modify current, matrix returns *this
  GMatrix<T>& concat(const GMatrix<T>& In)  
  {
    if (this == &In)
    {
      return *this;
    }

    std::array<T, 9> ConcatMat;

    int counter = 0;
    for (int i = 0; i < 9; i += 3)
    {
      for (int j = 0; j < 3; ++j, ++counter)
      {	
	ConcatMat[counter] = Matrix[i] * In.Matrix[j] + Matrix[i + 1] * In.Matrix[j+3] + Matrix[i+2] * In.Matrix[j+6];
      }
    }

    Matrix = ConcatMat;

    return *this;
  }

  GMatrix<T> twoRowInverse() const
  {
    T det = Matrix[0] * Matrix[4] - Matrix[1] * Matrix[3];
	 
	  if (det == T(0.0))
	  {
		  printf("Matrix is not invertible\n");
		  return GMatrix<T>();
	  }
	 
	  std::array<T, 9> Inverse {{
		  Matrix[4], -Matrix[1], Matrix[1] * Matrix[5] - Matrix[4] * Matrix[2],
      -Matrix[3], Matrix[0], Matrix[3] * Matrix[2] - Matrix[0] * Matrix[5],
      0 , 0, 1
	  }};
    
    for (auto& Element: Inverse) {
      Element /= det;
    }
    
    return GMatrix<T>(Inverse);
  }
  
  GMatrix<T> inverse() const  //Calculate the inverse of the matrix and return the new matrix
  {
    const T& a11 = Matrix[0], a12 = Matrix[1], a13 = Matrix[2];
    const T& a21 = Matrix[3], a22 = Matrix[4], a23 = Matrix[5];
    const T& a31 = Matrix[6], a32 = Matrix[7], a33 = Matrix[8];
    //Calculate the determinant
    T minor1 = a22 * a33 - a23 * a32;
    T minor2 = a21 * a33 - a23 * a31;
    T minor3 = a21 * a32 - a22 * a31;

    T det = a11 * minor1 - a12 * minor2 + a13 * minor3;

    std::array<T, 9> Inverse {{
      minor1,                   a13 * a32 - a12 * a33,     a12 * a23 - a13 * a22,
      a23 * a31 - a21 * a33,    a11 * a33 - a13 * a31,     a13 * a21 - a11 * a23,
      minor3,                   a12 * a31 - a11 * a31,     a11 * a22 - a12 * a21
    }};

    for (auto& Element: Inverse) {
      Element /= det;
    }

    return GMatrix<T>(Inverse);
  }

  void GetTwoRows(T ctm[6]) const //Get the first two rows into an array
  {
    for (int i = 0; i < 6; ++i) {
      ctm[i] = Matrix[i];
    }
  }

  bool preservesRect() const  //Checks to see if matrix preserves rectangle shape
  {
    return Matrix[1] == T(0.0) && Matrix[3] == T(0.0);
  }
};
