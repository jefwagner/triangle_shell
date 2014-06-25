/*!*******************************************************************
 * \author Jef Wagner
 * \email jefwagner@gmail.com
 *********************************************************************
 */
/*!
 * This file contains the main routine, as well as a few small
 * convenience functions.
 */
/*
The first argument should be a parameter filename, the second argument
is options, and is an integer that gives the simulation number
(explained below). The program neads a filename for the parameter
file. An example parameter file is:
```
--------------------------
# Comment line
# give the value of the Foppl von Karman number
var gamma 2
# give a list of values for the spontaneous radius of curvature
list r_0 0.925 0.95 0.975 1.0 1.025 1.05
---------------------------
```
The character `#` specifies a comment and runs to the end of the line.
A parameter is specified with the `var` keyword followed by the name
of the parameter followed by its value. A list of parameter values is
specified by the `list` keyword followed by the name of the parameter
followed by a list of the values separeated by whitespace (spaces,
tabs, or linebreaks). The parameters that can be set are:
 - gamma : the Foppl von Karman number,
 - r_0 : the spontaneous radius of curvature,
 - sigma : the width of the gaussian random choice,
 - delta_b : the distance at which two edges are joined,
 - r_membrane: the radius of the surrounding membrane,
 - r_genome: the radius of genome,
 - delta_b : the distance at which two edges are joined.

If multiple values are given for one or more parameters, then the
simulation number chooses which are used. The simulation number ranges
from 0 and goes through all combination, where the gamma changes
most rapidly, r_0 next, and on down in the order listed above. For
example, if the parameter file reads
```
------------------
list gamma 1 2
# note that sigma and r_0 are reversed from that listed above
list sigma 1.e-4 30
list r_0 1.0 1.5
-------------------
```
then there are 2^3=8 total options, and the simulation number can
range from 0 to 7, explicitly:
-------------------------------------------
| simulation_number | gamma | r_0 | sigma |
-------------------------------------------
|                 0 |     1 | 1.0 | 1.e-4 |
|                 1 |     2 | 1.0 | 1.e-4 |
|                 2 |     1 | 1.5 | 1.e-4 |
|                 3 |     2 | 1.5 | 1.e-4 |
|                 4 |     1 | 1.0 |    30 |
|                 5 |     2 | 1.0 |    30 |
|                 6 |     1 | 1.5 |    30 |
|                 7 |     2 | 1.5 |    30 |
-------------------------------------------

*/


#include <stdio.h>
#include <string.h>
#include <math.h>

#include "main.h"
#include "inout.h"

void usage( char* argv0){
  fprintf( stdout, "%s parameter_filename [simulation_number]\n", argv0);
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
    case 1:
      fprintf( stderr, "Must provide parmeter filename.\n");
    default:
      fprintf( stderr, "Too many command line arguments.\n")
      usage(arv[0]);
      return 1;
    }
  }
  return 0;
}