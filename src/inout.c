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
#include <string.h>

#include "shell.h"
#include "inout.h"

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
  fprintf( f, "t0 [t1]\n");
  for( i=0; i<num_l; i++){
    if( ld[i].oe == yes){
      fprintf( f, "%u\n", ld[i].t[0]);
    }else{
      fprintf( f, "%u %u\n", ld[i].t[0], ld[i].t[1]);
    }
  }
  fprintf( f, "Triangle Data %u\n", num_t);
  fprintf( f, "Vertex indices \n");
  fprintf( f, "v0 v1 v2\n");
  for( i=0; i<num_t; i++){
    fprintf( f, "%u %u %u\n", t[i].i[0], t[i].i[1], t[i].i[2]);
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
    fprintf( f, "%u %u %u\n", td[i].l[0], td[i].l[1], td[i].l[2]);
  }
}

int shell_read( shell *s, FILE *f){
  unsigned int i, j, n;
  point *v = s->v;
  vertex_data *vd = s->vd;
  line *l = s->l;
  line_data *ld = s->ld;
  triangle *t = s->t;
  triangle_data *td = s->td;
  char str1[81], str2[81], str3[81], str4[81];

  /*****************************************************************
   * Vertex data
   *****************************************************************
   */
  n = fscanf( f, "%80s %80s %u", str1, str2, &(s->num_v));
  if( n != 3 || strcmp( str1, "Vertex") || strcmp( str2, "Data")){
    return INOUT_FORMAT_ERROR;
  }
  n = fscanf( f, "%80s", str1);
  if( n != 1 || strcmp( str1, "Positions")){
    return INOUT_FORMAT_ERROR; 
  }
  n = fscanf( f, "%80s %80s %80s", str1, str2, str3);
  if( n != 3 || strcmp( str1, "x") ||
     strcmp( str2, "y") || strcmp( str3, "z")){
    return INOUT_FORMAT_ERROR; 
  }
  for( i=0; i<s->num_v; i++){
    n = fscanf( f, "%lg %lg %lg", &(v[i].x[0]),
               &(v[i].x[1]), &(v[i].x[2]));
    if( n!= 3 ){
      return INOUT_FORMAT_ERROR;
    }
  }
  n = fscanf( f, "%80s %80s %80s", str1, str2, str3);
  if( n != 3 || strcmp( str1, "Vertex") ||
     strcmp( str2, "on") || strcmp( str3, "edge")){
    return INOUT_FORMAT_ERROR;
  }
  n = fscanf( f, "%80s", str1);
  if( n != 1 || strcmp( str1, "true/false")){
    return INOUT_FORMAT_ERROR;
  }
  for( i=0; i<s->num_v; i++){
    n = fscanf( f, "%80s", str1);
    if( n != 1 ){
      return INOUT_FORMAT_ERROR;
    }else{
      if( !strcmp( str1, "true")){
        vd[i].oe = yes;
      }else if( !strcmp( str1, "false")){
        vd[i].oe = no;
      }else{
        return INOUT_FORMAT_ERROR;
      }
    }
  }
  n = fscanf( f, "%80s %80s %80s", str1, str2, str3);
  if( n != 3 || strcmp( str1, "Lines") ||
     strcmp( str2, "containing") || strcmp( str3, "vertex")){
    return INOUT_FORMAT_ERROR;
  }
  n = fscanf( f, "%80s %80s %80s %80s", str1, str2, str3, str4);
  if( n != 4 || strcmp( str1, "num_l") ||
     strcmp( str2, "l0") || strcmp( str3, "l1") ||
     strcmp( str4, "...")){
    return INOUT_FORMAT_ERROR;
  }
  for( i=0; i<s->num_v; i++){
    n = fscanf( f, "%u", &vd[i].num_l );
    if( n != 1 || vd[i].num_l == 0 ){
      return INOUT_FORMAT_ERROR;
    }
    for( j=0; j<vd[i].num_l; j++){
      n = fscanf( f, "%u", &(vd[i].l[j]));
      if( n != 1){
        return INOUT_FORMAT_ERROR;
      }
    }
  }
  n = fscanf( f, "%80s %80s %80s", str1, str2, str3);
  if( n != 3 || strcmp( str1, "Triangles") ||
     strcmp( str2, "containing") || strcmp( str3, "vertex")){
    return INOUT_FORMAT_ERROR;
  }
  n = fscanf( f, "%80s %80s %80s %80s", str1, str2, str3, str4);
  if( n != 4 || strcmp( str1, "num_t") ||
     strcmp( str2, "t0") || strcmp( str3, "t1") ||
     strcmp( str4, "...")){
    return INOUT_FORMAT_ERROR;
  }
  for( i=0; i<s->num_v; i++){
    n = fscanf( f, "%u", &vd[i].num_t );
    if( n != 1 || vd[i].num_t == 0 ){
      return INOUT_FORMAT_ERROR;
    }
    for( j=0; j<vd[i].num_t; j++){
      n = fscanf( f, "%u", &(vd[i].t[j]));
      if( n != 1){
        return INOUT_FORMAT_ERROR;
      }
    }
  }
  /*****************************************************************
   * Line data
   *****************************************************************
   */
  n = fscanf( f, "%80s %80s %u", str1, str2, &(s->num_l));
  if( n != 3 || strcmp( str1, "Line") || strcmp( str2, "Data")){
    return INOUT_FORMAT_ERROR;
  }
  n = fscanf( f, "%80s %80s", str1, str2);
  if( n != 2 || strcmp( str1, "Vertex") ||
     strcmp( str2, "indices")){
    return INOUT_FORMAT_ERROR; 
  }
  n = fscanf( f, "%80s %80s", str1, str2);
  if( n != 2 || strcmp( str1, "v0") ||
     strcmp( str2, "v1")){
    return INOUT_FORMAT_ERROR; 
  }
  for( i=0; i<s->num_l; i++){
    n = fscanf( f, "%u %u", &(l[i].i[0]), &(l[i].i[1]));
    if( n!= 2 ){
      return INOUT_FORMAT_ERROR;
    }
  }
    n = fscanf( f, "%80s %80s %80s", str1, str2, str3);
  if( n != 3 || strcmp( str1, "Line") ||
     strcmp( str2, "on") || strcmp( str3, "edge")){
    return INOUT_FORMAT_ERROR;
  }
  n = fscanf( f, "%80s", str1);
  if( n != 1 || strcmp( str1, "true/false")){
    return INOUT_FORMAT_ERROR;
  }
  for( i=0; i<s->num_l; i++){
    n = fscanf( f, "%80s", str1);
    if( n != 1 ){
      return INOUT_FORMAT_ERROR;
    }else{
      if( !strcmp( str1, "true")){
        ld[i].oe = yes;
      }else if( !strcmp( str1, "false")){
        ld[i].oe = no;
      }else{
        return INOUT_FORMAT_ERROR;
      }
    }
  }
  n = fscanf( f, "%80s %80s %80s", str1, str2, str3);
  if( n != 3 || strcmp( str1, "Triangles") ||
     strcmp( str2, "containing") || strcmp( str3, "line")){
    return INOUT_FORMAT_ERROR;
  }
  n = fscanf( f, "%80s %80s", str1, str2);
  if( n != 2 || 
     strcmp( str1, "t0") || strcmp( str2, "[t1]")){
    return INOUT_FORMAT_ERROR;
  }
  for( i=0; i<s->num_l; i++){
    if( ld[i].oe == yes ){
      n = fscanf( f, "%u", &(ld[i].t[0]) );
      if( n != 1 ){
        return INOUT_FORMAT_ERROR;
      }
    }else{
      n = fscanf( f, "%u %u", &(ld[i].t[0]), &(ld[i].t[1]) );
      if( n != 2 ){
        return INOUT_FORMAT_ERROR;
      }
    }
  }
  /*****************************************************************
   * Triangle data
   *****************************************************************
   */
  n = fscanf( f, "%80s %80s %u", str1, str2, &(s->num_t));
  if( n != 3 || strcmp( str1, "Triangle") || strcmp( str2, "Data")){
    return INOUT_FORMAT_ERROR;
  }
  n = fscanf( f, "%80s %80s", str1, str2);
  if( n != 2 || strcmp( str1, "Vertex") ||
     strcmp( str2, "indices")){
    return INOUT_FORMAT_ERROR; 
  }
  n = fscanf( f, "%80s %80s %80s", str1, str2, str3);
  if( n != 3 || strcmp( str1, "v0") ||
     strcmp( str2, "v1") || strcmp( str3, "v2")){
    return INOUT_FORMAT_ERROR; 
  }
  for( i=0; i<s->num_t; i++){
    n = fscanf( f, "%u %u %u", &(t[i].i[0]), 
               &(t[i].i[1]), &(t[i].i[2]));
    if( n!= 3 ){
      return INOUT_FORMAT_ERROR;
    }
  }
    n = fscanf( f, "%80s %80s %80s", str1, str2, str3);
  if( n != 3 || strcmp( str1, "Triangle") ||
     strcmp( str2, "on") || strcmp( str3, "edge")){
    return INOUT_FORMAT_ERROR;
  }
  n = fscanf( f, "%80s", str1);
  if( n != 1 || strcmp( str1, "true/false")){
    return INOUT_FORMAT_ERROR;
  }
  for( i=0; i<s->num_t; i++){
    n = fscanf( f, "%80s", str1);
    if( n != 1 ){
      return INOUT_FORMAT_ERROR;
    }else{
      if( !strcmp( str1, "true")){
        td[i].oe = yes;
      }else if( !strcmp( str1, "false")){
        td[i].oe = no;
      }else{
        return INOUT_FORMAT_ERROR;
      }
    }
  }
  n = fscanf( f, "%80s %80s %80s", str1, str2, str3);
  if( n != 3 || strcmp( str1, "Lines") ||
     strcmp( str2, "on") || strcmp( str3, "triangle")){
    return INOUT_FORMAT_ERROR;
  }
  n = fscanf( f, "%80s %80s %80s", str1, str2, str3);
  if( n != 3 || strcmp( str1, "l0") ||
     strcmp( str2, "l1") || strcmp( str3, "l2")){
    return INOUT_FORMAT_ERROR;
  }
  for( i=0; i<s->num_t; i++){
    n = fscanf( f, "%u %u %u", &(td[i].l[0]), &(td[i].l[1]), 
               &(td[i].l[2]));
    if( n != 3 ){
      return INOUT_FORMAT_ERROR;
    }
  }


  return INOUT_SUCCESS;
}

