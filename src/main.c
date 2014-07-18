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
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "main.h"
#include "inout.h"

#define MAX_TRI 100

void usage( char* argv0){
  fprintf( stdout, "%s parameter_filename [simulation_number]\n", argv0);
  fprintf( stdout, "See the README for full details\n");
}

int get_cl_args( int argc, const char **argv,
                   char *parameter_filename, int *n){
  int status;
  switch( argc){
    case 2:
      strcpy( parameter_filename, argv[1]);
      *n = 0;
      status = 0;
      break;
    case 3:
      strcpy( parameter_filename, argv[1]);
      *n = atoi( argv[2]);
      status = 0;
      break;
    case 1:
      fprintf( stderr, "Must provide parmeter filename.\n");
      usage( argv[0]);
      status = 1;
      break;
    default:
      fprintf( stderr, "Too many command line arguments.\n");
      usage( argv[0]);
      status = 1;
      break;
  }
  return status;
}

int get_params( const char *parameter_filename, FILE *f,
                 shell_params *sp){
  int status;
  if( f == NULL ){
    fprintf( stderr, "Could not open the parameter file %s.\n",
            parameter_filename);
    usage( argv[0]);
    return 1;
  }
  status = read_param_file( &sp, f, n);
  if( status == INOUT_OUT_OF_BOUNDS ){
    fprintf( stderr, "Simulation number out of bounds.\n");
    usage( argv[0]);
    return 1;
  }
  if( status == INOUT_FORMAT_ERROR ){
    fprintf( stderr, "Error parsing parameter file.\n");
    fprintf( stderr, "Please see README for formatting instructuions.\n");
    usage( argv[0]);
    return 1;
  }
  return 0;
}

int main( int argc, char **argv){
  int n, status;
  char parameter_filename[80];
  shell_run sr;
  shell_params sp;
  FILE *f;

  status = get_cl_args( argc, argv, parameter_filename, &n);
  if( status != 0){
    return status;
  }

  f = fopen( parameter_filename, "r");
  status = get_params( parameter_filename, f, &sp);
  fclose( f);
  if( status != 0){
    return status;
  }

  printf( "%1.3f %1.3f %1.3f %1.3f %1.3f %1.3f %u\n",
         sp.gamma, sp.th0, sp.sigma, sp.r_membrane,
         sp.r_genome, sp.delta_b, sp.seed);

  // printf( "gamma = %1.3f\n", sp.gamma);
  // printf( "th0 = %1.3f\n", sp.th0);
  // printf( "sigma = %1.3f\n", sp.sigma);
  // printf( "r_membrane = %1.3f\n", sp.r_membrane);
  // printf( "r_genome = %1.3f\n", sp.r_genome);
  // printf( "delta_b = %1.3f\n", sp.delta_b);
  // printf( "seed = %u\n", sp.seed);

  sr.sp = &sp;
  sr.s = shell_malloc( MAX_TRI);

  shell_free( sr.s);

  return 0;
}