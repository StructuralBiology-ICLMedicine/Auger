
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

stl *make_screw(double x, double y, double z, double beta, double gamma, double r, double h, double g, double fill){
  // Make STL of a screw given x, y, z, alpha, beta, length and fill 
  int32_t i;

  stl *cyl = NULL;
  cyl = calloc(1, sizeof(stl));
  if (cyl == NULL){
    printf("\n\t Error assigning memory for screw! \n");
    exit(1);
  }

  cyl->shift[0] = x;
  cyl->shift[1] = y;
  cyl->shift[2] = z;

  cyl->matrix = make_matrix(0, beta, gamma);

  cyl->fill  = fill;

  cyl->nfacets = 720;
  cyl->facets = NULL;
  cyl->facets = calloc(cyl->nfacets, sizeof(triangle));
  if (cyl->facets == NULL){
    printf("\n\t Error assigning memory for screw! \n");
    exit(1);
  }

  h /= 2.0;
  double f = g - r;
  float ob[3] = {0.0, 0.0, f-h};
  float ot[3] = {0.0, 0.0, g+h};
  float vb[3] = {0.0,   r, g-h};
  float vt[3] = {0.0,   r, g+h};

  for (i = 0; i < 180; i++){

    // Copy initial vertices into triangles
    memcpy(&cyl->facets[i].vtx1[0],       &ob, 3 * sizeof(float));
    memcpy(&cyl->facets[i].vtx3[0],       &vb, 3 * sizeof(float));
    memcpy(&cyl->facets[i + 180].vtx1[0], &vb, 3 * sizeof(float));
    memcpy(&cyl->facets[i + 180].vtx3[0], &vt, 3 * sizeof(float));
    memcpy(&cyl->facets[i + 360].vtx1[0], &vt, 3 * sizeof(float));
    memcpy(&cyl->facets[i + 540].vtx1[0], &ot, 3 * sizeof(float));
    memcpy(&cyl->facets[i + 540].vtx2[0], &vt, 3 * sizeof(float));

    // Update vertices by rotation
    vb[0] = sin((((double) i + 1) / 90.0) * M_PI) * r;
    vt[0] = sin((((double) i + 1) / 90.0) * M_PI) * r;
    vb[1] = cos((((double) i + 1) / 90.0) * M_PI) * r;
    vt[1] = cos((((double) i + 1) / 90.0) * M_PI) * r;

    // Copy modified vertices into triangles
    memcpy(&cyl->facets[i].vtx2[0],       &vb, 3 * sizeof(float));
    memcpy(&cyl->facets[i + 180].vtx2[0], &vb, 3 * sizeof(float));
    memcpy(&cyl->facets[i + 360].vtx2[0], &vb, 3 * sizeof(float));
    memcpy(&cyl->facets[i + 360].vtx3[0], &vt, 3 * sizeof(float));
    memcpy(&cyl->facets[i + 540].vtx3[0], &vt, 3 * sizeof(float));
  }

  return cyl;
}

stl *make_cylinder(double x, double y, double z, double beta, double gamma, double r, double h, double g, double fill){
  // Make STL of a cylinder given x, y, z, alpha, beta, length and fill 
  int32_t i;

  stl *cyl = NULL;
  cyl = calloc(1, sizeof(stl));
  if (cyl == NULL){
    printf("\n\t Error assigning memory for screw! \n");
    exit(1);
  }

  cyl->shift[0] = x;
  cyl->shift[1] = y;
  cyl->shift[2] = z;

  cyl->matrix = make_matrix(0, beta, gamma);

  cyl->fill  = fill;

  cyl->nfacets = 720;
  cyl->facets = NULL;
  cyl->facets = calloc(cyl->nfacets, sizeof(triangle));
  if (cyl->facets == NULL){
    printf("\n\t Error assigning memory for screw! \n");
    exit(1);
  }

  h /= 2.0;
  float ob[3] = {0.0, 0.0, g-h};
  float ot[3] = {0.0, 0.0, g+h};
  float vb[3] = {0.0,   r, g-h};
  float vt[3] = {0.0,   r, g+h};

  for (i = 0; i < 180; i++){

    // Copy initial vertices into triangles
    memcpy(&cyl->facets[i].vtx1[0],       &ob, 3 * sizeof(float));
    memcpy(&cyl->facets[i].vtx3[0],       &vb, 3 * sizeof(float));
    memcpy(&cyl->facets[i + 180].vtx1[0], &vb, 3 * sizeof(float));
    memcpy(&cyl->facets[i + 180].vtx3[0], &vt, 3 * sizeof(float));
    memcpy(&cyl->facets[i + 360].vtx1[0], &vt, 3 * sizeof(float));
    memcpy(&cyl->facets[i + 540].vtx1[0], &ot, 3 * sizeof(float));
    memcpy(&cyl->facets[i + 540].vtx2[0], &vt, 3 * sizeof(float));

    // Update vertices by rotation
    vb[0] = sin((((double) i + 1) / 90.0) * M_PI) * r;
    vt[0] = sin((((double) i + 1) / 90.0) * M_PI) * r;
    vb[1] = cos((((double) i + 1) / 90.0) * M_PI) * r;
    vt[1] = cos((((double) i + 1) / 90.0) * M_PI) * r;

    // Copy modified vertices into triangles
    memcpy(&cyl->facets[i].vtx2[0],       &vb, 3 * sizeof(float));
    memcpy(&cyl->facets[i + 180].vtx2[0], &vb, 3 * sizeof(float));
    memcpy(&cyl->facets[i + 360].vtx2[0], &vb, 3 * sizeof(float));
    memcpy(&cyl->facets[i + 360].vtx3[0], &vt, 3 * sizeof(float));
    memcpy(&cyl->facets[i + 540].vtx3[0], &vt, 3 * sizeof(float));
  }

  return cyl;
}
