Parameter Sweep Example
=======================

Overview
--------

Example using :code:`vspace`,:code:`multi-planet`, and :code:`bigplanet` in
conjunction by exploring the THERMINT parameters of dTCore (initial core
temperature) and d40KPowerCore (initial radiogenic power from potassium-40).


===================   ============
**Date**              1/26/2020
**Author**            Caitlyn Wilhelm
**Modules**           THERMINT
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

.. figure:: ParameterSweep.png
   :width: 600px
   :align: center
