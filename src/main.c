/*!*******************************************************************
 * \author Jef Wagner
 * \email jefwagner@gmail.com
 *********************************************************************
 */
/*!
 * This file contains the main routine, as well as a few small
 * convenience functions.
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "main.h"
#include "inout.h"

void usage( char* argv0){
  fprintf( stdout, "%s parameter_filename [simulation_number]\n", argv0);
/*
The first argument should be a parameter filename, the second argument
is options, and is an integer that gives the simulation number
(explained below). The program neads a filename for the parameter
file. An example parameter file is:
---------------------------
# Comment line
# give the value of the Foppl von Karman number
var gamma 2
# give a list of values for the spontaneous radius of curvature
list r_0 0.925 0.95 0.975 1.0 1.025 1.05
----------------------------
The character `#` specifies a comment and runs to the end of the line.
A parameter is specified with the `var` keyword followed by the name
of the parameter followed by its value. A list of parameter values is
specified by the `list` keyword followed by the name of the parameter
followed by a list of the values separeated by whitespace (spaces,
tabs, or linebreaks). The parameters that can be set.
*/
}

int main( int argc, char **argv){
  int n;
  char base[80];

  switch( argc){
    case 1:
      strcpy( base, "test.prm");
      break;
    case 2:
      strcpy( base, argv[2]);
      n = 1;
      break;
    case 3:
      strcpy( base, argv[2]);
      n = atoi( argv[3])
      break;
    default:
      fprintf( stderr "Too many command line arguments.\n")
      usage(arv[0]);
      return 1;
    }
  }
  return 0;
}