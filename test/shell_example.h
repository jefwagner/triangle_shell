/*!*******************************************************************
 * \author Jef Wagner
 * \email jefwagner@gmail.com
 *********************************************************************
 */
/*!
 * These are a few example shells to work with the test. The shells
 * defined with variables `ss1` through `ss6` contain 1 through 6
 * triangles respectively. Everything is declared here using c style
 * initializer list.
 */

#include "../src/shell.h"
#include "../src/math_const.h"

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
 * The points used in the test shells.
 */
point pp0 = {{0.,0.,0.}};
point pp1 = {{0.5, ROOT3_2, 0.}};
point pp2 = {{1.,0.,0.}};
point pp3 = {{-0.5, ROOT3_2, 0.}};
point pp4 = {{0., 2*ROOT3_2, 0.}};
point pp5 = {{1., 2*ROOT3_2, 0.}};
point pp6 = {{1.5, ROOT3_2, 0.}};

/*!
 * The list of points used for all test shells.
 */
point pplist[] = {{{0.,0.,0.}},
                  {{0.5, ROOT3_2, 0.}},
                  {{1.,0.,0.}},
                  {{-0.5, ROOT3_2, 0.}},
                  {{0., 2*ROOT3_2, 0.}},
                  {{1., 2*ROOT3_2, 0.}},
                  {{1.5, ROOT3_2, 0.}}};
/*!
 * The list of lines used for all shells.
 */
line lllist[] = {{{0,1}},{{1,2}},{{2,0}},{{0,3}},{{3,1}},
                 {{3,4}},{{4,1}},{{4,5}},{{5,1}},{{5,6}},
                 {{6,1}},{{6,2}}};
/*!
 * The list of triangles used for all shells.
 */
triangle ttlist[] = {{{0,1,2}},{{0,3,1}},{{3,4,1}},
                     {{4,5,1}},{{5,6,1}},{{6,2,1}}};


/*!
 * The vertex data, line data, and triangle data arrays for the
 * shells.
 */
vertex_data vd1list[] = {{yes, 2, {0,2},1,{0}},
                         {yes, 2, {0,1},1,{0}},
                         {yes, 2, {1,2},1,{0}}};
line_data ld1list[] = {{yes,{0}},{yes,{0}},{yes,{0}}};
triangle_data td1list[] = {{yes,{0,1,2}}};

vertex_data vd2list[] = {{yes, 3, {0,2,3},2,{0,1}},
                         {yes, 3, {0,1,4},2,{0,1}},
                         {yes, 2, {1,2},1,{0}},
                         {yes, 2, {3,4},1,{1}}};
line_data ld2list[] = {{no,{0,1}},{yes,{0}},{yes,{0}},
                       {yes,{1}},{yes,{1}}};
triangle_data td2list[] = {{yes,{0,1,2}},{yes,{3,4,0}}};

vertex_data vd3list[] = {{yes, 3, {0,2,3},2,{0,1}},
                         {yes, 4, {0,1,4,6},3,{0,1,2}},
                         {yes, 2, {1,2},1,{0}},
                         {yes, 3, {3,4,5},2,{1,2}},
                         {yes, 2, {5,6},1,{2}}};
line_data ld3list[] = {{no,{0,1}},{yes,{0}},{yes,{0}},{yes,{1}},
                       {no,{1,2}},{yes,{2}},{yes,{2}}};
triangle_data td3list[] = {{yes,{0,1,2}},{yes,{3,4,0}},{yes,{5,6,4}}};

vertex_data vd4list[] = {{yes, 3, {0,2,3},2,{0,1}},
                         {yes, 5, {0,1,4,6,8},4,{0,1,2,3}},
                         {yes, 2, {1,2},1,{0}},
                         {yes, 3, {3,4,5},2,{1,2}},
                         {yes, 3, {5,6,7},2,{2,3}},
                         {yes, 2, {7,8},1,{3}}};
line_data ld4list[] = {{no,{0,1}},{yes,{0}},{yes,{0}},{yes,{1}},
                       {no,{1,2}},{yes,{2}},{no,{2,3}},{yes,{3}},
                       {yes,{3}}};
triangle_data td4list[] = {{yes,{0,1,2}},{yes,{3,4,0}},
                           {yes,{5,6,4}},{yes,{7,8,6}}};

vertex_data vd5list[] = {{yes, 3, {0,2,3},2,{0,1}},
                         {yes, 6, {0,1,4,6,8,10},5,{0,1,2,3,4}},
                         {yes, 2, {1,2},1,{0}},
                         {yes, 3, {3,4,5},2,{1,2}},
                         {yes, 3, {5,6,7},2,{2,3}},
                         {yes, 3, {7,8,9},2,{3,4}},
                         {yes, 2, {9,10},1,{4}}};
line_data ld5list[] = {{no,{0,1}},{yes,{0}},{yes,{0}},{yes,{1}},
                       {no,{1,2}},{yes,{2}},{no,{2,3}},{yes,{3}},
                       {no,{3,4}},{yes,{4}},{yes,{4}}};
triangle_data td5list[] = {{yes,{0,1,2}},{yes,{3,4,0}},
                           {yes,{5,6,4}},{yes,{7,8,6}},
                           {yes,{9,10,8}}};

vertex_data vd6list[] = {{yes, 3, {0,2,3},2,{0,1}},
                         {no, 6, {0,1,4,6,8,10}, 6,{0,1,2,3,4,5}},
                         {yes, 3, {1,2,11},2,{0,5}},
                         {yes, 3, {3,4,5},2,{1,2}},
                         {yes, 3, {5,6,7},2,{2,3}},
                         {yes, 3, {7,8,9},2,{3,4}},
                         {yes, 3, {9,10,11},2,{4,5}}};
line_data ld6list[] = {{no,{0,1}},{no,{0,5}},{yes,{0}},{yes,{1}},
                       {no,{1,2}},{yes,{2}},{no,{2,3}},{yes,{3}},
                       {no,{3,4}},{yes,{4}},{no,{4,5}},{yes,{5}}};
triangle_data td6list[] = {{yes,{0,1,2}},{yes,{3,4,0}},
                           {yes,{5,6,4}},{yes,{7,8,6}},
                           {yes,{9,10,8}},{yes,{11,1,10}}};

/*!
 * The shells.
 */
shell ss1 = {3, &pplist[0], &vd1list[0],
             3, &lllist[0], &ld1list[0],
             1, &ttlist[0], &td1list[0]};


shell ss2 = {4, &pplist[0], &vd2list[0],
             5, &lllist[0], &ld2list[0],
             2, &ttlist[0], &td2list[0]};

shell ss3 = {5, &pplist[0], &vd3list[0],
             7, &lllist[0], &ld3list[0],
             3, &ttlist[0], &td3list[0]};

shell ss4 = {6, &pplist[0], &vd4list[0],
             9, &lllist[0], &ld4list[0],
             4, &ttlist[0], &td4list[0]};

shell ss5 = {7, &pplist[0], &vd5list[0],
             11, &lllist[0], &ld5list[0],
             5, &ttlist[0], &td5list[0]};

shell ss6 = {7, &pplist[0], &vd6list[0],
             12, &lllist[0], &ld6list[0],
             6, &ttlist[0], &td6list[0]};
