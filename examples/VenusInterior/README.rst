VenusInterior
==========

Overview
--------

Internal evolution of Venus.

===================   ============
**Date**              10/15/18
**Author**            Peter Driscoll
**Modules**           radheat thermint
**Approx. runtime**   <1 second
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/VenusInterior>`_
===================   ============

This example uses the stagnant lid model of Venus to estimate its thermal and magnetic
evolution. Without much data on its interior, this problem is underconstrained, but
**VPLANET** does reproduce the available observations.

To run this example
-------------------

.. code-block:: bash

   vplanet vpl.in
   python makeplot.py <pdf | png>

Expected output
---------------

.. figure:: VenusInterior1.png
.. figure:: VenusInterior2.png
   :width: 600px
   :align: center

Analagous to EarthInterior.  Nominal thermal history of the Venus, mantle and core.  Radiogenic
heating is from radheat. Magnetic moment evolution is estimated from
the core energy balance and inner core growth.
