#include <cstdlib>
#include <cmath>

#include "Vectors.h"

using namespace std;

// This function creates a new vector from two points.
Vector *new_vector (float x1, float y1, float z1, float x2, float y2, float z2) {
  Vector *result = new Vector;
  result->x = x1;
  result->y = y1;
  result->z = z1;
  result->xd = x2 - x1;
  result->yd = y2 - y1;
  result->zd = z2 - z1;
  result->distance = vector_length(result);
  return result;
}

// This function creates a copy of an existing vector.
Vector *copy_vector (Vector *vector) {
  Vector *copy = new Vector;
  copy->x = vector->x;
  copy->y = vector->y;
  copy->z = vector->z;
  copy->xd = vector->xd;
  copy->yd = vector->yd;
  copy->zd = vector->zd;
  copy->distance = vector->distance;
  return copy;
}

// This function returns the length of a vector.
float vector_length (Vector *vector) {
  return sqrt(pow(vector->xd, 2)+pow(vector->yd, 2)+pow(vector->zd, 2));
}

// This function scales a vector by a given scalar.
void scale_vector (Vector *vector, float scalar) {
  vector->xd = vector->xd * scalar;
  vector->yd = vector->yd * scalar;
  vector->zd = vector->zd * scalar;
  return;
}

// This function sets a vector to unit length.
void unit_vector (Vector *vector) {
  scale_vector(vector, 1/vector_length(vector));
  return;
}

// This function adds two vectors together and returns the resulting vector.
Vector *add_vectors (Vector *vector1, Vector *vector2) {
  Vector *result = new Vector;
  result->x = vector1->x;
  result->y = vector1->y;
  result->z = vector1->z;
  result->xd = vector1->xd + vector2->xd;
  result->yd = vector1->yd + vector2->yd;
  result->zd = vector1->zd + vector2->zd;
  result->distance = vector_length(result);
  return result;
}

Vector *subtract_vectors (Vector *vector1, Vector *vector2) {
  scale_vector(vector2, -1);
  Vector *result = add_vectors(vector1, vector2);
  scale_vector(vector2, -1);
  return result;
}

float dot_product (Vector *vector1, Vector *vector2) {
  return (vector1->xd*vector2->xd) + (vector1->yd*vector2->yd) + (vector1->zd*vector2->zd);
}

Vector *cross_product (Vector *vector1, Vector *vector2) {
  Vector *result = new Vector;
  result->x = vector1->x;
  result->y = vector1->y;
  result->z = vector1->z;
  result->xd = (vector1->yd*vector2->zd) - (vector1->zd*vector2->yd);
  result->yd = (vector1->zd*vector2->xd) - (vector1->xd*vector2->zd);
  result->zd = (vector1->xd*vector2->yd) - (vector1->yd*vector2->xd);
  result->distance = vector_length(result);
  return result;
}
