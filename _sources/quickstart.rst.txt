Quickstart
==========

.. contents:: :local:

Very Quick Start
----------------

To get started very quickly with one example:

.. code-block:: bash

  git clone https://github.com/VirtualPlanetaryLaboratory/vplanet.git
  cd vplanet
  make opt
  cd examples/VenusWaterLoss
  ../../vplanet vpl.in

Downloading the Code
--------------------

There are two ways to download VPLanet: 1) cloning via GitHub, or 2) downloading
the source of a specific release from GitHub.

To clone from GitHub, navigate to the directory on your local computer in which
you want to place :code:`vplanet` and type

.. code-block:: bash

  git clone https://github.com/VirtualPlanetaryLaboratory/vplanet.git

This creates a sub-directory called vplanet that contains all the files in the
repository.

To build from the source code, point your web browser to `releases <https://github.com/VirtualPlanetaryLaboratory/vplanet/releases>`_, 
select the release you want (we strongly recommend a version greater than or equal
to 1.0.0). Place the downloaded file in the directory on your local computer in which
you want to place :code:`vplanet` and type

.. code-block:: bash

  unzip vplanet-x.x.x

or

.. code-block:: bash

  tar xvfz vplanet-x.x.x.tar.gz

where "x.x.x" is the version number of the source you selected.


Compiling the Code
------------------

Change your working directory to vplanet to build the executable. :code:`vplanet`
is written in C, and the current version requires gcc. :code:`vplanet` has been
developed on MacOS and Linux operating systems.

In the top-level directory is a `Makefile` that will build the executable. Most
users should use the optimized version:

.. code-block:: bash

  make opt

You should now have a file called vplanet in the directory.

.. note::

  We recommend you add :code:`vplanet` to your PATH variable.



A Simple Example
----------------

Let's go over how to use :code:`VPLanet` to simulate the evolution of
a hypothetical ocean on the surface of early Venus. Isotopic evidence
suggests Venus may have had a similar amount of water to Earth in the
past :cite:`Donahue1982`, but because of vigorous hydrodynamic escape it probably lost all
of it in the first few hundred Myr :cite:`Hunten1973`. Here we're going to use the :doc:`stellar </src/stellar>`
and :doc:`atmesc </src/atmesc>` modules of :code:`VPLanet` to jointly model the evolution
of the Sun and Venus.

.. note::

  This guide shows how to interpret the :doc:`VenusWaterLoss </examples/VenusWaterLoss>`
  example, but here we will only include one planet, whereas the example uses three.
  Modifying that example to match the input files shown below will generate the
  figure at the end of this guide.

The basic workflow for a :code:`VPLanet` simulation is to create input
(:code:`.in`) files with the pertinent system, star, and planet parameters
and to call the executable directly from the command line. Let's go over
the three input files for this example.

The Input Files
---------------

The first input file, which is called the "primary input file," is for the system, which tells :code:`VPLanet` what bodies
are in the simulation and sets some configuration options. This file is usually
called :code:`vpl.in`, but you can actually call it whatever you'd like:

vpl.in
~~~~~~

.. code-block:: bash

    # General Options
    sSystemName   solarsystem        # System Name
    iVerbose      5                  # Verbosity level
    bOverwrite    1                  # Allow file overwrites?
    saBodyFiles   sun.in $           # List of all bodies in the system
                  venus.in

    # Input/Output Units
    sUnitMass      solar             # Options: gram, kg, Earth, Neptune, Jupiter, solar
    sUnitLength    AU                # Options: cm, m, km, Earth, Jupiter, solar, AU
    sUnitTime      YEARS             # Options: sec, day, year, Myr, Gyr
    sUnitAngle     d                 # Options: deg, rad

    # Input/Output
    bDoLog         1                 # Write a log file?
    iDigits        6                 # Maximum number of digits to right of decimal
    dMinValue      1e-10             # Minimum value of eccentricity/obliquity

    # Evolution Parameters
    bDoForward    1                  # Perform a forward evolution?
    bVarDt        1                  # Use variable timestepping?
    dEta          0.01               # Coefficient for variable timestepping
    dStopTime     4.6e9              # Stop time for evolution
    dOutputTime   1e6                # Output timesteps (assuming in body files)


First, the input parameters are specified with the name of the parameter
(e.g., :code:`dEta` or :code:`dStopTime`) followed by one or more spaces (or tabs)
and then the
value of the parameter. Note that parameter names are case-sensitive! The leading
lower case letter describes the type (cast) of parameter(s) to be provided: b = Boolean,
i = integer, d = double precision, s = string. If an "a" is appended to one of
these letters, then the input may be an "array," i.e. multiple values can be
input.

