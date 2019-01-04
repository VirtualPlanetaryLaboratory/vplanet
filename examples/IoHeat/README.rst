Tidal Heating of Io
==========

Overview
--------

Surface energy flux on Io due to tidal dissipation according the the eqtide-CPL
model.

===================   ============
**Date**              07/25/18
**Author**            Rory Barnes
**Modules**           eqtide
**Approx. runtime**   1 minute
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/IoHeat>`_
===================   ============

This example reproduces the surface energy flux on Io over a range of eccentricity
and obliquity. It also shows how to use vspace, a script in this repository that
can generate input files across a parameter range. vspace will create a large
number of directories, each of which has 3 input files. In this example, the makeplot
script will run each individual trial, gather the results, and create the summary
plot below. The yellow strip corresponds to the observed heat flow of Io :cite::Veeder04.


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
