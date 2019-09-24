Atmospheric Erosion of H Envelopes in the Energy, Radiation/Recombination, and Bondi-limited Regimes
====================================================================================================

Overview
--------

This examples compares the atmospheric erosion of four 1 Earth-mass planets subject
to energy-limited escape, radiation/recombination-limited escape, Bondi-limited escape,
and one planet whose escape regime evolves depending on its mass, radius, and
incident XUV flux.

===================   ============
**Date**              09/24/19
**Author**            David Fleming
**Modules**           ATMESC
                      STELLAR
**Approx. runtime**   10 minutes
===================   ============

xxx

The orbital evolution of Kepler-16 b, a circumbinary planet, is shown in this example
using the semi-analytic model of `Leung & Lee (2013) <https://ui.adsabs.harvard.edu/abs/2013ApJ...763..107L/abstract>`_, see their Fig. 5. The orbit is non-Keplerian
due to the changing positions of the host stars.


To run this example
-------------------

.. code-block:: bash

    vplanet vpl.in
    python makeplot.py <pdf | png>


Expected output
---------------

.. figure:: regimes.png
   :width: 600px
   :align: center

   xxx
   Orbital evolution of circumbinary planet Kepler-16b according to
   **VPLanet**'s **BINARY** module.  Perturbations from the central binary
   force orbital eccentricity and inclination oscillations for the planet
   (top left and right panels, respectively) and cause appreciable precession
   of the longitudes of the periapse and ascending node (bottom left and right
   panels, respectively).
