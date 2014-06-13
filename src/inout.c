/*!*******************************************************************
 * \author Jef Wagner
 * \email jefwagner@gmail.com
 *********************************************************************
 */
/*!
 * This file contains the input/output routines. It reads the
 * parameter file, can read a shell file, and write the shell file.
 */

#include <stdio.h>

#include "shell.h"

void shell_write( shell *s, FILE *f){
  unsigned int i, j;
  unsigned int num_v = s->num_v;
  point *v = s->v;
  vertex_data *vd = s->vd;
  unsigned int num_l = s->num_l;
  line *l = s->l;
  line_data *ld = s->ld;
  unsigned int num_t = s->num_t;
  triangle *t = s->t;
  triangle_data *td = s->td;

  fprintf( f, "Vertex Data %u\n", num_v);
  fprintf( f, "Positions \n");
  fprintf( f, "   x      y      z  \n");
  for( i=0; i<num_v; i++){
    fprintf( f, "%1.6e %1.6e %1.6e \n", 
            v[i].x[0], v[i].x[1], v[i].x[2]);
  }
  fprintf( f, "Vertex on edge \n");
  fprintf( f, "true/false\n");
  for( i=0; i<num_v; i++){
    if( vd[i].oe == yes){
      fprintf( f, "true\n");
    }else{
      fprintf( f, "false\n");
    }
  }
  fprintf( f, "Lines containing vertex\n");
  fprintf( f, "num_l l0 l1 ...\n");
  for( i=0; i<num_v; i++){
    fprintf( f, "%u ", vd[i].num_l);
    for( j=0; j<vd[i].num_l; j++){
      fprintf( f, "%u ", vd[i].l[j]);
    }
    fprintf( f, "\n");
  }
  fprintf( f, "Triangles containing vertex\n");
  fprintf( f, "num_t t0 t1 ...\n");
  for( i=0; i<num_v; i++){
    fprintf( f, "%u ", vd[i].num_t);
    for( j=0; j<vd[i].num_t; j++){
      fprintf( f, "%u ", vd[i].t[j]);
    }
    fprintf( f, "\n");
  }
  fprintf( f, "Line Data %u\n", num_l);
  fprintf( f, "Vertex indices \n"); 
  fprintf( f, "v0 v1 \n");
  for( i=0; i<num_l; i++){
    fprintf( f, "%u %u\n", l[i].i[0], l[i].i[1]);
  }
  fprintf( f, "Line on edge \n");
  fprintf( f, "true/false\n");
  for( i=0; i<num_l; i++){
    if( ld[i].oe == yes){
      fprintf( f, "true\n");
    }else{
      fprintf( f, "false\n");
    }
  }
  fprintf( f, "Triangles containing line\n");
  fprintf( f, "num_t t0 t1\n");
  for( i=0; i<num_l; i++){
    if( ld[i].oe == yes){
      fprintf( "1 %u\n", ld[i].t[0]);
    }else{
      fprintf( "2 %u\n", ld[i].t[0], ld[i].t[1]);
    }
  }
  fprintf( f, "Triangle Data\n");
  fprintf( f, "Vertex indices \n");
  fprintf( f, "v0 v1 v2\n");
  for( i=0; i<num_t; i++){
    fprintf( f, "%u %u\n", t[i].i[0], t[i].i[1], t[i].i[2]);
  }
  fprintf( f, "Triangle on edge \n");
  fprintf( f, "true/false\n");
  for( i=0; i<num_t; i++){
    if( td[i].oe == yes){
      fprintf( f, "true\n");
    }else{
      fprintf( f, "false\n");
    }
  }
  fprintf( f, "Lines on triangle\n");
  fprintf( f, "l0 l1 l2\n");
  for( i=0; i<num_t; i++){
    fprintf( "%u %u", td[i].l[0], td[i].l[1], td[i].l[2]);
  }
}

int shell_read( shell *s, FILE *f){
  unsigned int i, j, n;
  unsigned int num_v = s->num_v;
  point *v = s->v;
  vertex_data *vd = s->vd;
  unsigned int num_l = s->num_l;
  line *l = s->l;
  line_data *ld = s->ld;
  unsigned int num_t = s->num_t;
  triangle *t = s->t;
  triangle_data *td = s->td;
  char str1[81]; str2[81]; str3[81];

  n = fscanf( f, "%80s %80s %u", str1, str2, &(s->num_v));
  if( n != 3 || !strcmp( str1, "Vertex") || !strcmp( str2, "Data")){
    return INOUT_FORMAT_ERROR;
  }
  n = fscanf( "%80s", str1);
  if( n != 1 || !strcmp( str1, "Positions")){
    return INOUT_FORMAT_ERROR; 
  }
  n = fscanf( "%80s %80s %80s", str1, str2, str3);
  if( n != 2 || !strcmp( str1, "Positions") ||
     !strcmp( str2, "y") || !strcmp( str3, "z")){
    return INOUT_FORMAT_ERROR; 
  }
  for( i=0; i<s->num_v; i++){
    n = fscanf( f, "%lg %lg %lg", &(s->v[i].i[0]),
               &(s->v[i].i[1]), &(s->v[i].i[2]));
    if( n!= 3 ){
      return INOUT_FORMAT_ERROR;
    }
  }

  return INOUT_SUCCESS;
}