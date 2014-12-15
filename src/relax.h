/*!*******************************************************************
 * \author Jef Wagner
 * \email jefwagner@gmail.com
 *********************************************************************
 */
/*!
 * This file contains the calculation of the elastic energy of the
 * shell.
 */

#ifndef JW_RELAX
#define JW_RELAX

#include "main.h"

int relax_total( shell_run *sr, shell *s);
int relax_partial( shell_run *sr, int center);

#endif