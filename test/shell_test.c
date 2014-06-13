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

  printf( "add_line: ");
  s = shell_malloc(100);
  shell_copy( s, &ss1);
  add_line( s, 0, 1, 0);
  status = 0;
  if( s->num_l != 4){
    status++;
  }
  if( s->l[3].i[0] != 0 || s->l[3].i[1] != 1){
    status++;
  }
  if( s->ld[3].oe != yes || s->ld[3].t[0] != 0){
    status++;
  }
  if( s->vd[0].num_l != 3){
    status++;
  }
  if( s->vd[0].l[2] != 3 ){
    status++;
  }
  if( s->vd[1].num_l != 3){
    status++;
  }
  if( s->vd[1].l[2] != 3){
    status++;
  }

  if( status == 0){
    printf( "pass\n");
  }else{
    printf( "fail\n");
  }
  shell_free(s);
}

/*!
 * `remove_line` test.
 */
void remove_line_test(){
  int status;
  shell *s;

  printf( "remove_line: ");
  s = shell_malloc( 20);
  shell_copy( s, &ss2);
  remove_line( s, 2);
  status = 0;
  if( s->num_l != 4){
    status++;
  }
  if( s->l[0].i[0] != ss2.l[0].i[0] || 
     s->l[0].i[1] != ss2.l[0].i[1] ){
    status++;
  }
  if( s->l[1].i[0] != ss2.l[1].i[0] ||
     s->l[1].i[1] != ss2.l[1].i[1]){
    status++;
  }
  if( s->l[2].i[0] != ss2.l[3].i[0] ||
     s->l[2].i[1] != ss2.l[3].i[1]){
    status++;
  }
  if( s->l[3].i[0] != ss2.l[4].i[0] ||
     s->l[3].i[1] != ss2.l[4].i[1]){
    status++;
  }
  if( s->vd[0].num_l != 2){
    status++;
  }
  if( s->vd[2].num_l != 1){
    status++;
  }

  if( status == 0){
    printf( "pass\n");
  }else{
    printf( "fail\n");
  }
  shell_free(s);
}

/*!
 * `add_triangle` test.
 */
void add_triangle_test(){
  int status;
  shell *s;

  printf("add_triangle: ");
  s = shell_malloc( 20);
  shell_copy(s, &ss1);
  add_triangle(s, 0, 1, 2, 0, 1, 2);
  status = 0;
  if( s->num_t != 2){
    status++;
  }
  if( s->vd[0].num_t != 2 ||
     s->vd[1].num_t != 2 || 
     s->vd[2].num_t != 2 ){
    status++;
  }

  if( status == 0){
    printf( "pass\n");
  }else{
    printf( "fail\n");
  }
  shell_free(s);
}

/*!
 * `midpoint` test.
 */
void midpoint_test(){
  int status;
  point pout;
  point p0 = {{-1., 0., 3.}};
  point p1 = {{1., 2., 4.}};
  point ptarget = {{0., 1., 3.5}};
  double tol = 1.e-4;

  printf( "midpoint: ");
  pout = midpoint( p0, p1);
  status = 0;
  if( dist_sqr( pout, ptarget) > tol ){
    status++;
  }

  if( status == 0){
    printf( "pass\n");
  }else{
    printf( "fail\n");
  }
}

/*!
 * `merge_vertex` test.
 */
