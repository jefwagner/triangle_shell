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

#include "math_const.h"

void usage( const char* argv0);
int get_cl_args( int argc, const char **argv,
                   char *parameter_filename, int *n);
shell_run* shell_run_malloc( shell_params *sp);
void shell_run_free( shell_run *sr);
int shell_read_params( shell_params *sp, const char* filename, 
                       unsigned int n);
int shell_run_initialize( shell_run *sr);
int create_output_directories( shell_run *sr, const char* filename);
int print_params( shell_params *sp, unsigned int n, 
                 const char* filename);
int print_shell( shell *s, unsigned int n, const char* filename);
void print_movie_frame( shell *s, unsigned int i);


int main( int argc, const char **argv){
  int n, status, grow_status, relax_status;
  int frame_count, partial_count, relax_long_count;
  double en6, en5;
  char parameter_filename[80];
  shell_run *sr;
  shell_params sp;

  status = get_cl_args( argc, argv, parameter_filename, &n);
  if( status != 0){
    usage( argv[0]);
    return status;
  }

  status = shell_read_params( &sp, parameter_filename, n);
  if( status != 0){
    usage( argv[0]);
    return status;
  }
  sr = shell_run_malloc( &sp);
  if( sr == NULL ){
    fprintf( stderr, "Could not allocated needed memory.\n");
    usage( argv[0]);
    return 1;
  }
  status = shell_run_initialize( sr);
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

  frame_count=0;
  partial_count=0;
  relax_status = relax_total( sr, sr->s);
  if( sr->sp->movie ){
    print_movie_frame( sr->s, frame_count);
  }
  frame_count++;
  while( sr->s->num_t < sr->sp->max_tri &&
         relax_long_count < 5 ){
    grow_status = grow( sr);
    if( grow_status == -3 ){
      relax_status = relax_total( sr, sr->s);
      en6 = sr->hmin/((double) sr->s->num_t);
      relax_status += relax_total( sr, sr->s5);
      en5 = sr->hmin/((double) sr->s5->num_t);
      partial_count = 0;
      if( en5 < en6 ){
        shell_copy( sr->s, sr->s5);
        printf( "g: 5* " ); 
      }else{
        printf( "g: 6* " ); 
      }
    }else if( grow_status == -2 || partial_count == 20 ){
      relax_status = relax_total(sr, sr->s);
      partial_count = 0;
      printf( "g: * " ); 
    }else if( grow_status == -1){
      break;
    }else{
      relax_status = relax_partial( sr, grow_status);
      partial_count++;
      printf( "g: %u ", grow_status);
    }
    if( relax_status > 20000){
      relax_long_count++;
    }else{
      relax_long_count = 0;
    }
    printf( "r: %1.3f %u   ", sr->hmin, relax_status);
    if( sr->sp->movie ){
      print_movie_frame( sr->s, frame_count);
    }
    frame_count++;
    printf( "t: %u\n", sr->s->num_t);
  }
  printf( "\n");

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

shell_run* shell_run_malloc( shell_params *sp){
  shell_run *sr;
  unsigned int max_vert, max_moves;
  unsigned int max_tri = sp->max_tri;

  max_vert = max_tri + 2;
  max_moves = 2*max_tri+1 + max_vert; 

  sr = (shell_run *) malloc( sizeof(shell_run));
  if( sr == NULL ){
    return NULL;
  }
  sr->sp = sp;
  sr->s = shell_malloc( max_tri);
  if( sr->s == NULL ){
    free( sr);
    return NULL;
  }
  sr->s5 = shell_malloc( max_tri);
  if( sr->s5 == NULL ){
    shell_free( sr->s);
    free( sr);
    return NULL;
  }
  sr->nlcg = nlcg_malloc(3*(max_vert+2));
  if( sr->nlcg == NULL ){
    shell_free( sr->s5);
    shell_free( sr->s);
    free( sr);
    return NULL;
  }
  sr->ml = (move *) malloc( max_moves*sizeof(move));
  if( sr->ml == NULL){
    nlcg_free( sr->nlcg);
    shell_free( sr->s5);
    shell_free( sr->s);
    free( sr);
    return NULL;
  }

  return sr;
}

void shell_run_free( shell_run *sr){
  free( sr->ml);
  nlcg_free( sr->nlcg);
  shell_free( sr->s5);
  shell_free( sr->s);
  free( sr);
}

int shell_read_params( shell_params *sp, const char *filename, 
                       unsigned int n){
  int status;
  FILE *f;
  f = fopen( filename, "r");
  if( f == NULL ){
    fprintf( stderr, "Could not open the parameter file %s.\n",
            filename);
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
  fclose( f);
  return 0;  
}

int shell_run_initialize( shell_run *sr){
  srand( sr->sp->seed);
  shell_initialize( sr->s, sr->sp->r_genome);
  nlcg_set_tol( 0., 0., 1.e-2, 20000, sr->nlcg);
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
  if( sp->movie ){
    sprintf( prm_file, "movie/movie.prm");
    f = fopen( prm_file, "w");
    if( f == NULL ){
      fprintf( stderr, "Could not open outputfile %s\n", prm_file);
      return 1;
    }
    shell_params_write( sp, f, filename, n);
    fclose(f);
  }
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
