
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
#include "map.h"

void seed_map(thread_arg *arg){
  // Thread function for multpilying maps
  int32_t i, size = arg->vol->x_max * arg->vol->y_max * arg->vol->z_max;
  for (i = arg->thrd ; i < size; i += arg->step){
    if (arg->vol->map[arg->vnum][i] == 0.0){
      arg->vol->map[arg->vnum][i] = arg->vol->stl[arg->vnum]->fill;
    }
  }
  return;
}

void flood_map_f(thread_arg *arg){
  // Fill map with internal fill value
  int32_t i, j, k, index;
  for (k = 0; k < arg->vol->z_max; k++){
    for (j = 0; j < arg->vol->y_max; j++){
      for (i = 0; i < arg->vol->x_max; i++){
	index = k * arg->vol->z_step + j * arg->vol->y_step + i;
	if (i == 0 || j == 0 || k == 0 || i == arg->vol->x_max - 1 || j == arg->vol->y_max - 1 || k == arg->vol->z_max - 1){
	  arg->vol->map[arg->vnum][index] = 0.0;
	  continue;
	} else if (arg->vol->map[arg->vnum][index] == 0.0 || arg->vol->map[arg->vnum][index] == arg->vol->stl[arg->vnum]->fill / 2.0){
	  continue;
	} else if (arg->vol->map[arg->vnum][index - 1] == 0.0 || \
		   arg->vol->map[arg->vnum][index + 1] == 0.0 || \
		   arg->vol->map[arg->vnum][index - arg->vol->y_step] == 0.0 || \
		   arg->vol->map[arg->vnum][index + arg->vol->y_step] == 0.0 || \
		   arg->vol->map[arg->vnum][index - arg->vol->z_step] == 0.0 || \
		   arg->vol->map[arg->vnum][index + arg->vol->z_step] == 0.0){
	  arg->vol->map[arg->vnum][index] = 0.0;
	}
      }
    }
  }
  return;
}

void flood_map_r(thread_arg *arg){
  // Fill map with internal fill value
  int32_t i, j, k, index, zblock = (arg->vol->z_max) / arg->step + 1;
  for (k = arg->vol->z_max - 1; k >= 0; k--){
    for (j = arg->vol->y_max - 1; j >= 0; j--){
      for (i = arg->vol->x_max - 1; i >= 0; i--){
	index = k * arg->vol->z_step + j * arg->vol->y_step + i;
	if (i == 0 || j == 0 || k == 0 || i == arg->vol->x_max - 1 || j == arg->vol->y_max - 1 || k == arg->vol->z_max - 1){
	  arg->vol->map[arg->vnum][index] = 0.0;
	  continue;
	} else if (arg->vol->map[arg->vnum][index] == 0.0 || arg->vol->map[arg->vnum][index] == arg->vol->stl[arg->vnum]->fill / 2.0){
	  continue;
	} else if (arg->vol->map[arg->vnum][index - 1] == 0.0 || \
		   arg->vol->map[arg->vnum][index + 1] == 0.0 || \
		   arg->vol->map[arg->vnum][index - arg->vol->y_step] == 0.0 || \
		   arg->vol->map[arg->vnum][index + arg->vol->y_step] == 0.0 || \
		   arg->vol->map[arg->vnum][index - arg->vol->z_step] == 0.0 || \
		   arg->vol->map[arg->vnum][index + arg->vol->z_step] == 0.0){
	  arg->vol->map[arg->vnum][index] = 0.0;
	}
      }
    }
  }
  return;
}

void flip_screw_f(thread_arg *arg){
  // Fill map with internal fill value
  int32_t i, j, k, index;
  float fill = arg->vol->stl[arg->vnum]->fill;
  for (k = 0; k < arg->vol->z_max; k++){
    for (j = 0; j < arg->vol->y_max; j++){
      for (i = 0; i < arg->vol->x_max; i++){
	index = k * arg->vol->z_step + j * arg->vol->y_step + i;
	if (arg->vol->map[0][index] < 0.0){
	  continue;
	}
	if (arg->vol->map[arg->vnum][index] && (		\
		   arg->vol->map[arg->vnum][index - 1] > 0.0 || \
		   arg->vol->map[arg->vnum][index + 1] > 0.0 || \
		   arg->vol->map[arg->vnum][index - arg->vol->y_step] > 0.0 || \
		   arg->vol->map[arg->vnum][index + arg->vol->y_step] > 0.0 || \
		   arg->vol->map[arg->vnum][index - arg->vol->z_step] > 0.0 || \
		   arg->vol->map[arg->vnum][index + arg->vol->z_step] > 0.0)){
	  arg->vol->map[arg->vnum][index] = 1.0;
	}
      }
    }
  }
  return;
}

