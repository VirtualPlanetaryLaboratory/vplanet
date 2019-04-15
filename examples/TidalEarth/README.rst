Tidal Evolution of Earth-like Exoplanet
==========

Overview
--------

Internal and orbital evolution of Earth if it experienced strong tidal distortion
from its host star.

===================   ============
**Date**              04/09/19
**Author**            Peter Driscoll
**Modules**           RadHeat ThermInt EqTide
**Approx. runtime**   10 minutes
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/TidalEarth>`_
===================   ============

This example shows the thermal, magnetic, and orbital evolution of an
Earth-like planet using **ThermInt**, **RadHeat**, and **EqTide**.
The model is 1-D and many free parameters have been
tuned to Earth.  This example reproduces the results of
Driscoll & Barnes (2015).  The generated
figures should be compared to DB15 Figures 4 and 5.


To run this example
-------------------

.. code-block:: bash
   python makeplot.py <pdf | png>


Expected output
---------------

.. figure:: TidalEarth1.png
.. figure:: TidalEarth2.png
   :width: 600px
   :align: center

Evolution of various properties of a tidally heated Earth-like planet. The initial
eccentricity is 0.5, but the semi-major axis varies. The tidal model is very
simplified and ignores rotational angular momentum.
