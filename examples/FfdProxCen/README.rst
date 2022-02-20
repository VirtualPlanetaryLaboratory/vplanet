Flare frequency distribution of Proxima Centauri.
====================================================================================================

Overview
--------

This example compares the models of flare frequency distribution proposed by Davenport et. al 2019 and Lacy et. al 1976.


===================   ============
**Date**              09/15/21
**Author**            Laura Amaral
**Modules**           FLARE
                      STELLAR
**Approx. runtime**   5 seconds
===================   ============

To run this example
-------------------

.. code-block:: bash

    python makeplot.py <pdf | png>


Expected output
---------------

.. figure:: FfdReproduced.png
   :width:  7200px
   :align: center

   Flare frequency distribution (FFD) of energies between 1e33 and 1e36 ergs for Proxima Centauri .
   The blue, purple, brown and red lines represent the FFD using Davenport et. al 2019 model , at 1 Myr,
   10 Myr, 100 Myr and 1Gyr. The orange line represents the Lacy et. al 1976 model, where the FFD is
   constant along the stellar age.
