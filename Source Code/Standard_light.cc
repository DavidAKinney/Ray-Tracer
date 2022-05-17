#include <cstdlib>
#include <cmath>
#include <ctime>
#include <vector>

#include "Objects.h"
#include "Lights.h"
#include "Vectors.h"
#include "Casting.h"

using namespace std;

float Standard_light::illumination (Vector *normal, Vector *v, Intersection *intersection, float ko_d, float ko_s, float n, int rgb_index) {
  Vector *l = NULL;
  if (w == 0) { // For directional lights...
    l = new Vector;
    l->x = intersection->x;
    l->y = intersection->y;
    l->z = intersection->z;
    l->xd = Light::x;
    l->yd = Light::y;
    l->zd = Light::z;
    l->distance = 0;
    scale_vector(l, -1);
    unit_vector(l);
  }
  else { // For point lights...
    l = new_vector(intersection->x, intersection->y, intersection->z, Light::x, Light::y, Light::z);
    unit_vector(l);
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
  return Light::rgb[rgb_index]*(ko_d*n_dot_l + ko_s*pow(n_dot_h, n));
}


float Standard_light::shadow(Intersection *intersection, vector<Object*> &objects, Vector *normal) {
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

    Vector *shadow_ray = NULL;
    if (w == 0) {
      shadow_ray = new Vector;
      shadow_ray->x = intersection->x + x_bias;
      shadow_ray->y = intersection->y + y_bias;
      shadow_ray->z = intersection->z + z_bias;
      shadow_ray->xd = x_copy;
      shadow_ray->yd = y_copy;
      shadow_ray->zd = z_copy;
      shadow_ray->distance = 0;
      scale_vector(shadow_ray, -1);
      unit_vector(shadow_ray);
    }
    else {
      shadow_ray = new_vector(intersection->x + x_bias, intersection->y + y_bias, intersection->z + z_bias, x_copy, y_copy, z_copy);
      unit_vector(shadow_ray);
    }

    float pass = 1;
    for (vector<Object*>::iterator i = objects.begin(); i != objects.end(); ++i) {
      Intersection *contact = (*i)->ray_intersect(shadow_ray);

      if (w == 0) { // For directional lights...
        if (contact->distance > 0) {
          pass = pass*(1 - (*i)->get_material()->opacity);
        }
      }
      else { // For point lights...
        if (contact->distance > 0 && contact->distance < shadow_ray->distance) {
          pass = pass*(1 - (*i)->get_material()->opacity);
        }
      }
      delete contact;
    }
    ray_passes += pass;
    delete shadow_ray;
  }
  shadow_constant = ray_passes/ray_iterations;
  return shadow_constant;
}
