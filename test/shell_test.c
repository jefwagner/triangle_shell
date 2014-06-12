/*!*******************************************************************
 * \author Jef Wagner
 * \email jefwagner@gmail.com
 *********************************************************************
 */
/*!
 * Unit test for the functions in shell.c.
 */

#include <stdio.h>
#include <stdlib.h>

#include "../src/shell.c"
#include "shell_example.h"


/*!
 * Square of the distance between two points.
 */
double dist_sqr( point p1, point p2){
  return ((p1.x[0]-p2.x[0])*(p1.x[0]-p2.x[0])+
          (p1.x[1]-p2.x[1])*(p1.x[1]-p2.x[1])+
          (p1.x[2]-p2.x[2])*(p1.x[2]-p2.x[2]));
}

/*!
 * Shell comparison function.
 *
 * This function compares two shells to see if they are equal or not.
 * The function returns zero if they are different, 1 if they are the
 * same.
 */
int shell_comp( shell *s1, shell *s2){
  unsigned int i, j;
  double tol = 1.e-4;

  /* Check if size is same. */
  if(s1->num_v != s2->num_v ||
     s1->num_l != s2->num_l ||
     s1->num_t != s2->num_t ){
    return 0;
  }else{
    for( i=0; i<s1->num_v; i++){
      /* Check if the points are the same. */
      if( dist_sqr( s1->v[i], s2->v[i]) > tol ||
      /* Check if size of the vertex data is the same. */
          s1->vd[i].oe != s2->vd[i].oe ||
          s1->vd[i].num_l != s2->vd[i].num_l ||
          s1->vd[i].num_t != s2->vd[i].num_t ){
        return 0;
      }
      /* Check if vertex data is the same. */
      for( j=0; j<s1->vd[i].num_l; j++){
        if( s1->vd[i].l[j] != s2->vd[i].l[j] ){
          return 0;
        }
      }
      for( j=0; j<s1->vd[i].num_t; j++){
        if( s1->vd[i].t[j] != s2->vd[i].t[j] ){
          return 0;
        }
      }
    }
    for( i=0; i<s1->num_l; i++){
      /* Check if the lines are the same */
      if( s1->l[i].i[0] != s2->l[i].i[0] || 
         s1->l[i].i[1] != s2->l[i].i[1] ||
         s1->ld[i].oe != s2->ld[i].oe ){
        return 0;
      }
      /* Check if line data is the same */
      if( s1->ld[i].oe == yes ){
        if( s1->ld[i].t[0] != s2->ld[i].t[0] ){
          return 0;
        }
      }else{
        if( s1->ld[i].t[0] != s2->ld[i].t[0] ||
           s1->ld[i].t[1] != s2->ld[i].t[1] ){
          return 0;
        }
      }
    }
    for( i=0; i<s1->num_t; i++){
      /* Check if the triangles are the same */
      if( s1->t[i].i[0] != s2->t[i].i[0] ||
         s1->t[i].i[1] != s2->t[i].i[1] ||
         s1->t[i].i[2] != s2->t[i].i[2] ||
         s1->td[i].oe != s2->td[i].oe ){
        return 0;
      }
      /* Check if the triangle data is the same */
      if( s1->td[i].l[0] != s2->td[i].l[0] ||
         s1->td[i].l[1] != s2->td[i].l[1] ||
         s1->td[i].l[2] != s2->td[i].l[2] ){
        return 0;
      }
    }
  }
  return 1;
}

/*!
 * `shell_malloc` test.
 */
void shell_malloc_test(){
  printf( "shell_malloc: ");
  shell *s = shell_malloc( 1000);
  if( s == NULL ){
    printf( "fail\n");
  }else{
    printf( "pass\n");
  }
  free( s);
}

/*!
 * `shell_copy` test.
 */
void shell_copy_test(){
  shell *s;

  printf( "shell_copy: ");
  s = shell_malloc( 1000);
  shell_copy( s, &ss1);
  if( shell_comp( s, &ss1) ){
    printf( "pass\n");
  }else{
    printf( "fail\n");
  }
  shell_free( s);
}

/*!
 * `add_vertex` test.
 */
void add_vertex_test(){
  int status;
  shell *s;
  double tol = 1.e-4;

  printf( "add_vertex: ");
  s = shell_malloc( 100);
  shell_copy( s, &ss1);
  add_vertex( s, pp3);
  status = 0.;
  if( s->num_v != 4){
    status++;
  }
  if( dist_sqr( s->v[3], pp3) > tol){
    status++;
  }
  if( status == 0){
    printf( "pass\n");
  }else{
    printf( "fail\n");
  }
  shell_free( s);
}

/*!
 * `remove_vertex` test.
 */
void remove_vertex_test(){
  int status;
  shell *s;
  double tol = 1.e-4;

  printf( "remove_vertex: ");
  s = shell_malloc( 100);
  shell_copy( s, &ss2);
  remove_vertex( s, 0);
  status = 0.;
  if( s->num_v != 3){
    status++;
  }
  if( dist_sqr( s->v[0], pp1) > tol){
    status++;
  }
  if( dist_sqr( s->v[1], pp2) > tol){
    status++;
  }
  if( dist_sqr( s->v[2], pp3) > tol){
    status++;
  }
  if( s->l[0].i[1] != 0 ){
    status++;
  }
  if( s->l[1].i[0] != 0 || s->l[1].i[1] != 1 ){
    status++;
  }

  if( status == 0){
    printf( "pass\n");
  }else{
    printf( "fail\n");
  }
  shell_free( s);
}

/*!
 * `add_line` test.
 */
void add_line_test(){
  int status;
  shell *s;

  printf( "add_line:");
  s = shell_malloc(100);
  shell_free(s);
}

int main(){
  shell_malloc_test();
  shell_copy_test();
  add_vertex_test();
  remove_vertex_test();
  return 0;
}