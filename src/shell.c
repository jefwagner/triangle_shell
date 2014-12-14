/*!*******************************************************************
 * \author Jef Wagner
 * \email jefwagner@gmail.com
 *********************************************************************
 */
/*!
 * This file contains the functions we need to work with the shell
 * object. The exported function are:
 * - `shell_malloc`, a constructor,
 * - `shell_copy`, a copy method,
 * - `shell_free`, a destructor,
 * - `shell_initialize`, initialize the shell with 1 triangle,
 * - `shell_attach`, add a triangle to the shell,
 * - `shell_insert`, insert a triangle into the shell,
 * - `shell_close`, close two neighboring edges in a shell,
 * - `shell_join`, join two non-neighboring edges in a shell,
 * - `shell_remove`, remove a triangle from the shell. 
 *
 * Local to the file (so declared `static`) are the bookkeeping
 * functions:
 * - `add_vertex`, add a vertex to the shell,
 * - `remove_vertex`, remove a vertex from the shell,
 * - `add_line`, add a line to the shell,
 * - `remove_line`, remove a line from the shell,
 * - `midpoint`, find the midpoint between two points,
 * - `merge_vertex`, merge two vertices,
 * - `triangle_on_edge`, find if a triangle is on the edge,
 * - `vertex_on_edge`, find if a vertex is on the edge,
 * - `merge_line`, merge two lines,
 * - `align_line`, possibly reorder the points in a line.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "shell.h"
#include "math_const.h"

/*!
 * A constructor for the shell object.
 */
shell* shell_malloc( unsigned int max_t ){
  unsigned int max_v, max_l;

  shell *s = (shell *) malloc( sizeof(shell));
  if( s== NULL ){ return NULL; }
  max_v = max_t + 2;
  max_l = 2*max_t + 1;
  s->vg = (point *) malloc( (max_v+1)*sizeof(point));
  if( s->vg == NULL ){ 
    free( s); 
    return NULL; 
  }
  s->v = &(s->vg[1]);
  s->vd = (vertex_data *) malloc( max_v*sizeof(vertex_data));
  if( s->vd == NULL ){ 
    free( s->vg); 
    free( s);
    return NULL; 
  }
  s->l = (line *) malloc( max_l*sizeof(line));
  if( s->l == NULL ){ 
    free( s->vd);
    free( s->vg); 
    free( s);
    return NULL; }
  s->ld = (line_data *) malloc( max_l*sizeof(line_data));
  if( s->ld == NULL ){ 
    free( s->l);
    free( s->vd);
    free( s->vg); 
    free( s);
    return NULL; 
  }
  s->t = (triangle *) malloc( max_t*sizeof(triangle));
  if( s->t == NULL ){ 
    free( s->ld);
    free( s->l);
    free( s->vd);
    free( s->vg); 
    free( s);
    return NULL; 
  }
  s->td = (triangle_data *) malloc( max_t*sizeof(triangle_data));
  if( s->td == NULL ){ 
    free( s->t);
    free( s->ld);
    free( s->l);
    free( s->vd);
    free( s->vg); 
    free( s);
    return NULL; 
  }

  return s;
}

/*!
 * A copy (clone) method for the shell object.
 */
void shell_copy( shell *dest, const shell *source){
  dest->num_v = source->num_v;
  memcpy( dest->vg, source->vg, (source->num_v+1)*sizeof(point));
  memcpy( dest->vd, source->vd, source->num_v*sizeof(vertex_data));
  dest->num_l = source->num_l;
  memcpy( dest->l, source->l, source->num_l*sizeof(line));
  memcpy( dest->ld, source->ld, source->num_l*sizeof(line_data));
  dest->num_t = source->num_t;
  memcpy( dest->t, source->t, source->num_t*sizeof(triangle));
  memcpy( dest->td, source->td, source->num_t*sizeof(triangle_data));
}

/*!
 * A destructor for the shell object.
 */
void shell_free( shell *s){
  free( s->td);
  free( s->t);
  free( s->ld);
  free( s->l);
  free( s->vd);
  free( s->vg);
  free( s);
}

