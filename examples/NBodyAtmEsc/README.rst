Coupling Atmospheric Escape and N-Body Evolution: Application to PSR1257+12
============

Overview
--------

Coupled atmospheric and orbital evolution of the PSR1257+12 pulsar planetary
system. This example connects the energy-limited escape/Lopez et al. (2012)
envelope model to the N-Body module SpiNBody.

===================   ============
**Date**              03/10/2020
**Author**            Rory Barnes
**Modules**           AtmEsc SpiNBody
**Approx. runtime**   30 min
===================   ============

The orbital parameters are taken from `Konacki & Wolszczan (2003)
<https://ui.adsabs.harvard.edu/abs/2003ApJ...591L.147K/abstract>`_. The properties
of the pulsar from XXX. The luminosity of the pulsar is assumed to be 0.001
L_sun, with all flux emitted in the XUV. These assumptions are consistent with
YYY. The initial envelope mass of planet A is 1 Earth mass, while the core mass
is 0.02 Earth masses `Konacki & Wolszczan (2003)
<https://ui.adsabs.harvard.edu/abs/2003ApJ...591L.147K/abstract>`_. While such an
initial composition seems unlikely, the large mass loss allows for the coupling
to be apparent in a simulation that requires ~1 hour to complete. Mass loss for
the B and C companions can occur of billion year timescales, see the 
`AtmEsc <AtmEsc>`_ directory.


To run this example
-------------------

.. code-block:: bash

    python makeplot.py <pdf | png>

Expected output
---------------

.. figure:: NBodyAtmEsc.png
   :width: 600px
   :align: center

**Top left**: Envelope mass of planet A. **Top right**: Radius of planet A.
**Bottom left**: Eccentricity evolution of all planets. **Bottom right**:
Inclination evolution of all planets. Note the second and third eccentricity
cycles are lower than the first, which is due to A's mass loss.
