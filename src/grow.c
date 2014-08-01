/*!*******************************************************************
 * \author Jef Wagner
 * \email jefwagner@gmail.com
 *********************************************************************
 */
/*!
 * This file contains the functions for changing the shell.
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "main.h"
#include "shell.h"
#include "vec3.h"
#include "math_const.h"

/*!
 * Find the position of the new point when attaching a triangle.
 */
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
      + l*( cos(th0)*b[i] - sin(th0)*n[i]);
  }
  return pout;
}

/*!
 * Find the opening angle at a vertex on the edge.
 */
static double get_vertex_angle( shell *s, unsigned int vi){
  unsigned int lil, lir;
  unsigned int vi1, vi3, vi4, vi5;
  double *x1, *x2, *x3, *x4, *x5;
  vertex_lines_on_edge( s, vi, &lil, &lir);

  vi1 = s->l[lil].i[0];
  vi3 = s->l[lir].i[1];
  vi4 = other_index( s->t[s->ld[lil].t[0]].i, vi1, vi);
  vi5 = other_index( s->t[s->ld[lir].t[0]].i, vi, vi3);
  x1 = s->v[vi1].x;
  x2 = s->v[vi].x;
  x3 = s->v[vi3].x;
  x4 = s->v[vi4].x;
  x5 = s->v[vi5].x;

  return angle( x1, x2, x3, x4, x5);
}

/*!
 * Find an angle for the two vertices on a line.
 *
 * This finds the opening angle for two vertices on a line, then
 * returns the harmonic mean of the two. Using the harmonic mean
 * serves to weight the smaller angel more, but somehow account for
 * the opening angle of both vertices.
 */
static double get_line_angle( shell *s, unsigned int li){
  unsigned int lil, li_temp, lir;
  unsigned int vi1, vi2, vi3, vi4, vi5;
  double *x1, *x2, *x3, *x4, *x5;
  double a_l, a_r;
  vertex_lines_on_edge( s, s->l[li].i[0], &lil, &li_temp);
  vertex_lines_on_edge( s, s->l[li].i[1], &li_temp, &lir);

  vi1 = s->l[lil].i[0];
  vi2 = s->l[li].i[0];
  vi3 = s->l[li].i[1];
  vi4 = other_index( s->t[s->ld[lil].t[0]].i, vi1, vi2);
  vi5 = other_index( s->t[s->ld[li].t[0]].i, vi2, vi3);
  x1 = s->v[vi1].x;
  x2 = s->v[vi2].x;
  x3 = s->v[vi3].x;
  x4 = s->v[vi4].x;
  x5 = s->v[vi5].x;
  a_l = angle( x1, x2, x3, x4, x5);

  vi1 = s->l[li].i[0];
  vi2 = s->l[li].i[1];
  vi3 = s->l[lir].i[1];
  vi4 = other_index( s->t[s->ld[li].t[0]].i, vi1, vi2);
  vi5 = other_index( s->t[s->ld[lir].t[0]].i, vi2, vi3);
  x1 = s->v[vi1].x;
  x2 = s->v[vi2].x;
  x3 = s->v[vi3].x;
  x4 = s->v[vi4].x;
  x5 = s->v[vi5].x;
  a_r = angle( x1, x2, x3, x4, x5);

  return (2./(1./a_l+1./a_r));
}

/*!
 * Fill up the array of possible moves;
 *
 * This function fill the array of possible moves. Currently it loops
 * over vertices and lines. In the vertex loop, if the vertex is on
 * the edge, and has 4 or more triangle attached it is possible to do
 * a vertex_move. In the line loop, if the line is on the edge, and
 * the two vertices have no more than 5 triangle attached, it is
 * possible to do an line_move.
 */
