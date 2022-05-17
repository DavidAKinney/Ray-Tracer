#ifndef CASTING_H_
#define CASTING_H_

#include <cstdlib>
#include <vector>

#include "Properties.h"
#include "Vectors.h"
#include "Objects.h"
#include "Lights.h"

using namespace std;

// These resolve cyclical includes.
class Object;
class Light;
struct Properties;

// These macros allow for easy adjustment of shadows for the entire program.
#define SOFT_SHADOWS 0  // This macro toggles soft shadows on and off.
#define MAX_SHADOW_RAYS 100  // When soft shadows are enabled, this many rays are produced.
#define OFFSET_MAX 0.015
#define OFFSET_MIN -0.015
#define SHADOW_BIAS 0.001

#define MAX_DEPTH 5  // This is a hard cap on the number of reflection/transparency recursions allowed.

struct Intersection {
  float x;
  float y;
  float z;
  float distance;
};

Object *closest_intersection(std::vector<Object*> &objects, Vector *target_ray);

float *color_pixel (Object *target, Vector *target_ray,
                    vector<Object*> &objects,
                    vector<Light*> &lights,
                    Properties *properties,
                    int **pixels, int pixel_index,
                    vector<float> &refraction_indices, int depth);

float sum_lights (Object *target, vector<Object*> &objects, vector<Light*> &lights,
                  Properties *properties, Intersection *intersection,
                  float (&color)[3], int rgb_index);

#endif
