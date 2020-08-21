Validate MagmOc against `Hamano et al. (2013) <https://doi.org/10.1038/nature12163>`_
===========

Overview
--------

Simulation of the magma ocean and atmosphere evolution for Earth containing 5TO
water compared to results presented by
`Hamano et al. (2013) <https://doi.org/10.1038/nature12163>`_
(Fig. 1).

=============================   ===============
**Date**                        03/30/20
**Author**                      Patrick Barth
**Planet name**                 Earth
**Initial water content**       5 TO
**XUV absorption efficiency**   0.3
**Atmospheric flux model**      `grey atmosphere <https://doi.org/10.1016/j.epsl.2008.03.062>`_
**Modules**                     magmoc, stellar, atmesc
**Approx. runtime**             1 minute
**Source code**                 `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/magmoc3/examples/MagmOc_Earth>`_
=============================   ===============

To run this case:
-------------------

.. code-block:: bash

    vplanet vpl.in
    python plot_magmoc_earth_hamano.py


Expected output
---------------

.. figure:: Earth_5TO_Hamano.png
   :width: 600px
   :align: center

Evolution of atmospheric pressure (top), mantle temperature (middle, white line),
cumulate and melt fraction (middle, black and grey areas), and water content in
the different reservoirs (atmospere - red, magma ocean - green, solid - blue),
for Earth with an initial water reservoir of 5 terrestrial oceans.
The red arrows indicate the catastrophic outgassing of water into the atmosphere
when the melt fraction of the mantle decreases rapidly.
