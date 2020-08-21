Magma ocean evolution on GJ1132b
===========

Overview
--------

Thermal and volatile evolution of GJ1132b's mantle and atmosphere.

===================   ============
**Date**              02/18/19
**Author**            Patrick Barth
**Modules**           magmoc, atmesc, stellar
**Approx. runtime**   3 minutes
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/magmoc/examples/MagmOc_GJ1132b>`_
===================   ============


To run this example
-------------------

.. code-block:: bash

    vplanet vpl.in
    python plot_magmoc.py


Expected output
---------------

.. figure:: Results_GJ1132b.png
   :width: 600px
   :align: center

Evolution of various planetary parameters for the magma ocean evolution on
GJ1132b with an initial water inventory of 100 terrestrial oceans, using the grey
atmosphere model.
Anti-clockwise from top left: Mantle and surface temperature; water masses in the
different reservoirs; atmospheric pressures; atmospheric net cooling flux;
melt fraction of the magma ocean (not including solidified mantle); mass fraction
of water and Fe2O3 in the melt; oxygen mass in different reservoirs; solidification
radius of the magma ocean.
