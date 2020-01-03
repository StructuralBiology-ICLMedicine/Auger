
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
#include   "io.h"

// Argument parsing
vol_arg *parse_args(int argc, char **argv){

  // Initialise arguments and settings
  int i;
  vol_arg *vol = NULL;
  vol = calloc(1, sizeof(vol_arg));
  if (!vol){
    printf("Error allocating memory");
    exit(1);
  }
  vol->stl = calloc(64, sizeof(stl*));
  vol->res = 1.0;
  vol->dim = INT32_MIN;

  vol->r =  2.6;
  vol->h = 45.0;

  // Set up bounds
  vol->x_min = INT32_MAX;
  vol->x_max = INT32_MIN;
  vol->y_min = INT32_MAX;
  vol->y_max = INT32_MIN;
  vol->z_min = INT32_MAX;
  vol->z_max = INT32_MIN;

  // Initialise mutexes
  pthread_mutex_init(&vol->x_min_mutx, NULL);
  pthread_mutex_init(&vol->x_max_mutx, NULL);
  pthread_mutex_init(&vol->y_min_mutx, NULL);
  pthread_mutex_init(&vol->y_max_mutx, NULL);
  pthread_mutex_init(&vol->z_min_mutx, NULL);
  pthread_mutex_init(&vol->z_max_mutx, NULL);

  // Pad for complete volume
  vol->nvol = 1;

  // Capture user requested settings
  for (i = 1; i < argc; i++){
    if (!strcmp(argv[i], "--full") && ((i + 1) < argc)){
      vol->stl[0] = calloc(1, sizeof(stl));
      if (vol->stl[0] == NULL){
	printf("\n\t Too many volumes! \n");
	exit(1);
      }
      vol->stl[0]->matrix = make_matrix(0.0, 0.0, 0.0);
      vol->stl[0]->filename = argv[i + 1];
      vol->stl[0]->fill = BONE;
      printf("\n\t Complete bone surface to consider: %s \n", vol->stl[0]->filename);
    } else if (!strcmp(argv[i], "--opt") && ((i + 1) < argc)){
      vol->stl[vol->nvol] = calloc(1, sizeof(stl));
      if (vol->nvol + 1 > 64 || vol->stl[vol->nvol] == NULL){
	printf("\n\t Too many volumes! \n");
	exit(1);
      }
      vol->stl[vol->nvol]->matrix = make_matrix(0.0, 0.0, 0.0);
      vol->stl[vol->nvol]->filename = argv[i + 1];
      vol->stl[vol->nvol]->fill = PEDICLE;
      printf("\n\t Surface to optimise within: %s \n", vol->stl[vol->nvol]->filename);
      vol->nvol++;
    } else if (!strcmp(argv[i], "--res") && ((i + 1) < argc)){
      vol->res = atof(argv[i + 1]);
      printf("\n\t Requested resolution %f \n", vol->res);
      vol->res = 1.0 / vol->res;
    } else if (!strcmp(argv[i], "--screw") && ((i + 2) < argc)){
      vol->r = atof(argv[i + 1]);
      vol->h = atof(argv[i + 2]);
      printf("\n\t Requested screws %f by %f \n", vol->r, vol->h);
    }
  }

  // Print usage and disclaimer if used incorrectly
  if (vol->nvol < 2 || vol->res < 1e-10 || vol->stl[0] == NULL){
    printf("\n\t Usage - %s --full full_volume.stl --opt optimisation_region.stl [--res resolution][--screw radius height]\n\n", argv[0]);
    exit(1);
  }

  // Read in stls if figures reasonable
  for (i = 0; i < vol->nvol; i++){
    if (vol->stl[i]->matrix == NULL){
      printf("\n\t Usage - %s --full full_volume.stl --opt optimisation_region.stl [--res resolution][--screw radius height]\n\n", argv[0]);
      exit(1);
    }
    read_stl(vol, i);
  }

  vol->r *= vol->res;
  vol->h *= vol->res;

  // Return read stl volumes
  return vol;
}

// Read in STL file
void read_stl(vol_arg *vol, int32_t vnum){
  // Read stl header
  int i, in;
  FILE *f = NULL;
  f = fopen(vol->stl[vnum]->filename, "rb");
  if (f == NULL){
    printf("Error reading %s - bad file handle\n", vol->stl[vnum]->filename);
    exit(1);
  }
  fread(&vol->stl[vnum]->head, 80, 1, f);
  fread(&vol->stl[vnum]->nfacets, 1, 4, f);
  printf("\n\t Reading file %s - %i facets \n", vol->stl[vnum]->filename, vol->stl[vnum]->nfacets);
  vol->stl[vnum]->facets = NULL;
  vol->stl[vnum]->facets = calloc(vol->stl[vnum]->nfacets, sizeof(triangle));
  if (vol->stl[vnum]->facets == NULL){
    printf("Error reading %s - bad file handle\n", vol->stl[vnum]->filename);
    exit(1);
  }
  // Fill stl struct
  for ( i = 0 ; i < vol->stl[vnum]->nfacets; i++){
    in = fread(&vol->stl[vnum]->facets[i], 50, 1, f);
    if (in != 1){
      printf("Error reading %s - bad file handle\n", vol->stl[vnum]->filename);
      exit(1);
    }
  }
  return;
}

