
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
 *  STRUCTURES
 */

// Thread argument structure
typedef struct {
  // Volume depiction
  vol_arg *vol;
  // For thread handling 
  int32_t inpt;
  int32_t outp;
  int32_t thrd;
  int32_t step;
  int32_t nvox;
  double  prod;
} map_arg;

/* 
 *  FUNCTIONS
 */

void add_map_thread(map_arg *arg);
// Thread function for adding maps

void mult_map_thread(map_arg *arg);
// Thread function for multiplying maps