int read_param_file( shell_params *sp, FILE *file){
  int i, k, n[10];
  int tmp_i, tmp_i_array[100];
  double tmp_d, tmp_d_array[100];
  pdata pd;
  double *var_d[6];
  int *var_i[1];
  const char *var_d_names[] = { "gamma", "th0", "sigma", 
  "r_membrane", "r_genome", "delta_b"};
  const char *var_i_names[] = { "seed"};
  var_d[0] = &(sp->gamma);
  var_d[1] = &(sp->th0);
  var_d[2] = &(sp->sigma);
  var_d[3] = &(sp->r_membrane);
  var_d[4] = &(sp->r_genome);
  var_d[5] = &(sp->delta_b);
  var_i[0] = &(sp->seed);

  pdata_read_file( &pd, file);
  for( i=0; i<n; i++){
    status = pdata_get_var_d( &pd, var_d_names[i], &tmp);
    if( status == PDATA_SUCCESS){
      *(var_d[i]) = tmp;
    }
    n[k] = pdata_get_array_d( &pd, var_d_names[i], tmp_d_array);
  }
  status = pdata_get_var_d( &pd, "gamma", &tmp);
  if( status == PDATA_SUCCESS ){
    sp->gamma = tmp;
  }
  status = pdata_get_var_d( &pd, "th0", &tmp);
  if( status == PDATA_SUCCESS ){
    sp->th0 = PI_180*tmp;
  }
  status = pdata_get_var_d( &pd, "delta_b", &tmp);
  if( status == PDATA_SUCCESS ){
    sp->delta_b = tmp;
  }
  status = pdata_get_var_d( &pd, "sigma", &tmp);
  if( status == PDATA_SUCCESS ){
    sp->sigma = tmp;
  }
}
