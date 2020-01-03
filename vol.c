
/*                                                                         
 * Copyrightx 27/11/2018 - Dr. Christopher H. S. Aylett                     
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

// Generate scoring map of complete volume
double **generate_spine_map(vol_arg *stls, int32_t nthreads){

  // Parameters
  int32_t i, j, map_size;
  pthread_t threads[nthreads];
  thread_arg   args[nthreads];

  // Return vectors
  double **vectors = NULL;
  vectors = calloc(stls->nvol, sizeof(double *));

  // Populate thread arg from arguments
  printf("\n\t Initialising threads \n");
  for (i = 0; i < nthreads; i++){
    args[i].vol  = stls;
    args[i].vnum = 0;
    args[i].thrd = i;
    args[i].step = nthreads;
    args[i].x = 0.0;
    args[i].y = 0.0;
    args[i].z = 0.0;
  }

  // Open each stl in turn and generate map
  for (j = 0; j < stls->nvol; j++){
    printf("\n\t Reorienting STL %i \n", j);
    // Rotate then shift triangle as requested
    for(i = 0; i < nthreads; i++){
      args[i].vnum = j;
      if (pthread_create(&threads[i], NULL, (void*) reorient_stl, &args[i])){
	printf("\n\t Thread initialisation failed!\n");
	fflush(stdout);
	exit(1);
      }
    }
    for(i = 0; i < nthreads; i++){
      if (pthread_join(threads[i], NULL)){
	printf("\n\t Thread failed during run!\n");
	fflush(stdout);
	exit(1);
      }
    }
  }

  // Set shift vectors
  if (stls->x_min < 0 || stls->y_min < 0 || stls->z_min < 0){
    printf("\n\t The map needs to be shifted to fit all STLs \n");
  }
  for (j = 0; j < stls->nvol; j++){
    stls->stl[j]->shift[0] = (float) stls->x_min;
    stls->stl[j]->shift[1] = (float) stls->y_min;
    stls->stl[j]->shift[2] = (float) stls->z_min;
  }

  // Set up bounds
  stls->x_min = INT32_MAX;
  stls->x_max = INT32_MIN;
  stls->y_min = INT32_MAX;
  stls->y_max = INT32_MIN;
  stls->z_min = INT32_MAX;
  stls->z_max = INT32_MIN;

  // Rescale surface
  for (j = 0; j < stls->nvol; j++){
    vectors[j] = calloc(3, sizeof(double));
    if (vectors[j] == NULL){
      printf("\n\t Error allocating memory! \n");
      exit(1);
    }
    printf("\n\t Scaling STL %i \n", j);
    for(i = 0; i < nthreads; i++){
      args[i].vnum = j;
      if (pthread_create(&threads[i], NULL, (void*) scale_stl, &args[i])){
	printf("\n\t Thread initialisation failed!\n");
	fflush(stdout);
	exit(1);
      }
    }
    for(i = 0; i < nthreads; i++){
      if (pthread_join(threads[i], NULL)){
	printf("\n\t Thread failed during run!\n");
	fflush(stdout);
	exit(1);
      }
      vectors[j][0] += (double) args[i].x;
      vectors[j][1] += (double) args[i].y;
      vectors[j][2] += (double) args[i].z;
      args[i].x = 0;
      args[i].y = 0;
      args[i].z = 0;
    }
    vectors[j][0] /= (double) stls->stl[j]->nfacets * -3.0;
    vectors[j][1] /= (double) stls->stl[j]->nfacets * -3.0;
    vectors[j][2] /= (double) stls->stl[j]->nfacets * -3.0;
  }

  // Set up volume values
  stls->y_step = stls->x_max;
  stls->z_step = stls->y_max * stls->x_max;
  map_size = stls->x_max * stls->y_max * stls->z_max;

  // Allocate maps
  stls->map = calloc(stls->nvol + 3, sizeof(float*));
  for (j = 0; j < stls->nvol; j++){
    printf("\n\t Allocating map %i \n", j);
    stls->map[j] = NULL;
    stls->map[j] = calloc(map_size, sizeof(float));
    if(stls->map[j] == NULL){
      printf("\n\t Error allocating map memory! \n");
      exit(1);
    }
    // Convert surface into density
    printf("\n\t Converting STL %i to map \n", j);
    for(i = 0; i < 1; i++){
      args[i].vnum = j;
      args[0].step = 1;
      if (pthread_create(&threads[i], NULL, (void*) stl_to_map, &args[i])){
	printf("\n\t Thread initialisation failed!\n");
	fflush(stdout);
	exit(1);
      }
    }
    for(i = 0; i < 1; i++){
      if (pthread_join(threads[i], NULL)){
	printf("\n\t Thread failed during run!\n");
	fflush(stdout);
	exit(1);
      }
    }
    args[0].step = nthreads;
    if (j > 0){
      free(stls->stl[j]->facets);
      free(stls->stl[j]->matrix);
    }
    // Fill map with fill value
    printf("\n\t Filling map %i \n", j);
    for(i = 0; i < nthreads; i++){
      args[i].vnum = j;
      if (pthread_create(&threads[i], NULL, (void*) seed_map, &args[i])){
	printf("\n\t Thread initialisation failed!\n");
	fflush(stdout);
	exit(1);
      }
    }
    for(i = 0; i < nthreads; i++){
      if (pthread_join(threads[i], NULL)){
	printf("\n\t Thread failed during run!\n");
	fflush(stdout);
	exit(1);
      }
    }
    for(i = 0; i < nthreads; i++){
      args[i].vnum = j;
      if (pthread_create(&threads[i], NULL, (void*) flood_map_f, &args[i])){
	printf("\n\t Thread initialisation failed!\n");
	fflush(stdout);
	exit(1);
      }
    }
    for(i = 0; i < nthreads; i++){
      if (pthread_join(threads[i], NULL)){
	printf("\n\t Thread failed during run!\n");
	fflush(stdout);
	exit(1);
      }
    }
    for(i = 0; i < nthreads; i++){
      args[i].vnum = j;
      if (pthread_create(&threads[i], NULL, (void*) flood_map_r, &args[i])){
	printf("\n\t Thread initialisation failed!\n");
	fflush(stdout);
	exit(1);
      }
    }
    for(i = 0; i < nthreads; i++){
      if (pthread_join(threads[i], NULL)){
	printf("\n\t Thread failed during run!\n");
	fflush(stdout);
	exit(1);
      }
    }
  }

  stls->stl[0]->fill = TISSUE;
  for(i = 0; i < nthreads; i++){
    args[i].vnum = 0;
    if (pthread_create(&threads[i], NULL, (void*) seed_map, &args[i])){
      printf("\n\t Thread initialisation failed!\n");
      fflush(stdout);
      exit(1);
    }
  }
  for(i = 0; i < nthreads; i++){
    if (pthread_join(threads[i], NULL)){
      printf("\n\t Thread failed during run!\n");
      fflush(stdout);
      exit(1);
    }
  }

  // Combine maps into first by addition
  printf("\n\t Combining maps %i to %i \n", 1, stls->nvol);
  add_maps(stls, 0, stls->nvol - 1, nthreads);
  for (j = 1; j < stls->nvol; j++){
    free(stls->map[j]);
  }

  write_mrc(stls, "full.mrc", 0);

  return vectors;
}

#define N_SEGMENTS 1

// Generate scoring map of cylinder volume
void generate_screw_map(int32_t *seed, vol_arg *stls, int32_t nthreads){

  // Parameters
  int32_t i, j, map_size;
  pthread_t threads[nthreads];
  thread_arg   args[nthreads];

  // Populate thread arg from arguments
  // printf("\n\t Initialising threads \n");
  for (i = 0; i < nthreads; i++){
    args[i].vol  = stls;
    args[i].vnum = stls->nvol;
    args[i].thrd = i;
    args[i].step = nthreads;
  }

  // Open each stl in turn and generate map
  for (j = stls->nvol + N_SEGMENTS; j >= stls->nvol; j--){
    // printf("\n\t Reorienting STL %i \n", j);
    // Rotate then shift triangle as requested
    for(i = 0; i < nthreads; i++){
      args[i].vnum = j;
      if (pthread_create(&threads[i], NULL, (void*) reorient_stl, &args[i])){
	printf("\n\t Thread initialisation failed!\n");
	fflush(stdout);
	exit(1);
      }
    }
    for(i = 0; i < nthreads; i++){
      if (pthread_join(threads[i], NULL)){
	printf("\n\t Thread failed during run!\n");
	fflush(stdout);
	exit(1);
      }
    }
  }

  // Set up volume values
  stls->y_step = stls->x_max;
  stls->z_step = stls->y_max * stls->x_max;
  map_size = stls->x_max * stls->y_max * stls->z_max;

  // Allocate maps
  for (j = stls->nvol + N_SEGMENTS; j >= stls->nvol; j--){
    // printf("\n\t Allocating map %i \n", j);
    stls->map[j] = NULL;
    stls->map[j] = calloc(map_size, sizeof(float));
    if(stls->map[j] == NULL){
      printf("\n\t Error allocating map memory! \n");
      exit(1);
    }
    // Convert surface into density
    // printf("\n\t Converting STL %i to map \n", j);
    for(i = 0; i < 1; i++){
      args[i].vnum = j;
      args[0].step = 1;
      if (pthread_create(&threads[i], NULL, (void*) stl_to_map, &args[i])){
	printf("\n\t Thread initialisation failed!\n");
	fflush(stdout);
	exit(1);
      }
    }
    for(i = 0; i < 1; i++){
      if (pthread_join(threads[i], NULL)){
	printf("\n\t Thread failed during run!\n");
	fflush(stdout);
	exit(1);
      }
    }
    args[0].step = nthreads;
    if (j > stls->nvol){
      free(stls->stl[j]->facets);
      free(stls->stl[j]->matrix);
      free(stls->stl[j]);
    }
    // Fill map with fill value
    // printf("\n\t Filling map %i \n", j);
    for(i = 0; i < nthreads; i++){
      args[i].vnum = j;
      if (pthread_create(&threads[i], NULL, (void*) seed_map, &args[i])){
	printf("\n\t Thread initialisation failed!\n");
	fflush(stdout);
	exit(1);
      }
    }
    for(i = 0; i < nthreads; i++){
      if (pthread_join(threads[i], NULL)){
	printf("\n\t Thread failed during run!\n");
	fflush(stdout);
	exit(1);
      }
    }
    for(i = 0; i < 2; i++){
      args[i].vnum = j;
      if (pthread_create(&threads[i], NULL, (void*) flood_map_f, &args[i])){
	printf("\n\t Thread initialisation failed!\n");
	fflush(stdout);
	exit(1);
      }
    }
    for(i = 0; i < 2; i++){
      if (pthread_join(threads[i], NULL)){
	printf("\n\t Thread failed during run!\n");
	fflush(stdout);
	exit(1);
      }
    }
    for(i = 0; i < 2; i++){
      args[i].vnum = j;
      if (pthread_create(&threads[i], NULL, (void*) flood_map_r, &args[i])){
	printf("\n\t Thread initialisation failed!\n");
	fflush(stdout);
	exit(1);
      }
    }
    for(i = 0; i < 2; i++){
      if (pthread_join(threads[i], NULL)){
	printf("\n\t Thread failed during run!\n");
	fflush(stdout);
	exit(1);
      }
    }
  }

  // Combine maps into first by addition
  // printf("\n\t Combining maps %i to %i \n", stls->nvol+1, stls->nvol + 3);
  add_maps(stls, stls->nvol, stls->nvol + N_SEGMENTS, nthreads);
  for (j = stls->nvol + N_SEGMENTS; j > stls->nvol; j--){
    free(stls->map[j]);
  }
  /*
  stls->map[stls->nvol][(seed[2] * stls->z_step + seed[1] * stls->y_step + seed[0])] = 1.0;
  for(i = 0; i < 2; i++){
    args[i].vnum = stls->nvol;
    if (pthread_create(&threads[i], NULL, (void*) flip_screw_f, &args[i])){
      printf("\n\t Thread initialisation failed!\n");
      fflush(stdout);
      exit(1);
    }
  }
  for(i = 0; i < 2; i++){
    if (pthread_join(threads[i], NULL)){
      printf("\n\t Thread failed during run!\n");
      fflush(stdout);
      exit(1);
    }
  }
  for(i = 0; i < 2; i++){
    args[i].vnum = stls->nvol;
    if (pthread_create(&threads[i], NULL, (void*) flip_screw_r, &args[i])){
      printf("\n\t Thread initialisation failed!\n");
      fflush(stdout);
      exit(1);
    }
  }
  for(i = 0; i < 2; i++){
    if (pthread_join(threads[i], NULL)){
      printf("\n\t Thread failed during run!\n");
      fflush(stdout);
      exit(1);
    }
  }
  */
  return;
}

