EarthInterior
==========


Overview
--------

===================   ============
**Date**              10/03/18
**Author**            Peter Driscoll
**Modules**           `radheat, thermint <../src/radheat.html, ../src/thermint.html>`_
**Approx. runtime**   <1 second
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/EarthInterior>`_
===================   ============



To run this example
-------------------

.. code-block:: bash

   vplanet vpl.in
   python makeplot.py <pdf | png>


Expected output
---------------

.. figure:: earthinterior1.png
.. figure:: earthinterior2.png
   :width: 600px
   :align: center

Nominal thermal history of the Earth, mantle and core.  Radiogenic
heating is from radheat. Magnetic moment evolution is estimated from
the core energy balance and inner core growth.
