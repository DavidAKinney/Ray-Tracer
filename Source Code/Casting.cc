#include <cstdlib>
#include <vector>
#include <cmath>
#include <cfloat>

#include "Properties.h"
#include "Vectors.h"
#include "Objects.h"
#include "Lights.h"

using namespace std;

const float pi = 4.0 * atan(1.0);

// This function returns the closest object with a positive intersection distance.
Object *closest_intersection(vector<Object*> &objects, Vector *target_ray) {
  Intersection *intersection = NULL;
  Object *closest_object = NULL;
  float closest_t = FLT_MAX;

  for (vector<Object*>::iterator i = objects.begin(); i != objects.end(); ++i) {
    intersection = (*i)->ray_intersect(target_ray);
    if (intersection->distance > 0 && intersection->distance < closest_t) {
      closest_t = intersection->distance;
      closest_object = *i;
    }
    delete intersection;
  }
  return closest_object;
}


float *color_pixel (Object *target, Vector *target_ray,
                    vector<Object*> &objects,
                    vector<Light*> &lights,
                    Properties *properties,
                    int **pixels, int pixel_index,
                    vector<float> &refraction_indices, int depth) {

  if (target != NULL && depth < MAX_DEPTH) { //If there is an intersecting object, then the pixel is assigned a color based on the extended Phong Illumination Model.
     Intersection *intersection = target->ray_intersect(target_ray);

     float color[3];
     // If the object has a texture, it will override the base color.
     Texture *texture = target->get_texture();
     if (texture != NULL) {
       target->texture_color(intersection, color);
     }
     else {
       color[0] = target->get_material()->color[0];
       color[1] = target->get_material()->color[1];
       color[2] = target->get_material()->color[2];
     }

     Vector *normal = target->find_normal(intersection);

     Vector *incident = copy_vector(target_ray);
     scale_vector(incident, -1);

     float transmit_index;
     float incident_index;

     bool exiting = dot_product(normal, target_ray) > 0;

     // The exiting check is used to see if the normal should be inverted.
     // The orientation of the refraction indicies is decided here as well.
     if (exiting) {
       scale_vector(normal, -1);
       transmit_index = refraction_indices.back();
       incident_index = target->get_material()->refraction_index;
     }
     else {
       transmit_index = target->get_material()->refraction_index;
       incident_index = refraction_indices.back();
     }

     // The normal's origin is adjusted to avoid unwanted self-collision.
     float x_bias = (normal->xd*SHADOW_BIAS);
     float y_bias = (normal->yd*SHADOW_BIAS);
     float z_bias = (normal->zd*SHADOW_BIAS);
     normal->x += x_bias;
     normal->y += y_bias;
     normal->z += z_bias;

     // The reflection ray is computer here.
     Vector *reflection_ray = copy_vector(normal);
     scale_vector(reflection_ray, 2*dot_product(normal, incident));
     Vector *subtraction = subtract_vectors(reflection_ray, incident);
     delete reflection_ray;
     reflection_ray = subtraction;
     unit_vector(reflection_ray);

     float incident_angle = acos(dot_product(incident, normal));

     // The Fresnel values are found next.
     float r_index = target->get_material()->refraction_index;
     float opacity = target->get_material()->opacity;
     float f_0 = pow((r_index - 1)/(r_index + 1), 2);
     float fresnel = f_0 + (1 - f_0)*pow(1 - cos(incident_angle), 5);

     // For reflection rays during a medium exit, the current internal refraction index is pushed back.
     if (exiting) {
       refraction_indices.push_back(target->get_material()->refraction_index);
     }

     // The color returned by the reflected ray is recursively found.
     Object *reflection_contact = closest_intersection(objects, reflection_ray);
     float *reflection_result = color_pixel(reflection_contact, reflection_ray, objects, lights, properties, pixels, pixel_index, refraction_indices, depth + 1);

     // The state of the stack is reverted for previous calls.
     if (exiting) {
       refraction_indices.pop_back();
     }

     // The normal origin is adjusted again for the transmitted ray.
     normal->x -= 2*x_bias;
     normal->y -= 2*y_bias;
     normal->z -= 2*z_bias;

     float *transmit_result;

     // This if check is used to detect total internal reflecion.
     if (sin(incident_angle) <= transmit_index/incident_index) {
       Vector *addition1 = copy_vector(normal);
       scale_vector(addition1, -1*sqrt(1 - (pow(incident_index/transmit_index, 2)*(1 - pow(cos(incident_angle), 2)))));
       Vector *subtraction = copy_vector(normal);
       scale_vector(subtraction, cos(incident_angle));
       Vector *addition2 = subtract_vectors(subtraction, incident);
       scale_vector(addition2, incident_index/transmit_index);
       Vector *transmitted_ray = add_vectors(addition1, addition2);
       unit_vector(transmitted_ray);
       delete subtraction;
       delete addition1;
       delete addition2;

       Object *transmit_contact = closest_intersection(objects, transmitted_ray);


       // The refraction index stack is adjusted depending on whether the ray is entering or exiting the current object.
       if (exiting) {
         refraction_indices.pop_back();
       }
       else {
         refraction_indices.push_back(target->get_material()->refraction_index);
       }

       // The color returned by the transmitted ray is found with recursion.
       transmit_result = color_pixel(transmit_contact, transmitted_ray, objects, lights, properties, pixels, pixel_index, refraction_indices, depth + 1);

       // The stack is reverted for previous calls.
       if (exiting) {
         refraction_indices.push_back(target->get_material()->refraction_index);
       }
       else {
         refraction_indices.pop_back();
       }

       delete transmitted_ray;
     }
     else {
       // If total internal reflection occurs, then the transmitted result will have no effect.
       transmit_result = new float[3];
       transmit_result[0] = 0;
       transmit_result[1] = 0;
       transmit_result[2] = 0;
     }

     float ambient_r = target->get_material()->k_ads[0]*color[0];
     float l_r = ambient_r + sum_lights(target, objects, lights, properties, intersection, color, 0) + (fresnel*reflection_result[0]) + (1 - fresnel)*(1 - opacity)*transmit_result[0];
     if (l_r > 1) {
       l_r = 1;
     }

     float ambient_g = target->get_material()->k_ads[0]*color[1];
     float l_g = ambient_g + sum_lights(target, objects, lights, properties, intersection, color, 1) + (fresnel*reflection_result[1]) + (1 - fresnel)*(1 - opacity)*transmit_result[1];
     if (l_g > 1) {
       l_g = 1;
     }

     float ambient_b = target->get_material()->k_ads[0]*color[2];
     float l_b = ambient_b + sum_lights(target, objects, lights, properties, intersection, color, 2) + (fresnel*reflection_result[2]) + (1 - fresnel)*(1 - opacity)*transmit_result[2];
     if (l_b > 1) {
       l_b = 1;
     }

     delete intersection;
     delete normal;
     delete incident;
     delete reflection_ray;
     delete[] reflection_result;
     delete[] transmit_result;

     // If the recursion depth is zero, then the color is copied into the pixel array.
     if (depth == 0) {
       pixels[pixel_index][0] = (int)(255*l_r);
       pixels[pixel_index][1] = (int)(255*l_g);
       pixels[pixel_index][2] = (int)(255*l_b);
       return NULL;
     }
     // Otherwise, the color is returned to previous calls.
     else {
       float *result_color = new float[3];
       result_color[0] = l_r;
       result_color[1] = l_g;
       result_color[2] = l_b;
       return result_color;
     }
  }
  // If an intersection occurs past the max depth, then the non-recursed color is added.
  else if (target != NULL && depth == MAX_DEPTH) {
    Intersection *intersection = target->ray_intersect(target_ray);

    float color[3];
    // If the object has a texture, it will override the base color.
    Texture *texture = target->get_texture();
    if (texture != NULL) {
      target->texture_color(intersection, color);
    }
    else {
      color[0] = target->get_material()->color[0];
      color[1] = target->get_material()->color[1];
      color[2] = target->get_material()->color[2];
    }

    float ambient_r = target->get_material()->k_ads[0]*color[0];
    float l_r = ambient_r + sum_lights(target, objects, lights, properties, intersection, color, 0);
    if (l_r > 1) {
      l_r = 1;
    }

    float ambient_g = target->get_material()->k_ads[0]*color[1];
    float l_g = ambient_g + sum_lights(target, objects, lights, properties, intersection, color, 1);
    if (l_g > 1) {
      l_g = 1;
    }

    float ambient_b = target->get_material()->k_ads[0]*color[2];
    float l_b = ambient_b + sum_lights(target, objects, lights, properties, intersection, color, 2);
    if (l_b > 1) {
      l_b = 1;
    }

    delete intersection;

    float *result_color = new float[3];
    result_color[0] = l_r;
    result_color[1] = l_g;
    result_color[2] = l_b;
    return result_color;
  }
  //If there is no intersection, then the pixel is assigned the color of the background.
  else if (target == NULL && depth == 0) {
    pixels[pixel_index][0] = (int)(255*properties->bkgcolor[0]);
    pixels[pixel_index][1] = (int)(255*properties->bkgcolor[1]);
    pixels[pixel_index][2] = (int)(255*properties->bkgcolor[2]);
    return NULL;
  }
  else {
    float *result_color = new float[3];
    result_color[0] = properties->bkgcolor[0];
    result_color[1] = properties->bkgcolor[1];
    result_color[2] = properties->bkgcolor[2];
    return result_color;
  }
}

// This function sums illumination from multiple lights for the Phong-Illumination model.
float sum_lights (Object *target, vector<Object*> &objects, vector<Light*> &lights, Properties *properties, Intersection *intersection, float (&color)[3], int rgb_index) {
  float sum = 0;
  Vector *normal = target->find_normal(intersection);
  Vector *v = new_vector(intersection->x, intersection->y, intersection->z, properties->eye[0], properties->eye[1], properties->eye[2]);
  unit_vector(v);
  float ko_d = target->get_material()->k_ads[1]*color[rgb_index];
  float ko_s = target->get_material()->k_ads[2]*target->get_material()->specular[rgb_index];

  for (vector<Light*>::iterator i = lights.begin(); i != lights.end(); ++i) {
    sum += (*i)->shadow(intersection, objects, normal)*(*i)->illumination(normal, v, intersection, ko_d, ko_s, target->get_material()->n, rgb_index);
  }
  delete normal;
  delete v;
  return sum;
}
