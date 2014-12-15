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
#include <time.h>
#include <math.h>

#include "main.h"
#include "shell.h"
#include "inout.h"
#include "params.h"
#include "math_const.h"

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

  fprintf( f, "Position of Genome\n");
  fprintf( f, "x y z\n");
  fprintf( f, "%1.6e %1.6e %1.6e\n", s->p_gen->x[0], 
          s->p_gen->x[1], s->p_gen->x[2]);
  fprintf( f, "Position of Membrane\n");
  fprintf( f, "x y z\n");
  fprintf( f, "%1.6e %1.6e %1.6e\n", s->p_mem->x[0],
          s->p_mem->x[1], s->p_mem->x[2]);
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

/*!
 * Read the parameter file.
 *
 * This reads in the parameters when they exist from a parameter file.
 * All variables are treated as if they are read in from list. The
 * integer parameter chooses from the list, where the first variable
 * listed is the least significant to the most significant.
 */
 #define ND 8
 #define NI 4
int read_param_file( shell_params *sp, FILE *file, unsigned int n){
  int k; 
  int nd[ND], ni[NI], sd[ND], si[NI]; 
  int id[ND], ii[ND], tmp_n;
  int tmp_i, n_tot;
  double tmp_d;
  double *var_d[ND], r_0;
  int *var_i[NI];
  /* Initialize parameter data structure with all zeros */
  pdata pd = { {{0, "\0", "\0"}}, 0};
  /* List out the names of the parameters in the parameter file. */
  const char *var_d_names[] = { 
    "gamma", 
    "r_0", 
    "sigma", 
    "r_membrane", 
    "r_genome", 
    "delta_b",
    "a_upper",
    "a_lower"
  };
  const char *var_i_names[] = { 
    "seed",
    "movie",
    "depth",
    "max_tri"
  };
  /* List out pointers to the parameter in the program. */
  var_d[0] = &(sp->gamma);
  var_d[1] = &r_0;
  var_d[2] = &(sp->sigma);
  var_d[3] = &(sp->r_membrane);
  var_d[4] = &(sp->r_genome);
  var_d[5] = &(sp->delta_b);
  var_d[6] = &(sp->a_upper);
  var_d[7] = &(sp->a_lower);
  var_i[0] = (int *) &(sp->seed);
  var_i[1] = (int *) &(sp->movie);
  var_i[2] = (int *) &(sp->depth);
  var_i[3] = (int *) &(sp->max_tri);

  /* First give the default parameters */
  sp->gamma = 2;
  r_0 = 1.1;
  sp->sigma = 1.e-4;
  sp->r_membrane = 0.;
  sp->r_genome = 0.;
  sp->delta_b = 0.8;
  sp->a_upper = 32.;
  sp->a_lower = 15.;
  sp->seed = (unsigned int) time(NULL);
  sp->movie = 0;
  sp->depth = 4;
  sp->max_tri = 1500;

  /* read the parameter file */
  if( pdata_read_file( &pd, file) == PDATA_FORMAT ){
    return INOUT_FORMAT_ERROR;
  }

  /* get the number of each parameter */
  for( k=0; k<ND; k++){
    nd[k] = 0;
    sd[k] = pdata_get_var_d( &pd, var_d_names[k], &tmp_d);
    if( sd[k] == PDATA_SUCCESS){
      nd[k] = 1;
    }else{
      nd[k] = pdata_array_length( &pd, var_d_names[k]);
    }
  }
  for( k=0; k<NI; k++){
    ni[k] = 0;
    si[k] = pdata_get_var_i( &pd, var_i_names[k], &tmp_i);
    if( si[k] == PDATA_SUCCESS){
      ni[k] = 1;
    }else{
      ni[k] = pdata_array_length( &pd, var_i_names[k]);
    }
  }
  /* Check that n < n_tot */
  n_tot = 1;
  for( k=0; k<ND; k++){
    if( nd[k] != 0 ){
      n_tot *= nd[k];
    }
  }
  for( k=0; k<NI; k++){
    if( ni[k] != 0 ){
      n_tot *= ni[k];
    }
  }
  if( n >= n_tot ){
    return INOUT_OUT_OF_BOUNDS;
  }
  /* Find the index for each parameter */
  tmp_n = n;
  for( k=0; k<ND; k++){
    if( nd[k] != 0 ){
      id[k] = tmp_n%nd[k];
      tmp_n /= nd[k];
    }else{
      id[k] = -1;
    }
  }
  for( k=0; k<NI; k++){
    if( ni[k] != 0 ){
      ii[k] = tmp_n%ni[k];
      tmp_n /= ni[k];
    }else{
      ii[k] = -1;
    }
  }
  /* Read each parameter */
  for( k=0; k<ND; k++){
    if( nd[k] != 0){
      if( sd[k] == PDATA_SUCCESS){
        pdata_get_var_d( &pd, var_d_names[k], var_d[k]);
      }else{
        pdata_get_element_d( &pd, var_d_names[k], id[k], var_d[k]);
      }
    }
  }
  for( k=0; k<NI; k++){
    if( ni[k] != 0){
      if( si[k] == PDATA_SUCCESS){
        pdata_get_var_i( &pd, var_i_names[k], var_i[k]);
      }else{
        pdata_get_element_i( &pd, var_i_names[k], ii[k], var_i[k]);
      }
    }
  }

  /* Treat the data */
  /* Translate from r_0 to th0 */
  sp->th0 = 2.*asin(1./sqrt(12.*r_0*r_0-3.));
  /* Change sigma into radians */
  sp->sigma *= PI_180;
  /* Change a_upper and a_lower into radians */
  sp->a_upper *= PI_180;
  sp->a_lower *= PI_180;

  return INOUT_SUCCESS;
}

void shell_params_write( shell_params *sp, FILE *file,
                        const char *filename, unsigned int n){
  double s = sin(0.5*sp->th0);
  double r_0 = sqrt(1./(12*s*s)+1./4.);
  double sigma = sp->sigma/PI_180;
  double a_upper = sp->a_upper/PI_180;
  double a_lower = sp->a_lower/PI_180;
  fprintf( file, "# Parameter file for %s run number %u\n",
          filename, n);
  fprintf( file, "var gamma %1.10e\n", sp->gamma);
  fprintf( file, "var r_0 %1.10e\n", r_0);
  fprintf( file, "var sigma %1.10e\n", sigma);
  fprintf( file, "var r_membrane %1.10e\n", sp->r_membrane);
  fprintf( file, "var r_genome %1.10e\n", sp->r_genome);
  fprintf( file, "var delta_b %1.10e\n", sp->delta_b);
  fprintf( file, "var a_upper %1.10e\n", a_upper);
  fprintf( file, "var a_lower %1.10e\n", a_lower);
  fprintf( file, "var seed %u\n", sp->seed);
  fprintf( file, "var movie %u\n", sp->movie);
  fprintf( file, "var depth %u\n", sp->depth);
  fprintf( file, "var max_tri %u\n", sp->max_tri);
}
