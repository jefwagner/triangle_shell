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
double energy_lines( const shell *s, const shell_params *sp,
                    unsigned int num_l, const unsigned int *l,
                    double *dx);
double energy_gen( const shell *s, const shell_params *sp,
                          double *dx);
double energy_mem( const shell *s, const shell_params *sp,
                          double *dx);
double energy_gen_partial( const shell *s, const shell_params *sp,
                           double *dx, unsigned int num_vl, 
                           const unsigned int *vl);
double energy_mem_partial( const shell *s, const shell_params *sp,
                           double *dx, unsigned int num_vl,
                           const unsigned int *vl); 

#endif
