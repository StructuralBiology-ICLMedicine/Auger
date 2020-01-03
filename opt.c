
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
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_randist.h>
#include "head.h"

void optimise_screw(vol_arg *vol, double x, double y, double z, double b, double g){
  // Minimise screw cost function and return minimising vector

  const gsl_multimin_fminimizer_type *T = gsl_multimin_fminimizer_nmsimplex2;
  gsl_multimin_fminimizer *s = NULL;
  gsl_vector *ss, *v;
  gsl_multimin_function minex_func;

  int32_t iter = 0;
  int i, status;
  double size;
  double *vec = NULL;
  vec = calloc(5, sizeof(double));
  if (vec == NULL){
    printf("\n\t Insufficient memory to allocate vector! \n");
    exit(1);
  }

  /* Starting point */
  v = gsl_vector_alloc(5);
  gsl_vector_set(v, 0, x);
  gsl_vector_set(v, 1, y);
  gsl_vector_set(v, 2, z);
  gsl_vector_set(v, 3, b - 15.0);
  gsl_vector_set(v, 4, g - 15.0);

  /* Set initial step sizes to 1 */
  ss = gsl_vector_alloc(5);
  gsl_vector_set(ss, 0, 30.);
  gsl_vector_set(ss, 1, 30.);
  gsl_vector_set(ss, 2, 30.);
  gsl_vector_set(ss, 3, 30.);
  gsl_vector_set(ss, 4, 30.);

  /* Initialize method and iterate */
  minex_func.n = 5;
  minex_func.f = score_position;
  minex_func.params = vol;

  s = gsl_multimin_fminimizer_alloc(T, 5);
  gsl_multimin_fminimizer_set(s, &minex_func, v, ss);

  printf("\n\t Starting minimisation:\n\n");

  do {

      iter++;
      status = gsl_multimin_fminimizer_iterate(s);

      if (status){
        break;
      }

      size = gsl_multimin_fminimizer_size(s);
      status = gsl_multimin_test_size(size, 1e-1);

      if (status == GSL_SUCCESS){
	printf ("\n\t Converged to minimum at: \n\n");
      }

      printf ("\t%5d %.5f %.5f %.5f %.5f %.5f %10.5f %10.5f\n", iter,
              -gsl_vector_get (s->x, 0), 
              -gsl_vector_get (s->x, 1), 
              -gsl_vector_get (s->x, 2), 
               gsl_vector_get (s->x, 3), 
               gsl_vector_get (s->x, 4), 
              -s->fval, size);

  } while (status == GSL_CONTINUE && iter < 100);

  score_position(s->x, vol);

  gsl_vector_free(v);
  gsl_vector_free(ss);
  gsl_multimin_fminimizer_free(s);

  return;
}
