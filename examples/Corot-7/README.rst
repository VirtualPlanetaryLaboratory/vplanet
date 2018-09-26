Orbital Damping in the CoRoT-7 System
=================

Overview
--------

===================   ============
**Date**              9/12/18
**Author**            David Fleming
**Modules**           `distorb <../src/distorb.html>`_
                      `eqtide <../src/eqtide.html>`_
                      `stellar <../src/stellar.html>`_
**Approx. runtime**   500 seconds
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/corot7>`_
===================   ============

Using vplanet's distorb, eqtide, and stellar modules to simulate the tidal damping
and apsidal locking of CoRoT-7 b and c examined by Rodriguez et al. (2011).

To run this example
-------------------

.. code-block:: bash

    vplanet vpl.in
    vplot

Expected output
---------------

.. figure:: Rodriguez2011_Figs23.png
   :width: 300px
   :align: center

   Secular dynamical evolution of CoRoT-7b and c subject to tidal damping as
   computed by eqtide and distorb based on Figures 2 and 3 from
   Rodriguez et al. (2011). Top left: Semi-major axis evolution of CoRoT-7b.
   Top Right: Eccentricity evolution for both CoRoT-7b and c.  CoRoT-7b's
   eccentricity is initially excited by gravitational perturbations from
   CoRoT-7c, but eventually damps towards 0 due to tidal forces.  The inset
   shows the slightly non-zero eccentricities for planet b and c decaying
   towards 0 near the end of the simulation. Lower left: Semi-major axis
   evolution of CoRoT-7c. Lower right: Difference between the longitudes of
   pericenters of CoRoT-7b and c.  In the inset, we display both planets
   becoming apsidally locked within 3 Myr due to tidal damping.
