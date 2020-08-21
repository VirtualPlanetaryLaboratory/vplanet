Magma ocean evolution on Trappist-1 g
===========

Overview
--------

Thermal and volatile evolution of Trappist-1 g's mantle and atmosphere

===================   ============
**Date**              02/18/19
**Author**            Patrick Barth
**Modules**           magmoc, atmesc, radheat, eqtide, stellar
**Approx. runtime**   1 minute
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/magmoc/examples/MagmOc_Trappist1g>`_
===================   ============

We simulate the magma ocean evolution for Trappist-1 g with an initial water
inventory of 2 terrestrial oceans. We couple in radiogenic and tidal heating of
the mantle via the modules RadHeat and EqTide, respectively.
Atmospheric escape of hydrogen and oxygen is included via the AtmEsc module.


To run this example
-------------------

.. code-block:: bash

    vplanet vpl.in
    python plot_trappist.py


Expected output
---------------

.. figure:: Trappist1g_2TO.png
   :width: 600px
   :align: center

Evolution of various planetary parameters for the magma ocean evolution on
Trappist-1 g with an initial water inventory of 2 terrestrial oceans.
Anti-clockwise from top left: Mantle and surface temperature; water masses in the
different reservoirs; atmospheric pressures; atmospheric net cooling flux;
radiogenic and tidal heating rates; mass fraction of water and Fe2O3 in the melt;
oxygen mass in different reservoirs; solidification radius of the magma ocean.
The high-frequency variations in the atmospheric flux and oxygen mass in the solid
are numerical artefacts and owed to the fact that the greenhouse effect forces
the cooling flux to be low and the surface temperature to stay constant over a
long time.
