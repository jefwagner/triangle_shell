/*!*******************************************************************
 * \author Jef Wagner
 * \email jefwagner@gmail.com
 *********************************************************************
 */
/*!
 * This file contains the calculation of the elastic energy of the
 * shell.
 */

#include <stdlib.h>
#include <string.h>

#include "cg.h"
#include "energy.h"

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
  h += energy_gen( s, sp, dfdx);
  h += energy_mem( s, sp, dfdx);
  return h;
}

int relax_total( shell_run *sr ){
  unsigned int count = 0;
  nlcg_ws nlcg = sr->nlcg;
  int n = 3*((sr->s->num_v)+2);
  double *x = (double *) (sr->s->p_gen->x);
  fn_min_total_params p = { count, sr->s, sr->sp };
  nlcg_set_sys( &fn_min_total, n, (void *) &p, nlcg);
  sr->hmin = nlcg_optimize( x, nlcg);
  return p.count;
}

typedef struct{
  unsigned int count;
  shell *s;
  shell_params *sp;
  relax_partial_ws *rp;
} fn_min_partial_params;

double fn_min_partial( int n, const double *x, double *dfdx, void *p){
  int i;
  double h;
  unsigned int *count = &(((fn_min_partial_params *) p)->count);
  shell *s = ((fn_min_partial_params *) p)->s;
  shell_params *sp = ((fn_min_partial_params *) p)->sp;
  unsigned int num_vl = ((fn_min_partial_params *) p)->rp->num_vl;
  unsigned int *vl = ((fn_min_partial_params *) p)->rp->vl;
  unsigned int num_ll = ((fn_min_partial_params *) p)->rp->num_ll;
  unsigned int *ll = ((fn_min_partial_params *) p)->rp->ll;
  (*count)++;
  h = energy_lines( s, sp, num_ll, ll, &(dfdx[3*2]));
  h += energy_gen( s, sp, dfdx);
  h += energy_mem( s, sp, dfdx);
  for( i=0; i<num_vl; i++){
    dfdx[3*(vl[i]+2)+0] = 0.;
    dfdx[3*(vl[i]+2)+1] = 0.;
    dfdx[3*(vl[i]+2)+2] = 0.;
  }
  return h;
}

static void add_unique( unsigned int *num_list, unsigned int *list, 
                        unsigned int element){
  int i;
  for( i=0; i<*num_list; i++){
    if( list[i] == element){ break;}
  }
  if( i == *num_list ){
    list[i] = element;
    (*num_list) += 1;
  }
}

static void fill_line_and_vertex_list( shell *s, unsigned int vi,
                                      unsigned int depth,
                                      relax_partial_ws *rp){
  unsigned int i, j, k;
  unsigned int nl_old=0, nv_old=0, nv_moving, nv_fixed;
  unsigned int *nv = &(rp->num_vl);
  unsigned int *v = rp->vl;
  unsigned int *nl = &(rp->num_ll);
  unsigned int *l = rp->ll;

  v[0] = vi;
  *nv = 1;
  *nl = 0;

  for( k=0; k<depth; k++){
    nl_old = *nl;
    for( i=nv_old; i<*nv; i++){
      for( j=0; j<(s->vd[v[i]].num_l); j++){
        add_unique( nl, l, s->vd[v[i]].l[j]);
      }
    }
    nv_old = *nv;
    for( i=nl_old; i<*nl; i++){
      add_unique( nv, v, s->l[l[i]].i[0]);
      add_unique( nv, v, s->l[l[i]].i[1]);
    }
  }
  nv_moving = *nv;
  for( k=0; k<2; k++){
    nl_old = *nl;
    for( i=nv_old; i<*nv; i++){
      for( j=0; j<(s->vd[v[i]].num_l); j++){
        add_unique( nl, l, s->vd[v[i]].l[j]);
      }
    }
    nv_old = *nv;
    for( i=nl_old; i<*nl; i++){
      add_unique( nv, v, s->l[l[i]].i[0]);
      add_unique( nv, v, s->l[l[i]].i[1]);
    }
  }
  nv_fixed = *nv - nv_moving;
  memmove( &(v[0]), &(v[nv_moving]), nv_fixed*sizeof(unsigned int));
  *nv = nv_fixed;
}

relax_partial_ws* relax_partial_ws_malloc( unsigned int depth){
  unsigned int ed, size_vl, size_ll;
  relax_partial_ws *rp = 
    (relax_partial_ws *) malloc( sizeof(relax_partial_ws));
  if( rp == NULL ){
    return NULL;
  }
  ed = depth+2;
  size_vl = 3*ed*(ed+1)+1;
  size_ll = 3*ed*(3*ed-1);
  rp->vl = (unsigned int *) malloc( size_vl*sizeof(unsigned int));
  if( rp->vl == NULL ){
    free( rp);
    return NULL;
  }
  rp->ll = (unsigned int *) malloc( size_ll*sizeof(unsigned int));
  if( rp->ll == NULL ){
    free( rp->vl);
    free( rp);
    return NULL;
  }
  return rp;
}

void relax_partial_ws_free( relax_partial_ws *rp){
  free( rp->ll);
  free( rp->vl);
  free( rp);
}

int relax_partial( shell_run *sr , unsigned int vi){
  unsigned int count = 0;
  nlcg_ws nlcg = sr->nlcg;
  int n = 3*((sr->s->num_v)+2);
  double *x = (double *) (sr->s->p_gen->x);
  fn_min_partial_params p;

  fill_line_and_vertex_list( sr->s, vi, sr->sp->rp_depth, sr->rp);
  p.count = count;
  p.s = sr->s;
  p.sp = sr->sp;
  p.rp = sr->rp;

  nlcg_set_sys( &fn_min_partial, n, (void *) &p, nlcg);
  sr->hmin = nlcg_optimize( x, nlcg);

  return p.count;
}
