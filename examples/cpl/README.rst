Tidal evolution (CPL)
=====================

Overview
--------

===================   ============
**Date**              ??/??/??
**Author**            ??
**Modules**           `eqtide <../src/eqtide.html>`_
**Approx. runtime**   1 second
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/cpl>`_
===================   ============

A simple example of tidal evolution using the constant phase lag
(CPL) model for a mildly eccentric planet.

To run this example
-------------------

.. code-block:: bash

    vplanet vpl.in
    vplot


Expected output
---------------

.. figure:: cpl.png
   :width: 600px
   :align: center

   Evolution of several properties of the planet over 7 Gyr. As the planet circularizes due to
   tidal forces (top left), both its orbital and rotational periods decay (top right and center).
   As the eccentricity drops, the rate of tidal energy release decreases (bottom left).
   The plots at bottom center and bottom right show the (approximate) conservation of angular
   momentum and energy in the star-planet system.
