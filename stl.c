
/*                                                                         
 * Copyright 27/11/2018 - Dr. Christopher H. S. Aylett                     
 *                                                                         
 * This program is free software; you can redistribute it and/or modify    
 * it under the terms of version 3 of the GNU General Public License as    
 * published by the Free Software Foundation.                              
 *                                                                         
 * This program is distributed in the hope that it will be useful,         
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           
 * GNU General Public License for more details - YOU HAVE BEEN WARNED!     
 *                                                                         
 * Program: STL 2 MRC V0                                             
 *                                                                         
 * Authors: Chris Aylett                                                   
 *                                                                         
 */

// Library header inclusion for linking                                     
#include "head.h"
#include "stl.h"

void apply_matrix(float *vector, float *matrix){
  // Apply matrix from alpha, beta and gamma
  float tmp[3] = {0.0, 0.0, 0.0};
  tmp[0] = vector[0];
  tmp[1] = vector[1];
  tmp[2] = vector[2];
  vector[0] = matrix[0] * tmp[0] + matrix[1] * tmp[1] + matrix[2] * tmp[2];
  vector[1] = matrix[3] * tmp[0] + matrix[4] * tmp[1] + matrix[5] * tmp[2];
  vector[2] = matrix[6] * tmp[0] + matrix[7] * tmp[1] + matrix[8] * tmp[2];
  return;
}

void apply_shift(float *vector, float *shift){
  // Apply shift from x, y and z
  vector[0] -= shift[0];
  vector[1] -= shift[1];
  vector[2] -= shift[2];
  return;
}

void update_mimx(float *vector, vol_arg *vol, float margin){
  // Update min max values from vector
  if (vector[0] < (float) vol->x_min + margin){
    pthread_mutex_lock(&vol->x_min_mutx);
    if (vector[0] < (float) vol->x_min + margin){
      vol->x_min = (int) vector[0] - margin;
    }
    pthread_mutex_unlock(&vol->x_min_mutx);
  }
  if (vector[0] > (float) vol->x_max - margin){
    pthread_mutex_lock(&vol->x_max_mutx);
    if (vector[0] > (float) vol->x_max - margin){
      vol->x_max = (int) vector[0] + margin;
    }
    pthread_mutex_unlock(&vol->x_max_mutx);
  }
  if (vector[1] < (float) vol->y_min + margin){
    pthread_mutex_lock(&vol->y_min_mutx);
    if (vector[1] < (float) vol->y_min + margin){
      vol->y_min = (int) vector[1] - margin;
    }
    pthread_mutex_unlock(&vol->y_min_mutx);
  }
  if (vector[1] > (float) vol->y_max - margin){
    pthread_mutex_lock(&vol->y_max_mutx);
    if (vector[1] > (float) vol->y_max - margin){
      vol->y_max = (int) vector[1] + margin;
    } 
    pthread_mutex_unlock(&vol->y_max_mutx);
  }
  if (vector[2] < (float) vol->z_min + margin){
    pthread_mutex_lock(&vol->z_min_mutx);
    if (vector[2] < (float) vol->z_min + margin){
      vol->z_min = (int) vector[2] - margin;
    }
    pthread_mutex_unlock(&vol->z_min_mutx);
  }
  if (vector[2] > (float) vol->z_max - margin){
    pthread_mutex_lock(&vol->z_max_mutx);
    if (vector[2] > (float) vol->z_max - margin){
      vol->z_max = (int) vector[2] + margin;
    }
    pthread_mutex_unlock(&vol->z_max_mutx);
  }
  return;
}

void reorient_stl(thread_arg *arg){
  // Rotate then shift triangle as requested and set limits as required
  int32_t i;
  for (i = arg->thrd; i < arg->vol->stl[arg->vnum]->nfacets; i += arg->step){
    apply_matrix(arg->vol->stl[arg->vnum]->facets[i].vtx1, arg->vol->stl[arg->vnum]->matrix);
    apply_matrix(arg->vol->stl[arg->vnum]->facets[i].vtx2, arg->vol->stl[arg->vnum]->matrix);
    apply_matrix(arg->vol->stl[arg->vnum]->facets[i].vtx3, arg->vol->stl[arg->vnum]->matrix);
    apply_shift(arg->vol->stl[arg->vnum]->facets[i].vtx1, arg->vol->stl[arg->vnum]->shift);
    apply_shift(arg->vol->stl[arg->vnum]->facets[i].vtx2, arg->vol->stl[arg->vnum]->shift);
    apply_shift(arg->vol->stl[arg->vnum]->facets[i].vtx3, arg->vol->stl[arg->vnum]->shift);
    if (arg->vnum == 0){
      update_mimx(arg->vol->stl[arg->vnum]->facets[i].vtx1, arg->vol, 10.0);
      update_mimx(arg->vol->stl[arg->vnum]->facets[i].vtx2, arg->vol, 10.0);
      update_mimx(arg->vol->stl[arg->vnum]->facets[i].vtx3, arg->vol, 10.0);
    }
  }
  return;
}

void apply_scale(float *vector, float scale){
  // Apply shift from x, y and z
  vector[0] = vector[0] * scale;
  vector[1] = vector[1] * scale;
  vector[2] = vector[2] * scale;
  return;
}

