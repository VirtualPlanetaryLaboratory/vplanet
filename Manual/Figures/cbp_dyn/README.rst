cbp_dyn
=======

Overview
--------

===================   ============
**Date**              07/24/18
**Author**            David Fleming
**Modules**           `binary <../src/binary.html>`_
**Approx. runtime**   32 seconds
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/cbp_dyn>`_
===================   ============

To run this example
-------------------

.. code-block:: bash

    vplanet vpl.in
    python cbp_dyn.py


Expected output
---------------

.. figure:: cbp_dyn.png
   :width: 600px
   :align: center

   Orbital evolution of circumbinary planet Kepler-16b according to
   VPLANET's *binary* module.  Perturbations from the central binary
   force orbital eccentricity and inclination oscillations for the planet
   (top left and right panels, respectively) and cause appreciable precession
   of the longitude of the periapse and ascending node (bottom left and right
   panels, respectively).
