/*!*******************************************************************
 * \author Jef Wagner
 * \email jefwagner@gmail.com
 *********************************************************************
 */
/*!
 * This file contains the functions for changing the shell.
 */

#include <math.h>

#include "main.h"
#include "shell.h"
#include "vec3.h"
#include "math_const.h"

/*!
 * A simple one-liner to find the other index.
 *
 * Add all the indices together, and subtract out the two known
 * indices, leaving the unknown index.
 */
static inline unsigned int other_index( const unsigned int *t,
            unsigned int vi0, unsigned int vi1){
  return( t[0] + t[1] + t[2] - vi0 - vi1 );
}

static point new_point( shell *s, shell_params *sp, unsigned int li){
  int i;
  unsigned int vi0, vi1, vi2;
  double *v0, *v1, *v2, t[3], n[3], b[3];
  double l;
  point pout;
  double th0 = sp->th0;

  vi0 = s->l[li].i[0];
  vi1 = s->l[li].i[1];
  vi2 = other_index( s->t[s->ld[li].t[0]].i, vi0, vi1);
  v0 = s->v[vi0].x;
  v1 = s->v[vi1].x;
  v2 = s->v[vi2].x;
  dif_hat( v0, v1, t);
  n_hat( v0, v1, v2, n);
  cross( n, t, b);
  l = dist( v0, v1);
  l = sqrt(1.-0.25*l*l);
  for( i=0; i<3; i++){
    pout.x[i] = 0.5*( v0[i] + v1[i] )
      + l*( cos(th0)*b[i] + sin(th0)*n[i]);
  }
  return pout;
}

int grow( shell_run *sr ){
  int i;
  point pnew;
  unsigned int num_l = sr->s->num_l;

  for( i=0; i<num_l; i++){
    if( sr->s->ld[i].oe == yes ){
      break;
    }
  }
  if( i==num_l ){
    return 0;
  }
  pnew = new_point( sr->s, sr->sp, i);
  shell_attach( sr->s, pnew, i);
  return 1;
}