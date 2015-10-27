#pragma once

#include "GMatrix.h"
#include "GPoint.h"
/**
 *  Derived class of a GMatrix of type float. Size of the matrix is guaranteed to be a 3x3
 *  Inherits the Matrix since it is protected in the base class for fast manipulation
 **/
class GMatrix3x3f : public GMatrix<float>
{
public:
  // Do i need different constructors and static factories?
  // What about initializer_list vs vector cause i think i can pass initializer_list
  // into a vector, so one constructor might be useless
  GMatrix3x3f(std::initializer_list<float> Elements);
  GMatrix3x3f(std::vector<float> Elements);
  //Makes a Translation matrix from input x and y
  static GMatrix3x3f MakeTranslationMatrix(float x, float y);
  //Makes a scale matrix using the input dx and dy
  static GMatrix3x3f MakeScaleMatrix(float dx, float dy);
  //Makes a 3x3 identity matrix
  static GMatrix3x3f MakeIdentityMatrix();
  //Make any 3x3 matrix from the values. 9 Elements should come in
  static GMatrix3x3f MakeMatrix(std::initializer_list<float> Elements);

  /* Check whether the matrix will preserve the rectanglular shape*/
  bool PreservesRect();
  /* Rounds all elements from floats to ints*/
  void Round();
  /* Concat the input matrix onto the current matrix. Function will modify this Matrix
     SIDENOTE: GMatrix has a concat function so this should be unneccesary? */
  void concat(const GMatrix3x3f& a);
  /* Multiplies two matrices together, will not modify this Matrix */
  GMatrix3x3f operator*(const GMatrix3x3f& a);
  /* Multiply the input point by this matrix (3x3) * (3x1) */
  GPoint ConvertPoint(const GPoint& P) const;
  /* Calculate the inverse of the Matrix*/
  GMatrix3x3f GetInverse() const;
};