The order of the input options is irrelevant, and white space is ignored. Comments
can be specified anywhere with the pound (#) sign. Note that we divided the saBodyFiles
inputs into two lines for ease of reading. :code:`VPLanet` knows to look to the next line
because we used the line continuation character :code:`$`. The # and $ signs are
the only special characters in :code:`VPLanet` input files.

In this case, we set some fairly self-explanatory parameters. But here's a
line-by-line breakdown: We're calling the
system :code:`"solarsystem"` (our output files will have this prefix and any
plots generated with vplot will have this title). We specified maximum verbosity (:code:`5`),
so :code:`VPLanet` will talk a LOT. We're allowing output file overwrites,
and we're telling the code to expect two "body files": :code:`sun.in` and
:code:`venus.in`, which we'll create below. (Note that in
:doc:`VenusWaterLoss </examples/VenusWaterLoss>`
uses 3 planets, each representing a different amount of initial water content.)
Next, we set the default units for I/O: solar masses, astronomical units, years,
and degrees. Because we set them in the primary input file, the choices are
propagated to the body files, but a user can specify units for each body.

Next, we tell the code to generate a log file, set the
output precision, and define a tolerance parameter. The final section is probably
the most important: here we tell :code:`VPLanet` what to *actually* do
in the simulation. We want to evolve the system *forward* in time using
variable (adaptive) timestepping with a coefficient :code:`dEta = 0.01`. The
smaller this coefficient, the higher the precision of the integration, but
the slower it will run. We have found that values between 0.0001 and 0.1 work
for most cases. We then specify how long to run the simulation for
(the age of the solar system in our case) and how often to output (every
million years).

.. note::

    To get a list of all allowed
    parameters, their type (Boolean, integer, etc), a brief description, their
    associated modules, the files they may appear in (primary or body), and if
    the units may be changed with a negative sign (see below), type :code:`vplanet -h`
    in a terminal. You may also view the "long help" with the -H option, but not
    all parameters have this functionality yet.


In :code:`vpl.in` we specified two additional input files with the saBodyFiles
option, so let's look at them next. Here's :code:`sun.in`:

sun.in
~~~~~~

.. code-block:: bash

    # Star's Parameters
    sName           sun              # Body's name
    saModules       stellar          # Modules to apply, exact spelling required

    # Physical Parameters
    dMass           1.00             # Mass of the star in solar masses
    dAge            5e7              # Age in years at integration start

    # STELLAR Parameters
    sStellarModel   baraffe          # Stellar evolution model: `baraffe` or `none`
    dSatXUVFrac     1.e-3            # XUV luminosity fractional saturation level
    dSatXUVTime     1e8              # XUV saturation timescale in years

    # These are the parameters that vplanet will output as arrays in the
    # `.forward` or `.backward` evolution files. Run `vplanet -h` for a list
    # of all options. Note that the - sign is a request for custom units.
    saOutputOrder   Time -LXUVStellar

As before, the parameter names are fairly self-explanatory. Note that we're only
setting a few, and those that are not specified assume their default values.
Here we have a few differences with :doc:`VenusWaterLoss </examples/VenusWaterLoss>`:
The example assigns a hexadecimal color that can be used for plotting with `vplot`, and
uses the negative option for dSatXUVTime, which means the units are Gyr. For this
guide, we're running a shorter integration.


.. note::

    The units of some options can be changed by placing the minus sign ("-")
    character in front of the value. This symbol tells
    :code:`VPLanet` to use the custom units for the parameter.
    We'll see an example of this below. The onboard help
    provides information on the use of minus signs, but in general they are
    tailored to a Sun-Earth system.

We gave the star a name,
told :code:`VPLanet` we want to use the :code:`stellar` module to compute
its evolution. We assigned the mass and age at time = 0, and
set a few :code:`stellar`-specific properties. Specifically, we're using the
Baraffe et al. (2015) :cite:`Baraffe15` evolutionary tracks and the Ribas et al.
(2005) :cite:`Ribas05` XUV evolution power law with a saturation level of
:code:`1e-3` and timescale of 100 Myr. Note that `"time"` is different than
`"age"` in that the former is the internal counter for the simulation,
whereas the latter is the physical age of the star since some birth time. For a
compete description of the modules, please consult the :doc:`manual </Manual>`.

One of the most important :code:`VPLanet` options is saOutputOrder, which is a list
of all parameters to be output during the integration at a cadence defind be dOutputTime.
In this case we requested two columns: time and the XUV luminosity. (The example
outputs a few more parameters.) Unlike the option names, output names need only
be unique, but it's often easier to understand the output if the full name is provided.
Similar to the options, however, some output parameters (usually those that are
positive-definite) can be prepended with a minus sign to force the output into a
customized unit. This information can also be found in the help file.

