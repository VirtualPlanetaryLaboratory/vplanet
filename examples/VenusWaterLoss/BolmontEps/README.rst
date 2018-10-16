Water Loss on Venus
===========

Overview
--------

===================   ============
**Date**              07/25/18
**Author**            Rodrigo Luger
**Modules**           atmesc
                      stellar
**Approx. runtime**   49 seconds
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/venus_ocean>`_
===================   ============

Water loss on Venus from thermal processes.



To run this example
-------------------

.. code-block:: bash

    vplanet vpl.in
    python makeplot.py <pdf | png>


Expected output
---------------

.. figure:: EpsilonScaling.png
   :width: 600px
   :align: center

   The scaling of the Atmospheric Escape Efficiency for H2O based on
   the XUV flux received by the planet as given by Bolmont (2016)
   :cite:`Bolmont16`.
