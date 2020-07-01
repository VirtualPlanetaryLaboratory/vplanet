VSPACE
------
A quick way to generate a parameter space for ``VPLanet``.

Installation
============
Navigate to the vspace directory and run ``python setup.py``. All this does is put some 
path info in your shell files.

Examples
========
Run vspace on the command line by typing:

    vspace <input file>

An input file is necessary. Take a look at the ``input`` file in the examples directory. 
This file will contain a list of files to be copied (from some source directory to a 
destination directory) and all the ``VPLanet`` options you wish to change or add to each file. 

You will need to provide a directory that contains template/example input files (such as 
any directory in vplanet/examples) in the following syntax:
  
    srcfolder <path of directory>
    
Additionally, you will need to provide the path of a destination directory. This is where
your suite of initial conditions will live:
  
    destfolder <path of directory>
    
Then, provide a list of the files you wish to be copied and used by ``VPLanet``, and all 
options you wish to change, in the following format:
    
    file <name of file in srcfolder>
    <option name> <value>
    <option name> <value>
    ...
    
    file <another file in srcfolder>
    <option name> <value>
    ...
    
Notice that each file is given a separate line starting with ``file``. All options below
that up to the next instance of ``file`` will belong to the previous entry. 

Finally, it is advisable to provide a prefix for the generated directories: 

    trialname <some descriptive name for your simulations>
    
This defaults to ``default``. As an example, if you generate 3 trials, with the default 
trialname, these will be number like so:
    
    default0/
    default1/
    default2/

You can additionally remove options from a source file by the following syntax:
  
    rm <option name>
    
For the time being, this merely comments out the matching line.
    
How to generate a parameter space
=================================
VSPACE has two sampling modes: ``grid`` and ``random``, which are specified with the word
``samplemode``. For example:
    
    samplemode random

will allow you to generate trials from a random distribution. 

GRID MODE:
To generate trials that explore a gridded parameter space, simply provide a range of values for 
the desired option instead of a single value. These should have 3 entries, surrounded by
square brackets ``[]``, in the following manner:
  
    <option name> [start, end, spacing] <prefix>
    
(Here, ``prefix`` is a string that will be used in the naming of the output directories. 
More on this below.) For example, to generate trials changing ``dSemi`` from 1 to 2 with 
spacings of 0.1:
  
    dSemi  [1, 2, 0.1]  a
    
Alternatively, you can provide the number of trials instead of the spacing by putting an
``n`` before the 3rd numerical entry. For example,

    dSemi  [1, 2, n10]  a
    
would generate ten trials, equally spaced, from 1 to 2. You can also change to 
logarithmic spacing by using ``l`` instead of ``n``:
  
    dSemi  [1, 2, l10]  a
    
would generate ten trials, logarithmically spaced, from 1 to 2. 

Minus options are allowed, but if you are providing the spacing, rather than using the 
``n`` or ``l`` option, either provide a negative spacing or swap the start and end values.
Use:

    dRadius  [-1, -2, -0.1]  R

or,

    dRadius  [-2, -1, 0.1]  R
    
rather than ``dRadius [-1, -2, 0.1]  R``. VSPACE will not check whether a minus option is 
allowed by ``VPLanet``, so make sure you check ``vplanet -h``. 

You can vary more than one parameter at a time! For example, in your input file you might 
have,

    dSemi  [1, 2, n10]  a
    dEcc   [0.1, 0.2, n10]  e
    
would generate initial conditions for every possible combination of ``dSemi`` and 
``dEcc``, resulting in a total of 100 trials. You can do this for as many parameters as 
you like, so you have the power to generate a large number of files very quickly. Use this
wisely: test with small numbers first to ensure that files end up in the correct locations
and that initial conditions are indeed output with the desired values.  

RANDOM MODE:
The syntax for generating randomly sampled data is similar to grid mode, with a few 
noteable differences. If setting ``samplemode random``, one MUST also set the variable 
``randsize``, to an integer value--this is the number of samples/trials. 

Additionally, it is good practice to seed the random number generator, to allow for 
more easily reproducible results. This is accomplished with the variable ``seed``:
  
    seed <some integer>
    
The options for random mode are set in the same way as grid mode, in the third item
in the square brackets. The current options are ``g`` (Gaussian/normal distribution), 
``u`` (uniform distribution), ``s`` (uniform sine distribution), or ``c`` (uniform 
cosine distribution). 

For Gaussian/normal distributions, the syntax is as below:
   
    <option name> [<mean>, <width/stdev>, g] <prefix>
    
An example would be:
    
    dEcc  [0.1, 0.01, g]  e
    
For some parameters, you may want to truncate the distribution at certain values, 
for example, dEcc should not be < 0 or > 1. You can provide cutoffs with the ``g`` option
as the 4th or 5th item in ``[]`` with the keyword ``min`` or ``max``, like so:
    
    dEcc  [0.1, 0.01, g, min0.0, max1.0]  e

You do not need to provide both min and max, if you need only one, and the order does
not matter.

A uniform distribution is sampled like so:
  
    <option name> [<low>, <high>, u] <prefix>
    
For angles, you may want to sample the sine or cosine of the angle uniformly, rather 
than sampling the angle itself uniformly. You can accomplish this with ``s`` or ``c``,
for sine and cosine respectively:
  
    <option name> [<low angle>, <high angle>, s] <prefix>
    <option name> [<low angle>, <high angle>, c] <prefix>

Note that <low angle> and <high angle> should be the min and max values of the angle
ITSELF, not the sine or cosine of the angle. VSPACE does the conversion to and from 
trig output and angle. The units of the angle can be either radians or degrees, but
must be consistent with your source ``vpl.in`` file--VSPACE determines the unit from 
this file. 
