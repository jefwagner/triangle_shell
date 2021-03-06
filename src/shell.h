/*!*******************************************************************
 * \author Jef Wagner
 * \email jefwagner@gmail.com
 *********************************************************************
 */
/*!
 * Header file that contains the shell data structure and the
 * prototypes of the exported functions. See file `shell.c` for a
 * description of the functions.
 */
#ifndef JW_SHELL
#define JW_SHELL

/*!
 * A simple enum to determine if a part of the shell is on the edge of
 * the shell or not.
 */
typedef enum{ no, yes} on_edge;

/*!
 * Point data structure.
 *
 * A point is three doubles.
 */
typedef struct{ double x[3];} point;

/*!
 * Line data structure.
 *
 * A line is two unsigned ints (indices of the vertices).
 */
typedef struct{ unsigned int i[2];} line;

/*!
 * Triangle data structure.
 *
 * A triangle is three unsigned ints (indices of the vertices).
 */
typedef struct{ unsigned int i[3];} triangle;

/*!
 * Addiontal vertex data.
 *
 * In addition to the points in the vertex array, we keep track of
 * which lines and which triangles contain that vertex.
 */
typedef struct{
  on_edge oe;
  unsigned int num_l;
  unsigned int l[8];
  unsigned int num_t;
  unsigned int t[6];
} vertex_data;

/*!
 * Additional line data.
 *
 * In addition to the indices of the vertices on the line, we keep
 * track of which triangles use the line.
 */
typedef struct{
  on_edge oe;
  unsigned int t[2];
} line_data;

/*!
 * Additonal triangle data.
 *
 * In addition to the indices of the vertices of the triangle, we keep
 * track of which lines make up the triangle.
 */
typedef struct{
  on_edge oe;
  unsigned int l[3];
} triangle_data;

/*!
 * Shell object.
 *
 * This contains all the vertices, lines, and triangles that make up
 * the shell. It consist integers that determine the length of the
 * arrays, and pointers to the arrays of vertices, vertex data, lines,
 * line_data, triangles, and triangle data.
 */
typedef struct{
  point *vg; /* center of genome */
  unsigned int num_v;
  point *v;
  vertex_data *vd;
  unsigned int num_l;
  line *l;
  line_data *ld;
  unsigned int num_t;
  triangle *t;
  triangle_data *td;
} shell;

/*!
 * Prototypes of the public functions.
 */
shell* shell_malloc( unsigned int max_t );
void shell_copy( shell *dest, const shell *source);
void shell_free( shell *s);
void shell_initialize( shell *s, double r_genome);
void shell_attach( shell *s, point p, unsigned int li);
void shell_insert( shell *s, unsigned int vi);
void shell_merge_line( shell *s, unsigned int li0, unsigned int li1);
void shell_close( shell *s, unsigned int vi);
void shell_join( shell *s, unsigned int li0, unsigned int li1);
void shell_remove( shell *s, unsigned int ti);

/*!
 * Small functions that can be inlined
 */

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

/*!
 * For a vertex on the edge, find the lines on the edge.
 *
 * Search through all the lines on a vertex that is on the edge. If we
 * find one, compare the order to see if it is on the left or the
 * right. This will not work if a single vertex has two edges.
 */
static inline void vertex_lines_on_edge(shell *s, unsigned int vi,
                                        unsigned int *lil, 
                                        unsigned int *lir){
  unsigned int i, li;
  vertex_data *vd = s->vd;
  line_data *ld = s->ld;
  line *l = s->l;

  for( i=0; i<vd[vi].num_l; i++){
    li = vd[vi].l[i];
    if( ld[li].oe == yes && l[li].i[1] == vi ){
      *lil = li;
    }
    if( ld[li].oe == yes && l[li].i[0] == vi ){
      *lir = li;
    }
  }
}

#endif

