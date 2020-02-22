Coupled Atmospheric Escape and Tidal Evolution
==============================================

Overview
--------

Many planets on short-period orbits experience tidal effects and atmospheric
escape simultaneously. This example demonstrates this coupling for planets with
water or a hydrogen envelope. Currently the CTL model is less developed than
CPL.


===================   ============
**Date**              02/28/2020
**Author**            Rory Barnes
**Modules**           AtmEsc
                      EqTide
**Approx. runtime**   20 seconds
===================   ============

To run this example
-------------------

.. code-block:: bash

    python makeplot.py <pdf | png>


Expected output
---------------

.. figure:: WaterLossTides.png
   :width: 600px
   :align: center

   Evolution of planets losing water while tidally evolving. The black curves
   assume the CPL model; red the CTL model. In both cases water loss is
   energy-limited and the efficiency follows the results of Bolmont et al.
   (2016). For plotting clarity, the CPL model begins with 10 Earth oceans of
   water and CTL with 9. The .in files for this figure are in the `WaterCPL
   <WaterCPL>`_ and `WaterCTL <WaterCTL>`_ directories.

.. figure:: Lopez12CPL.png
    :width: 600px
    :align: center

    Evolution of a planets losing a hydrogen envelope while tidally evolving. In
    this case, H loss can proceed via Roche lobe overflow (Bondi-limited escape),
    radiation-recombination-limited escape, energy-limited escape. The "Auto"
    mode self-consistently tracks escape through these regimes. Note that the
    escape efficiency parameter has been set artificially large (0.99) to reveal
    some mass loss over 1 Myr. 
