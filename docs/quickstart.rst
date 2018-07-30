Quickstart
==========

sun.in
------

.. code-block:: bash

    # VPLANET Parameters
    sName           sun              # Body's name
    saModules	    stellar          # Modules to apply, exact spelling required
    saOutputOrder   Age $            # The parameters that vplanet will output
                    -Luminosity $    # as arrays in the `.forward` or
                    -LXUVStellar     # `.backward` evolution files. Run
                    -Radius $        # `vplanet -h` for a list of all options.
                    Temperature      # The - sign is a request for custom units.

    # Physical Parameters
    dMass           1.00             # Mass of the star in solar masses
    dAge            5e7              # Age in years at integration start

    # STELLAR Parameters
    sStellarModel   baraffe          # Stellar evolution model: `baraffe` or `none`
    dSatXUVFrac     1.e-3            # XUV luminosity fractional saturation level
    dSatXUVTime     1e8              # XUV saturation timescale un years


venus.in
--------

.. code-block:: bash


    # VPLANET Parameters
    sName            venus           # Body's name
    saModules        atmesc          # Modules to apply, exact spelling required
    saOutputOrder    Time $
                     -SurfWaterMass $
                     -RGLimit $
                     -OxygenMantleMass

    # Physical Parameters
    dMass         -0.815             # Here, the - sign means Earth masses
    dRadius       -0.9499            # Here, the - sign means Earth radii
    dSemi         -0.723             # Semi-major axis, negative -> AU
    dEcc          0.006772           # Eccentricity

    # ATMESC Parameters
    dSurfWaterMass       -1.0        # Initial surface water in Earth oceans
    sWaterLossModel      lbexact     # Water loss model; Luger and Barnes (2015)
    bInstantO2Sink       1           # O2 is absorbed instantly at the surface
    sAtmXAbsEffH2OModel  bolmont16   # XUV absorption efficiency model


vpl.in
------

.. code-block:: bash

    # General Options
    sSystemName   solarsystem        # System Name
    iVerbose      5                  # Verbosity level
    bOverwrite    1                  # Allow file overwrites?
    saBodyFiles   sun.in $           # List of all bodies in the system
                  venus.in

    # Input/Output Units
    sUnitMass      solar             # Options: gram, kg, Earth, Neptune, Jupiter, solar
    sUnitLength    aU                # Options: cm, m, km, Earth, Jupiter, solar, AU
    sUnitTime      YEARS             # Options: sec, day, year, Myr, Gyr
    sUnitAngle     d                 # Options: deg, rad

    # Input/Output
    bDoLog         1                 # Write a log file?
    iDigits        6                 # Maximum number of digits to right of decimal
    dMinValue      1e-10             # Minimum value of eccentricity/obliquity

    # Evolution Parameters
    bDoForward    1                  # Perform a forward evolution?
    bVarDt        1                  # Use variable timestepping?
    dEta          0.1                # Coefficient for variable timestepping
    dStopTime     4.6e9              # Stop time for evolution
    dOutputTime   1e5                # Output timesteps (assuming in body files)
