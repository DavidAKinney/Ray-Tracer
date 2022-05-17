#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <cstring>

#include "Objects.h"
#include "Lights.h"
#include "Vectors.h"
#include "Casting.h"
#include "Properties.h"

using namespace std;


int main (int argc, char *argv[]) {

  if (argc != 2) { //The caster first checks if a file was provided or not.
    printf("Please provide the path and name of your properties file as a command line argument.\n");
    return 1;
  }

  FILE *file_ptr = fopen(argv[1], "r");
  if (file_ptr == NULL) { //The user is notified if their file couldn't be opened.
    printf("Sorry, the file that you provided could not be opened.\n");
    printf("Please check your file's name/directory and try again.\n");
    return 1;
  }

  int failure_test;

  //The image information is extracted into appropriate structs from the file.
  Properties *properties = new Properties;
  vector <Object*> objects;
  vector <Light*> lights;
  vector <Material*> materials;
  vector<Texture*> textures;
  vector <Vertex*> vertices;
  vector <Normal*> normals;
  vector <T_coord*> t_coords;

  printf("Scanning your file now...\n\n");
  failure_test = extract_info(file_ptr, properties, objects, lights, materials, textures, vertices, normals, t_coords);
  if (failure_test) {
    fclose(file_ptr);
    delete properties;
    delete_materials(materials);
    delete_textures(textures);
    delete_vertices(vertices);
    delete_normals(normals);
    delete_t_coords(t_coords);
    return 1;
  }

  printf("\nYour file was scanned successfully!\n");
  printf("A .ppm file is being generated from the scan, please wait...\n\n");

  //Pi is used to convert degrees to radians for tan().
  const float pi = 4.0 * atan(1.0);

  float d = 2; //d is defined arbitrarily here.
  float h = 2*d*tan((properties->vfov)*pi/360);
  float w = h*(properties->imsize[0])/(properties->imsize[1]);

  Vector *eye = new_vector(0, 0, 0, properties->eye[0], properties->eye[1], properties->eye[2]);
  Vector *viewdir = new_vector(0, 0, 0, properties->viewdir[0], properties->viewdir[1], properties->viewdir[2]);
  unit_vector(viewdir);
  Vector *updir = new_vector(0, 0, 0, properties->updir[0], properties->updir[1], properties->updir[2]);
  unit_vector(updir);

  Vector *u = cross_product(viewdir, updir);
  unit_vector(u);

  Vector *v = cross_product(u, viewdir);
  unit_vector(v);

  Vector *n = copy_vector(viewdir);

  scale_vector(n, d); //n = n*d
  scale_vector(u, w/2); //u = u*(w/2)
  scale_vector(v, h/2); //v = v*(h/2)

  //Once the dimension vectors are defined, the structure of the viewing window is built.
  Vector *ul = add_vectors(eye, n);
  Vector *ul_subtraction = subtract_vectors(ul, u);
  Vector *ul_addition = add_vectors(ul_subtraction, v);
  delete ul;
  delete ul_subtraction;
  ul = ul_addition;

  Vector *ur = add_vectors(eye, n);
  Vector *ur_addition1 = add_vectors(ur, u);
  Vector *ur_addition2 = add_vectors(ur_addition1, v);
  delete ur;
  delete ur_addition1;
  ur = ur_addition2;

  Vector *ll = add_vectors(eye, n);
  Vector *ll_subtraction1 = subtract_vectors(ll, u);
  Vector *ll_subtraction2 = subtract_vectors(ll_subtraction1, v);
  delete ll;
  delete ll_subtraction1;
  ll = ll_subtraction2;

  Vector *lr = add_vectors(eye, n);
  Vector *lr_addition = add_vectors(lr, u);
  Vector *lr_subtraction = subtract_vectors(lr_addition, v);
  delete lr;
  delete lr_addition;
  lr = lr_subtraction;

  Vector *hc_change = subtract_vectors(ur, ul);
  scale_vector(hc_change, 1.0/(2.0*properties->imsize[0]));

  Vector *vc_change = subtract_vectors(ll, ul);
  scale_vector(vc_change, 1.0/(2.0*properties->imsize[1]));

  Vector *h_change = copy_vector(hc_change);
  scale_vector(h_change, 2);

  Vector *v_change = copy_vector(vc_change);
  scale_vector(v_change, 2);

  delete viewdir;
  delete updir;
  delete u;
  delete v;
  delete n;
  delete ur;
  delete ll;
  delete lr;

  //A 2D array is allocated for the pixel colors.
  int pixel_num = properties->imsize[0]*properties->imsize[1];
  int **pixels = new int*[pixel_num];
  for (int i=0; i < pixel_num; i++) {
    pixels[i] = new int[3];
  }

  //For each pixel, a ray is drawn and used to test for object intersections.
  for (int j=0; j<properties->imsize[1]; j++) {
    for (int i=0; i<properties->imsize[0]; i++) {
      int pixel_index = i+j*(properties->imsize[0]); //This is used to index the array.

      Vector *h_change_copy = copy_vector(h_change);
      scale_vector(h_change_copy, i);
      Vector *v_change_copy = copy_vector(v_change);
      scale_vector(v_change_copy, j);

      Vector *window_point = add_vectors(ul, h_change_copy);
      Vector *addition1 = add_vectors(window_point, v_change_copy);
      Vector *addition2 = add_vectors(addition1, hc_change);
      Vector *addition3 = add_vectors(addition2, vc_change);
      delete window_point;
      delete addition1;
      delete addition2;
      delete h_change_copy;
      delete v_change_copy;
      window_point = addition3;

      Vector *ray_direction = subtract_vectors(window_point, eye);
      ray_direction->x = properties->eye[0];
      ray_direction->y = properties->eye[1];
      ray_direction->z = properties->eye[2];
      delete window_point;
      unit_vector(ray_direction);
      //Once the ray is created, each object is checked to find the closest intersection.
      Object *contact = closest_intersection(objects, ray_direction);

      // This vector will be used as a stack to hold the scene's indices of refraction.
      vector<float> refraction_indices;
      refraction_indices.push_back(properties->refraction_index);

      //Once an intersection is or isn't found, the current pixel is colored accordingly.
      color_pixel(contact, ray_direction, objects, lights, properties, pixels, pixel_index, refraction_indices, 0);
      delete ray_direction;
    }
  }

  delete eye;
  delete ul;
  delete hc_change;
  delete vc_change;
  delete h_change;
  delete v_change;

  //The objects and are freed once the pixels are colored.
  delete_materials(materials);
  delete_textures(textures);
  delete_vertices(vertices);
  delete_normals(normals);
  delete_t_coords(t_coords);

  //The name for the .ppm file is copied from the original file.
  char *file_name = argv[1];
  size_t txt_len = strlen(file_name);
  size_t name_end = 0;
  for (unsigned int i = 0; i < txt_len; i++) {
    if (file_name[i] == '.') {
      name_end = i;
    }
  }
  if (name_end <= 0) {
    printf("Sorry, the name of your original file could not be recovered for a .ppm file.\n");
    fclose(file_ptr);
    delete properties;
    delete_pixels(pixels, pixel_num);
    return 1;
  }
  char ppm_name[name_end+6]; //+4 for .ppm, +1 for \0, and +1 for the index offset.
  strncpy(ppm_name, file_name, name_end);
  ppm_name[name_end] = '\0';
  strcat(ppm_name, ".ppm");

  //The original file is closed.
  fclose(file_ptr);

  //A .ppm file with the same name as the original file is created.
  FILE *ppm_ptr = fopen(ppm_name, "w");
  if (ppm_ptr == NULL) {
    printf("Sorry, the .ppm file could not be created/opened.\n");
    delete properties;
    delete_pixels(pixels, pixel_num);
    return 1;
  }

  //The data for the .ppm file is added to the beginning of the file.
  fprintf(ppm_ptr, "P3\n#Resolution:\n%d %d\n#Maximum Color Value:\n255\n\n", properties->imsize[0], properties->imsize[1]);

  printf("The parameters for your .ppm file have been generated and are shown below: \n\n");
  printf("P3\n#Resolution:\n%d %d\n#Maximum Color Value:\n255\n\n", properties->imsize[0], properties->imsize[1]);
  printf("Coloring the pixels now...\n\n");

  //Once the image properties are copied down, the pixels color values are added.
  for (int i=0; i<properties->imsize[0]*properties->imsize[1]; i++) {
    fprintf(ppm_ptr, "%d %d %d\n", pixels[i][0], pixels[i][1], pixels[i][2]);
  }

  printf("Your .ppm file has been created!\n");

  //Before the program ends, the properties struct and the pixel array are de-allocated.
  delete properties;
  delete_pixels(pixels, pixel_num);

  //The .ppm file is closed before the program ends.
  fclose(ppm_ptr);
  return 0;
}
