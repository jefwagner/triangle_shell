/*!*******************************************************************
 * \author Jef Wagner
 * \email jefwagner@gmail.com
 *********************************************************************
 */
/*!
 * This file contains the input/output routines. It reads the
 * parameter file, can read a shell file, and write the shell file.
 */
#ifndef JW_INOUT
#define JW_INOUT

#include "main.h"
#include "shell.h"

#define INOUT_OUT_OF_BOUNDS -2
#define INOUT_FORMAT_ERROR -1
#define INOUT_SUCCESS 0

void shell_write( shell *s, FILE *f);
int shell_read( shell *s, FILE *f);
int read_param_file( shell_params *sp, FILE *f, unsigned int n);

#endif