Next up is the input file for the planet, Venus. This file is based off venus1.in
in :doc:`VenusWaterLoss </examples/VenusWaterLoss>`.

venus.in
~~~~~~~~

.. code-block:: bash

    # Planet's Parameters
    sName            venus           # Body's name
    saModules        atmesc          # Modules to apply, exact spelling required
    saOutputOrder    Time $
                     -SurfWaterMass $
                     -OxygenMantleMass

    # Physical Parameters
    dMass         -0.815             # Here, the - sign means Earth masses
    dRadius       -0.9499            # Here, the - sign means Earth radii
    dSemi         0.723              # Semi-major axis
    dEcc          0.006772           # Eccentricity

    # ATMESC Parameters
    dSurfWaterMass       -1.0        # Initial surface water in Earth oceans
    sWaterLossModel      lbexact     # Water loss model; Luger and Barnes (2015)
    bInstantO2Sink       1           # O2 is absorbed instantly at the surface
    sAtmXAbsEffH2OModel  bolmont16   # XUV absorption efficiency model


This looks pretty similar to the previous one, but it's worth noting
a few things. Note that we appear to have given the planet a **negative**
mass and radius! As we mentioned above, this actually tells
:code:`VPLanet` we're specifying these values using *different* units. Many
parameters have an associated customized unit that overrides the default units
specified in :code:`vpl.in`. In this case, dMass and dRadius have customized units
of Earth masses and Earth radii, respectively.

Finally, we set some :code:`atmesc`-specific parameters. We told the code
to initialize the planet with one Earth ocean (the minus sign, again, indicates
custom units) and to compute the water loss using the :code:`lbexact` model
from Luger and Barnes (2015) :cite:`LugerBarnes15`. Oxygen will be absorbed
at the surface instantly, and the XUV absorption efficiency will be calculated
from the Bolmont et al. (2016) :cite:`Bolmont16` model.


Running the Code
----------------

We are now ready to run the code:

.. code-block:: bash

    vplanet vpl.in


Upon running this in a terminal, you should see all sorts of messages printed
to the screen:


.. code-block:: bash

    INFO: sUnitMass set in vpl.in, all bodies will use this unit.
    INFO: sUnitTime set in vpl.in, all bodies will use this unit.
    INFO: sUnitAngle set in vpl.in, all bodies will use this unit.
    INFO: sUnitLength set in vpl.in, all bodies will use this unit.
    INFO: sUnitTemp not set in file sun.in, defaulting to Kelvin.
    INFO: sUnitTemp not set in file venus1.in, defaulting to kelvin.
    INFO: sUnitTemp not set in file venus2.in, defaulting to kelvin.
    INFO: sUnitTemp not set in file venus3.in, defaulting to kelvin.
    INFO: dRotPeriod < 0 in file sun.in, units assumed to be Days.
    INFO: dMass < 0 in file venus.in, units assumed to be Earth masses.
    INFO: dSemi < 0 in file venus.in, units assumed to be AU.
    INFO: dRadius < 0 in file venus.in, units assumed to be Earth radii.
    INFO: dRotPeriod < 0 in file venus.in, units assumed to be Days.
    INFO: dSurfWaterMass < 0 in file venus.in, units assumed to be Terrestrial Oceans (TO).
    INFO: dMinSurfWaterMass < 0 in file venus.in, units assumed to be Terrestrial Oceans (TO).
    INFO: dJeansTime not set for body venus, defaulting to 3.16e+16 seconds.
    Input files read.
    INFO: Age set in one file, all bodies will have this age.
    INFO: sOutFile not set, defaulting to solarsystem.sun.forward.
    INFO: sOutFile not set, defaulting to solarsystem.venus.forward.
    INFO: sIntegrationMethod not set, defaulting to Runge-Kutta4.
    INFO: dEnvelopeMass < dMinEnvelopeMass. No envelope evolution will be included.
    INFO: dEnvelopeMass < dMinEnvelopeMass. No envelope evolution will be included.
    INFO: dEnvelopeMass < dMinEnvelopeMass. No envelope evolution will be included.
    INFO: Radius of Gyration set for body 0, but this value will be computed from the grid.
    All of sun's modules verified.
    All of venus's modules verified.
    Input files verified.
    Log file written.

You can safely ignore most of this output: :code:`VPLanet` is just being very
verbose (as requested!) about what it's about to do. It is, however, a good
idea to peruse those messages to ensure you haven't made any mistakes! If
:code:`VPLanet` thinks you're doing something dubious,
it will output a WARNING and you should take care that you are comfortable with
your options. If you've input options that are known to be
incompatible, it will issue an ERROR, and provide you the file and lines numbers
that contain the issue(s), and exit. Note that if you did run the examples/VenusWaterLoss
case you will see more Venuses in the output.

