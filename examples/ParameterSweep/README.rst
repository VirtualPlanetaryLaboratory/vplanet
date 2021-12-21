Parameter Sweep Example
=======================

Overview
--------

Example using :code:`vspace`, :code:`multiplanet`, and :code:`bigplanet` in
conjunction by exploring the ThermInt parameters of dTCore (initial core
temperature) and d40KPowerCore (initial radiogenic power from potassium-40).


===================   ============
**Date**              1/26/2021
**Author**            Caitlyn Wilhelm
**Modules**           ThermInt, vspace, multi-planet, bigplanet
**Approx. runtime**   160 Seconds
===================   ============


To run this example
-------------------

.. code-block:: bash

    vspace vspace.in
    multi-planet vspace.in
    bigplanet vspace.in
    python makeplot.py <pdf | png>


Expected output
---------------

.. figure:: ../../docs/BigPlanetExample.png
   :width: 600px
   :align: center
   
Inner core radius of Earth after 4.5 Gyr as a function of the initial core
temperature and the current power from potassium-40 decay. The black curve
is the current value. 
