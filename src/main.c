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
  fprintf( stdout, "See the README for full details\n");
}

int main( int argc, char **argv){
  int n;
  char base[80];

  switch( argc){
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
      usage(argv[0]);
      break;
    default:
      fprintf( stderr, "Too many command line arguments.\n")
      usage(argv[0]);
      return 1;
    }
  }
  return 0;
}