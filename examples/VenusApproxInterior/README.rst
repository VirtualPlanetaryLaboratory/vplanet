Thermal and Magnetic Evolution of Venus
==========

Overview
--------

Internal evolution of Venus.

===================   ============
**Date**              10/15/18
**Author**            Peter Driscoll
**Modules**           RadHeat ThermInt
**Approx. runtime**   <1 second
===================   ============

This example uses the stagnant lid model in **ThemInt** to estimate Venus' thermal and magnetic
evolution. Without much data on its interior, this problem is underconstrained, but
**VPLanet** does match the available observations. Note that in this example we assume Venus
is identical to Earth in mass, radius, and composition to directly compare with Fig. 6 from (`Driscoll & Bercovici (2014) <https://ui.adsabs.harvard.edu/abs/2014PEPI..236...36D/abstract>`_.

To run this example
-------------------

.. code-block:: bash

   python makeplot.py <pdf | png>

Expected output
---------------

.. figure:: VenusApproxInterior1.png
.. figure:: VenusApproxInterior2.png
   :width: 100%
   :align: center

Analagous to EarthInterior.  Nominal thermal history of the Venus' mantle and core.  Radiogenic
heating is from **RadHeat**. Magnetic moment evolution is estimated from
the core energy balance and inner core growth.
