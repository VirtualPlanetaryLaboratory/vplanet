VSPACE
------
``VSPACE`` is a tool to build input files for a parameter sweep with ``VPLanet``.

Overview
========
With ``VSPACE`` you can quickly and easily build input files with specific
parameters with a specific type of distribution. In **Grid Mode** you can build
input files in which the initial conditions have regular spacings within specified
limits and with either linear or logarithmic spacings. In **Random Mode** the
distributions are random, but can be **uniform, Gaussian** or uniform in *sine**
or **cosine**. Non-uniform distributions can be easily truncated, if necessary.
Histograms of the initial conditions will also be built. After generating the
trials, use the `_multi-planet <../multi-planet>`_ script to run.

Installation
============
Navigate to the vspace directory and run ``python setup.py``. This command places
some path info in your shell files.

Running VSPACE
==============
Run ``VSPACE`` on the command line by typing:

.. code-block:: bash

    vspace <input file>

where the input file contains a set of instructions to build the simulations. This
will create a subdirectory containing a) additional subdirectories for each trial,
b) histograms of each varied parameter, and c) a log file containing the value
of each varied parameter in the trial subdirectories.

The Input File
==============
The input file contains a list of files to be copied (from some source directory to a
destination directory) and all the ``VPLanet`` options you wish to change or add to each file.
An example input file, called ``vspace.in``, is included in this directory, its
lines are described below and is based off the `<../examples/IoHeat> IoHeat example`.

The first line is

    srcfolder <path-to-template-files>

This provides ``VSPACE`` with the location of a directory that contains template
``VPLanet`` input files, such as vpl.in, star.in, etc. The format of the input files
is slightly different when used with ``VSPACE`` as described below.

The next line is

    destfolder <path-to-output>

which is where your suite of initial conditions will live. **NOTE**: ``VSPACE``
will overwrite previously generated files!

The next line is

    trialname  <prefix>

which is a prefix for each case in the parameter sweep. The trialname defaults
to ``default`` if this command is not included.

With these top-level commands executed, the remaining lines describe how the
individual parameters are to be varied, and what the names of the trial directories
will be. In the example ``vspace.in`` file, these lines are

    file   jupiter.in

    file   io.in
    dEcc  [0.001,0.005,n5] ecc
    dObliquity [0,10,n5] obl

    file   vpl.in

The general syntax for these lines are:

    file <name>
    <option> <sampling rules> <identifier>
    <option> <sampling rules> <identifier>
    ...

where <name> is the name of the input file. The next lines describe how individual
options are varied. <option> is the name of the ``VPLanet``
input option (exact match required), the <sampling rules> describe the manner in
which the parameter is distributed (described in more detail below in the ``Sampling
Rules`` section), and <identifier> is a string that is appended to the trialname
prefix in the destfolder subdirectories. ``VSPACE`` will vary all parameters listed
after a "file" command until it reaches the next "file" command or the end of the
file.

This example case would create directories with names like

    test/trial_ecc0obl0

with files jupiter.in, io.in, and vpl.in that would be identical to those files
in the srcfolder **except** dEcc and dObliquity would have values that follow the
sampling rules in io.in.

Once the directories have been created, they can all be executed with a single command
using the `_multi-planet <../multi-planet>`_ script.

You can additionally remove options from a source file with the following command:

    rm <option name>

For the time being, this merely comments out the matching line.

Sampling Rules
==============
``VSPACE`` has two sampling modes: **grid** and **random**, which are specified with the word
"samplemode". For example:

    samplemode random

will allow you to generate trials that are randomly distributed.

**GRID MODE:**
``VSPACE`` allows for 3 submodes to generate trials that explore a gridded parameter
space, i.e even spacing. These submodes are **explicit**, **linear**, and
**logarithmic**. Each follows the following syntax:

    <option> [start, end, spacing] <identifier>