void flip_screw_r(thread_arg *arg){
  // Fill map with internal fill value
  int32_t i, j, k, index, zblock = (arg->vol->z_max) / arg->step + 1;
  for (k = arg->vol->z_max - 1; k >= 0; k--){
    for (j = arg->vol->y_max - 1; j >= 0; j--){
      for (i = arg->vol->x_max - 1; i >= 0; i--){
	index = k * arg->vol->z_step + j * arg->vol->y_step + i;
	if (arg->vol->map[0][index] < 0.0){
	  continue;
	}
	if (arg->vol->map[arg->vnum][index] && (		\
		   arg->vol->map[arg->vnum][index - 1] > 0.0 || \
		   arg->vol->map[arg->vnum][index + 1] > 0.0 || \
		   arg->vol->map[arg->vnum][index - arg->vol->y_step] > 0.0 || \
		   arg->vol->map[arg->vnum][index + arg->vol->y_step] > 0.0 || \
		   arg->vol->map[arg->vnum][index - arg->vol->z_step] > 0.0 || \
		   arg->vol->map[arg->vnum][index + arg->vol->z_step] > 0.0)){
	  arg->vol->map[arg->vnum][index] = 1.0;
	}
      }
    }
  }
  return;
}

void add_map_thread(map_arg *arg){
  // Thread function for adding maps
  int32_t i, size = arg->vol->x_max * arg->vol->y_max * arg->vol->z_max;
  for (i = arg->thrd ; i < size; i += arg->step){
    arg->vol->map[arg->outp][i] += arg->vol->map[arg->inpt][i];
  }
  return;
}

void add_maps(vol_arg *vol, int32_t start, int32_t end, int32_t nthreads){
  // Add n maps together into first map
  int32_t i, j;
  pthread_t threads[nthreads];
  map_arg       arg[nthreads];
  for (j = end; j > start; j--){
    for (i = 0; i < nthreads; i++){
      arg[i].vol  = vol;
      arg[i].inpt = j;
      arg[i].outp = start;
      arg[i].thrd = i;
      arg[i].step = nthreads;
      if (pthread_create(&threads[i], NULL, (void*) add_map_thread, &arg[i])){
	printf("\n\t Thread initialisation failed!\n");
	fflush(stdout);
	exit(1);
      }
    }
    for (i = 0; i < nthreads; i++){
      if (pthread_join(threads[i], NULL)){
	printf("\n\t Thread failed during run!\n");
	fflush(stdout);
	exit(1);
      }
    }
  }
  return;
}

void mult_map_thread(map_arg *arg){
  // Thread function for multpilying maps
  int32_t i, size = arg->vol->x_max * arg->vol->y_max * arg->vol->z_max;
  for (i = arg->thrd ; i < size; i += arg->step){
    if (arg->vol->map[arg->outp][i] == -1.0 && arg->vol->map[arg->inpt][i] == TISSUE){
      arg->prod -= fabs(-1.0 * TISSUE);
      arg->nvox++;
    } else if (arg->vol->map[arg->outp][i]){
      arg->prod += (arg->vol->map[arg->inpt][i] * arg->vol->map[arg->outp][i]);
      arg->nvox++;
    }
  }
  return;
}

double mult_maps(vol_arg *vol, int32_t start, int32_t end, int32_t nthreads){
  // Multiply n maps together into first map
  int32_t i, N = 0;
  double P = 0.0;
  pthread_t threads[nthreads];
  map_arg       arg[nthreads];
  for (i = 0; i < nthreads; i++){
    arg[i].vol  = vol;
    arg[i].inpt = end;
    arg[i].outp = start;
    arg[i].thrd = i;
    arg[i].step = nthreads;
    arg[i].nvox = 0;
    arg[i].prod = 0.0;
    if (pthread_create(&threads[i], NULL, (void*) mult_map_thread, &arg[i])){
      printf("\n\t Thread initialisation failed!\n");
      fflush(stdout);
      exit(1);
    }
  }
  for (i = 0; i < nthreads; i++){
    if (pthread_join(threads[i], NULL)){
      printf("\n\t Thread failed during run!\n");
      fflush(stdout);
      exit(1);
    }
    P += arg[i].prod;
    N += arg[i].nvox;
  }
  return - P / ((double) N);
}
