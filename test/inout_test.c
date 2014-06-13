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
#include "../src/shell.h"
#include "shell_example.h"

void shell_write(){
  int status;
  FILE *f;
  shell *s;

  printf( "shell_write: ");
  s = shell_malloc( 10);
  shell_copy( s, &ss1);
  f = fopen( "shell_1_test.dat", "w");
  shell_write( s, f);
  fclose( f);
  
}

int main(){
  return 0;
}
