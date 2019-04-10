Water Loss on Venus
===========

Overview
--------

Water loss on Venus from thermal escape processes.

===================   ============
**Date**              07/25/18
**Author**            Rodrigo Luger
**Modules**           AtmEsc
                      STELLAR
**Approx. runtime**   49 seconds
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/VenusWaterLoss>`_
===================   ============

The Sun's XUV radiation likely removed water from Venus early in the history of the
Solar System. If Venus was in a runaway greenhouse, then water would be in the
stratosphere where it can be photolyzed and the hydrogen can escape. Watson et al.
(1981) estimated 280 Myr, but did not account for early activity. Including that effect,
**VPLanet** predicts a desiccation timescale of about 100 Myr.


To run this example
-------------------

.. code-block:: bash

    vplanet vpl.in
    python makeplot.py <pdf | png>


Expected output
---------------

.. figure:: VenusWaterLoss.png
   :width: 600px
   :align: center

   Evolution of the surface water content (left) and amount of oxygen absorbed
   by the surface (right) as a function of time for three different initial
   surface water inventories. The vertical dashed line is the Watson et al. (1981)
   :cite:`Watson81` timescale for the complete desiccation of Venus.

To create the Bolmont scaling relationship figure
-------------------

.. code-block:: bash

   cd BolmontScaling
   vplanet vpl.in
   python makeplot.py <pdf | png>

.. figure:: BolmontScaling.png
   :width: 600px
   :align: center

   The scaling of the atmospheric escape efficiency for H2O based on the XUV flux
   received by the planet as given by Bolmont (2016) :cite:`Bolmont16`.
