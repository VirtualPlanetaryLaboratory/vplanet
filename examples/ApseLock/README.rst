Apsidal Locking of the CoRoT-7 System
=================

Overview
--------

Demonstration of apsidal locking, in which two planets' major axes
circulate with the same frequency after tidal damping.

===================   ============
**Date**              9/12/18
**Author**            David Fleming
**Modules**           DistOrb
                      EqTide
                      STELLAR
**Approx. runtime**   500 seconds
===================   ============

In a planetary system consisting of two or more planets and in which at least one
experiences damping, the orbits will evolve such that the eccentricity cycles stop
and the major axes evolve in lock stop. This "fixed point solution" `(Wu & Goldreich (2002) <https://ui.adsabs.harvard.edu/abs/2002ApJ...564.1024W/abstract>`_
is reproduced below for the case of CoRoT-7 b and c as examined by `Rodriguez et al. (2011) <https://ui.adsabs.harvard.edu/abs/2011CeMDA.111..161R/abstract>`_, see their Figs. 2 and 3.

To run this example
-------------------

.. code-block:: bash

    python makeplot.py <pdf | png>

Expected output
---------------

.. figure:: ApseLock.png
   :width: 100%
   :align: center

Secular dynamical evolution of CoRoT-7 b and c subject to tidal damping as
computed by **EqTide** and **DistOrb** based on Figures 2 and 3 from
`Rodriguez et al. (2011) <https://ui.adsabs.harvard.edu/abs/2011CeMDA.111..161R/abstract>`_.
Top left: Semi-major axis evolution of CoRoT-7 b.
Top Right: Eccentricity evolution for both CoRoT-7 b and c.  CoRoT-7 b's
eccentricity is initially excited by gravitational perturbations from
CoRoT-7 c, but eventually damps towards 0 due to tidal forces.  The inset
shows the slightly non-zero eccentricities for planet b and c near the end of the simulation. Lower left: Semi-major axis
evolution of CoRoT-7 c. Lower right: Difference between the longitudes of
pericenter of CoRoT-7 b and c.  In the inset, we display both planets
becoming apsidally locked within 3 Myr due to tidal damping.
