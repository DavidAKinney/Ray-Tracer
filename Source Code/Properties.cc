#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <vector>
#include <cstring>
#include <algorithm>

#include "Properties.h"
#include "Vectors.h"
#include "Objects.h"
#include "Lights.h"

using namespace std;

// This function creates a texture mapping struct from a given texture file.
Texture *scan_texture (FILE *tex_ptr) {
  int width;
  int height;
  int scan_test;
  // The width and height of the image are filtered out of the first line.
  scan_test = fscanf(tex_ptr, "%*c %*d %d %d %*d", &width, &height);
  if (scan_test != 2 || width <= 0 || height <= 0) {
    return NULL;
  }

  float r;
  float g;
  float b;
  Texture *new_texture = new Texture;
  float ***map = new float**[width];

  // A 3D array is allocated for the rgb values in the texture.
  for (int i = 0; i < width; i++) {
    map[i] = new float*[height];
    for (int j = 0; j < height; j++) {
      map[i][j] = new float[3];
    }
  }

  // Once the array is allocated, the rgb values are scanned in.
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      scan_test = fscanf(tex_ptr, "%f %f %f", &r, &g, &b);
      if (scan_test != 3) {
        return NULL;
      }
      map[i][j][0] = r/255;
      map[i][j][1] = g/255;
      map[i][j][2] = b/255;
    }
  }
  new_texture->width = width;
  new_texture->height = height;
  new_texture->map = map;
  return new_texture;
}

// This function deletes the texture structs stored in a vector.
void delete_textures (vector<Texture*> &textures) {
  for (vector<Texture*>::iterator t = textures.begin(); t != textures.end(); ++t) {
    for (int i = 0; i < (*t)->width; i++) {
      for (int j = 0; j < (*t)->height; j++) {
        delete[] (*t)->map[i][j];
      }
      delete[] (*t)->map[i];
    }
    delete[] (*t)->map;
    delete *t;
  }
}

// This function deletes the material structs in a vector.
void delete_materials (vector<Material*> &materials) {
  for (vector<Material*>::iterator i = materials.begin(); i != materials.end(); ++i) {
    delete *i;
  }
}

// This function deletes the vertex structs in a vector.
void delete_vertices (vector<Vertex*> &vertices) {
  for (vector<Vertex*>::iterator i = vertices.begin(); i != vertices.end(); ++i) {
    delete *i;
  }
}

// This function deletes the normal structs in a vector.
void delete_normals (vector<Normal*> &normals) {
  for (vector<Normal*>::iterator i = normals.begin(); i != normals.end(); ++i) {
    delete *i;
  }
}

// This function deletes the t_coord structs in a vector.
void delete_t_coords (vector<T_coord*> &t_coords) {
  for (vector<T_coord*>::iterator i = t_coords.begin(); i != t_coords.end(); ++i) {
    delete *i;
  }
}

// This function deletes a 2D array of pixels.
void delete_pixels (int **pixels, int pixel_num) {
  for (int i = 0; i < pixel_num; i++) {
    delete[] pixels[i];
  }
  delete[] pixels;
}

// This function checks whether a triangle vertex index is valid.
bool vertex_exists(vector<Vertex*> &vertices, int index) {
  if ((int)vertices.size() >= index && index > 0) {
    return true;
  }
  return false;
}

// This function checks whether a normal index is valid.
bool normal_exists(vector<Normal*> &normals, int index) {
  if ((int)normals.size() >= index && index > 0) {
    return true;
  }
  return false;
}

// This function checks whether a t_coord index is valid.
bool t_coord_exists(vector<T_coord*> &t_coords, int index) {
  if ((int)t_coords.size() >= index && index > 0) {
    return true;
  }
  return false;
}