// Calculate unit normal for a given triangle
void make_norm(triangle *triangle){

  float v1[3] = {0.0, 0.0, 0.0};
  float v2[3] = {0.0, 0.0, 0.0};
  double scale;
  
  v1[0] = triangle->vtx2[0] - triangle->vtx1[0];
  v1[1] = triangle->vtx2[1] - triangle->vtx1[1];
  v1[2] = triangle->vtx2[2] - triangle->vtx1[2];

  v2[0] = triangle->vtx3[0] - triangle->vtx2[0];
  v2[1] = triangle->vtx3[1] - triangle->vtx2[1];
  v2[2] = triangle->vtx3[2] - triangle->vtx2[2];

  triangle->norm[0] = v1[1] * v2[2] - v1[2] * v2[1];
  triangle->norm[1] = v1[0] * v2[2] - v1[2] * v2[0];
  triangle->norm[2] = v1[0] * v2[1] - v1[1] * v2[0];

  scale = sqrt(triangle->norm[0] * triangle->norm[0] + triangle->norm[1] * triangle->norm[1] + triangle->norm[2] * triangle->norm[2]);

  triangle->norm[0] /= scale;
  triangle->norm[1] /= scale;
  triangle->norm[2] /= scale;

  return;
}

// Write out STL file
void write_stl(vol_arg *vol, char *filename, int32_t vnum){

  // Variables
  int32_t i;
  char solid[80] = "\n\n\t Screw position generated by Auger - C H S Aylett, D Wang and A R Sadek \n\n";

  // Write out 84 byte header
  FILE *f;
  f = fopen(filename, "wb");
  if (!f){
    printf("Error writing %s - bad file handle\n", filename);
    exit(1);
  }
  fwrite(&solid,                   1, 80, f);
  fwrite(&vol->stl[vnum]->nfacets, 4,  1, f);

  // Export stl struct
  for ( i = 0 ; i < vol->stl[vnum]->nfacets; i++){
    make_norm(&vol->stl[vnum]->facets[i]);
    fwrite(&vol->stl[vnum]->facets[i], 50, 1, f);
  }
  return;
}

// Write out MRC file
void write_mrc(vol_arg *vol, char *filename, int32_t vnum){

  // Writes float MRC file given a map
  float *map = vol->map[vnum];
  int32_t i;
  int32_t x = vol->x_max;
  int32_t y = vol->y_max;
  int32_t z = vol->z_max;
  double total = x * y * z;
  double tmp, sum = 0;
  float  current;

  // Calculate new min, max and mean figures for header
  float d_min = map[0];
  float d_max = map[0];
  for (i = 0; i < total; i++){
    current =  map[i];
    if (current < d_min){
      d_min = current;
    }
    if (current > d_max){
      d_max = current;
    }
    sum += (double) current;
  }
  float d_mean = (float) (sum / total);

  // Calculate RMSD to fill in the rms field for scaling
  for (i = 0; i < total; i++){
    tmp  = (double) (d_mean - map[i]);
    sum += tmp * tmp;
  }
  float rms = (float) sqrt((sum / total));

  // Fill in header values
  int32_t mode = 2;
  int32_t start_crs[3] = {0, 0, 0};
  float length_xyz[3];
  length_xyz[0] = ((float) x) / vol->res;
  length_xyz[1] = ((float) y) / vol->res;
  length_xyz[2] = ((float) z) / vol->res;
  float angle_xyz[3] = {90.0, 90.0, 90.0};
  int32_t map_crs[3] = {1, 2, 3};
  int32_t ispg = 1;
  int32_t nsymbt = 0;
  int32_t extra[25];
  memset(extra, 0x00, 100);
  int32_t ori_xyz[3] = {0, 0, 0};
  char mapstr[4] = "MAP ";
  char machst[4] = {0x44, 0x44, 0x00, 0x00};
  int32_t nlabl = 0;
  char label[800];
  memset(label, 0x00, 800);

  // Write out 1024 byte header
  FILE *f;
  f = fopen(filename, "wb");
  if (!f){
    printf("Error writing %s - bad file handle\n", filename);
    exit(1);
  }
  fwrite(&x,          4, 1,   f);
  fwrite(&y,          4, 1,   f);
  fwrite(&z,          4, 1,   f);
  fwrite(&mode,       4, 1,   f);
  fwrite(&start_crs,  4, 3,   f);
  fwrite(&x,          4, 1,   f);
  fwrite(&y,          4, 1,   f);
  fwrite(&z,          4, 1,   f);
  fwrite(&length_xyz, 4, 3,   f);
  fwrite(&angle_xyz,  4, 3,   f);
  fwrite(&map_crs,    4, 3,   f);
  fwrite(&d_min,      4, 1,   f);
  fwrite(&d_max,      4, 1,   f);
  fwrite(&d_mean,     4, 1,   f);
  fwrite(&ispg,       4, 1,   f);
  fwrite(&nsymbt,     4, 1,   f);
  fwrite(&extra,      4, 25,  f);
  fwrite(&ori_xyz,    4, 3,   f);
  fwrite(&mapstr,     1, 4,   f);
  fwrite(&machst,     1, 4,   f);
  fwrite(&rms,        4, 1,   f);
  fwrite(&nlabl,      4, 1,   f);
  fwrite(&label,      1, 800, f);

  // Write data to file
  fwrite(map, 4, total, f);
  fclose(f);

  return;
}
