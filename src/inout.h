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

#define INOUT_FORMAT_ERROR -1
#define INOUT_SUCCESS 0

void shell_write( shell *s, FILE *f);

#endif