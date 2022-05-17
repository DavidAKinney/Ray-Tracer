#include <cstdlib>
#include <cmath>
#include <ctime>
#include <vector>

#include "Lights.h"
#include "Vectors.h"
#include "Casting.h"

using namespace std;

const float pi = 4.0 * atan(1.0);

float Att_spotlight::illumination (Vector *normal, Vector *v, Intersection *intersection, float ko_d, float ko_s, float n, int rgb_index) {

  Vector *l = new_vector(intersection->x, intersection->y, intersection->z, Light::x, Light::y, Light::z);
  unit_vector(l);
  float attenuation = 1 / (c1 + c2*l->distance + c3*pow(l->distance, 2));

  Vector *to_object = copy_vector(l);
  scale_vector(to_object, -1);

  // If the spotlight is out of range, no illumination is added.
  if (dot_product(direction, to_object) < cos(theta*(pi/180))) {
    delete l;
    delete to_object;
    return 0;
  }

  Vector *h = add_vectors(l, v);
  unit_vector(h);

  float n_dot_l = dot_product(normal, l);
  float n_dot_h = dot_product(normal, h);
  if (0 > n_dot_l) {
    n_dot_l = 0;
  }
  if (0 > n_dot_h) {
    n_dot_h = 0;
  }

  delete l;
  delete h;
  delete to_object;
  return attenuation*Light::rgb[rgb_index]*(ko_d*n_dot_l + ko_s*pow(n_dot_h, n));
}


float Att_spotlight::shadow(Intersection *intersection, vector<Object*> &objects, Vector *normal) {
  float shadow_constant = 0;
  int ray_iterations = 1;
  float ray_passes = 0;

  float x_bias = (normal->xd*SHADOW_BIAS);
  float y_bias = (normal->yd*SHADOW_BIAS);
  float z_bias = (normal->zd*SHADOW_BIAS);

  if (SOFT_SHADOWS) {
    ray_iterations = MAX_SHADOW_RAYS;
  }

  srand(time(NULL));

  for (int i = 0; i < ray_iterations; i++) {
    float x_copy = Light::x;
    float y_copy = Light::y;
    float z_copy = Light::z;

    if (SOFT_SHADOWS) {
      //For each loop, the target is adjusted randomly in order to help create softer shadows (if enabled).
      x_copy += OFFSET_MIN + (rand() / (RAND_MAX / (OFFSET_MAX - OFFSET_MIN)));
      y_copy += OFFSET_MIN + (rand() / (RAND_MAX / (OFFSET_MAX - OFFSET_MIN)));
      z_copy += OFFSET_MIN + (rand() / (RAND_MAX / (OFFSET_MAX - OFFSET_MIN)));
    }

    Vector *shadow_ray = new_vector(intersection->x + x_bias, intersection->y + y_bias, intersection->z + z_bias, x_copy, y_copy, z_copy);
    unit_vector(shadow_ray);

    float pass = 1;
    for (vector<Object*>::iterator i = objects.begin(); i != objects.end(); ++i) {
      Intersection *contact = (*i)->ray_intersect(shadow_ray);

      Vector *to_object = copy_vector(shadow_ray);
      scale_vector(to_object, -1);

      if (contact->distance > 0 && contact->distance < shadow_ray->distance && dot_product(direction, to_object) > cos(theta*(pi/180))) {
        pass = pass*(1 - (*i)->get_material()->opacity);
      }

      delete to_object;
      delete contact;
    }
    ray_passes += pass;
    delete shadow_ray;
  }
  shadow_constant = ray_passes/ray_iterations;
  return shadow_constant;
}
