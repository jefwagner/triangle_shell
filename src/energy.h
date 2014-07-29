/*!*******************************************************************
 * \author Jef Wagner
 * \email jefwagner@gmail.com
 *********************************************************************
 */
/*!
 * This file contains the prototype for the energy function.
 */
#ifndef JW_ENERGY
#define JW_ENERGY

#include "main.h"

 double energy_shell( const shell *s, const shell_params *sp,
                     double *dx){
  double h;
  int i, num_l = s->num_l;

  h = 0.;
  for( i=0; i<num_l; i++){
    h += energy_line( s, sp, dx, i);
  }
  return h;
}

#endif
