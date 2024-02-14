Tidal Locking of the Planet Candidate Gl 581 d
==========

Overview
--------

This example shows tidal locking for the planet candidate Gl 581 d.

===================   ============
**Date**              04/09/2019
**Author**            Rory Barnes
**Modules**           EqTide
**Approx. runtime**   10 seconds
===================   ============

Although Gl 581 d is no longer thought to be a planet, this example reproduces
Fig. 6 in `Heller et al. (2011) <https://ui.adsabs.harvard.edu/abs/2011A%26A...528A..27H/abstract>`_ and validates the **EqTide** module. Results for the
rotational period and obliquity for both the "constant-phase-lag" (CPL) and "constant-time-lag" (CTL) models are compared.


To run this example
-------------------

.. code-block:: bash

   python makeplot.py <pdf | png>


Expected output
---------------

.. figure:: TideLock.png
   :width: 100%
   :align: center

**Left:** Rotational period evolution for the CPL (dashed curve) and CTL (solid curve)
assuming an initial rotation period of 1 day, a tidal Q of 100, and and obliquity
of 23.5 degrees. **Right:** Same, but for obliquity.