// This function extracts image properties from a file and stores them in various data structures.
int extract_info (FILE *file_ptr, Properties *properties, vector<Object*> &objects, vector<Light*> &lights, vector<Material*> &materials, vector<Texture*> &textures, vector<Vertex*> &vertices, vector<Normal*> &normals, vector<T_coord*> &t_coords) {
  //This array will hold 1 or 0 values depending on whether a corresponding image property was successfully scanned in.
  int prop_count = 6;
  int prop_test[prop_count];
  for (int i = 0; i < prop_count; i++) {
    prop_test[i] = 0;
  }

  Material *current_material = NULL;
  Texture *current_texture = NULL;

  properties->refraction_index = 1;

  //Each loop extracts the string identifier for an object or property from the file. It then checks to see what type it is.
  char identifier[13];

  //This checks whether the correct number of values for an object or property are scanned.
  int scan_test;

  //The loop ends once an error occurs or scan_test returns 0 on an identifier read.
  while (true) {

    scan_test = fscanf(file_ptr,"%s", identifier);
    if (scan_test != 1) {
      break;
    }

    if(strcmp(identifier, "eye") == 0) {
      if (prop_test[0] == 1) {
        printf("The view origin is defined more than once in your file, please remove these extra definitions and try again.\n");
        return 1;
      }
      else {
        prop_test[0] = 1;
      }
      scan_test = fscanf(file_ptr, "%f %f %f", &properties->eye[0], &properties->eye[1], &properties->eye[2]);
      if (scan_test != 3) {
        printf("There was an error while scanning your file's view origin property, please check the file and try again.\n");
        return 1;
      }
    }

    else if(strcmp(identifier, "viewdir") == 0) {
      if (prop_test[1] == 1) {
        printf("The view direction is defined more than once in your file, please remove these extra definitions and try again.\n");
        return 1;
      }
      else {
        prop_test[1] = 1;
      }
      scan_test = fscanf(file_ptr, "%f %f %f", &properties->viewdir[0], &properties->viewdir[1], &properties->viewdir[2]);
      if (scan_test != 3) {
        printf("There was an error while scanning your file's view direction property, please check the file and try again.\n");
        return 1;
      }
      if (properties->viewdir[0] == 0 && properties->viewdir[1] == 0 && properties->viewdir[2] == 0) {
        printf("The viewing direction in your file is the zero vector. Please fix this error and try again.\n");
        return 1;
      }
    }

    else if(strcmp(identifier, "updir") == 0) {
      if (prop_test[2] == 1) {
        printf("The up direction is defined more than once in your file, please remove these extra definitions and try again.\n");
        return 1;
      }
      else {
        prop_test[2] = 1;
      }
      scan_test = fscanf(file_ptr, "%f %f %f", &properties->updir[0], &properties->updir[1], &properties->updir[2]);
      if (scan_test != 3) {
        printf("There was an error while scanning your file's up direction property, please check the file and try again.\n");
        return 1;
      }
      if (properties->updir[0] == 0 && properties->updir[1] == 0 && properties->updir[2] == 0) {
        printf("The up direction in your file is the zero vector. Please fix this error and try again.\n");
        return 1;
      }
    }

    else if(strcmp(identifier, "vfov") == 0) {
      if (prop_test[3] == 1) {
        printf("The field of view is defined more than once in your file. Please remove these extra definitions and try again.\n");
        return 1;
      }
      else {
        prop_test[3] = 1;
      }
      scan_test = fscanf(file_ptr, "%f", &properties->vfov);
      if (scan_test != 1) {
        printf("There was an error while scanning your file's field of view property. Please check the file and try again.\n");
        return 1;
      }
      if (properties->vfov <= 0 || properties->vfov >= 180) {
        printf("The field of view in your file is not within 0 and 180 degrees. Please fix this error and try again.\n");
        return 1;
      }
    }

    else if(strcmp(identifier, "imsize") == 0) {
      if (prop_test[4] == 1) {
        printf("The image size is defined more than once in your file, please remove these extra definitions and try again.\n");
        return 1;
      }
      else {
        prop_test[4] = 1;
      }
      scan_test = fscanf(file_ptr, "%d %d", &properties->imsize[0], &properties->imsize[1]);
      if (scan_test != 2) {
        printf("There was an error while scanning your file's image size property, please check the file and try again.\n");
        return 1;
      }
      if (properties->imsize[0] <= 0 || properties->imsize[1] <= 0) {
        printf("The image size in your file has a 0 or negative dimension. Please fix this error and try again.\n");
        return 1;
      }
    }

    else if(strcmp(identifier, "bkgcolor") == 0) {
      if (prop_test[5] == 1) {
        printf("The background color is defined more than once in your file, please remove these extra definitions and try again.\n");
        return 1;
      }
      else {
        prop_test[5] = 1;
      }
      scan_test = fscanf(file_ptr, "%f %f %f", &properties->bkgcolor[0], &properties->bkgcolor[1], &properties->bkgcolor[2]);
      if (scan_test != 3) {
        printf("There was an error while scanning your file's background color property, please check the file and try again.\n");
        return 1;
      }
      if (properties->bkgcolor[0] < 0 || properties->bkgcolor[0] > 1) {
        printf("The red value of your file's background color is not withint 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (properties->bkgcolor[1] < 0 || properties->bkgcolor[1] > 1) {
        printf("The green value of your file's background color is not withint 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (properties->bkgcolor[2] < 0 || properties->bkgcolor[2] > 1) {
        printf("The blue value of your file's background color is not withint 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
    }

    // This check will have no effect on the image since paralell projections are not fully implemented.
    else if(strcmp(identifier, "paralell") == 0) {
      properties->paralell = true;
    }

    // This check allows the user to change the refraction index of the scene's "atmosphere".
    else if(strcmp(identifier, "refraction") == 0) {
      scan_test = fscanf(file_ptr, "%f", &properties->refraction_index);
      if (scan_test != 1) {
        printf("There was an error while scanning your file's main refraction index. Please check the file and try again.\n");
        return 1;
      }
      if (properties->refraction_index <= 0) {
        printf("The main index of refraction is not a positive value. Please check the file and try again.\n");
        return 1;
      }
    }

    else if(strcmp(identifier, "mtlcolor") == 0) {
      current_material = new Material;
      materials.push_back(current_material);

      scan_test = fscanf(file_ptr, "%f %f %f %f %f %f %f %f %f %f %f %f", &current_material->color[0], &current_material->color[1], &current_material->color[2],
                                                                    &current_material->specular[0], &current_material->specular[1], &current_material->specular[2],
                                                                    &current_material->k_ads[0], &current_material->k_ads[1], &current_material->k_ads[2],
                                                                    &current_material->n, &current_material->opacity, &current_material->refraction_index);
      if (scan_test != 12) {
        printf("There was an error while scanning one of your file's material colors, please check the file and try again.\n");
        return 1;
      }
      if (current_material->color[0] < 0 || current_material->color[0] > 1) {
        printf("The red value of a material color in your file is not within 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (current_material->color[1] < 0 || current_material->color[1] > 1) {
        printf("The green value of a material color in your file is not within 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (current_material->color[2] < 0 || current_material->color[2] > 1) {
        printf("The blue value of a material color in your file is not within 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (current_material->specular[0] < 0 || current_material->specular[0] > 1) {
        printf("The red specular value of a material color in your file is not within 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (current_material->specular[1] < 0 || current_material->specular[1] > 1) {
        printf("The green specular value of a material color in your file is not within 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (current_material->specular[2] < 0 || current_material->specular[2] > 1) {
        printf("The blue specular value of a material color in your file is not within 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (current_material->k_ads[0] < 0 || current_material->k_ads[0] > 1) {
        printf("The k-a constant of a material color in your file is not within 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (current_material->k_ads[1] < 0 || current_material->k_ads[1] > 1) {
        printf("The k-d constant of a material color in your file is not within 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (current_material->k_ads[2] < 0 || current_material->k_ads[2] > 1) {
        printf("The k-s constant of a material color in your file is not within 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (current_material->refraction_index <= 0) {
        printf("One of the materials in your file has an invalid index of refraction. Please check the file and try again.\n");
        return 1;
      }
      if (current_material->opacity < 0 || current_material->opacity > 1) {
        printf("One of the materials in your file has an opacity outside the range of 0 - 1. Please check the file and try again.\n");
        return 1;
      }
    }

    else if (strcmp(identifier, "texture") == 0) {
      char texture_name[60];
      fscanf(file_ptr, "%s", texture_name);
      FILE *tex_ptr = fopen(texture_name, "r");
      if (tex_ptr == NULL) {
        printf("%s could not be opened, please check the file and try again.\n", texture_name);
        return 1;
      }
      printf("Scanning %s...\n", texture_name);
      Texture *scanned_texture = scan_texture(tex_ptr);
      if (scanned_texture == NULL) {
        printf("%s could not be scanned, please check the file and try again.\n", texture_name);
        return 1;
      }
      fclose(tex_ptr);
      textures.push_back(scanned_texture);
      current_texture = scanned_texture;
    }

    else if (strcmp(identifier, "v") == 0) {
      float x;
      float y;
      float z;

      scan_test = fscanf(file_ptr, "%f %f %f", &x, &y, &z);
      if (scan_test != 3) {
        printf("There was an error while scanning one of your file's triangle vertices, please check the file and try again.\n");
        return 1;
      }
      Vertex *new_vertex = new Vertex;
      new_vertex->x = x;
      new_vertex->y = y;
      new_vertex->z = z;
      vertices.push_back(new_vertex);
    }

    else if (strcmp(identifier, "vn") == 0) {
      float xd;
      float yd;
      float zd;

      scan_test = fscanf(file_ptr, "%f %f %f", &xd, &yd, &zd);
      if (scan_test != 3) {
        printf("There was an error while scanning one of your file's vertex normals, please check the file and try again.\n");
        return 1;
      }
      if (xd == 0 && yd == 0 && zd == 0) {
        printf("One of the vertex normals in your file represent a zero vector, please check the file and try again.\n");
        return 1;
      }
      float length = sqrt(pow(xd, 2) + pow(yd, 2) + pow(zd, 2));
      Normal *new_normal = new Normal;
      new_normal->xd = xd/length;
      new_normal->yd = yd/length;
      new_normal->zd = zd/length;
      normals.push_back(new_normal);
    }

    else if (strcmp(identifier, "vt") == 0) {
      float u;
      float v;

      scan_test = fscanf(file_ptr, "%f %f", &u, &v);
      if (scan_test != 2) {
        printf("There was an error while scanning one of your file's triangle texture coordinates, please check the file and try again.\n");
        return 1;
      }
      if (u < 0 || u > 1 || v < 0 || v > 1) {
        printf("One of the triangle texture coordinates in your file has range(s) outside of [0,1]. Please check the file and try again.\n");
        return 1;
      }

      T_coord *new_t_coord = new T_coord;
      new_t_coord->u = u;
      new_t_coord->v = v;
      t_coords.push_back(new_t_coord);
    }

    else if(strcmp(identifier, "sphere") == 0) {
      if (current_material == NULL) {
        printf("One or more of the objects in your file is not preceded by a background color, please check your file and try again.\n");
        return 1;
      }
      float xyz[3];
      float radius;

      scan_test = fscanf(file_ptr, "%f %f %f %f", &xyz[0], &xyz[1], &xyz[2], &radius);
      if (scan_test != 4) {
        printf("There was an error while scanning one of your file's sphere objects, please check the file and try again.\n");
        return 1;
      }
      if (radius <= 0) {
        printf("One of the sphere objects in your file has a 0 or negative radius. Please fix this error and try again.\n");
        return 1;
      }
      Object *new_sphere = new Sphere(xyz[0], xyz[1], xyz[2], radius, current_material, current_texture);
      objects.push_back(new_sphere);
    }

    else if(strcmp(identifier, "ellipsoid") == 0) {
      if (current_material == NULL) {
        printf("One or more of the objects in your file is not preceded by a background color, please check your file and try again.\n");
        return 1;
      }
      float xyz[3];
      float radii[3];

      scan_test = fscanf(file_ptr, "%f %f %f %f %f %f ", &xyz[0], &xyz[1], &xyz[2],
                                                         &radii[0], &radii[1], &radii[2]);
      if (scan_test != 6) {
        printf("There was an error while scanning one of your file's ellipsoid objects, please check the file and try again.\n");
        return 1;
      }
      if (radii[0] <= 0) {
        printf("One of the ellipsoid objects in your file has a 0 or negative x-radius. Please fix this error and try again.\n");
        return 1;
      }
      if (radii[1] <= 0) {
        printf("One of the ellipsoid objects in your file has a 0 or negative y-radius. Please fix this error and try again.\n");
        return 1;
      }
      if (radii[2] <= 0) {
        printf("One of the ellipsoid objects in your file has a 0 or negative z-radius. Please fix this error and try again.\n");
        return 1;
      }
      Object *new_ellipsoid = new Ellipsoid(xyz[0], xyz[1], xyz[2], radii[0], radii[1], radii[2], current_material, current_texture);
      objects.push_back(new_ellipsoid);
    }

    else if(strcmp(identifier, "f") == 0) {
      int v1;
      int v2;
      int v3;
      int n1;
      int n2;
      int n3;
      int t1;
      int t2;
      int t3;

      char vertex_1[12];
      char vertex_2[12];
      char vertex_3[12];

      char *line = (char *)malloc(40);
      size_t buffer = 39;
      getline(&line, &buffer, file_ptr);

      scan_test = sscanf(line, "%s %s %s", vertex_1, vertex_2, vertex_3);
      if (scan_test != 3) {
        free(line);
        printf("One of the triangle objects in your file is missing a vertex, please check the file and try again.\n");
        return 1;
      }

      free(line);

      // This determines how to parse the triangle data.
      int slash_count = 0;
      for (int i = 0; vertex_1[i] != '\0'; i++) {
        if (vertex_1[i] == '/') {
          slash_count++;
        }
      }

      if (slash_count == 2) {
        scan_test = sscanf(vertex_1, "%d %*c %d %*c %d", &v1, &t1, &n1);

        if (scan_test != 3) {
          scan_test = sscanf(vertex_1, "%d %*c %*c %d", &v1, &n1);
          if (scan_test != 2) {
            printf("One of the triangle objects in your file is formatted incorrectly, please check the file and try again.\n");
            return 1;
          }
          if (!vertex_exists(vertices, v1) || !normal_exists(normals, n1)) {
            printf("One of the triangle objects in your file has an invalid index, please check the file and try again.\n");
            return 1;
          }

          scan_test = sscanf(vertex_2, "%d %*c %*c %d", &v2, &n2);
          if (scan_test != 2) {
            printf("One of the triangle objects in your file is formatted incorrectly, please check the file and try again.\n");
            return 1;
          }
          if (!vertex_exists(vertices, v2) || !normal_exists(normals, n2)) {
            printf("One of the triangle objects in your file has an invalid index, please check the file and try again.\n");
            return 1;
          }

          scan_test = sscanf(vertex_3, "%d %*c %*c %d", &v3, &n3);
          if (scan_test != 2) {
            printf("One of the triangle objects in your file is formatted incorrectly, please check the file and try again.\n");
            return 1;
          }
          if (!vertex_exists(vertices, v3) || !normal_exists(normals, n3)) {
            printf("One of the triangle objects in your file has an invalid index, please check the file and try again.\n");
            return 1;
          }
          Object *new_triangle = new Triangle(vertices.at(v1-1), vertices.at(v2-1), vertices.at(v3-1),
                                              normals.at(n1-1), normals.at(n2-1), normals.at(n3-1),
                                              NULL, NULL, NULL,
                                              current_material, current_texture);
          objects.push_back(new_triangle);
        }

        else {
          if (!vertex_exists(vertices, v1) || !t_coord_exists(t_coords, t1) || !normal_exists(normals, n1)) {
            printf("One of the triangle objects in your file has an invalid index, please check the file and try again.\n");
            return 1;
          }

          scan_test = sscanf(vertex_2, "%d %*c %d %*c %d", &v2, &t2, &n2);
          if (scan_test != 3) {
            printf("One of the triangle objects in your file is formatted incorrectly, please check the file and try again.\n");
            return 1;
          }
          if (!vertex_exists(vertices, v2) || !t_coord_exists(t_coords, t2) || !normal_exists(normals, n2)) {
            printf("One of the triangle objects in your file has an invalid index, please check the file and try again.\n");
            return 1;
          }

          scan_test = sscanf(vertex_3, "%d %*c %d %*c %d", &v3, &t3, &n3);
          if (scan_test != 3) {
            printf("One of the triangle objects in your file is formatted incorrectly, please check the file and try again.\n");
            return 1;
          }
          if (!vertex_exists(vertices, v3) || !t_coord_exists(t_coords, t3) || !normal_exists(normals, n3)) {
            printf("One of the triangle objects in your file has an invalid index, please check the file and try again.\n");
            return 1;
          }
          Object *new_triangle = new Triangle(vertices.at(v1-1), vertices.at(v2-1), vertices.at(v3-1),
                                              normals.at(n1-1), normals.at(n2-1), normals.at(n3-1),
                                              t_coords.at(t1-1), t_coords.at(t2-1), t_coords.at(t3-1),
                                              current_material, current_texture);
          objects.push_back(new_triangle);
        }
      }

      else if (slash_count == 1) {
        scan_test = sscanf(vertex_1, "%d %*c %d", &v1, &t1);
        if (scan_test != 2) {
          printf("One of the triangle objects in your file is formatted incorrectly, please check the file and try again.\n");
          return 1;
        }
        if (!vertex_exists(vertices, v1) || !t_coord_exists(t_coords, t1)) {
          printf("One of the triangle objects in your file has an invalid index, please check the file and try again.\n");
          return 1;
        }

        scan_test = sscanf(vertex_2, "%d %*c %d", &v2, &t2);
        if (scan_test != 2) {
          printf("One of the triangle objects in your file is formatted incorrectly, please check the file and try again.\n");
        }
        if (!vertex_exists(vertices, v2) || !t_coord_exists(t_coords, t2)) {
          printf("One of the triangle objects in your file has an invalid index, please check the file and try again.\n");
          return 1;
        }

        scan_test = sscanf(vertex_3, "%d %*c %d", &v3, &t3);
        if (scan_test != 2) {
          printf("One of the triangle objects in your file is formatted incorrectly, please check the file and try again.\n");
        }
        if (!vertex_exists(vertices, v3) || !t_coord_exists(t_coords, t3)) {
          printf("One of the triangle objects in your file has an invalid index, please check the file and try again.\n");
          return 1;
        }

        Object *new_triangle = new Triangle(vertices.at(v1-1), vertices.at(v2-1), vertices.at(v3-1),
                                            NULL, NULL, NULL,
                                            t_coords.at(t1-1), t_coords.at(t2-1), t_coords.at(t3-1),
                                            current_material, current_texture);
        objects.push_back(new_triangle);
      }

      else if (slash_count == 0) {
        scan_test = sscanf(vertex_1, "%d", &v1);
        if (scan_test != 1) {
          printf("One of the triangle objects in your file is formatted incorrectly, please check the file and try again.\n");
          return 1;
        }
        if (!vertex_exists(vertices, v1)) {
          printf("One of the triangle objects in your file has an invalid index, please check the file and try again.\n");
          return 1;
        }

        scan_test = sscanf(vertex_2, "%d", &v2);
        if (scan_test != 1) {
          printf("One of the triangle objects in your file is formatted incorrectly, please check the file and try again.\n");
          return 1;
        }
        if (!vertex_exists(vertices, v2)) {
          printf("One of the triangle objects in your file has an invalid index, please check the file and try again.\n");
          return 1;
        }

        scan_test = sscanf(vertex_3, "%d", &v3);
        if (scan_test != 1) {
          printf("One of the triangle objects in your file is formatted incorrectly, please check the file and try again.\n");
          return 1;
        }
        if (!vertex_exists(vertices, v3)) {
          printf("One of the triangle objects in your file has an invalid index, please check the file and try again.\n");
          return 1;
        }

        Object *new_triangle = new Triangle(vertices.at(v1-1), vertices.at(v2-1), vertices.at(v3-1),
                                            NULL, NULL, NULL,
                                            NULL, NULL, NULL,
                                            current_material, NULL);
        objects.push_back(new_triangle);
      }

      else {
        printf("One of the triangle objects in your file is formatted incorrectly, please check the file and try again.\n");
        return 1;
      }
    }

    else if(strcmp(identifier, "light") == 0) {
      float xyz[3];
      float w;
      float color[3];
      scan_test = fscanf(file_ptr, "%f %f %f %f %f %f %f", &xyz[0], &xyz[1], &xyz[2], &w,
                                                           &color[0], &color[1], &color[2]);
      if (scan_test != 7) {
        printf("There was an error while scanning one of your file's light objects, please check the file and try again.\n");
        return 1;
      }
      if (color[0] < 0 || color[0] > 1) {
        printf("The red value of a light color in your file is not within 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (color[1] < 0 || color[1] > 1) {
        printf("The green value of a light color in your file is not within 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (color[2] < 0 || color[2] > 1) {
        printf("The blue value of a light color in your file is not within 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (w != 1 && w != 0) {
        printf("One of the lights in your file has an identifier that is not 1 or 0. Please fix this error and try again.\n");
        return 1;
      }
      Light *new_light = new Standard_light(xyz[0], xyz[1], xyz[2], w, color[0], color[1], color[2]);
      lights.push_back(new_light);
    }

    else if(strcmp(identifier, "attlight") == 0) {
      float xyz[3];
      float w;
      float color[3];
      float att[3];
      scan_test = fscanf(file_ptr, "%f %f %f %f %f %f %f %f %f %f", &xyz[0], &xyz[1], &xyz[2], &w,
                                                                    &color[0], &color[1], &color[2],
                                                                    &att[0], &att[1], &att[2]);
      if (scan_test != 10) {
        printf("There was an error while scanning one of your file's attlight objects, please check the file and try again.\n");
        return 1;
      }
      if (color[0] < 0 || color[0] > 1) {
        printf("The red value of an attlight color in your file is not within 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (color[1] < 0 || color[1] > 1) {
        printf("The green value of an attlight color in your file is not within 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (color[2] < 0 || color[2] > 1) {
        printf("The blue value of an attlight color in your file is not within 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (att[0] < 0 || att[1] < 0 || att[2] < 0) {
        printf("One of the attlight objects in your file has a negative C constant. Please fix this error and try again.\n");
        return 1;
      }
      if (att[0] == 0 && att[1] == 0 && att[2] == 0) {
        printf("One of the attlight objects in your file has C constants that would result in dividing by 0. Please fix this error and try again.\n");
        return 1;
      }
      Light *new_att_light = new Att_light(xyz[0], xyz[1], xyz[2], w, color[0], color[1], color[2], att[0], att[1], att[2]);
      lights.push_back(new_att_light);
    }

    else if(strcmp(identifier, "spotlight") == 0) {
      float xyz[3];
      float spot_dir[3];
      float spot_angle;
      float color[3];
      scan_test = fscanf(file_ptr, "%f %f %f %f %f %f %f %f %f %f", &xyz[0], &xyz[1], &xyz[2],
                                                                    &spot_dir[0], &spot_dir[1], &spot_dir[2], &spot_angle,
                                                                    &color[0], &color[1], &color[2]);
      if (scan_test != 10) {
        printf("There was an error while scanning one of your file's spotlight objects, please check the file and try again.\n");
        return 1;
      }
      if (color[0] < 0 || color[0] > 1) {
        printf("The red value of a spotlight color in your file is not within 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (color[1] < 0 || color[1] > 1) {
        printf("The green value of a spotlight color in your file is not within 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (color[2] < 0 || color[2] > 1) {
        printf("The blue value of a spotlight color in your file is not within 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (spot_angle <= 0 || spot_angle >= 90) {
        printf("One of the spotlights in your file has an offset angle that is not within 0 and 90 degrees. Please fix this error and try again.\n");
        return 1;
      }
      if (spot_dir[0] == 0 && spot_dir[1] == 0 && spot_dir[2] == 0) {
        printf("One of the spotlights in your file has a direction given by the 0 vector. Please fix this error and try again.\n");
        return 1;
      }
      Light *new_spotlight = new Spotlight(xyz[0], xyz[1], xyz[2], spot_dir[0], spot_dir[1], spot_dir[2], spot_angle, color[0], color[1], color[2]);
      lights.push_back(new_spotlight);
    }

    else if (strcmp(identifier, "attspotlight") == 0) {
      float xyz[3];
      float spot_dir[3];
      float spot_angle;
      float color[3];
      float att[3];
      scan_test = fscanf(file_ptr, "%f %f %f %f %f %f %f %f %f %f %f %f %f", &xyz[0], &xyz[1], &xyz[2],
                                                                             &spot_dir[0], &spot_dir[1], &spot_dir[2], &spot_angle,
                                                                             &color[0], &color[1], &color[2],
                                                                             &att[0], &att[1], &att[2]);
      if (scan_test != 13) {
        printf("There was an error while scanning one of your file's attspotlight objects, please check the file and try again.\n");
        return 1;
      }
      if (color[0] < 0 || color[0] > 1) {
        printf("The red value of an attspotlight color in your file is not within 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (color[1] < 0 || color[1] > 1) {
        printf("The green value of an attspotlight color in your file is not within 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (color[2] < 0 || color[2] > 1) {
        printf("The blue value of an attspotlight color in your file is not within 0 and 1 inclusive. Please fix this error and try again.\n");
        return 1;
      }
      if (spot_angle <= 0 || spot_angle >= 90) {
        printf("One of the attspotlights in your file has an offset angle that is not within 0 and 90 degrees. Please fix this error and try again.\n");
        return 1;
      }
      if (spot_dir[0] == 0 && spot_dir[1] == 0 && spot_dir[2] == 0) {
        printf("One of the attspotlights in your file has a direction given by the 0 vector. Please fix this error and try again.\n");
        return 1;
      }
      if (att[0] < 0 || att[1] < 0 || att[2] < 0) {
        printf("One of the attlight objects in your file has a negative C constant. Please fix this error and try again.\n");
        return 1;
      }
      if (att[0] == 0 && att[1] == 0 && att[2] == 0) {
        printf("One of the attlight objects in your file has C constants that would result in dividing by 0. Please fix this error and try again.\n");
        return 1;
      }
      Light *new_att_spotlight = new Att_spotlight(xyz[0], xyz[1], xyz[2], spot_dir[0], spot_dir[1], spot_dir[2], spot_angle, color[0], color[1], color[2], att[0], att[1], att[2]);
      lights.push_back(new_att_spotlight);
    }

    else {
      printf("There is an invalid object or property identifier in your file. Please fix this and try again.\n");
      return 1;
    }
  }

  //Once all of the identifiers have been scanned, the property checking array is traversed to see if any are missing.
  for (int i = 0; i < prop_count; i++) {
    if (!prop_test[i]) {
      printf("Your file is missing one or more of the properties needed to generate an image, please check your file and try again.\n");
      return 1;
    }
  }

  Vector *updir = new_vector(0, 0, 0, properties->updir[0], properties->updir[1], properties->updir[2]);
  Vector *viewdir = new_vector(0, 0, 0, properties->viewdir[0], properties->viewdir[1], properties->viewdir[2]);
  Vector *paralell_check = cross_product(updir, viewdir);
  if (vector_length(paralell_check) == 0) {
    printf("The up and viewing directions in your file are paralell to each other. Please fix this error and try again.\n");
    delete updir;
    delete viewdir;
    delete paralell_check;
    return 1;
  }

  delete updir;
  delete viewdir;
  delete paralell_check;

  //If all is successful, the scan will return 0.
  return 0;
}