unsigned int fill_move_array( shell *s, move *ml){
  unsigned int i, k;

  k=0;
  for( i=0; i<s->num_v; i++){
    if( s->vd[i].oe == yes && 
       s->vd[i].num_t >= 5){
      ml[k].type = vertex_move;
      ml[k].index = i;
      ml[k].angle = get_vertex_angle( s, i);
      k++;
    }
  }
  for( i=0; i<s->num_l; i++){
    if( s->ld[i].oe == yes &&
       s->vd[s->l[i].i[0]].num_t <= 5 &&
       s->vd[s->l[i].i[0]].num_t <= 5){
      ml[k].type = line_move;
      ml[k].index = i;
      ml[k].angle = get_line_angle( s, i);
      k++;
    }
  }
  return k;
}

/*!
 * Randomly choose a move from the move list.
 *
 * In this simulation, each possible move *might* be choosen. The
 * probablity of a move being choosen is related to the floating point
 * number in the move list (currently the angle).
 */
unsigned int choose_move( unsigned int num_k, double sigma, move *ml){
  double min_a, p_max, p_cum, r, lp;
  int k;
  min_a = 2.*TWOPI;
  for( k=0; k<num_k; k++){
    min_a = min( min_a, ml[k].angle);
  }
  p_max = 0.;
  for( k=0; k<num_k; k++){
    lp = 0.5*(ml[k].angle-min_a)*(ml[k].angle-min_a)/(sigma*sigma);
    p_max += exp( -lp);
  }
  r = (p_max*rand())/RAND_MAX;
  p_cum = 0.;
  for( k=0; k<num_k; k++){
    lp = 0.5*(ml[k].angle-min_a)*(ml[k].angle-min_a)/(sigma*sigma);
    p_cum += exp( -lp);
    if( p_cum > r ){
      break;
    }
  }
  return k;
}

/*!
 * Delete a move from the move list.
 *
 * This function deletes the kth move from the move list, and reduces
 * number of moves num_k by one.
 */
static void delete_move( unsigned int k, unsigned int *num_k, move *ml){
  memmove( &(ml[k]), &(ml[k+1]), (*num_k-k-1)*sizeof(move));
  *num_k -= 1;
}

/*!
 * Test if it is possible to perform a shell_close opperation.
 */
static int test_close( shell *s, unsigned int vi){
  unsigned int lil=0, lir=0, vil, vir, num_tl, num_tr;
  vertex_lines_on_edge( s, vi, &lil, &lir);
  vil = s->l[lil].i[0];
  vir = s->l[lir].i[1];
  num_tl = s->vd[vil].num_t;
  num_tr = s->vd[vir].num_t;
  return (num_tr+num_tl <= 6);
}

/*!
 * Test if it is possible to perform a shell_insert opperation.
 */
static int test_insert( shell *s, unsigned int vi){
  unsigned int lil=0, lir=0, vil, vir;
  unsigned int num_tl, num_tr, num_t;
  vertex_lines_on_edge( s, vi, &lil, &lir);
  vil = s->l[lil].i[0];
  vir = s->l[lir].i[1];
  num_tl = s->vd[vil].num_t;
  num_t = s->vd[vi].num_t;
  num_tr = s->vd[vir].num_t;  
  return (num_tl <= 5 && num_tr <= 6 && num_t <= 5);
}

/*!
 * Find the center point of a triangle.
 */
static inline point tri_center( shell *s, unsigned int ti){
  point center;
  unsigned int i;
  for( i=0; i<3; i++){
    center.x[i] = (s->v[s->t[ti].i[0]].x[i] +
                   s->v[s->t[ti].i[1]].x[i] +
                   s->v[s->t[ti].i[2]].x[i])/3.;
  }
  return center;
}

/*!
 * Test if possible to attach a new triangle.
 *
 * This function calculates the distance between the new point and the
 * center of all triangles in the shell. If the distance is ever
 * smaller than root3/4 (which is the distance from the center of an
 * equilateral triangle to a point) we reject that point. We don't
 * have to worry about the triangle per vertex because those
 * unacceptable positions were not ever listed in the move list.
 */
