Quickstart
==========

.. contents:: :local:


A simple example
----------------

Let's go over how to use :code:`VPLANET` to simulate the evolution of
a hypothetical ocean on the surface of early Venus. Isotopic evidence
suggests Venus may have had a similar amount of water to Earth in the
past, but because of vigorous hydrodynamic escape it probably lost all
of it in the first few hundred Myr. Here we're going to use the :code:`stellar`
and :code:`atmesc` modules of :code:`VPLANET` to jointly model the evolution
of the Sun and Venus.

The basic workflow for a :code:`VPLANET` simulation is to create input
(:code:`.in`) files with the pertinent system, star, and planet parameters
and to call the executable directly from the command line. Let's go over
the three input files for this example.

The input files
---------------

The first input file is for the system, which tells :code:`VPLANET` what bodies
are in the simulation and sets some configuration options. This file is usally
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
    dEta          1                  # Coefficient for variable timestepping
    dStopTime     4.6e9              # Stop time for evolution
    dOutputTime   1e6                # Output timesteps (assuming in body files)


First off, before we delve in: input parameters are specified with the name
of the parameter (:code:`dEta` or
:code:`dStopTime`) followed by one or more spaces (or tabs) and the
value of the parameter. Note that parameter names are case-sensitive! Comments
can be specified anywhere with the pound sign.

In this case, we set some fairly self-explanatory parameters.
But here's a line-by-line breakdown.
We're calling the
system :code:`"solarsystem"` (our output files will have this prefix and any
plots will have this title). We specified maximum verbosity (:code:`5`),
so :code:`VPLANET` will talk a LOT. We're allowing output file overwrites,
and we're telling the code to expect two body files: :code:`sun.in` and
:code:`venus.in`, which we'll create below. Next, we set the default units
for I/O: solar masses, astronomical units, years, and degrees.

.. note::

    It isn't always convenient to have the same units for all bodies.
    For instance, it's kind of a pain to input planet masses in solar
    masses, or the other way around. Fortunately, the default unit can
    be overriden with the minus sign ("-") character, which tells
    :code:`VPLANET` to assume custom units for the parameter in question.
    We'll see an example of this below.

Moving along, we tell the code to spit out a log file at the end, set the
output precision and a tolerance parameter. The final section is probably
the most important: here we tell :code:`VPLANET` what to *actually* do
in the simulation. We want to evolve the system *forward* in time using
variable (adaptive) timestepping with a coefficient :code:`dEta = 1`. The
smaller this coefficient, the higher the precision of the integration (but
the slower it will run). We then specify how long to run the simulation for
(the age of the solar system in our case) and how often to output (every
million years).

Ok, so we told :code:`VPLANET` to expect two additional input files.
Here's :code:`sun.in`:

sun.in
~~~~~~

.. code-block:: bash

    # VPLANET Parameters
    sName           sun              # Body's name
    saModules       stellar          # Modules to apply, exact spelling required

    # These are the parameters that vplanet will output as arrays in the
    # `.forward` or `.backward` evolution files. Run `vplanet -h` for a list
    # of all options. Note that the - sign is a request for custom units.
    saOutputOrder   Time -LXUVStellar

    # Physical Parameters
    dMass           1.00             # Mass of the star in solar masses
    dAge            5e7              # Age in years at integration start

    # STELLAR Parameters
    sStellarModel   baraffe          # Stellar evolution model: `baraffe` or `none`
    dSatXUVFrac     1.e-3            # XUV luminosity fractional saturation level
    dSatXUVTime     1e8              # XUV saturation timescale in years


As before, the parameters are fairly straightforward. Note that we're only
setting a few, and those that are not
specified assume their default values.

.. note::

    To get a list of all the allowed
    parameters and their default values, type :code:`vplanet -h` in a terminal.

We gave the star a name,
told :code:`VPLANET` we want to use the :code:`stellar` module to compute
its evolution, requested that the code output the XUV luminosity at each
step, gave it a mass and an age, and set a few :code:`stellar`-specific
properties. Specifically, we're using the Baraffe et al. (2015) :cite:`Baraffe15`
evolution tracks and the Ribas et al. (2005) :cite:`Ribas05` XUV evolution power law
with a saturation level of :code:`1e-3` and timescale of 100 Myr.

Next up is the input file for the planet, Venus:

