/*!*******************************************************************
 * \author Jef Wagner
 * \email jefwagner@gmail.com
 *********************************************************************
 */
/*!
 * This file contains the calculation of the elastic energy of the
 * shell.
 */

#include "cg.h"
#include "energy.h"

typedef struct{
  int count;
  shell *s;
  shell_params *sp;
} fn_min_params;

double fn_min( int n, const double *x, double *dfdx, void *p ){
  int *count = &(((fn_min_params *) p)->count);
  shell *s = ((fn_min_params *) p)->s;
  shell_params *sp = ((fn_min_params *) p)->sp;
  (*count)++;
  return energy_shell( s, sp, dfdx);
}

int relax( shell_run *sr ){
  double count = 0;
  nlcg_ws nlcg = sr->nlcg;
  int n = 3*(sr->s->num_v);
  double *x = (double *) (sr->s->v[0].x);
  fn_min_params p = { count, sr->s, sr->sp };
  nlcg_set_sys( &fn_min, n, (void *) &p, nlcg);
  sr->hmin = nlcg_optimize( x, nlcg);
  return p.count;
}