/*!
 * Add a vertex.
 *
 * This function adds a vertex to the shell object. It takes as
 * arguments the point `p` for the new vertex, and modifies the shell
 * pointed to by the pointer `s`. The new vertex is assumed to have no
 * triangle or lines attached when it is added.
 */
 static void add_vertex( shell *s, point p){
  unsigned int n = s->num_v;
  point *v = s->v;
  vertex_data *vd = s->vd;

  /* Add the point. */
  v[n] = p;
  /* Initialize the vertex data. */
  vd[n].oe = yes;
  vd[n].num_l = 0;
  vd[n].num_t = 0;
  /* Increment the vertex count. */
  s->num_v++;
}

/*!
 * Remove a vertex.
 *
 * This function removes a vertex from the shell. It assumes that
 * there are no more references to that vertex in the line or triangle
 * list. It uses `memmove` to shift the array forward one spot at
 * position `vi`. It then goes through the triangle and line list and
 * shift indices greater than `vi` down by 1.
 */
 static void remove_vertex( shell *s, unsigned int vi){
  unsigned int i, j;
  unsigned int num_l = s->num_l;
  line *l = s->l;
  unsigned int num_t = s->num_t;
  triangle *t = s->t;
  point *v = s->v;
  vertex_data *vd = s->vd;
  /* The number of points that need to be shifted */
  unsigned int size = s->num_v - vi - 1;

  /* Shift up by one */
  memmove( &(v[vi]), &(v[vi+1]), size*sizeof(point)); 
  memmove( &(vd[vi]), &(vd[vi+1]), size*sizeof(vertex_data)); 
  /* Decrement the vertex count */
  s->num_v--;

  /* For vertex numbers larger than vi, subtract one */
  for( i=0; i<num_l; i++){
    for( j=0; j<2; j++){
      if( l[i].i[j] > vi){
        l[i].i[j]--;
      }
    }
  }
  for( i=0; i<num_t; i++){
    for( j=0; j<3; j++){
      if( t[i].i[j] > vi){
        t[i].i[j]--;
      }
    }
  }
}

/*!
 * Add a line.
 *
 * This function adds a line to the shell. It takes as arguments the
 * indices of the vertices `i0` and `i1` and the index of a triangle
 * `t0`. It assumes that the new line is on the edge, and is only
 * attached to one triangle when it is created.
 */
static void add_line( shell *s, unsigned int i0, unsigned int i1,
                    unsigned int t0){
  unsigned int vdn;
  unsigned int n = s->num_l;
  line *l = s->l;
  line_data *ld = s->ld;
  vertex_data *vd = s->vd;

  /* Add the vertex indices to the line */
  l[n].i[0] = i0;
  l[n].i[1] = i1;
  /* add the triangle index to the line data */
  ld[n].oe = yes;
  ld[n].t[0] = t0;
  /* Increment the line count. */
  s->num_l++;
  /* add the line reference to the vertex data */ 
  vdn = vd[i0].num_l;
  vd[i0].l[vdn] = n;
  vd[i0].num_l++;
  vdn = vd[i1].num_l;
  vd[i1].l[vdn] = n;
  vd[i1].num_l++;
}

static void remove_line( shell *s, unsigned int li){
  unsigned int i, j, vi, size, decr;
  vertex_data *vd = s->vd;
  line *l = s->l;
  line_data *ld = s->ld;
  triangle_data *td = s->td;

  for( j=0; j<2; j++){  
  /* Get the vertex indices that contain the line. */
    vi = l[li].i[j];
  /* Remove the reference to the line from the vertex data. */
    decr = 0;
    for( i=0; i<vd[vi].num_l; i++){
      if( vd[vi].l[i] == li){
        size = vd[vi].num_l - i -1;
        memmove( &(vd[vi].l[i]), &(vd[vi].l[i+1]), 
                size*sizeof(unsigned int));
        decr++;
      }
    }
    vd[vi].num_l -= decr;
  }

  /* Decrement all references to lines larger than `li`. */
  for( i=0; i<s->num_v; i++){
    for( j=0; j<vd[i].num_l; j++){
      if( vd[i].l[j] > li){
        vd[i].l[j]--;
      }
    }
  }
  for( i=0; i<s->num_t; i++){
    for( j=0; j<3; j++){
      if( td[i].l[j] > li ){
        td[i].l[j]--;
      } 
    }
  }

  /* Shift the rest of the lines down by 1. */
  size = s->num_l - li - 1;
  memmove( &(l[li]), &(l[li+1]), size*sizeof(line));
  memmove( &(ld[li]), &(ld[li+1]), size*sizeof(line_data));
  /* Decrement the line count. */
  s->num_l--;
}