In all modes the "start" and "end" values represent the limits of the parameter
to be surveyed and are inclusive of the end points.

**Explicit Submode**: In this grid submode, the
"spacing" value is just a number that represents the interval in between trials.
``VSPACE`` will create as many trials as necessary to follow the sampling rules,
and will not necessarily include a trial at the end value. For example, to
generate trials that vary ``dSemi`` from 1 to 2 with a spacing of 0.1, the syntax
is:

    dSemi  [1, 2, 0.1]  a

**Linear Submode**: To sample the grid linearly with a specific number of trials
that are evenly spaced, change the spacing to be an "n" followed
by an integer that represents the number of values to generate. For example, the
previous example could be rewritten as

    dSemi  [1, 2, n11]  a

which would generate 11 trials, equally spaced, from 1 to 2, i.e. every 0.1.

**Logarithmic Submode**: To change the spacing to be logarithmic, use "l" instead
of "n":

    dSemi  [1, 1000, l10]  a

which would generate ten trials, logarithmically spaced, from 1 to 1000.

**Pro Tips**: Minus options are allowed, but if you are providing the spacing,
rather than using the "n" or "l" option, either provide a negative spacing or
swap the start and end values. For example:

    dRadius  [-1, -2, -0.1]  R

or,

    dRadius  [-2, -1, 0.1]  R

rather than ``dRadius [-1, -2, 0.1]  R``. **NOTE:** ``VSPACE`` will NOT check
whether a minus option causes ``VPLanet`` to change the units, so make sure you check
``vplanet -h``. If you use negative values for a parameter that has alternate
units for a negative option, the outcome will most likely be wrong!

As described above, you can vary more than one parameter at a time. While this
can be very useful, **you have the power to generate a large number of files very
quickly**. Use this wisely: test with small numbers first to ensure that files end
up in the correct locations and that initial conditions are indeed output with
the desired values (check the histograms).

**RANDOM MODE:**
The random mode contains four submodes: **uniform**, **Gaussian**, **sine** and
**cosine**. The syntax for generating randomly sampled data is similar to grid
mode, with a few noteable differences. In random mode, one MUST set the variable
``randsize`` to an integer value that is the number of trials:

    randsize <number of trials>

Additionally, it is good practice to seed the random number generator, to allow for
more easily reproducible results. This initialization is accomplished with the
variable ``seed``:

    seed <some integer>

**Uniform Submode**: A uniform distribution is sampled like so:

        <option> [<min>, <max>, u] <prefix>

where <min> and <max> are the limits.

**Gaussian Submode**: For Gaussian/normal distributions, the syntax is:

    <option> [<mean>, <width/stdev>, g] <prefix>

An example would be:

    dEcc  [0.1, 0.01, g]  e

For some parameters, you may want to truncate the distribution at certain values,
for example, dEcc should not be < 0 or > 1. You can provide cutoffs with 4th and/or
5th arguments in the brackets with the keywords "min" or "max":

    dEcc  [0.1, 0.01, g, min0.0, max1.0]  e

You do not need to provide both min and max, if you need only one, and the order does
not matter.

**Sine and Cosine Submodes**: For angles, you may want to sample the sine or cosine
of the angle uniformly, rather than sampling the angle itself uniformly. You can
accomplish this with ``s`` or ``c``, for sine and cosine respectively:

    <option name> [<low angle>, <high angle>, s] <prefix>
    <option name> [<low angle>, <high angle>, c] <prefix>

Note that <low angle> and <high angle> should be the min and max values of the ANGLE,
not the sine or cosine of the angle. ``VSPACE`` does the conversion to and from
trig output and angle. **NOTE**: The units of the angle can be either radians or degrees, but
must be consistent with your source ``vpl.in`` file -- ``VSPACE`` determines the unit from
this file.

Template Files
==============
The template files are nearly identical to standard ``VPLanet`` input files except
that they should not include the parameters to be varied.
