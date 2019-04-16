Evolution of a Wide Stellar Binary due to Galactic Effects
================

Overview
--------

Orbital evolution of objects in wide (>10,000 AU) orbits due to galactic migration,
the galactic tide, and passing field stars.

===================   ============
**Date**              07/25/18
**Author**            Russell Deitrick
**Modules**           GalHabit
**Approx. runtime**   6 minutes
===================   ============

Wide orbits are subject to torques from the galactic tide, as well as impulses from
passing stars. Complicating the evolution is the possibility of migration, in which
stars can migrate multiple kpc from their birth location. This example shows that
evolution for an M dwarf orbiting a Sun-like star.

To run this example
-------------------

.. code-block:: bash

    # Run the main example
    vplanet vpl.in

    # Run the `tides_only` example
    cd tides_only
    vplanet vpl.in
    cd ..

    # Plot the figure
    python makeplot.py <pdf | png>


Expected output
---------------

.. figure:: GalaxyEffects.png
   :width: 600px
   :align: center

   Evolution of an M dwarf orbiting the sun under the influence of the galactic
   environment. The blue curves represent the evolution due to the galactic tide
   alone, while the black includes the effects of random stellar encounters. The
   dash curves are the semi-major axis (unaffected by the tide); the solid curves
   are the perihelion distance.
