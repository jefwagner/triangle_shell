/*!*******************************************************************
 * \author Jef Wagner
 * \email jefwagner@gmail.com
 *********************************************************************
 */
/*!
 * This file contains the calculation of the elastic energy of the
 * shell.
 */

#include <math.h>

#include "shell.h"
#include "vec3.h"
#include "energy.h"

/*!
 * The energy of an unshared line.
 *
 * This file calculates the energy of an unshared line. It also
 * changes the derivative of the energy with respect to the particle
 * positions (adding the change the the previous result).
 */ 
static double energy_unshared( const shell *s, const shell_params *sp,
                              double *dx, unsigned int i){
  double h;
  unsigned int vi0, vi1;
  double *x0, *x1;
  double b, db_dx0[3], db_dx1[3];
  double k_s;

  k_s = sqrt(sp->gamma);

  vi0 = s->l[i].i[0];
  vi1 = s->l[i].i[1];

  x0 = (double *) s->v[vi0].x;
  x1 = (double *) s->v[vi1].x;  

  b = dist_d( x0, x1, db_dx0, db_dx1);
  h = 0.5*k_s*(b-1)*(b-1);
  for( i=0; i<3; i++){
    dx[3*vi0+i] += k_s*(b-1)*db_dx0[i];
    dx[3*vi1+i] += k_s*(b-1)*db_dx1[i];
  }

  return h;  
}

/*!
 * The energy of a shared line.
 *
 * This file calculates the energy of a shared line. It also changes
 * the derivative of the energy with respect to the particle positions
 * (adding the change the the previous result).
 */ 
static double energy_shared( const shell *s, const shell_params *sp,
                            double *dx, unsigned int i){
  double h;
  unsigned int vi0, vi1, vi2, vi3;
  double *x0, *x1, *x2, *x3;
  double b, db_dx0[3], db_dx1[3];
  double co, dco_dx0[3], dco_dx1[3], dco_dx2[3], dco_dx3[3];
  double si, dsi_dx0[3], dsi_dx1[3], dsi_dx2[3], dsi_dx3[3];
  double k_s, k_b, th0;

  k_s = sqrt(sp->gamma);
  k_b = 1./k_s;
  th0 = sp->th0;

  vi0 = s->l[i].i[0];
  vi1 = s->l[i].i[1];
  vi2 = other_index( s->t[s->ld[i].t[0]].i, vi0, vi1);
  vi3 = other_index( s->t[s->ld[i].t[1]].i, vi0, vi1);

  x0 = (double *) s->v[vi0].x;
  x1 = (double *) s->v[vi1].x;  
  x2 = (double *) s->v[vi2].x;
  x3 = (double *) s->v[vi3].x;

  b = dist_d( x0, x1, db_dx0, db_dx1);
  h = k_s*(b-1)*(b-1);
  for( i=0; i<3; i++){
    dx[3*vi0+i] += 2.*k_s*(b-1)*db_dx0[i];
    dx[3*vi1+i] += 2.*k_s*(b-1)*db_dx1[i];
  }

  dihedral_d( x0, x1, x2, x3, &co, &si,
             dco_dx0, dco_dx1, dco_dx2, dco_dx3,
             dsi_dx0, dsi_dx1, dsi_dx2, dsi_dx3);
  h += k_b*(1-co*cos(th0)-si*sin(th0));
  for( i=0; i<3; i++){
    dx[3*vi0+i] += k_b*(-cos(th0)*dco_dx0[i] - sin(th0)*dsi_dx0[i]);
    dx[3*vi1+i] += k_b*(-cos(th0)*dco_dx1[i] - sin(th0)*dsi_dx1[i]);
    dx[3*vi2+i] += k_b*(-cos(th0)*dco_dx2[i] - sin(th0)*dsi_dx2[i]);
    dx[3*vi3+i] += k_b*(-cos(th0)*dco_dx3[i] - sin(th0)*dsi_dx3[i]);
  }
  
  return h;  
}

/*!
 * Calculate the energy of a line.
 *
 * This calculates the checks whether the line is shared or unshared
 * (on edge) and calls the respective function.
 */
static double energy_line( const shell *s, const shell_params *sp,
                          double *dx, unsigned int i){
  if( s->ld[i].oe == yes ){
    return energy_unshared( s, sp, dx, i);
  }else{
    return energy_shared( s, sp, dx, i);
  }
}

/*!
 * Calculate the elastic energy for the shell.
 *
 * This calculates the energy for the entire shell, and fills in *dx
 * as the derivative of the energy with respect to the vertex
 * positions.
 */
double energy_shell( const shell *s, const shell_params *sp,
                    double *dx){
  double h;
  int i, num_l = s->num_l, num_v = s->num_v;

  for( i=0; i<3*num_v; i++){
    dx[i] = 0.;
  }

  h = 0.;
  for( i=0; i<num_l; i++){
    h += energy_line( s, sp, dx, i);
  }
  return h;
}

/*!
 * Calculate the elastic energy for a set of lines on the shell.
 */
double energy_lines( const shell *s, const shell_params *sp,
                    unsigned int num_l, const unsigned int *l,
                    double *dx){
  double h;
  int i, num_v = s->num_v;

  for( i=0; i<3*num_v; i++){
    dx[i] = 0.;
  }

  h = 0.;
  for( i=0; i<num_l; i++){
    h += energy_line( s, sp, dx, l[i]);
  }
  return h;
}

/*!
 * Calculate the energy between the shell and the genome.
 */
double energy_gen( const shell *s, const shell_params *sp,
                          double *dx){
  int i, j;
  double h, b, db_dgen[3], db_dv[3];
  double r_gen = sp->r_genome;

  dx[3*0+0] = 0.;
  dx[3*0+1] = 0.;
  dx[3*0+2] = 0.;

  h = 0.;
  for( i=0; i<3; i++){
    b = dist_d( s->p_gen->x, s->v[i].x, db_dgen, db_dv);
    h += 0.5*(b-r_gen)*(b-r_gen);
    for( j=0; j<3; j++){
      dx[3*0+j] += (b-r_gen)*db_dgen[j];
      dx[3*(i+2)+j] += (b-r_gen)*db_dv[j];
    }
  }
  for( i=3; i<s->num_v; i++){
    b = dist_d( s->p_gen->x, s->v[i].x, db_dgen, db_dv);
    if( b < r_gen ){
      h += 0.5*(b-r_gen)*(b-r_gen);
      for( j=0; j<3; j++){
        dx[3*0+j] += (b-r_gen)*db_dgen[j];
        dx[3*(i+2)+j] += (b-r_gen)*db_dv[j];
      }
    }
  }

  return h;
}

/*!
 * Calculate the energy between the shell and the membrane.
 */
double energy_mem( const shell *s, const shell_params *sp,
                          double *dx){
  int i, j;
  double h, b, db_dmem[3], db_dv[3];
  double r_mem = sp->r_membrane;

  dx[3*1+0] = 0.;
  dx[3*1+1] = 0.;
  dx[3*1+2] = 0.;

  h = 0.;
  for( i=0; i<s->num_v; i++){
    b = dist_d( s->p_mem->x, s->v[i].x, db_dmem, db_dv);
    if( b > r_mem ){
      h += 0.5*(b-r_mem)*(b-r_mem);
      for( j=0; j<3; j++){
        dx[3*1+j] += (b-r_mem)*db_dmem[j];
        dx[3*(i+2)+j] += (b-r_mem)*db_dv[j];
      }
    }
  }

  return h;
}
