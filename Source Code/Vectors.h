#ifndef VECTORS_H_
#define VECTORS_H_

#include <cstdlib>

using namespace std;

struct Vector {
  // This represents the "origin" and distance for use as a ray.
  float x;
  float y;
  float z;
  float distance;

  // This is the true vector component.
  float xd;
  float yd;
  float zd;
};

Vector *new_vector(float x1, float y1, float z1, float x2, float y2, float z2);
Vector *copy_vector(Vector *vector);

float vector_length(Vector *vector);
void scale_vector(Vector *vector, float scalar);
void unit_vector(Vector *vector);

Vector *add_vectors(Vector *vector1, Vector *vector2);
Vector *subtract_vectors(Vector *vector1, Vector *vector2);

float dot_product(Vector *vector1, Vector *vector2);
Vector *cross_product(Vector *vector1, Vector *vector2);

#endif