/*!
 * Add a triangle.
 *
 * This function adds a triangle to the shell. It takes as arguments
 * the the indices to the vertices `i0`, `i1`, and `i2`, and the line
 * indices `l0`, `l1`, and `l2`. It assumes that the newly created
 * triangle is on the edge.
 */
static void add_triangle( shell *s, unsigned int i0,
                         unsigned int i1, unsigned int i2,
                         unsigned int l0, unsigned int l1,
                         unsigned int l2){
  unsigned int vdn;
  unsigned int n = s->num_t;
  triangle *t = s->t;
  triangle_data *td = s->td;
  vertex_data *vd = s->vd;

  /* Set the new triangle. */
  t[n].i[0] = i0;
  t[n].i[1] = i1;
  t[n].i[2] = i2;
  /* Set the new triangle data. */
  td[n].oe = yes;
  td[n].l[0] = l0;
  td[n].l[1] = l1;
  td[n].l[2] = l2;
  /* Increment the triangle count. */
  s->num_t++;

  /* Add the triangle reference vertex data. */
  vdn = vd[i0].num_t;
  vd[i0].t[vdn] = n;
  vd[i0].num_t++;
  vdn = vd[i1].num_t;
  vd[i1].t[vdn] = n;
  vd[i1].num_t++;
  vdn = vd[i2].num_t;
  vd[i2].t[vdn] = n;
  vd[i2].num_t++;
}

/*!
 * Midpoint between two points.
 */
static point midpoint( point p1, point p2){
  unsigned int i;
  point p;
  for( i=0; i<3; i++){
    p.x[i] = 0.5*(p1.x[i]+p2.x[i]);
  }
  return p;
}

/*!
 * Merge two vertices.
 *
 * This function merges two vertices in the shell. It takes as
 * arguments the indices of the vertices to be merged `vi0` and `vi1`.
 * The function takes the following steps:
 * - changes references in line and triangle arrays,
 * - copy's triangle and line data from the vertex data array.
 * - move the new vertex to the midpoint of the two old vertices.
 * - remove old vertices.
 */
static void merge_vertex( shell *s, unsigned int vi0,
                         unsigned int vi1){
  unsigned int i, j, vim, vip, li, ti, n;
  point *v = s->v;
  vertex_data *vd = s->vd;
  line *l = s->l;
  triangle *t = s->t;
 
  /* Order the vertices such that we always keep the lesser. */
  if( vi0 < vi1){
    vim = vi0;
    vip = vi1;
  }else{
    vim = vi1;
    vip = vi0;
  }
 
  /* Change reference to vip in l and t to vim. */
  /* First loop over the li-s in vd[vip].l. */
  for( i=0; i<vd[vip].num_l; i++){
    li = vd[vip].l[i];
    /* Check for the two values for vip, switch if neccesary. */
    for( j=0; j<2; j++){
      if( l[li].i[j] == vip ){
        l[li].i[j] = vim;
      }
    }
  }
  /* Same for ti-s in vd[vip].t. */
  for( i=0; i<vd[vip].num_t; i++){
    ti = vd[vip].t[i];
    for( j=0; j<3; j++){
      if( t[ti].i[j] == vip ){
        t[ti].i[j] = vim;
      }
    }
  }

  /* Copy the all lines and triangles from vd[vip] to vd[vim]. */
  n = vd[vim].num_l;
  memmove( &(vd[vim].l[n]), &(vd[vip].l[0]), 
          vd[vip].num_l*sizeof(unsigned int));
  vd[vim].num_l += vd[vip].num_l;
  n = vd[vim].num_t;
  memmove( &(vd[vim].t[n]), &(vd[vip].t[0]), 
          vd[vip].num_t*sizeof(unsigned int));
  vd[vim].num_t += vd[vip].num_t;

  /* Replace v[vim] with the midpoint of the two vertices. */
  v[vim] = midpoint( v[vim], v[vip]);

  /* Remove vertex vip. static */
  remove_vertex( s, vip);
}

