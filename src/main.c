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
shell_run* shell_run_malloc( unsigned int max_tri, 
                            unsigned int depth);
void shell_run_free( shell_run *sr);
int shell_params_initialize( shell_params *sp, const char* filename, 
                            int n);
void shell_run_initialize( shell_run *sr, shell_params *sp);
int create_output_directories( shell_run *sr, const char* filename);
int print_params( shell_params *sp, unsigned int n, 
                 const char* filename);
int print_shell( shell *s, unsigned int n, const char* filename);
void print_movie_frame( shell *s, unsigned int i);


int main( int argc, const char **argv){
  int n, status, grow_status, relax_status, i, max_eval_count;
  char parameter_filename[80];
  shell_run *sr;
  shell_params sp;

  status = get_cl_args( argc, argv, parameter_filename, &n);
  if( status != 0){
    usage( argv[0]);
    return status;
  }

  status = shell_params_initialize( &sp, parameter_filename, n);
  if( status != 0){
    usage( argv[0]);
    return status;
  }
  sr = shell_run_malloc( sp.max_tri, sp.rp_depth);
  if( sr == NULL ){
    fprintf( stderr, "Could not allocated needed memory.\n");
    usage( argv[0]);
    return 1;
  }
  shell_run_initialize( sr, &sp);
  status = create_output_directories( sr, parameter_filename);
  if( status != 0){
    usage( argv[0]);
    return status;
  }
  status = print_params( sr->sp, n, parameter_filename);

  i=0;
  if( sr->sp->movie ){
    print_movie_frame( sr->s, i);
  }
  i++;
  grow_status = 0;
  max_eval_count = 0;
  grow_status = grow( sr);
  while( grow_status != -1 && 
        sr->s->num_t < sr->sp->max_tri &&
        max_eval_count < 3 ){
    if( i%10 == 0 ){
      relax_status = relax_total( sr);
    }else{
      relax_status = relax_partial( sr, grow_status);
    }
    if( relax_status >= sr->sp->nlcg_max_eval){
      max_eval_count++;
    }else{
      max_eval_count = 0;
    }
    if( sr->sp->movie ){
      print_movie_frame( sr->s, i);
    }
    printf( ".");
    fflush( stdout);
    if( i%30 == 0 ){
      printf( "\n");
    }
    i++;
    grow_status = grow( sr);
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

shell_run* shell_run_malloc( unsigned int max_tri, unsigned int depth){
  shell_run *sr;
  unsigned int max_vert, max_moves;

  max_vert = max_tri + 2;
  max_moves = 2*max_tri+1 + max_vert; 

  sr = (shell_run *) malloc( sizeof(shell_run));
  if( sr == NULL ){
    return NULL;
  }
  sr->s = shell_malloc( max_tri);
  if( sr->s == NULL ){
    free( sr);
    return NULL;
  }
  sr->nlcg = nlcg_malloc(3*(max_vert+2));
  if( sr->nlcg == NULL ){
    shell_free( sr->s);
    free( sr);
    return NULL;
  }
  sr->ml = (move *) malloc( max_moves*sizeof(move));
  if( sr->ml == NULL){
    nlcg_free( sr->nlcg);
    shell_free( sr->s);
    free( sr);
    return NULL;
  }
  sr->rp = relax_partial_ws_malloc( depth);
  if( sr->rp == NULL ){
    free( sr->ml);
    nlcg_free( sr->nlcg);
    shell_free( sr->s);
    free( sr);
    return NULL;    
  }

  return sr;
}

void shell_run_free( shell_run *sr){
  relax_partial_ws_free( sr->rp);
  free( sr->ml);
  nlcg_free( sr->nlcg);
  shell_free( sr->s);
  free( sr);
}

int shell_params_initialize( shell_params *sp, const char* filename, int n){
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

void shell_run_initialize( shell_run *sr, shell_params *sp){
  double z, r_gen = sp->r_genome;
  if( r_gen != 0 ){
    z = sqrt(r_gen*r_gen-3./9.);
  }else{
    z = 0.;
  }
  sr->sp = sp;
  srand( sp->seed);
  shell_initialize( sr->s);
  sr->s->p_gen->x[0] = 0.5; 
  sr->s->p_gen->x[1] = ROOT3/6.;
  sr->s->p_gen->x[2] = z;
  sr->s->p_mem->x[0] = 0.; 
  sr->s->p_mem->x[1] = 0.;
  sr->s->p_mem->x[2] = 0.;
  nlcg_set_tol( 0., 0., 1.e-2, sp->nlcg_max_eval, sr->nlcg);
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
