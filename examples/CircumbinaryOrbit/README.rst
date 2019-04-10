Orbital Evolution of the Circumbinary Planet Kepler-16 b
=========

Overview
--------

The orbits of circumbinary planets evolve due to the binary stars' changing positions.

===================   ============
**Date**              07/24/18
**Author**            David Fleming
**Modules**           BINARY
**Approx. runtime**   24 seconds
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/cbp_dyn>`_
===================   ============


The orbital evolution of Kepler-16 b, a circumbinary planet, ir shown in this example
using the semi-analytic model of Leeung & Lee (2013). The orbit is non-Keplerian
due to the changing positions of the host stars.


To run this example
-------------------

.. code-block:: bash

    vplanet vpl.in
    python makeplot.py <pdf | png>


Expected output
---------------

.. figure:: Kepler-16.png
   :width: 600px
   :align: center

   Orbital evolution of circumbinary planet Kepler-16b according to
   VPLANET's *binary* module.  Perturbations from the central binary
   force orbital eccentricity and inclination oscillations for the planet
   (top left and right panels, respectively) and cause appreciable precession
   of the longitude of the periapse and ascending node (bottom left and right
   panels, respectively).
