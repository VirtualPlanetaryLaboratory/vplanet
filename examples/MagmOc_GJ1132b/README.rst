Magma ocean evolution on GJ1132b
===========

Overview
--------

Thermal and volatile evolution of GJ1132b's mantle and atmosphere.

===================             ============
**Date**                        08/21/20
**Author**                      Patrick Barth
**Planet name**                 GJ1132b
**Initial water content**       100 TO
**Modules**                     magmoc, atmesc, stellar
**Approx. runtime**             3 minutes
===================             ============


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
