#include <cstdlib>
#include <cmath>

#include "Vectors.h"
#include "Casting.h"
#include "Objects.h"

using namespace std;

const float pi = 4.0 * atan(1.0);

// This function returns an intersection struct for an array and ellipsoid.
Intersection *Ellipsoid::ray_intersect(Vector *ray) {
  float distance = 0;
  float a = pow(ray->xd/x_radius, 2) + pow(ray->yd/y_radius, 2) + pow(ray->zd/z_radius, 2);
  float b = (2*ray->xd*(ray->x - x))/pow(x_radius, 2) + (2*ray->yd*(ray->y - y))/pow(y_radius, 2) + (2*ray->zd*(ray->z - z))/pow(z_radius, 2);
  float c = (pow(x, 2) - 2*x*ray->x + pow(ray->x, 2))/pow(x_radius, 2) + (pow(y, 2) - 2*y*ray->y + pow(ray->y, 2))/pow(y_radius, 2) + (pow(z, 2) - 2*z*ray->z + pow(ray->z, 2))/pow(z_radius, 2) - 1;
  float discriminant = pow(b, 2)-4*a*c;
  if (discriminant == 0) {
    distance = -b/(2*a);
  }
  else if (discriminant > 0) {
    float solution1 = (-b+sqrt(discriminant))/(2*a);
    float solution2 = (-b-sqrt(discriminant))/(2*a);
    if (solution1 <= solution2 && solution1 > 0) {
      distance = solution1;
    }
    else if (solution1 > 0 && solution2 <= 0) {
      distance = solution1;
    }
    else {
      distance = solution2;
    }
  }

  Intersection *contact = new Intersection;
  contact->x = ray->x + (ray->xd * distance);
  contact->y = ray->y + (ray->yd * distance);
  contact->z = ray->z + (ray->zd * distance);
  contact->distance = distance;
  return contact;
}

// This function returns the normal vector at a given intersection with an ellipsoid.
Vector *Ellipsoid::find_normal(Intersection *intersection) {
  Vector *normal = new Vector;
  normal->x = intersection->x;
  normal->y = intersection->y;
  normal->z = intersection->z;
  normal->xd = 2*(intersection->x - x)/pow(x_radius, 2);
  normal->yd = 2*(intersection->y - y)/pow(y_radius, 2);
  normal->zd = 2*(intersection->z - z)/pow(z_radius, 2);
  normal->distance = 0;
  unit_vector(normal);
  return normal;
}

void Ellipsoid::texture_color(Intersection *intersection, float (&color)[3]) {
  Vector *normal = find_normal(intersection);
  float nan_test = normal->zd;
  if (nan_test < -1) { // This eliminates error due to floating point inaccuracy.
    nan_test = -1;
  }
  else if (nan_test > 1) {
    nan_test = 1;
  }

  float phi = acos(nan_test);
  float theta = atan(normal->yd/normal->xd);
  delete normal;

  float v = phi/pi;

  if (theta < 0) {
  theta += (2*pi);
  }
  float u = theta/(2*pi);

  // I chose to use the clostest-neighbor coordinate conversion method.
  int i = (int)round(u*(texture->width - 1));
  int j = (int)round(v*(texture->height - 1));

  color[0] = texture->map[i][j][0];
  color[1] = texture->map[i][j][1];
  color[2] = texture->map[i][j][2];
  return;
}
