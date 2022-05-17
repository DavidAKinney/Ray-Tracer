#ifndef PROPERTIES_H_
#define PROPERTIES_H_

#include <cstdlib>
#include <cstdio>
#include <vector>

#include "Vectors.h"
#include "Objects.h"
#include "Lights.h"

using namespace std;

// These resolve cyclical inclusions.
class Object;
class Light;

struct Properties {
  float eye[3];
  float viewdir[3];
  float updir[3];
  float vfov;
  int imsize[2];
  float bkgcolor[3];
  float refraction_index;
  bool paralell;
};

struct Material {
  float color[3];
  float k_ads[3];
  float specular[3];
  float n;
  float opacity;
  float refraction_index;
};

struct Vertex {
 float x;
 float y;
 float z;
};

struct Normal {
  float xd;
  float yd;
  float zd;
};

struct T_coord {
  float u;
  float v;
};

struct Texture {
  int width;
  int height;
  float ***map;
};

Texture *scan_texture(FILE *tex_ptr);

void delete_objects (vector<Object*> &objects);
void delete_lights (vector<Light*> &lights);
void delete_materials (vector<Material*> &materials);
void delete_textures (vector<Texture*> &textures);
void delete_vertices (vector<Vertex*> &vertices);
void delete_normals (vector<Normal*> &normals);
void delete_t_coords (vector<T_coord*> &t_coords);
void delete_pixels (int **pixels, int pixel_num);

bool vertex_exists(vector<Vertex*> &vertices, int index);
bool normal_exists(vector<Normal*> &normals, int index);
bool t_coord_exists(vector<T_coord*> &t_coords, int index);

int extract_info (FILE *file_ptr, Properties *properties, vector<Object*> &objects, vector<Light*> &lights, vector<Material*> &materials, vector<Texture*> &textures, vector<Vertex*> &vertices, vector<Normal*> &normals, vector<T_coord*> &t_coords);

#endif
