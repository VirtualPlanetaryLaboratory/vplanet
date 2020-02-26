Tidal/Atmospheric/Internal Evolution of Proxima b into a "Habitable Evaporated Core."
=====================================================================================

Overview
--------

Each subdirectory contains a simulation of the long-term tidal evolution of Proxima Centauri b.
In cpl, we model tidal evolution with eqtide and the stellar evolution of the host star using stellar.
In no_ocean, we generalize the cpl runs by additionally simulating the planetary thermal interior by including radheat
and thermint.  In ocean, we generalize the ocean case by modeling the tidal impact of liquid surface oceans.
Finally, in env_ocean, we generalize the ocean case by modeling the tidal impact and atmospheric escape on
Proxima Centauri b if it initially formed with an extended hydrogen envelope. These simulations
reproduce Figure 25 from Barnes et al. (2016).


===================   ============
**Date**              8/17/2018
**Author**            David Fleming
**Modules**           atmesc
                      eqtide
                      radheat
                      stellar
                      thermint
**Approx. runtime**   3 minutes
===================   ============

To run this example
-------------------

.. code-block:: bash

    vplanet vpl.in

Repeat process for each sub-directory: HEC/cpl, HEC/no_ocean, HEC/ocean, HEC/env_ocean.

Once the simulations are finished, run

.. code-block:: bash

    python makeplot.py < png | pdf >


Expected output
---------------

.. figure:: HEC.png
   :width: 600px
   :align: center

   Evolution of the orbital, tidal and atmospheric properties of Proxima Centauri b for the “CPL” case in light blue, “No Ocean” case in
   red, “Ocean” case in dark blue, and the “Envelope” case in orange, with the dashed line for clarity. See Barnes et al. (2016), Section 4.6
   for a detailed description of the initial conditions. The grey shaded region indicates when the planet is in the habitable zone.
   Top left: Surface Flux. Top right: Tidal Q. Middle left: Orbital Eccentricity. Middle right: Semi-major Axis.
   Bottom left: Envelope Mass. Bottom right: Surface Water Mass.
