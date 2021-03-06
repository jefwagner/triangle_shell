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
                     double *dx);
double energy_partial( const shell *s, const shell_params *sp,
                       unsigned int num_l, const unsigned int *l,
                       unsigned int num_v, const unsigned int *v,
                       double *dx);

#endif