// Score cylinder orientation
double score_position(const gsl_vector *v, void *args){

  int32_t nthreads = thread_number();
  double score;

  vol_arg *vol = (vol_arg *) args;

  double x = gsl_vector_get(v, 0);
  double y = gsl_vector_get(v, 1);
  double z = gsl_vector_get(v, 2);
  double b = gsl_vector_get(v, 3) * (M_PI / 180.0);
  double g = gsl_vector_get(v, 4) * (M_PI / 180.0);

  int32_t seeds[3] = {(int) -x, (int) -y, (int) -z};

  vol->stl[vol->nvol + 0] = make_screw(x, y, z, b, g, vol->r, vol->h, 0.0, 1.0);
  vol->stl[vol->nvol + 1] = make_cylinder(x, y, z, b, g, vol->r, 5.0 * vol->res, (vol->h / 2.0) + (10.0 * vol->res), -2.0);

  generate_screw_map(seeds, vol, nthreads);
  write_mrc(vol, "screw.mrc", vol->nvol);

  score = mult_maps(vol, vol->nvol, 0, nthreads);
  free(vol->map[vol->nvol]);

  return score;
}

// Provide gradient
void grad_position(const gsl_vector *v, void *args, gsl_vector *g){

  int i;
  double eps[5] = {1.0, 1.0, 1.0, 1.0, 1.0};
  double f = score_position(v, args);

  gsl_vector *u = gsl_vector_alloc(5);
  gsl_vector_memcpy(u, v);

  for (i = 0; i < 5; i++){
    gsl_vector_set(u, i, gsl_vector_get(v, i) + eps[i]);
    gsl_vector_set(g, i, (score_position(u, args) - f) / eps[i]);
    gsl_vector_set(u, i, gsl_vector_get(v, i));
  }

  gsl_vector_free(u);

  return;
}

// Provide gradient and score
void s_g_position(const gsl_vector *v, void *args, double *f, gsl_vector *g){

  int i;
  double eps[5] = {1.0, 1.0, 1.0, 1.0, 1.0};
  *f = score_position(v, args);

  gsl_vector *u = gsl_vector_alloc(5);
  gsl_vector_memcpy(u, v);

  for (i = 0; i < 5; i++){
    gsl_vector_set(u, i, gsl_vector_get(v, i) + eps[i]);
    gsl_vector_set(g, i, (score_position(u, args) - *f) / eps[i]);
    gsl_vector_set(u, i, gsl_vector_get(v, i));
  }  

  gsl_vector_free(u);

  return;
}