/*!
 * Check if triangle is on the edge.
 * 
 * This function checks if a triangle is on the edge. It takes as
 * arguments the index of the triangle `ti`. It returns either `yes`
 * or `no`. The function works by looping over the lines of the
 * triangle and checking if they are on the edge.
 */
static on_edge triangle_on_edge( const shell *s, unsigned int ti){
  unsigned int j, li;
  triangle_data *td = s->td;
  line_data *ld = s->ld;
  on_edge oe = no;
  for( j=0; j<3; j++){
    li = td[ti].l[j];
    if( ld[li].oe == yes ){
      oe = yes;
      break;
    } 
  }
  return oe;
}

/*!
 * Check if vertex is on the edge.
 * 
 * This function checks if a vertex is on the edge. It takes as
 * arguments the index of the vertex `vi`. It returns either `yes`
 * or `no`. The function works by looping over the lines of the
 * vertex and checking if they are on the edge.
 */
static on_edge vertex_on_edge( const shell *s, unsigned int vi){
  unsigned int j, li;
  vertex_data *vd = s->vd;
  line_data *ld = s->ld;
  on_edge oe = no;
  for( j=0; j<vd[vi].num_l; j++){
    li = vd[vi].l[j];
    if( ld[li].oe == yes ){
      oe = yes;
      break;
    } 
  }
  return oe;
}

/*!
 * Merge two lines.
 *
 * This function merges two lines on the shell. It takes as arguments
 * the indices to the two lines `li0` and `li1`. It assumes that the
 * both lines are on the edge, and that the vertices of the two lines
 * have already been merged. The function take the following steps:
 * - removes the duplicate line from the vertex data array,
 * - changes the reference to the old line in triangle data array,
 * - moves the line off the edge, adds the triangle data,
 * - possibly moves the triangle off the edge,
 * - possibly moves vertices off the edge,
 * - removes the old line.
 */
static void merge_line( shell *s, unsigned int li0,
                       unsigned int li1){
  unsigned int i, j, lim, lip, ti, vi, size, decr;
  vertex_data *vd = s->vd;
  line *l = s->l;
  line_data *ld = s->ld;
  triangle_data *td = s->td;

  if( li0 < li1){
    lim = li0;
    lip = li1;
  }else{
    lim = li1;
    lip = li0;
  }

  /* Remove `lip` from the vertex data for both vertices. */
  for( i=0; i<2; i++){
    vi = l[lip].i[i];
    decr = 0;
    for( j=0; j<vd[vi].num_l; j++){
      if( vd[vi].l[j] == lip ){
        size = vd[vi].num_l-j-1;
        memmove( &(vd[vi].l[j]), &(vd[vi].l[j+1]),
                size*sizeof(unsigned int));
        decr++;
      }
    }
    vd[vi].num_l -= decr;
  }

  /* Change reference to `lip` in triangle data to `lim`. */
  ti = ld[lip].t[0];
  for( i=0; i<3; i++){
    for( j=0; j<3; j++){
      if( td[ti].l[j] == lip){
        td[ti].l[j] = lim;
      }
    }
  }
  /* Move line `lim` off the edge. */
  ld[lim].oe = no;
  /* Add the triangle from `lip` to `lim`. */
  ld[lim].t[1] = ti;

  /* Possibly move the triangles off the edge. */
  for( i=0; i<2; i++){
    ti = ld[lim].t[i];
    td[ti].oe = triangle_on_edge( s, ti);
  }
  /* Possible move the vertices off the edge. */
  for( i=0; i<2; i++){
    vi = l[lim].i[i];
    vd[vi].oe = vertex_on_edge( s, vi);
  }

  /* remove the line `lip`. */
  remove_line( s, lip);
}

