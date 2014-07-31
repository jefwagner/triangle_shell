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
#include <unistd.h>
#include <sys/stat.h>

#include "main.h"
#include "inout.h"
#include "cg.h"
#include "relax.h"
#include "grow.h"

#define MAX_TRI 100

void usage( const char* argv0);
int get_cl_args( int argc, const char **argv,
                   char *parameter_filename, int *n);
shell_run* shell_run_malloc( unsigned int max_tri);
void shell_run_free( shell_run *sr);
int shell_run_initialize( shell_run *sr, const char* filename, int n);
int create_output_directories( shell_run *sr, const char* filename);
int print_params( shell_params *sp, unsigned int n, 
                 const char* filename);
int print_shell( shell *s, unsigned int n, const char* filename);
void print_movie_frame( shell *s, unsigned int i);


int main( int argc, const char **argv){
  int n, status, run_status, i;
  char parameter_filename[80];
  shell_run *sr;

  status = get_cl_args( argc, argv, parameter_filename, &n);
  if( status != 0){
    usage( argv[0]);
    return status;
  }

  sr = shell_run_malloc( MAX_TRI);
  if( sr == NULL ){
    fprintf( stderr, "Could not allocated needed memory.\n");
    usage( argv[0]);
    return 1;
  }
  status = shell_run_initialize( sr, parameter_filename, n);
  if( status != 0){
    usage( argv[0]);
    return status;
  }
  status = create_output_directories( sr, parameter_filename);
  if( status != 0){
    usage( argv[0]);
    return status;
  }
  status = print_params( sr->sp, n, parameter_filename);

  i=0;
  run_status = relax( sr);
  if( sr->sp->movie ){
    print_movie_frame( sr->s, i);
  }
  i++;
  while( i < 10){
    run_status = grow( sr);
    printf( "g: %u  ", run_status);
    run_status = relax_total( sr);
    printf( "r: %1.3f %u   ", sr->hmin, run_status);
    if( sr->sp->movie ){
      print_movie_frame( sr->s, i);
    }
    i++;
    printf( "t: %u\n", sr->s->num_t);
  }

  status += print_shell( sr->s, n, parameter_filename);

  shell_run_free( sr);

  return status;
}

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

shell_run* shell_run_malloc( unsigned int max_tri){
  shell_run *sr;
  unsigned int max_vert, max_moves;

  max_vert = max_tri + 2;
  max_moves = 2*max_tri+1 + max_vert; 

  sr = (shell_run *) malloc( sizeof(shell_run));
  if( sr == NULL ){
    return NULL;
  }
  sr->sp = (shell_params *) malloc( sizeof(shell_params));
  if( sr->sp == NULL ){
    free( sr);
    return NULL;
  }
  sr->s = shell_malloc( max_tri);
  if( sr->s == NULL ){
    free( sr->sp);
    free( sr);
    return NULL;
  }
  sr->nlcg = nlcg_malloc(max_vert);
  if( sr->nlcg == NULL ){
    shell_free( sr->s);
    free( sr->sp);
    free( sr);
    return NULL;
  }
  sr->ml = (move *) malloc( max_moves*sizeof(move));
  if( sr->ml == NULL){
    nlcg_free( sr->nlcg);
    shell_free( sr->s);
    free( sr->sp);
    free( sr);
    return NULL;
  }

  return sr;
}

void shell_run_free( shell_run *sr){
  free( sr->ml);
  nlcg_free( sr->nlcg);
  shell_free( sr->s);
  free( sr->sp);
  free( sr);
}

int shell_run_initialize( shell_run *sr, const char* filename, int n){
  int status;
  FILE *f;
  srand( sr->sp->seed);
  f = fopen( filename, "r");
  if( f == NULL ){
    fprintf( stderr, "Could not open the parameter file %s.\n",
            filename);
    return 1;
  }
  status = read_param_file( sr->sp, f, n);
  if( status == INOUT_OUT_OF_BOUNDS ){
    fprintf( stderr, "Simulation number out of bounds.\n");
    return 1;
  }
  if( status == INOUT_FORMAT_ERROR ){
    fprintf( stderr, "Error parsing parameter file.\n");
    fprintf( stderr, "Please see README for formatting instructuions.\n");
    return 1;
  }
  fclose( f);
  shell_initialize( sr->s);
  nlcg_set_tol( 0., 0., 1.e-4, 50000, sr->nlcg);
  return 0;
}

/*!
 * Strip the file extension off of a c string.
 */
void strip_ext( char* str){
  int i;
  i = strlen( str)-1;
  while( str[i] != '.' && i > 0 ){
    i--;
  }
  if( i != 0 ){
    str[i] = '\0';
  }
}

int print_params( shell_params *sp, unsigned int n, 
                 const char* filename){
  char filebase[80], prm_file[80];
  FILE *f;

  strcpy( filebase, filename);
  strip_ext( filebase);
  sprintf( prm_file, "results/%s_%05u.prm", filebase, n);
  f = fopen( prm_file, "w");
  if( f == NULL ){
    fprintf( stderr, "Could not open outputfile %s\n", prm_file);
    return 1;
  }
  shell_params_write( sp, f, filename, n);
  fclose( f);
  return 0;
}

int print_shell( shell *s, unsigned int n, const char* filename){
  char filebase[80], shell_file[80];
  FILE *f;

  strcpy( filebase, filename);
  strip_ext( filebase);
  sprintf( shell_file, "results/%s_%05u.shell", filebase, n);
  f = fopen( shell_file, "w");
  if( f == NULL ){
    fprintf( stderr, "Could not open outputfile %s\n", shell_file);
    return 1;
  }
  shell_write( s, f);
  fclose( f);
  return 0;
}

void print_movie_frame( shell *s, unsigned int i){
  char filename[] = "movie/frame_xxxxx.shell";
  FILE *f;

  sprintf( filename, "movie/frame_%05d.shell", i);
  f = fopen( filename, "w");
  if( f==NULL){
    fprintf( stderr, "Could not open moviefile %s\n", filename);
    return;
  }
  shell_write( s, f);
  fclose( f);
}

/*!
 * Check if a directory exist, if not create it.
 *
 * The function returns zero on success (either the directory exist,
 * or I can successfully create it). Upon failure (a directory does
 * not exist, and I cannot create it) it returns non-zero.
 */
int check_or_make_dir( const char *dirname){
  struct stat dirstat;
  if( stat( dirname, &dirstat) == 0 ){
    return !S_ISDIR(dirstat.st_mode); 
  }else{
    return mkdir( dirname, S_IRWXU);
  }
}

int create_output_directories( shell_run *sr, const char* filename){
  int status;
  const char results_dir[] = "results";
  const char movie_dir[] = "movie";
  status = check_or_make_dir( results_dir);
  if( status != 0){
    fprintf( stderr, "Could not access or create directory %s\n",
            results_dir);
    return 1;
  }
  if( sr->sp->movie ){
  status = check_or_make_dir( movie_dir);
    if( status != 0){
      fprintf( stderr, "Could not access or create directory %s\n",
              movie_dir);
      return 1;
    }
  }
  return 0;
}