static int test_attach( shell *s, point p){
  unsigned int i;
  point center;
  int status;
  double b;

  status = 0;
  for( i=0; i<s->num_t; i++){
    center = tri_center( s, i);
    b = dist( center.x, p.x);
    if( b < ROOT3/4.){
      status++;
    }
  }

  return (status == 0);
}

/*!
 * This will give the value of an index after a close shell move.
 *
 * Durring a close_shell opperation we merge two vertices, and the
 * vertex indices get renumbered. This funciton checks if the center
 * vertex is renumbered, and returns the new vertex index.
 */
static unsigned int close_index( shell *s, unsigned int vi){
  unsigned int lil, lir, vil, vir, vi_min, vi_out;
  vertex_lines_on_edge( s, vi, &lil, &lir);
  vil = s->l[lil].i[0];
  vir = s->l[lir].i[1];
  vi_min = min( vil, vir);
  vi_out = vi>vi_min?vi-1:vi;
  return vi_out;
}

/*!
 * merge lines when they total distance between the points is less
 * than delta_b.
 */
static int merge_move( shell *s, double delta_b){
  int i, j;
  int vi0, vi1, vi2, vi3;
  double b;

  for( i=0; i<s->num_l-1; i++){
    if( s->ld[i].oe == yes ){
      vi0 = s->l[i].i[0];
      vi1 = s->l[i].i[1];
      for( j=i+1; j<s->num_l; j++){
        if( s->ld[j].oe == yes ){
          vi2 = s->l[j].i[0];
          vi3 = s->l[j].i[1];
          if( vi0 == vi3 && vi1 == vi2 ){
            shell_merge_line( s, i, j);
            vi0 = min(vi0, vi1);
            vi0 = min(vi0, vi2);
            vi0 = min(vi0, vi3);
            return vi0;            
          }
          if( vi0 != vi3 && vi1 != vi2 &&
              s->vd[vi0].num_t + s->vd[vi3].num_t <= 6 &&
              s->vd[vi1].num_t + s->vd[vi2].num_t <= 6 ){
            b = dist( s->v[vi0].x, s->v[vi3].x)
              + dist( s->v[vi1].x, s->v[vi2].x);
            if( b < delta_b ){
              shell_join( s, i, j);
              vi0 = min(vi0, vi1);
              vi0 = min(vi0, vi2);
              vi0 = min(vi0, vi3);
              return vi0;
            }
          }
        }
      }
    }
  }

  return -1;
}

/*!
 * Perform 1 grow move.
 *
 * This function performs a single grow move (attach, insert, close)
 * to the shell. The return status is the index of the vertex involved
 * in the move. If no acceptable moves are found then the return value
 * is negative.
 */
int grow( shell_run *sr ){
  unsigned int k, num_k;
  point pnew;
  int status = -1;
  shell *s = sr->s;
  shell_params *sp = sr->sp;
  move *ml = sr->ml;
  double sigma = sr->sp->sigma;
  double a_cutoff = PI_6;
  double delta_b = sr->sp->delta_b;

  status = merge_move( s, delta_b);
  if( status != -1){
    return status;
  }

  num_k = fill_move_array( s, ml);
  while( num_k > 0 ){
    k = choose_move( num_k, sigma, ml);
    if( ml[k].type == vertex_move){
      if( ml[k].angle < a_cutoff ){
        if( test_close( s, ml[k].index)){
          status = close_index( s, ml[k].index);
          shell_close( s, ml[k].index);
          break;
        }
      }
      if( test_insert( s, ml[k].index)){
        status = ml[k].index;
        shell_insert( s, ml[k].index);
        break;
      }
    }else{
      pnew = new_point( s, sp, ml[k].index);
      if( test_attach( s, pnew)){
        status = s->num_v;
        shell_attach( s, pnew, ml[k].index);
        break;
      }
    }
    delete_move( k, &num_k, ml);
  }
  return status;
}