/*!
 * Initilization.
 *
 * This function initializes the shell to a single equilateral
 * triangle, and sets the center of the genome.
 */
void shell_initialize( shell *s, double r_genome){
  /* The three points the define the equilateral triangle. */
  point p0 = {{ 0., 0., 0.}};
  point p1 = {{0.5, ROOT3_2, 0.}}; 
  point p2 = {{1., 0., 0.}};
 
  /* The center of the genome*/
  double z;
  if( r_genome != 0.){
    z = sqrt( r_genome*r_genome+1/3.);
    s->vg[0].x[0] = 0.5;
    s->vg[0].x[1] = ROOT3/6.;
    s->vg[0].x[2] = z;
  }else{
    s->vg[0].x[0] = 0.;
    s->vg[0].x[1] = 0.;
    s->vg[0].x[2] = 0.;
  }

  /* The shell starts out empty. */
  s->num_v = 0;
  s->num_l = 0;
  s->num_t = 0;
 
  /* Add the three points. */
  add_vertex( s, p0);
  add_vertex( s, p1);
  add_vertex( s, p2);
  /* Add the three lines. */
  /* We know we start out with triangle 0. */
  add_line( s, 0, 1, 0);
  add_line( s, 1, 2, 0);
  add_line( s, 2, 0, 0);
  /* Add the first triangle. */
  add_triangle( s, 0, 1, 2, 0, 1, 2);
}

/*!
 *
 */
void shell_attach( shell *s, point p, unsigned int li){
  unsigned int vi0, vi1, ti;
  unsigned int num_v = s->num_v;
  unsigned int num_l = s->num_l;
  unsigned int num_t = s->num_t;
  line_data *ld = s->ld;
  triangle_data *td = s->td;

  vi0 = s->l[li].i[0];
  vi1 = s->l[li].i[1];
  add_vertex( s, p);
  /* Watch the order of the vertices when I add lines. */
  add_line( s, vi0, num_v, num_t);
  add_line( s, num_v, vi1, num_t);
  /* Watch the order of the vertices when I add the triangles. */
  add_triangle( s, vi0, num_v, vi1, num_l, num_l+1, li);

  ld[li].oe = no;
  ld[li].t[1] = num_t;
  ti = ld[li].t[0];
  td[ti].oe = triangle_on_edge( s, ti);
}

/*!
 * 
 */
void shell_insert( shell *s, unsigned int vi){
  unsigned int vi0, vi1, ti;
  line *l = s->l;
  unsigned int num_l = s->num_l;
  line_data *ld = s->ld;
  unsigned int num_t = s->num_t;
  vertex_data *vd = s->vd;
  triangle_data *td = s->td;
  unsigned int lil = 0, lir = 0;

  vertex_lines_on_edge( s, vi, &lil, &lir);
  vi0 = l[lil].i[0];
  vi1 = l[lir].i[1];
  add_line( s, vi0, vi1, num_t);
  add_triangle( s, vi0, vi1, vi, num_l, lir, lil);

  ld[lir].oe = no;
  ld[lir].t[1] = num_t;
  ti = ld[lir].t[0];
  td[ti].oe = triangle_on_edge( s, ti);
  ld[lil].oe = no;
  ld[lil].t[1] = num_t;
  ti = ld[lil].t[0];
  td[ti].oe = triangle_on_edge( s, ti);
  vd[vi].oe = vertex_on_edge( s, vi);
}

/*!
 *
 */
void shell_close( shell *s, unsigned int vi){
  unsigned int vi0, vi1;
  line *l = s->l;
  unsigned int lil = 0, lir = 0;

  vertex_lines_on_edge( s, vi, &lil, &lir);
  vi0 = l[lil].i[0];
  vi1 = l[lir].i[1];
  merge_vertex( s, vi0, vi1);
  merge_line( s, lil, lir);
}

