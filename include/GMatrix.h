#pragma once

#include "GPoint.h"

class GMatrix
{
private:
  std::array<float, 9> Matrix;
public:
  GMatrix();                  //Defaults to identity matrix if no parameters
  GMatrix(const float in[6]); //Fills in top two rows
  GMatrix(const GMatrix& a);  //Copy constructor , change into a move construcor later on

  static GMatrix MakeTranslationMatrix(float x, float y);   //Creates a translation matrix from the input values
  static GMatrix MakeScaleMatrix(float dx, float dy);       //Creates a scale matrix from input scale values

  void convertPoint(GPoint& P) const;      //Convert's input point by the current matrix
  void convertPoint(float& x, float& y) const;

  GMatrix& concat(const GMatrix& InMat);  //Concat's two matrices together. Will modify current, matrix returns *this
  GMatrix inverse() const;                       //Calculate the inverse of the matrix and return the new matrix

  void GetTwoRows(float ctm[6]) const //Get the first two rows into an array
  {
    for (int i = 0; i < 6; ++i) {
      ctm[i] = Matrix[i];
    }
  }

  bool preservesRect() const {                  //Checks to see if matrix preserves rectangle shape
    return Matrix[1] == 0.0f && Matrix[3] == 0.0f;
  }
};