void scale_stl(thread_arg *arg){
  // Scale and shift stl to fit map coordinates and resolution
  int32_t i;
  for (i = arg->thrd; i < arg->vol->stl[arg->vnum]->nfacets; i += arg->step){
    apply_shift(arg->vol->stl[arg->vnum]->facets[i].vtx1, arg->vol->stl[arg->vnum]->shift);
    apply_shift(arg->vol->stl[arg->vnum]->facets[i].vtx2, arg->vol->stl[arg->vnum]->shift);
    apply_shift(arg->vol->stl[arg->vnum]->facets[i].vtx3, arg->vol->stl[arg->vnum]->shift);
    apply_scale(arg->vol->stl[arg->vnum]->facets[i].vtx1, arg->vol->res);
    apply_scale(arg->vol->stl[arg->vnum]->facets[i].vtx2, arg->vol->res);
    apply_scale(arg->vol->stl[arg->vnum]->facets[i].vtx3, arg->vol->res);
    if (arg->vnum == 0){
      update_mimx(arg->vol->stl[arg->vnum]->facets[i].vtx1, arg->vol, 10.0 * arg->vol->res);
      update_mimx(arg->vol->stl[arg->vnum]->facets[i].vtx2, arg->vol, 10.0 * arg->vol->res);
      update_mimx(arg->vol->stl[arg->vnum]->facets[i].vtx3, arg->vol, 10.0 * arg->vol->res);
    }
    arg->x += (int) round(arg->vol->stl[arg->vnum]->facets[i].vtx1[0]);
    arg->x += (int) round(arg->vol->stl[arg->vnum]->facets[i].vtx2[0]);
    arg->x += (int) round(arg->vol->stl[arg->vnum]->facets[i].vtx3[0]);
    arg->y += (int) round(arg->vol->stl[arg->vnum]->facets[i].vtx1[1]);
    arg->y += (int) round(arg->vol->stl[arg->vnum]->facets[i].vtx2[1]);
    arg->y += (int) round(arg->vol->stl[arg->vnum]->facets[i].vtx3[1]);
    arg->z += (int) round(arg->vol->stl[arg->vnum]->facets[i].vtx1[2]);
    arg->z += (int) round(arg->vol->stl[arg->vnum]->facets[i].vtx2[2]);
    arg->z += (int) round(arg->vol->stl[arg->vnum]->facets[i].vtx3[2]);
  }
  return;
}

void sierpinski(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, thread_arg *arg){
  // Recursively cover triangular facet
  float d1 = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2);
  float d2 = (x2 - x3) * (x2 - x3) + (y2 - y3) * (y2 - y3) + (z2 - z3) * (z2 - z3);
  float d3 = (x3 - x1) * (x3 - x1) + (y3 - y1) * (y3 - y1) + (z3 - z1) * (z3 - z1);
  int32_t index, map_size;
  if (d1 < 1 && d2 < 1 && d3 < 1){
    float fill = arg->vol->stl[arg->vnum]->fill / 2.0;
    map_size = arg->vol->z_max * arg->vol->y_max * arg->vol->x_max;
    index = ((int) z1) * arg->vol->z_step + ((int) y1) * arg->vol->y_step + ((int) x1) % map_size;
    arg->vol->map[arg->vnum][index] = fill;
    index = ((int) z2) * arg->vol->z_step + ((int) y2) * arg->vol->y_step + ((int) x2) % map_size;
    arg->vol->map[arg->vnum][index] = fill;
    index = ((int) z3) * arg->vol->z_step + ((int) y3) * arg->vol->y_step + ((int) x3) % map_size;
    arg->vol->map[arg->vnum][index] = fill;
  } else {
    sierpinski(x1, y1, z1, (x1 + x2) / 2.0, (y1 + y2) / 2.0, (z1 + z2) / 2.0, (x3 + x1) / 2.0, (y3 + y1) / 2.0, (z3 + z1) / 2.0, arg);
    sierpinski(x2, y2, z2, (x1 + x2) / 2.0, (y1 + y2) / 2.0, (z1 + z2) / 2.0, (x2 + x3) / 2.0, (y2 + y3) / 2.0, (z2 + z3) / 2.0, arg);
    sierpinski(x3, y3, z3, (x2 + x3) / 2.0, (y2 + y3) / 2.0, (z2 + z3) / 2.0, (x3 + x1) / 2.0, (y3 + y1) / 2.0, (z3 + z1) / 2.0, arg);
    sierpinski((x1 + x2) / 2.0, (y1 + y2) / 2.0, (z1 + z2) / 2.0, (x2 + x3) / 2.0, (y2 + y3) / 2.0, (z2 + z3) / 2.0, (x3 + x1) / 2.0, (y3 + y1) / 2.0, (z3 + z1) / 2.0, arg);
  }
  return;
}

void stl_to_map(thread_arg *arg){
  // Convert surface into density
  int32_t i;
  for (i = arg->thrd; i < arg->vol->stl[arg->vnum]->nfacets; i += arg->step){
    float *v1 = arg->vol->stl[arg->vnum]->facets[i].vtx1;
    float *v2 = arg->vol->stl[arg->vnum]->facets[i].vtx2;
    float *v3 = arg->vol->stl[arg->vnum]->facets[i].vtx3;
    sierpinski(v1[0], v1[1], v1[2], v2[0], v2[1], v2[2], v3[0], v3[1], v3[2], arg);
  }
  return;
}
