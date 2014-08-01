/*!*******************************************************************
 * \author Jef Wagner
 * \email jefwagner@gmail.com
 *********************************************************************
 */
/*!
 * This file contains objects for running the program.
 */
#ifndef JW_MAIN
#define JW_MAIN

#include "shell.h"
#include "cg.h"

/*!
 * A move type enum.
 *
 * There are two move types: a vertex move, and a line move, (in the
 * future there will be a third type: a triangle move).
 */
typedef enum{ vertex_move, line_move} move_type;

/*!
 * A data structure to hold info for a move.
 *
 * This data structure holds the relevant infomation for a move. The
 * type of move, the index for the move, and a floating point number
 * associated with the probablity of that move occuring. Currently the
 * floating point number is an angle. In the future it should probably
 * be an energy associated with the move.
 */
typedef struct{
  move_type type;
  unsigned int index;
  double angle;
} move;

/*!
 * A data structure to hold the list of lines and vertices for the
 * relax partial function.
 */
typedef struct{
  unsigned int num_vl;
  unsigned int *vl;
  unsigned int num_ll;
  unsigned int *ll;
} relax_partial_ws;

typedef struct {
  double gamma;
  double th0;
  double sigma;
  double r_membrane;
  double r_genome;
  double delta_b;
  unsigned int seed;
  unsigned int movie;
  unsigned int rp_depth;
  unsigned int max_tri;
} shell_params;

typedef struct {
  shell *s;
  shell_params *sp;
  nlcg_ws nlcg;
  double hmin;
  move *ml;
  relax_partial_ws *rp;
} shell_run;

#endif