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
#include "energy.c"

typedef struct{
  int count;
  shell *s,
  shell_params *sp
} fmin_params;

double fmin( int n, const double *x, double *dfdx, void *p ){
  int *count = &(((fmin_params *) p)->count);
  shell *s = ((fmin_params *) p)->s;
  shell_params *sp = ((fmin_params *) p)->sp;
  (*count)++;
  return energy_shell( s, sp, dfdx);
}

int relax( shell_run *sr ){
  double count = 0;
  nlcg_ws nlcg = sr->nlcg;
  int n = 3*(sr->s->num_v);
  double *x = (double *) (sr->s->v[0].x);
  fmin_params p = { count, sr->s, sr->sp };
  nlcg_set_sys( &fmin, n, (void *) &p, nlcg);
  sr->hmin = nlcg_optimize( x, nlcg);
  return p.count;
}