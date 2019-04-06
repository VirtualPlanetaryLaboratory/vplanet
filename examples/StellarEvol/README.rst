(Pre-)Main Sequence Stellar Evolution
=================

Overview
--------

Stellar evolution for stars between 0.1 and 1.3 solar masses.

===================   ============
**Date**              07/25/18
**Author**            Rodrigo Luger
**Modules**           stellar
**Approx. runtime**   2 minutes
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/MainSequence>`_
===================   ============

This example shows the evolution of pre- and main sequence stars as predicted by
Baraffe et al. (2016). **VPLanet** interpolates the results of that paper, accounting
for the changes that occur as convection sets in for higher mass stars.

To run this example
-------------------

.. code-block:: bash

    python makeplot.py <pdf | png>


Expected output
---------------

.. figure:: MainSequence.png
   :width: 600px
   :align: center

   Evolution of the radius, radius of gyration, luminosity, XUV luminosity,
   and temperature, of different masses according to the Baraffe (2015)
   :cite:`Baraffe15` stellar evolution tracks.
