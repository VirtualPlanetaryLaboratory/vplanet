EarthInterior
==========

Overview
--------

Evolution of Earth's interior.

===================   ============
**Date**              10/03/18
**Author**            Peter Driscoll
**Modules**           radheat thermint
**Approx. runtime**   <1 second
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/EarthInterior>`_
===================   ============

This example shows the thermal and magnetic evolution of Earth's interior from
**THERMINT** and **RADHEAT**. The model is 1-D and many free parameters have been
tuned to reproduce Earth's current properties. Earth is divided in a core, mantle,
and crust. The evolution depends only on the temperature of the core and mantle.


To run this example
-------------------

.. code-block:: bash

   vplanet vpl.in
   python makeplot.py <pdf | png>


Expected output
---------------

.. figure:: EarthInterior1.png
.. figure:: EarthInterior2.png
   :width: 600px
   :align: center

Nominal thermal history of the Earth, mantle and core.  Radiogenic
heating is from radheat. Magnetic moment evolution is estimated from
the core energy balance and inner core growth.
