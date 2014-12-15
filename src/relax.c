/*!*******************************************************************
 * \author Jef Wagner
 * \email jefwagner@gmail.com
 *********************************************************************
 */
/*!
 * This file contains the calculation of the elastic energy of the
 * shell.
 */

#include <string.h>

#include "cg.h"
#include "energy.h"

void add_unique( int new, int *size, int *list);
void add_lines( int start, int end, const int *v_list,
                int *size, int *l_list, const shell *s);
void add_vertices( int start, int end, const int *l_list,
                   int *size, int *v_list, const shell *s);
void neighbors( int center, int depth, 
                int *v_size, int *v_list, int *start_vf,
                int *l_size, int *l_list, const shell *s);


typedef struct{
  unsigned int count;
  shell *s;
  shell_params *sp;
} fn_min_total_params;

double fn_min_total( int n, const double *x, double *dfdx, void *p ){
  double h;
  unsigned int *count = &(((fn_min_total_params *) p)->count);
  shell *s = ((fn_min_total_params *) p)->s;
  shell_params *sp = ((fn_min_total_params *) p)->sp;
  (*count)++;
  h = energy_shell( s, sp, &(dfdx[3*2]));
  if( sp->r_genome != 0.0 ){
    h += energy_gen( s, sp, dfdx);
  }
  if( sp->r_membrane != 0.0 ){
    h += energy_mem( s, sp, dfdx);
  }
  return h;
}

int relax_total( shell_run *sr , shell *s){
  double count = 0;
  nlcg_ws nlcg = sr->nlcg;
  int n = 3*(s->num_v+1);
  double *x = (double *) (s->vg[0].x);
  fn_min_params p = { count, s, sr->sp };
  nlcg_set_sys( &fn_min, n, (void *) &p, nlcg);
  sr->hmin = nlcg_optimize( x, nlcg);
  return p.count;
}

typedef struct{
  int count;
  shell *s;
  shell_params *sp;
  int v_size, *v_list;
  int l_size, *l_list;
  int start_vf;
} fn_partial_params;

double fn_partial( int n, const double *x, double *dfdx, void *p){
  double h;
  int i, j, vi;
  int *count = &(((fn_partial_params *) p)->count);
  shell *s = ((fn_partial_params *) p)->s;
  shell_params *sp = ((fn_partial_params *) p)->sp;
  int v_size = ((fn_partial_params *) p)->v_size;
  int *v_list = ((fn_partial_params *) p)->v_list;
  int l_size = ((fn_partial_params *) p)->l_size;
  int *l_list = ((fn_partial_params *) p)->l_list;
  int start_vf = ((fn_partial_params *) p)->start_vf;
  (*count)++;
  h = energy_partial( s, sp, l_size, (unsigned int *) l_list, 
                      v_size, (unsigned int *) v_list, dfdx);
  for( i=start_vf; i<v_size; i++){
    vi = v_list[i];
    for( j=0; j<3; j++){
      dfdx[3*vi+j+3] = 0.;
    }
  }
  return h;
}

int relax_partial( shell_run *sr, int center){
  double count =0;
  nlcg_ws nlcg = sr->nlcg;
  int n = 3*(sr->s->num_v+1);
  double *x = (double *) (sr->s->vg[0].x);
  int v_size, v_list[500], start_vf;
  int l_size, l_list[500];
  neighbors( center, sr->sp->depth, 
             &v_size, v_list, &start_vf, 
             &l_size, l_list, sr->s);
  fn_partial_params p = {count, sr->s, sr->sp, 
                         v_size, v_list, l_size, l_list, start_vf};
  nlcg_set_sys( &fn_partial, n, (void *) &p, nlcg);
  sr->hmin = nlcg_optimize( x, nlcg);
  return p.count;
}

/*!
 * Add a unique element to a list.
 */
void add_unique( int new, int *size, int *list){
  int i;
  for( i=0; i<*size; i++){
    if( new == list[i]){
      return;
    }
  }
  list[*size] = new;
  *size += 1;
}

/*!
 * Go through a list of vertices from start to end, and for each
 * vertex go through the vertex data line list, and add each line. 
 */
void add_lines( int start, int end, const int *v_list,
                int *size, int *l_list, const shell *s){
  int i, vi, j;
  for( i=start; i<end; i++){
    vi = v_list[i];
    for( j=0; j<s->vd[vi].num_l; j++){
      add_unique( s->vd[vi].l[j], size, l_list);
    }
  }
}

/*!
 * Go through a list of lines from start to end, and for each line
 * go add each vertex to a list.
 */
void add_vertices( int start, int end, const int *l_list,
                   int *size, int *v_list, const shell *s){
  int i, li, j;
  for( i=start; i<end; i++){
    li = l_list[i];
    for( j=0; j<2; j++){
      add_unique( s->l[i].i[j], size, v_list);
    }
  }
}

/*!
 * Fill up a list of vertices and lines that are neighbors to the 
 * vertex `center`.
 */
void neighbors( int center, int depth, 
                int *v_size, int *v_list, int *start_vf, 
                int *l_size, int *l_list, const shell *s){
    int i;
    int v_start, l_start;

    *l_size = 0;
    *v_size = 1;
    v_list[0] = center;
    v_start = 0;

    for( i=0; i<depth-1; i++){
      l_start = *l_size;
      add_lines( v_start, *v_size, v_list, l_size, l_list, s);
      v_start = *v_size;
      add_vertices( l_start, *l_size, l_list, v_size, v_list, s);
    }
    *start_vf = v_start;
    l_start = *l_size;
    add_lines( v_start, *v_size, v_list, l_size, l_list, s);
    add_vertices( l_start, *l_size, l_list, v_size, v_list, s);
}
