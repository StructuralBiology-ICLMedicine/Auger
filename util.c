
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

// Obtain thread number
int thread_number(void){
  // Obtain thread number from environmental variables
  char* thread_number = getenv("OMP_NUM_THREADS");
  int n = 0;
  if (thread_number){
    // If thread number specified by user - use this one
    n = atoi(thread_number);
  }
  if (n < 1){
    // If thread number still not set - try sysconf
    n = sysconf(_SC_NPROCESSORS_ONLN);
  }
  if (n < 1){
    // If variables are both empty - use a single thread
    n = 1;
  }
  // printf("\n\t Running on %i threads \n", n);
  return n;
}

float *make_matrix(double alpha, double beta, double gamma){
  // Make matrix from alpha, beta and gamma
  float *matrix = calloc(9, sizeof(float));
  double c1 = cos(alpha);
  double c2 = cos(beta);
  double c3 = cos(gamma);
  double s1 = sin(alpha);
  double s2 = sin(beta);
  double s3 = sin(gamma);
  matrix[0] =  c2;
  matrix[1] = -c3 * s2;
  matrix[2] =  s2 * s3;
  matrix[3] =  c1 * s2;
  matrix[4] =  c1 * c2 * c3 - s1 * s3;
  matrix[5] = -c3 * s1 - c1 * c2 * s3;
  matrix[6] =  s1 * s2;
  matrix[7] =  c1 * s3 + c2 * c3 * s1;
  matrix[8] =  c1 * c3 - c2 * s1 * s3;
  return matrix;
}
