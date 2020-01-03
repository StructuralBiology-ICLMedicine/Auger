
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
//#include "head.h"

/* 
 *  FUNCTIONS
 */

void apply_matrix(float *vector, float *matrix);
// Apply matrix from alpha, beta and gamma

void apply_shift(float *vector, float *shift);
// Apply shift from x, y, z

void apply_scale(float *vector, float scale);
// Apply scale factor to vector

void update_mimx(float *vector, vol_arg *vol, float margin);
// Update minimum and maximum values

void sierpinski(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, thread_arg *arg);
// Recursively cover triangular facet
