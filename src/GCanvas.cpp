/**
 *  Copyright 2015 Mike Reed
 */

#include "GCanvas.h"

void GCanvas::translate(float tx, float ty) {
    const float mat[6] = {
        1, 0, tx,
        0, 1, ty,
    };
    this->concat(mat);
}

void GCanvas::scale(float sx, float sy) {
    const float mat[6] = {
        sx, 0, 0,
        0, sy, 0,
    };
    this->concat(mat);
}

void GCanvas::rotate(float radians) {
    const float c = cos(radians);
    const float s = sin(radians);
    const float mat[6] = {
        c, -s, 0,
        s, c, 0,
    };
    this->concat(mat);
}