Things will go silent for a couple seconds, and then you'll see:

.. code-block:: bash

    Evolution completed.
    Log file updated.
    Simulation completed.

The code is done running, and you should see several output files in the current directory.


The Output Files
----------------

The log file records the details of the simulation and captures a snapshot of the
system at the initial step and the final step of the evolution. Here's a
very condensed version of what you should see:

solarsystem.log
~~~~~~~~~~~~~~~

.. code-block:: bash

    Executable: vplanet
    Version: <GITVERSION>
    System Name: solarsystem
    Primary Input File: vpl.in
    Body File #1: sun.in
    ...

    ---- INITIAL SYSTEM PROPERTIES ----
    (Age) System Age [sec]: 1.577880e+15
    (Time) Simulation Time [sec]: 0.000000
    ...
    ----- BODY: sun ----
    Active Modules: STELLAR
    (Mass) Mass [kg]: 1.988416e+30
    ...
    ----- STELLAR PARAMETERS (sun)------
    (LXUVStellar) Base X-ray/XUV Luminosity [LSUN]: 0.000677
    Output Order: Time[year] LXUVStellar[LSUN]

    ----- BODY: venus ----
    Active Modules: ATMESC
    (Mass) Mass [kg]: 4.867332e+24
    ...
    ----- ATMESC PARAMETERS (venus)------
    (SurfWaterMass) Surface Water Mass [TO]: 1.000000
    ...
    Output Order: Time[year] SurfWaterMass[TO] OxygenMantleMass[bars]

    ---- FINAL SYSTEM PROPERTIES ----
    (Age) System Age [sec]: 1.467428e+17
    (Time) Simulation Time [sec]: 1.451650e+17
    ...
    ----- BODY: sun ----
    Active Modules: STELLAR
    (Mass) Mass [kg]: 1.988416e+30
    ...
    ----- STELLAR PARAMETERS (sun)------
    (LXUVFrac) X-ray/XUV Luminosity Fraction []: 8.892684e-06
    ...
    ----- BODY: venus ----
    Active Modules: ATMESC
    (Mass) Mass [kg]: 4.867332e+24
    ----- ATMESC PARAMETERS (venus)------
    (SurfWaterMass) Surface Water Mass [TO]: 0.000000
    (OxygenMantleMass) Mass of Oxygen in Mantle [bars]: 199.365415
    ...

Note that the log file list parameters in system units, SI. This choice allows
you to be sure that all the calculations will proceed correctly. Also note that
the log file contains the complete initial and final conditions. The first word
in many of the log file lines is in parentheses, which indicates the parameter
can be supplied to saOutputOrder.

Next, we have the **forward** evolution files, one per body. The
columns in these files correspond to names in the :code:`saOutputOrder`
option in the corresponding input files. Recall that for the
Sun, we requested that :code:`VPLanet` output the timestamp
and the XUV luminosity (in solar units since we used the minus sign):


solarsystem.sun.forward
~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

    0.000000     0.000677
    1.000000e+06 0.000677
    2.000000e+06 0.000678
    3.000000e+06 0.000678
    4.000000e+06 0.000678
    ...


You can check the units in the log file if you're unsure what they are. For the
planet, we asked for the time, the amount of surface water (with a minus
sign, indicating in units of Earth oceans), and the amount of oxygen absorbed by
the mantle (with a minus sign, indicating units of bars):


solarsystem.venus.forward
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

    0.000000     1.000000 0.000000
    1.000000e+06 0.978763 4.238860
    2.000000e+06 0.957524 8.477719
    3.000000e+06 0.936283 12.716579
    4.000000e+06 0.915044 16.955439
    ...


Plotting
--------

With the output generated, it is often convenient to plot the output. While any plotting
package can be used, the VPLanet team has created a customized tool that enables
both fast plotting of output, as well as tools to generate publication worthy figures.
The :code:`vplot` tool (:doc:`docs here <vplot>`) can be used to easily visualize the results of
any :code:`VPLanet` simulation. If you run

.. code-block:: bash

    vplot


in a terminal, you should see the following plot appear:

.. figure:: quickstart.png
   :width: 300px
   :align: center


Here we see the evolution of the stellar XUV emission, which declines
dramatically after the saturation timescale ends (top); the increase
in the amount of oxygen in the planet's mantle, which is absorbed from
the oxygen released from the photolysis of water (center); and
the desiccation of the planet's surface, caused by the hydrodynamic escape of
hydrogen to space. In this simulation, Venus loses all of its surface water
in about 50 Myr.

Next Steps
----------

The above example describes one example, but :code:`VPLanet` can simulate many
more phenomena. Navigate to the :doc:`examples <examples>` directory to see the
physics that are currently available. Each example provides instructions on how
to generate output and a figure.
