Solar System Dynamics
=====================

Overview
--------

===================   ============
**Date**              07/24/18
**Author**            Russell Deitrick
**Modules**           `distorb <../src/distorb.html>`_
                      `distrot <../src/distrot.html>`_
**Approx. runtime**   | 176 seconds (:code:`vpl.in`)
                      | 159 seconds (:code:`womoon/vpl.in`)
                      | 9 seconds (:code:`marshnb/vpl.in`)
                      | 87 seconds (:code:`marsvpl/vpl.in`)
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/dist_solsys2>`_
===================   ============

Uses DistOrb to model the orbits of the solar system planets. Also, uses
DistRot to model the obliquity evolution of Earth and Mars. In this main
directory, the precession of the Earth's spin axis is forced to its present
day value to emulate the effect of the moon. In the `womoon` directory,
the same simulation is run, but without this precessional forcing. The
directory `marsvpl` contains a simulation of Mars' obliquity backward in time
using DistOrb and DistRot. This is compared to `marshnb`, which utilizes
orbital data from HNBody (Rauch & Hamilton 2002) to demonstrate the effects
of secular resonances not resolved by DistOrb.


To run this example
-------------------

.. code-block:: bash

    # Run the main script
    vplanet vpl.in

    # Run the script without the Earth's moon
    cd womoon
    vplanet vpl.in
    cd ..

    # Run the Mars N-Body script
    cd marshnb
    vplanet vpl.in
    cd ..

    # Run the Mars vplanet script
    cd marsvpl
    vplanet vpl.in
    cd ..

    python plotsolsys.py


Expected output
---------------


.. figure:: InnerSolOrbs.png
   :width: 600px
   :align: center

   Eccentricity evolution (left) and inclination evolution (right) for the
   inner solar system planets over the next 1 Myr. Initial condition are taken
   from Appendix A of :cite:`MurrayDermott99`.


.. figure:: OuterSolOrbs.png
   :width: 600px
   :align: center

   Eccentricity evolution (left) and inclination evolution (right) for the
   outer solar system planets over the next 1 Myr. Initial condition are taken
   from Appendix A of :cite:`MurrayDermott99`.


.. figure:: ObliqComp.png
   :width: 600px
   :align: center

   Obliquity evolution of Earth with and without the Moon over the next Myr
   (left) and Mars using secular and N-body models for the orbit over the
   last 10 Myr (right).
