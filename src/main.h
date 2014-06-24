/*!*******************************************************************
 * \author Jef Wagner
 * \email jefwagner@gmail.com
 *********************************************************************
 */
/*!
 * This file contains objects for running the program.
 */
#ifndef JW_MAIN
#define JW_MAIN

typedef struct {
  double gamma;
  double th0;
  double sigma;
  double r_membrane;
  double r_genome;
  double delta_b;
  unsigned int seed;
} shell_params;

typedef struct {
  shell *s;
  shell_params *sp;
} shell_run;

#endif