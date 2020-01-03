
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
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>

#define PEDICLE  1.0
#define BONE    1e-6
#define WALL    -1.0
#define TISSUE  -1.0

/* 
 *  STRUCTURES
 */

// STL triangle structure
typedef struct {
  float norm[3];
  float vtx1[3];
  float vtx2[3];
  float vtx3[3];
  uint16_t junk;
} triangle;

// STL surface structure
typedef struct {
  char   *filename;
  float   shift[3]; // Shift vector
  float    *matrix; // Rotation matrix
  float       fill; // Fill volume value
  // STL header values
  uint8_t head[80];
  uint32_t nfacets;
  triangle *facets;
} stl;

// Definition of 3D volume
typedef struct {
  // Screw definition
  float                    r;
  float                    h;
  // Volume grid definition
  int32_t              x_min;
  int32_t              y_min;
  int32_t              z_min;
  int32_t              x_max;
  int32_t              y_max;
  int32_t              z_max;
  int32_t                dim;
  float                  res;
  // Operational parameters
  pthread_mutex_t x_min_mutx;
  pthread_mutex_t y_min_mutx;
  pthread_mutex_t z_min_mutx;
  pthread_mutex_t x_max_mutx;
  pthread_mutex_t y_max_mutx;
  pthread_mutex_t z_max_mutx;
  int32_t               nvol;
  int32_t             y_step;
  int32_t             z_step;
  // For triangles / STL
  stl                  **stl;
  // For MRC volume
  float                **map;
} vol_arg;

// Thread argument structure
typedef struct {
  // Volume depiction
  vol_arg *vol;
  // For thread handling
  int32_t vnum;
  int32_t thrd;
  int32_t step;
  int64_t    x;
  int64_t    y;
  int64_t    z;
} thread_arg;

/* 
 *  FUNCTIONS
 */

vol_arg *parse_args(int argc, char **argv);
// Read arguments and return number of stls

void read_stl(vol_arg *vol, int32_t vnum);
// Read stl header and fill stl struct

void write_stl(vol_arg *vol, char *filename, int32_t vnum);
// Writes out STL file structure

void write_mrc(vol_arg *vol, char *filename, int32_t vnum);
// Writes float MRC file given a map

int thread_number(void);
// Get thread number

float *make_matrix(double alpha, double beta, double gamma);
// Make matrix from alpha, beta and gamma

stl *make_screw(double x, double y, double z, double beta, double gamma, double r, double h, double g, double fill);
// Make STL of a cylinder given x, y, z, alpha, beta, length and fill

stl *make_cylinder(double x, double y, double z, double beta, double gamma, double r, double h, double g, double fill);
// Make STL of a cylinder given x, y, z, alpha, beta, length and fill

void scale_stl(thread_arg *arg);
// Scale and shift stl to fit map coordinates and resolution

void reorient_stl(thread_arg *arg);
// Rotate then shift triangle as requested
// THREAD SAFE

void find_map_bounds(vol_arg* vol);
// Find min and max map bounds
// THREADED

void stl_to_map(thread_arg *arg);
// Convert surface into density
// THREAD SAFE

void seed_map(thread_arg *arg);
// Fill map apart from surface with fill value
// THREAD SAFE

void flood_map_f(thread_arg *arg);
// Flood exterior to zero forwards
// THREAD SAFE

void flood_map_r(thread_arg *arg);
// Flood exterior to zero backwards
// THREAD SAFE

void flip_screw_f(thread_arg *arg);
// Flood interior to fill forwards
// THREAD SAFE

void flip_screw_r(thread_arg *arg);
// Flood interior to fill backwards
// THREAD SAFE

void add_maps(vol_arg *vols, int32_t start, int32_t end, int32_t nthreads);
// Combine maps into first by addition
// THREADED

double mult_maps(vol_arg *vols, int32_t start, int32_t end, int32_t nthreads);
// Combine maps into first by multiplication
// THREADED

double **generate_spine_map(vol_arg *stls, int32_t nthreads);
// Generate complete spinal scoring map and return array of vectors to targets

void generate_screw_map(int32_t *seed, vol_arg *stls, int32_t nthreads);
// Generate screw, move to position and generate map

void optimise_screw(vol_arg *vol, double x, double y, double z, double b, double g);
// Minimise screw cost function and return minimising vector

double score_position(const gsl_vector *v, void *args);
// Score screw position

void grad_position(const gsl_vector *v, void *args, gsl_vector *g);

void s_g_position(const gsl_vector *v, void *args, double *f, gsl_vector *g);

