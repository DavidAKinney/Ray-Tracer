#ifndef LIGHTS_H_
#define LIGHTS_H_

#include <cstdlib>
#include <vector>

#include "Vectors.h"
#include "Casting.h"
#include "Properties.h"

using namespace std;

// These resolve cyclical includes.
class Object;
struct Intersection;

// This is the base class for lights.
class Light {
  protected:
    float x;
    float y;
    float z;

    float rgb[3] = {1, 1, 1};

  public:
    Light (float xc = 0, float yc = 0, float zc = 0,
           float rv = 1, float gv = 1, float bv = 1) :
      x(xc), y(yc), z(zc) {
      rgb[0] = rv;
      rgb[1] = gv;
      rgb[2] = bv;
    }

    virtual float illumination (Vector *normal, Vector *v, Intersection *intersection, float ko_d, float ko_s, float n, int rgb_index) = 0;
    virtual float shadow(Intersection *intersection, vector<Object*> &objects, Vector *normal) = 0;
};

// This is the derived class for point and directional lights.
class Standard_light : public Light {
  private:
    float w;

  public:
    Standard_light (float xc = 0, float yc = 0, float zc = 0, float wv = 0,
                 float rv = 1, float gv = 1, float bv = 1) :
                 Light(xc, yc, zc, rv, gv, bv), w(wv) {}

    float illumination (Vector *normal, Vector *v, Intersection *intersection, float ko_d, float ko_s, float n, int rgb_index);
    float shadow (Intersection *intersection, vector<Object*> &objects, Vector *normal);
};


class Att_light : public Light {
  private:
    float w;
    float c1;
    float c2;
    float c3;

  public:
    Att_light (float xc = 0, float yc = 0, float zc = 0, float wv = 0,
               float rv = 1, float gv = 1, float bv = 1,
               float c1v = 1, float c2v = 1, float c3v = 1) :
               Light(xc, yc, zc, rv, gv, bv), w(wv), c1(c1v), c2(c2v), c3(c3v) {}

    float illumination (Vector *normal, Vector *v, Intersection *intersection, float ko_d, float ko_s, float n, int rgb_index);
    float shadow(Intersection *intersection, vector<Object*> &objects, Vector *normal);
};


class Spotlight : public Light {
  private:
    Vector *direction;
    float theta;

  public:
    Spotlight (float xc = 0, float yc = 0, float zc = 0,
                   float xdv = 1, float ydv = 1, float zdv = 1, float angle = 45,
                   float rv = 1, float gv = 1, float bv = 1) :
                   Light(xc, yc, zc, rv, gv, bv), theta(angle) {
      direction = new_vector(0, 0, 0, xdv, ydv, zdv);
      unit_vector(direction);
    }
    ~Spotlight() {
      delete direction;
    }

    float illumination (Vector *normal, Vector *v, Intersection *intersection, float ko_d, float ko_s, float n, int rgb_index);
    float shadow(Intersection *intersection, vector<Object*> &objects, Vector *normal);
};


class Att_spotlight : public Light {
  private:
    Vector *direction;
    float theta;

    float c1;
    float c2;
    float c3;

  public:
    Att_spotlight (float xc = 0, float yc = 0, float zc = 0,
               float xdv = 1, float ydv = 1, float zdv = 1, float angle = 45,
               float rv = 1, float gv = 1, float bv = 1,
               float c1v = 1, float c2v = 1, float c3v = 1) :
               Light(xc, yc, zc, rv, gv, bv), theta(angle), c1(c1v), c2(c2v), c3(c3v) {
      direction = new_vector(0, 0, 0, xdv, ydv, zdv);
      unit_vector(direction);
    }
    ~Att_spotlight() {
      delete direction;
    }

    float illumination (Vector *normal, Vector *v, Intersection *intersection, float ko_d, float ko_s, float n, int rgb_index);
    float shadow(Intersection *intersection, vector<Object*> &objects, Vector *normal);
};

#endif
