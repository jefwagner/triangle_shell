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
#include "cg.h"
#include "relax.h"
#include "grow.h"

#define MAX_TRI 100

void usage( const char* argv0){
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
      status = 1;
      break;
    default:
      fprintf( stderr, "Too many command line arguments.\n");
      status = 1;
      break;
  }
  return status;
}

int get_params( const char *parameter_filename, FILE *f,
               int n, shell_params *sp){
  int status;
  if( f == NULL ){
    fprintf( stderr, "Could not open the parameter file %s.\n",
            parameter_filename);
    return 1;
  }
  status = read_param_file( sp, f, n);
  if( status == INOUT_OUT_OF_BOUNDS ){
    fprintf( stderr, "Simulation number out of bounds.\n");
    return 1;
  }
  if( status == INOUT_FORMAT_ERROR ){
    fprintf( stderr, "Error parsing parameter file.\n");
    fprintf( stderr, "Please see README for formatting instructuions.\n");
    return 1;
  }
  return 0;
}

int main( int argc, const char **argv){
  int n, status;
  char parameter_filename[80];
  shell_run sr;
  shell_params sp;
  FILE *f;

  status = get_cl_args( argc, argv, parameter_filename, &n);
  if( status != 0){
    usage( argv[0]);
    return status;
  }

  f = fopen( parameter_filename, "r");
  status = get_params( parameter_filename, f, n, &sp);
  fclose( f);
  if( status != 0){
    usage( argv[0]);
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
  if( sr.s == NULL ){
    return 1;
  }

  sr.nlcg = nlcg_malloc( MAX_TRI);
  if( sr.nlcg == NULL ){
    free( sr.s);
    return 1;
  }

  shell_initialize( sr.s);
  nlcg_set_tol( 0., 0., 1.e-4, 50000, sr.nlcg);

  n = relax( &sr);
  printf( "Found minimum energy %1.3f after %u evaluations\n",
         sr.hmin, n);
  n = grow( &sr);
  printf( "Grow step.\n");
  shell_write( sr.s, stdout);
  n = relax( &sr);
  printf( "Found minimum energy %1.3f after %u evaluations\n",
         sr.hmin, n);
  shell_write( sr.s, stdout);
  n = grow( &sr);
  printf( "Grow step.\n");
  n = relax( &sr);
  printf( "Found minimum energy %1.3f after %u evaluations\n",
         sr.hmin, n);


  nlcg_free( sr.nlcg);
  shell_free( sr.s);

  return 0;
}