#ifndef OBJECTS_H_
#define OBJECTS_H_

#include <cstdlib>

#include "Vectors.h"
#include "Casting.h"
#include "Properties.h"

using namespace std;

// These resolve cyclical inclusions.
struct Intersection;
struct Material;
struct Texture;
struct Vertex;
struct Normal;
struct T_coord;

// This the base object class from which spheres, ellipsoids, etc. are derived from.
class Object {
  protected:
    Material *material;
    Texture *texture;

  public:
    Object (Material *mat_ptr = NULL, Texture *tex_ptr = NULL) :
      material(mat_ptr), texture(tex_ptr) {}

    Material *get_material() {return material;}
    Texture *get_texture() {return texture;}

    // Every object will have its own implementation for testing intersections.
    virtual Intersection *ray_intersect(Vector *ray) = 0;
    // The way normal vectors are calculated is unique to each object type.
    virtual Vector *find_normal(Intersection *intersection) = 0;
    // Texture retrieval is based on normal vectors.
    virtual void texture_color(Intersection *intersection, float (&color)[3]) = 0;
};

// This is the derived class for spheres.
class Sphere : public Object {
  private:
    float x;
    float y;
    float z;

    float radius;

  public:
    Sphere (float xc = 0, float yc = 0, float zc = 0, float r = 1,
            Material *mat_ptr = NULL, Texture *tex_ptr = NULL) :
            Object(mat_ptr, tex_ptr), x(xc), y(yc), z(zc), radius(r) {}

    Intersection *ray_intersect(Vector *ray);
    Vector *find_normal(Intersection *point);
    void texture_color(Intersection *intersection, float (&color)[3]);
};

// This is the derived class for ellipsoids.
class Ellipsoid : public Object {
  private:
    float x;
    float y;
    float z;

    float x_radius;
    float y_radius;
    float z_radius;

  public:
    Ellipsoid (float xc = 0, float yc = 0, float zc = 0,
               float xr = 1, float yr = 1, float zr = 1,
               Material *mat_ptr = NULL, Texture *tex_ptr = NULL) :
               Object(mat_ptr, tex_ptr), x(xc), y(yc), z(zc), x_radius(xr), y_radius(yr), z_radius(zr) {}

    Intersection *ray_intersect(Vector *ray);
    Vector *find_normal(Intersection *intersection);
    void texture_color(Intersection *intersection, float (&color)[3]);
};

// This is the derived class for triangles.
class Triangle : public Object {
  private:
    Vertex *v1;
    Vertex *v2;
    Vertex *v3;

    Normal *n1;
    Normal *n2;
    Normal *n3;

    T_coord *t1;
    T_coord *t2;
    T_coord *t3;

  public:
    Triangle (Vertex *vertex1 = NULL, Vertex *vertex2 = NULL, Vertex *vertex3 = NULL,
              Normal *normal1 = NULL, Normal *normal2 = NULL, Normal *normal3 = NULL,
              T_coord *t1v = NULL, T_coord *t2v = NULL, T_coord *t3v = NULL,
              Material *mat_ptr = NULL, Texture *tex_ptr = NULL) :
               Object(mat_ptr, tex_ptr), v1(vertex1), v2(vertex2), v3(vertex3), n1(normal1), n2(normal2), n3(normal3), t1(t1v), t2(t2v), t3(t3v) {}

    Intersection *ray_intersect(Vector *ray);
    Vector *find_normal(Intersection *intersection);
    void texture_color(Intersection *intersection, float (&color)[3]);
};

#endif