/*!
 *
 */
void shell_join( shell *s, unsigned int li0, unsigned int li1){
  unsigned int vi00, vi01, vi10, vi11;
  line *l = s->l;

  vi00 = l[li0].i[0];
  vi01 = l[li0].i[1];
  vi10 = l[li1].i[0];
  vi11 = l[li1].i[1];

  merge_vertex( s, vi00, vi11);
  merge_vertex( s, vi01, vi10);
  merge_line( s, li0, li1);
}

/*!
 *
 */
void align_line( shell *s, unsigned int ti,
                 unsigned int *v0, unsigned int *v1){
  if( s->t[ti].i[0] == *v0 && s->t[ti].i[1] == *v1 ){
    *v0 = s->t[ti].i[1];
    *v1 = s->t[ti].i[0];
  }else if( s->t[ti].i[1] == *v0 && s->t[ti].i[2] == *v1 ){
    *v0 = s->t[ti].i[2];
    *v1 = s->t[ti].i[1];
  }else if( s->t[ti].i[2] == *v0 && s->t[ti].i[0] == *v1 ){
    *v0 = s->t[ti].i[0];
    *v1 = s->t[ti].i[2];
  }
}

/*!
 *
 */
void shell_remove( shell *s, unsigned int ti){
  unsigned int i, j, vi, li, size, decr;
  triangle *t = s->t;
  vertex_data *vd = s->vd;
  triangle_data *td = s->td;
  line *l = s->l;
  line_data *ld = s->ld;

  /* Handle the vertices. */
  for( i=0; i<3; i++){
    vi = t[ti].i[i];
    /* Check if this is the only triangle. */
    if( vd[vi].num_t == 1){
      /* If so, remove. */
      remove_vertex( s, vi);
    }else{
      /* Otherwize remove `ti` from vertex data array. */
      decr = 0;
      for( j=0; j<vd[vi].num_t; j++){
        if( vd[vi].t[j] == ti ){
          size = vd[vi].num_t-j-1;
          memmove( &(vd[ti].t[j]), &(vd[ti].t[j+1]),
                  size*sizeof(unsigned int));
          decr++;
        }
      }
      vd[vi].num_t -= decr;
      /* And set on the edge. */
      vd[vi].oe = yes;
    }
  }

  /* Handle the lines. */
  for( i=0; i<3; i++){
    li = td[ti].l[i];
    /* Check if each line is on the edge. */
    if( ld[li].oe == yes){
      /* If so, remove. */
      remove_line( s, li);
    }else{
      /* Otherwize, move it to the edge, */
      ld[li].oe = yes;
      /* and remove `ti` from line data array. */
      if( ld[li].t[0] == ti ){
        ld[li].t[0] = ld[li].t[1];
      }
      /* Move the neighboring triangle to the edge. */
      td[ld[li].t[0]].oe = yes;
      /* And reorder the vertex indices in the line. */
      align_line(s, ti, &l[li].i[0], &l[li].i[1]);
    }
  }

  /* Decrement references to triangles larger than `ti`. */
  for( i=0; i<s->num_v; i++){
    for( j=0; j<vd[i].num_t; j++){
      if( vd[i].t[j] > ti){
        vd[i].t[j]--;
      }
    }
  }
  for( i=0; i<s->num_l; i++){
    if( ld[i].t[0] > ti){
      ld[i].t[0]--;
    }
    if( ld[i].oe == no){
      if( ld[i].t[1] > ti){
        ld[i].t[1]--;
      }
    }
  }

  /* Finally, remove the triangle from the triangle and */
  /* triangle data arrays. */
  size = s->num_t-ti-1;
  memmove( &(t[ti]), &(t[ti+1]), size*sizeof(triangle));
  memmove( &(td[ti]), &(td[ti+1]), size*sizeof(triangle_data));
  /* Decrement triangle counter. */
  s->num_t--;
}