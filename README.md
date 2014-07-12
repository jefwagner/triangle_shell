Triangle shell
==============

This is a program that simulations the growth of a shell of triangles
through the sequential addition of triangles to a growing edge. There
are a number of parameters that can be set at runtime through a
parameter file. The program is invoked through the command line with
the command:

```
$> ./shell filename [simulation_number]
```

The first argument after the program name (here `shell`) should be a
parameter filename. The second argument is optional, and is an
integer that gives the simulation number (explained below). 

An example parameter file is:
```
# Comment line
# Foppl von Karman number
var gamma 2
# A list of values for the spontaneous radius of curvature
list r_0 0.925 0.95 0.975 1.0 1.025 1.05
```
The character `#` specifies a comment and runs to the end of the
line. A parameter is specified with the `var` keyword followed by the
name of the parameter followed by its value. A list of parameter
values is specified by the `list` keyword followed by the name of the
parameter followed by a list of the values separated by whitespace
(spaces, tabs, or line breaks). The parameters that can be set are:
 
 1. `gamma` : the Foppl von Karman number,
 2. `r_0` : the spontaneous radius of curvature,
 3. `sigma` : the width of the Gaussian random choice,
 4. `delta_b` : the distance at which two edges are joined,
 5. `r_membrane` : the radius of the surrounding membrane,
 6. `r_genome` : the radius of genome,
 7. `delta_b` : the distance at which two edges are joined.

The second (optional) argument is the simulation number. If a list of
values are given for one or more parameters, then the simulation
number chooses which values used. The simulation number can range
from 0 and goes through all combination, where the first parameter
listed above (`gamma`) changes most rapidly, the second parameter
(`r_0`) next most rapidly , and on down through all parameters in the
order listed above. For example, if the parameter file had two values
for `gamma`, `r_0`, and `sigma` each
```
list gamma 1 2
# note that sigma and r_0 are reversed from that listed above
list sigma 1.e-4 30
list r_0 1.0 1.5
```
then there are \f$2^3=8\f$ total options, and the simulation number
can range from 0 to 7. Specifically, the table below gives the values
taken on for each of the simulation numbers.

| `simulation_number` | {`gamma`, `r_0`, `sigma`} |
|:-------------------:|:-------------------------:|
|          0          |      {1, 1.0, 1.e-4}      |
|          1          |      {2, 1.0, 1.e-4}      |
|          2          |      {1, 1.5, 1.e-4}      |
|          3          |      {2, 1.5, 1.e-4}      |
|          4          |        {1, 1.0, 30}       |
|          5          |        {2, 1.0, 30}       |
|          6          |        {1, 1.5, 30}       |
|          7          |        {2, 1.5, 30}       |
