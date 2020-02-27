Proxima Centauri b as a "Habitable Evaporated Core."
=====================================================================================

Overview
--------

A planet orbiting the habitable zone of a low mass star, e.g. Proxima Centauri b,
can lose a hydrogen envelope during the pre-main sequence. If the core is
volatile rich, then it may become habitable, but only if the water isn't
photolyzed and permanently lost via hydrogen escape. This example demonstrates
this process for 6 different assumptions: two different radius models and three
different escape models. The envelope radius may evolve according to a modified
version of the Lopez et al. (2012) radius tracks (ProxCenB) and the isothermal
model of Lehmer & Catling (2017). When a hydrogen envelope is present, the H may
escapes via energy-limited, radiation-recombination-limited, Roche lobe overflow,
and an automatic mode that determines the regime based on the environment.

===================   ============
**Date**              02/26/2020
**Author**            Rory Barnes
**Modules**           atmesc
                      stellar
**Approx. runtime**   10 seconds
===================   ============

To run this example
-------------------

.. code-block:: bash


To run the simulations and make the plot:

.. code-block:: bash

    python makeplot.py < png | pdf >


Expected output
---------------

.. figure:: HEC.png
   :width: 600px
   :align: center

   Sample evolution of Proxima b if it is a habitable evaporated core. Top left:
   The ratio of the XUV luminosity to the total luminosity. The early constant
   stage is known as the "satured" phase, which is here set to be 100 Myr. Top
   right: Comparison of the habitable zone (HZ) with the current orbit of
   Proxima b. The star evolves according to a modified track from Baraffe et al.
   (2016) that matches observations (ProxCen). Middle left: The envelope mass.
   For the ProxCenB radius model, AtmEscAuto selects the energy-limited (E-Lim)
   model, so the dark blue is behind the orange line for this panel and the
   subsequent panels. Middle right: Radius of the planet. The water layer is
   assumed to have no thickness. Bottom left: The amount of water on the surface
   in units of Earth oceans (TO). Bottom right: The maximum oxygen pressure that
   can accumulate in the atmosphere from water photolysis and hydrogen escape.