void merge_vertex_test(){
  int status;
  shell *s;
  point pnew = {{1.25, ROOT3/4., 0.}};
  double tol = 1.e-4;

  s = shell_malloc( 30);
  shell_copy(s, &ss5);
  printf("merge_vertex: ");
  merge_vertex( s, 2, 6);
  status = 0;
  if( s->num_v != 6 ){
    status++;
  }
  if( dist_sqr( pnew, s->v[2]) > tol ){
    status++;
  }
  if( s->vd[2].num_l != 4 ){
    status++;
  }
  if( s->vd[2].l[0] != ss5.vd[2].l[0] ||
     s->vd[2].l[1] != ss5.vd[2].l[1] ||
     s->vd[2].l[2] != ss5.vd[6].l[0] ||
     s->vd[2].l[3] != ss5.vd[6].l[1] ){
    status++;
  }
  if( s->l[10].i[0] != 2 || 
     s->l[9].i[1] != 2 ){
    status++;
  }
  if( s->vd[2].num_t != 2 ){
    status++;
  }
  if( s->vd[2].t[0] != ss5.vd[2].t[0] ||
     s->vd[2].t[1] != ss5.vd[6].t[0] ){
    status++;
  }
  if( s->t[4].i[1] != 2 ){
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
 * `triangle_on_edge` test.
 */
void triangle_on_edge_test(){
  int status;
  shell *s;

  printf( "triangle_on_edge: ");
  s = shell_malloc( 20);
  shell_copy( s, &ss1);
  status = 0;
  if( triangle_on_edge( s, 0) != yes){
    status++;
  }
  s->ld[0].oe = no;
  if( triangle_on_edge( s, 0) != yes){
    status++;
  }
  s->ld[1].oe = no;
  if( triangle_on_edge( s, 0) != yes){
    status++;
  }
  s->ld[2].oe = no;
  if( triangle_on_edge( s, 0) != no){
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
 * `vertex_on_edge` test.
 */
void vertex_on_edge_test(){
  int status;
  shell *s;

  printf( "vertex_on_edge: ");
  s = shell_malloc( 20);
  shell_copy( s, &ss6);
  status = 0;
  if( vertex_on_edge(s, 0) != yes ){
    status++;
  }
  if( vertex_on_edge(s, 1) != no ){
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
 * `merge_line` test.
 */
void merge_line_test(){
  int i, status, count;
  shell *s;

  s = shell_malloc( 30);
  shell_copy(s, &ss5);
  printf("merge_line: ");
  merge_vertex( s, 2, 6);
  merge_line( s, 10, 1);
  status = 0;
  if( s->vd[1].num_l != 5 ){
    status++;
  }
  count = 0;
  for( i=0; i<s->vd[1].num_l; i++){
    if( s->vd[1].l[i] == 10 ){
      count++;
    }
  }
  if( count != 0 ){
    status++;
  }
  if( s->vd[2].num_l != 3 ){
    status++;
  }
  count = 0;
  for( i=0; i<s->vd[3].num_l; i++){
    if( s->vd[3].l[i] == 10 ){
      count++;
    }
  }
  if( count != 0 ){
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
 * `shell_initialize` test.
 */
 void shell_initialize_test(){
  int status;
  shell *s;

  printf( "shell_initialize: ");
  s = shell_malloc( 10);
  shell_initialize( s);
  status = 0;
  if( !shell_comp( s, &ss1)){
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
 * `shell_attach` test.
 */
void shell_attach_test(){
  int status;
  shell *s;

  printf( "shell_attach: ");
  s = shell_malloc( 20);
  shell_copy( s, &ss1);
  shell_attach( s, pp3, 0);
  status = 0;
  if( !shell_comp( s, &ss2)){
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
 * `vertex_lines_on_edge` test.
 */
void vertex_lines_on_edge_test(){
  int status;
  unsigned int lil, lir;
  shell *s;

  printf( "vertex_lines_on_edge: ");
  s = shell_malloc( 100);
  shell_copy( s, &ss4);
  status = 0;
  vertex_lines_on_edge( s, 0, &lil, &lir);
  if( lil != 2 || lir != 3 ){
    status++;
  }
    vertex_lines_on_edge( s, 1, &lil, &lir);
  if( lil != 8 || lir != 1 ){
    status++;
  }
  vertex_lines_on_edge( s, 2, &lil, &lir);
  if( lil != 1 || lir != 2 ){
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
 * `shell_insert` test.
 */
void shell_insert_test(){
  int status;
  shell *s;

  printf( "shell_insert: ");
  s = shell_malloc( 100);
  shell_copy( s, &ss5);
  shell_insert( s, 1);
  status = 0;
  if( !shell_comp( s, &ss6)){
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
 * `shell_close` test.
 */
void shell_close_test(){
  int status;
  shell *s1, *s2;

  printf( "shell_close: ");
  s1 = shell_malloc( 30);
  shell_copy(s1, &ss5);
  merge_vertex( s1, 2, 6);
  merge_line( s1, 10, 1);
  s2 = shell_malloc( 100);
  shell_copy( s2, &ss5);
  shell_close( s2, 1);
  status = 0;
  if( !shell_comp( s1, s2)){
    status++;
  }

  if( status == 0){
    printf( "pass\n");
  }else{
    printf( "fail\n");
  }
  shell_free( s1);
  shell_free( s2);    
}

/*!
 * `shell_join` test.
 */
void shell_join_test(){
  int status;
  shell *s;

  printf( "shell_join: ");
  s = shell_malloc( 30);
  shell_copy( s, &ss1);
  add_vertex( s, pp3);
  add_vertex( s, pp1);
  add_vertex( s, pp0);
  add_line( s, 5, 3, 1);
  add_line( s, 3, 4, 1);
  add_line( s, 4, 5, 1);
  add_triangle( s, 5, 3, 4, 3, 4, 5);
  shell_join( s, 0, 5);
  status = 0;
  if( !shell_comp( s, &ss2)){
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
 *
 */
void align_line_test(){
  int status;
  shell *s;
  unsigned int v0, vv0, v1, vv1;

  printf( "align_line: ");
  s = shell_malloc( 100);
  shell_copy( s, &ss6);
  status = 0;
  v0 = s->l[10].i[0];
  v1 = s->l[10].i[1];
  vv0 = v0;
  vv1 = v1;
  align_line( s, 6, &vv0, &vv1);
  if( vv0 != v0 || vv1 != v1 ){
    status++;
  }
  v0 = s->l[1].i[0];
  v1 = s->l[1].i[1];
  vv0 = v0;
  vv1 = v1;
  align_line( s, 6, &vv0, &vv1);
  if( vv0 != v0 || vv1 != v1 ){
    status++;
  }
  v0 = s->l[1].i[0];
  v1 = s->l[1].i[1];
  vv0 = v0;
  vv1 = v1;
  align_line( s, 0, &vv0, &vv1);
  if( vv0 != v1 || vv1 != v0 ){
    status++;
  }
  v0 = s->l[0].i[0];
  v1 = s->l[0].i[1];
  vv0 = v0;
  vv1 = v1;
  align_line( s, 0, &vv0, &vv1);
  if( vv0 != v1 || vv1 != v0 ){
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
 * `shell_remove` test.
 */
void shell_remove_test(){ 
  int status;
  shell *s;

  printf( "shell_remove: ");
  s = shell_malloc( 100);
  status = 0;
  shell_copy( s, &ss2);
  shell_remove( s, 1);
  if( !shell_comp( s, &ss1)){
    status++;
  }
  shell_copy( s, &ss6);
  shell_remove( s, 5);
  if( !shell_comp( s, &ss5)){
    status++;
  }
  shell_copy( s, &ss6);
  shell_remove( s, 0);
  if( s->l[0].i[0] != 1 && s->l[0].i[1] != 0 ){
    status++;
  }
  if( s->l[1].i[0] != 2 && s->l[1].i[1] != 1 ){
    status++;
  }

  if( status == 0){
    printf( "pass\n");
  }else{
    printf( "fail\n");
  }
  shell_free( s);
}

int main(){
  shell_malloc_test();
  shell_copy_test();
  add_vertex_test();
  remove_vertex_test();
  add_line_test();
  remove_line_test();
  add_triangle_test();
  midpoint_test();
  merge_vertex_test();
  triangle_on_edge_test();
  vertex_on_edge_test();
  merge_line_test();
  shell_initialize_test();
  shell_attach_test();
  vertex_lines_on_edge_test();
  shell_insert_test();
  shell_close_test();
  shell_join_test();
  align_line_test();
  shell_remove_test();
  return 0;
}