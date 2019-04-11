Tidal Evolution of Earth-like Exoplanet
==========

Overview
--------

===================   ============
**Date**              04/09/19
**Author**            Peter Driscoll
**Modules**           radheat thermint eqtide
**Approx. runtime**   <1 second
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/TidalEarth>`_
===================   ============

This example shows the thermal, magnetic, and orbital evolution of an
Earth-like planet using **THERMINT**, **RADHEAT**, and **EQTIDE**.
The model is 1-D and many free parameters have been
tuned to Earth.  This example reproduces the results of
Driscoll & Barnes (2015).  The makeplot.py file in ec0.5 will
execute the simulations in ec0.5/au0.01, ec0.5/au0.02, and
ec0.5/au0.05, and then generate figures in ec0.5.  These
figures should be compared to DB15 Figures 4 and 5.
The directory ec0.1/au0.042 contains an example that
reproduces DB15 Figure A1.



To run this example
-------------------

.. code-block:: bash
   cd ec0.5
   python makeplot.py <pdf | png>


Expected output
---------------

.. figure:: TidalEarth1.png
.. figure:: TidalEarth2.png
   :width: 600px
   :align: center

