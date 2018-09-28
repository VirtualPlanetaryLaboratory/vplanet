Water Loss on Venus
===========

Overview
--------

===================   ============
**Date**              07/25/18
**Author**            Rodrigo Luger
**Modules**           atmesc
                      stellar
**Approx. runtime**   49 seconds
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/venus_ocean>`_
===================   ============

Water loss on Venus from thermal processes.



To run this example
-------------------

.. code-block:: bash

    vplanet vpl.in
    python makeplot.py <pdf | png>


Expected output
---------------

.. figure:: VenusWaterLoss.png
   :width: 600px
   :align: center

   Evolution of the surface water content (left) and amount of oxygen absorbed
   by the surface (right) as a function of time for three different initial
   surface water inventories. The vertical dashed line is the Watson et al. (1981)
   :cite:`Watson81` timescale for the complete desiccation of Venus.
