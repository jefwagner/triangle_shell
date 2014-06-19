/*!*******************************************************************
 * \author Jef Wagner
 * \email jefwagner@gmail.com
 *********************************************************************
 */
/*!
 * Unit test for the functions in inout.c.
 */

#include <stdio.h>
#include <stdlib.h>

#include "../src/inout.c"
#include "../src/shell.c"
#include "shell_example.h"

/*!
 * `shell_write` test.
 *
 * Writes and example file, and checks it length.
 */
void shell_write_test(){
  int status, size;
  FILE *f;
  shell *s;

  printf( "shell_write: ");
  s = shell_malloc( 10);
  shell_copy( s, &ss6);
  status = 0;
  f = fopen( "shell_1_test.dat", "w");
  if( f == NULL){
    status++;
  }else{
    shell_write( s, f);
    fclose( f);
    f = NULL;
    f = fopen( "shell_1_test.dat", "r");
    if( f == NULL){
      status++;
    }else{
      fseek( f, 0, SEEK_END);
      size = 0;
      size = ftell( f);
      fclose( f);
      if( size <= 0 ){
        status++;
      }
    }
  }

  if( status == 0){
    printf( "pass\n");
  }else{
    printf( "fail\n");
  }
  shell_free( s);
}

/*!
 * `shell_read` test.
 */
void shell_read_test(){
  int status, ioerr;
  shell *s;
  FILE *f;

  printf( "shell_read: ");
  s = shell_malloc( 10);
  f = fopen( "shell_1_test.dat", "r");
  status = 0;
  if( f == NULL){
    status++;
  }else{
    ioerr = shell_read( s, f);
    if( ioerr != INOUT_SUCCESS){
      status++;
    }
    fclose( f);
    if( !shell_comp( s, &ss6)){
      status++;
    }
    fclose( f);
  }

  if( status == 0){
    printf( "pass\n");
  }else{
    printf( "fail\n");
  }
  shell_free( s);
}

int main(){
  shell_write_test();
  shell_read_test();
  return 0;
}
