
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

// Main algorithm function
int main(int argc, char *argv[]){

  // Parameters
  int32_t i, j;
  double **vec = NULL;
  double  *out = NULL;
  char strbuffer[128];

  // Read stdin and get number of stls etc.
  printf("\n\t Reading arguments \n");
  vol_arg *stls = parse_args(argc, argv);

  thread_arg arg;
  arg.thrd = 0;
  arg.step = 1;
  arg.vol = stls;
  arg.vnum = stls->nvol;

  // Generate and write out complete stl
  vec = generate_spine_map(stls, thread_number());
  j = snprintf(strbuffer, (strlen(stls->stl[0]->filename) + 8), "full_%s", stls->stl[0]->filename);
  stls->stl[0]->filename = (char *) &strbuffer; 
  write_stl(stls, strbuffer, 0);

  if (vec == NULL){
    printf("\n\t Error allocating memory! \n");
    exit(1);
  }

  for (i = 1; i < stls->nvol; i++){

    // Optimise screw
    optimise_screw(stls, vec[i][0], vec[i][1], vec[i][2], 180.0, 90.0);

    // Write out stl
    j = snprintf(strbuffer, (strlen(stls->stl[i]->filename) + 8), "screw_%s", stls->stl[i]->filename);
    write_stl(stls, strbuffer, stls->nvol);

    // Release memory
    free(stls->stl[stls->nvol]->facets);
    free(stls->stl[stls->nvol]->matrix);
    free(stls->stl[stls->nvol]);

  }

  // Over and out
  printf("\n\t ++++ That's all folks! ++++ \n\n");
  return 0;
}
