IoHeat
==========


Overview
--------

===================   ============
**Date**              07/25/18
**Author**            Rory Barnes
**Modules**           eqtide
**Approx. runtime**   1 minute
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/IoHeat>`_
===================   ============

Surface energy flux on Io due to tidal dissipation according the the eqtide-CPL
model.


To run this example
-------------------

.. code-block:: bash

   vspace vspace.in
   python makeplot.py <pdf | png>


Expected output
---------------

.. figure:: IoHeat.png
   :width: 600px
   :align: center

Contour plot of surface tidal heat flux of Io as a function of its eccentricity
and obliquity. Contour units are W/m^2, the vertical line corresponds to Io's
observed eccentricity, and the horizontal line is the expected obliquity if Io
is in a Cassini state :cite:`BillsRay00`. The orange shaded region corresponds
to the observed value of 1.5-3 W/m^2 :cite:`Veeder12`.
