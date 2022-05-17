#include <cstdlib>
#include <cmath>

#include "Vectors.h"
#include "Casting.h"
#include "Objects.h"
#include "Properties.h"

using namespace std;

// These functions all make use of barycentric coordinates.

Intersection *Triangle::ray_intersect(Vector *ray) {
  float distance = 0;

  Vector *e1 = new_vector(v1->x, v1->y, v1->z, v2->x, v2->y, v2->z);
  Vector *e2 = new_vector(v1->x, v1->y, v1->z, v3->x, v3->y, v3->z);
  Vector *n = cross_product(e1, e2);

  float d = -(n->xd*v1->x + n->yd*v1->y + n->zd*v1->z);

  float denominator = (n->xd*ray->xd + n->yd*ray->yd + n->zd*ray->zd);

  if (denominator != 0) {
    float numerator = -(n->xd*ray->x + n->yd*ray->y + n->zd*ray->z + d);
    distance = numerator/denominator;

    float px = ray->x + distance*ray->xd;
    float py = ray->y + distance*ray->yd;
    float pz = ray->z + distance*ray->zd;

    Vector *e1_x_e2 = cross_product(e1, e2);
    float total_area = 0.5*vector_length(e1_x_e2);

    Vector *e3 = new_vector(v2->x, v2->y, v2->z, px, py, pz);
    Vector *e4 = new_vector(v3->x, v3->y, v3->z, px, py, pz);

    Vector *e3_x_e4 = cross_product(e3, e4);
    Vector *e4_x_e2 = cross_product(e4, e2);
    Vector *e1_x_e3 = cross_product(e1, e3);

    float area_a = 0.5*vector_length(e3_x_e4);
    float area_b = 0.5*vector_length(e4_x_e2);
    float area_c = 0.5*vector_length(e1_x_e3);

    delete e3;
    delete e4;
    delete e1_x_e2;
    delete e3_x_e4;
    delete e4_x_e2;
    delete e1_x_e3;

    float alpha = area_a/total_area;
    float beta = area_b/total_area;
    float gamma = area_c/total_area;

    float epsilon = 0.00001;

    if ((0 <= alpha && alpha <= 1) && (0 <= beta && beta <= 1) && (0 <= gamma && gamma <= 1) && (alpha + beta + gamma - 1 < epsilon)) {
      delete e1;
      delete e2;
      delete n;

      Intersection *contact = new Intersection;
      contact->x = px;
      contact->y = py;
      contact->z = pz;
      contact->distance = distance;
      return contact;
    }
  }
  
  delete e1;
  delete e2;
  delete n;

  // An invalid intersection is returned if the point lies outside the triangle.
  Intersection *contact = new Intersection;
  contact->x = -1;
  contact->y = -1;
  contact->z = -1;
  contact->distance = -1;
  return contact;
}


Vector *Triangle::find_normal(Intersection *intersection) {
  Vector *e1 = new_vector(v1->x, v1->y, v1->z, v2->x, v2->y, v2->z);
  Vector *e2 = new_vector(v1->x, v1->y, v1->z, v3->x, v3->y, v3->z);
  Vector *n = cross_product(e1, e2);

  if (n1 == NULL || n2 == NULL || n3 == NULL) {
    delete e1;
    delete e2;
    unit_vector(n);
    return n;
  }

  float px = intersection->x;
  float py = intersection->y;
  float pz = intersection->z;

  Vector *e1_x_e2 = cross_product(e1, e2);
  float total_area = 0.5*vector_length(e1_x_e2);

  Vector *e3 = new_vector(v2->x, v2->y, v2->z, px, py, pz);
  Vector *e4 = new_vector(v3->x, v3->y, v3->z, px, py, pz);

  Vector *e3_x_e4 = cross_product(e3, e4);
  Vector *e4_x_e2 = cross_product(e4, e2);
  Vector *e1_x_e3 = cross_product(e1, e3);

  float area_a = 0.5*vector_length(e3_x_e4);
  float area_b = 0.5*vector_length(e4_x_e2);
  float area_c = 0.5*vector_length(e1_x_e3);

  delete n;
  delete e1;
  delete e2;
  delete e3;
  delete e4;
  delete e1_x_e2;
  delete e3_x_e4;
  delete e4_x_e2;
  delete e1_x_e3;

  float alpha = area_a/total_area;
  float beta = area_b/total_area;
  float gamma = area_c/total_area;

  Vector *normal = new Vector;
  normal->x = intersection->x;
  normal->y = intersection->y;
  normal->z = intersection->z;
  normal->xd = alpha*n1->xd + beta*n2->xd + gamma*n3->xd;
  normal->yd = alpha*n1->yd + beta*n2->yd + gamma*n3->yd;
  normal->zd = alpha*n1->zd + beta*n2->zd + gamma*n3->zd;
  normal->distance = 0;
  unit_vector(normal);
  return normal;
}


void Triangle::texture_color(Intersection *intersection, float (&color)[3]) {
  Vector *e1 = new_vector(v1->x, v1->y, v1->z, v2->x, v2->y, v2->z);
  Vector *e2 = new_vector(v1->x, v1->y, v1->z, v3->x, v3->y, v3->z);
  Vector *n = cross_product(e1, e2);

  float px = intersection->x;
  float py = intersection->y;
  float pz = intersection->z;

  Vector *e1_x_e2 = cross_product(e1, e2);
  float total_area = 0.5*vector_length(e1_x_e2);

  Vector *e3 = new_vector(v2->x, v2->y, v2->z, px, py, pz);
  Vector *e4 = new_vector(v3->x, v3->y, v3->z, px, py, pz);

  Vector *e3_x_e4 = cross_product(e3, e4);
  Vector *e4_x_e2 = cross_product(e4, e2);
  Vector *e1_x_e3 = cross_product(e1, e3);

  float area_a = 0.5*vector_length(e3_x_e4);
  float area_b = 0.5*vector_length(e4_x_e2);
  float area_c = 0.5*vector_length(e1_x_e3);

  delete n;
  delete e1;
  delete e2;
  delete e3;
  delete e4;
  delete e1_x_e2;
  delete e3_x_e4;
  delete e4_x_e2;
  delete e1_x_e3;

  float alpha = area_a/total_area;
  float beta = area_b/total_area;
  float gamma = area_c/total_area;

  float u = alpha*t1->u + beta*t2->u + gamma*t3->u;
  float v = alpha*t1->v + beta*t2->v + gamma*t3->v;

  int i = (int)round(u*(texture->width - 1));
  int j = (int)round(v*(texture->height - 1));
  color[0] = texture->map[i][j][0];
  color[1] = texture->map[i][j][1];
  color[2] = texture->map[i][j][2];
  return;
}