venus.in
~~~~~~~~

.. code-block:: bash

    # VPLANET Parameters
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
a few things. First, because the arguments for :code:`saOutputOrder` were pretty lengthy,
we split the input over multiple lines with the line continuation character
:code:`$`. Second, note that we appear to have given the planet a **negative**
mass and radius! As we mentioned above, this actually tells
:code:`VPLANET` we're specifying these values using *custom* units. Every
parameter has an associated custom unit that overrides the default units
specified in :code:`vpl.in`. In this case, we're using Earth units for the mass
and radius.

Finally, we set some :code:`atmesc`-specific parameters. We told the code
to initialize the planet with one Earth ocean (the minus sign, again, indicates
custom units) and to compute the water loss using the :code:`lbexact` model
from Luger and Barnes (2015) :cite:`LugerBarnes2015`. Oxygen will be absorbed
at the surface instantly, and the XUV absorption efficiency will be calculated
from the Bolmont et al. (2016) :cite:`Bolmont16` model.


Running the code
----------------

We are now ready to run the code:

.. code-block:: bash

    vplanet vpl.in


Upon running this in a terminal, you should see all sorts of messages printed
to the screen:


.. code-block:: bash

    WARNING: sUnitMass set in vpl.in, all bodies will use this unit.
    WARNING: sUnitTime set in vpl.in, all bodies will use this unit.
    WARNING: sUnitAngle set in vpl.in, all bodies will use this unit.
    WARNING: sUnitLength set in vpl.in, all bodies will use this unit.
    WARNING: sUnitTemp not set in file sun.in, defaulting to Kelvin.
    WARNING: sUnitTemp not set in file venus.in, defaulting to kelvin.
    WARNING: dMass < 0 in file venus.in, units assumed to be Earth masses.
    WARNING: dSemi < 0 in file venus.in, units assumed to be AU.
    WARNING: dRadius < 0 in file venus.in, units assumed to be Earth radii.
    WARNING: dSurfWaterMass < 0 in file venus.in, units assumed to be Terrestrial Oceans (TO).
    Input files read.
    WARNING: sOutFile not set, defaulting to solarsystem.sun.forward.
    WARNING: sOutFile not set, defaulting to solarsystem.venus.forward.
    WARNING: solarsystem.sun.forward exists.
    WARNING: solarsystem.venus.forward exists.
    WARNING: sIntegrationMethod not set, defaulting to Runge-Kutta4.
    WARNING: No rotational information set in file sun.in. Defaulting to dRotRate = 2*pi/day.
    WARNING: No rotational information set in file venus.in. Defaulting to dRotRate = 2*pi/day.
    All of sun's modules verified.
    All of venus's modules verified.
    Input files verified.
    Log file written.


You can safely ignore all these warnings: :code:`VPLANET` is just being very
verbose (as requested!) about what it's about to do. It is, however, a good
idea to peruse those messages to ensure you're using the correct units for
the parameters!

Things will go silent for a couple seconds, and then you'll see:

.. code-block:: bash

    Evolution completed.
    Runtime = 3.000000 s
    Log file updated.

The code is done running, and you should see several output files in the current directory.


The output files
----------------

The log file records the details of the simulation and captures a snapshot of the
system at the initial step and the final step of the evolution. Here's a
very condensed version of what you should see:

solarsystem.log
~~~~~~~~~~~~~~~

.. code-block:: bash

    Executable: vplanet
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

    Runtime = 3.000000 s
    Total Number of Steps = 10776


Next, we have the **forward** evolution files, one per body. The
columns in these files correspond to the :code:`saOutputOrder`
parameters in the corresponding input files. Recall the for the
Sun, we requested that :code:`VPLANET` output the timestamp
and the XUV luminosity:


solarsystem.sun.forward
~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

    0.000000     0.000677
    1.000000e+06 0.000677
    2.000000e+06 0.000678
    3.000000e+06 0.000678
    4.000000e+06 0.000678
    ...


For the planet, we asked for the timestamp, the amount of surface
water (with a minus sign, indicating in units of Earth oceans), and
the amount of oxygen absorbed by the mantle (in bars):


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

The :code:`vplot` tool (:doc:`docs here <vplot>`) can be used to easily visualize the results of
any :code:`VPLANET` simulation. If you run


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
