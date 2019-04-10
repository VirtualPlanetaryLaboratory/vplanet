Earth's Seasonal and Milankovitch Climate Cycles
===================

Overview
--------

Annual and Milankovitch climate cycles on Earth.

===================   ============
**Date**              07/25/18
**Author**            Russell Deitrick
**Modules**           POISE
                      DistOrb
                      DistRot
**Approx. runtime**   5 minutes
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/Milankovitch>`_
===================   ============

This example validates **VPLanet**'s 1-D climate model with dynamic ice sheets over annual
and Myr timescales. On annual timescales, the seasons cycle back and forth on the
northern and southern hemispheres. On longer timescales, ice sheets grow and retreat
due to changes in eccentricity and obliquity, which are caused by perturbations
from other bodies. Note that we assume every latitude grid is 75% water, 25% land
so the latitudinal extent of the ice sheets does not exactly match the geologic
record, but the frequencies and intensities do (Huybers & Tzipermann 2008).

To run this example
-------------------

.. code-block:: bash

    vplanet vpl.in
    python makeplot.py <pdf | png>


Expected output
---------------

.. figure:: EarthClimateSeasons.png
   :width: 600px
   :align: center

Insolation (upper left), surface temperature (upper right), ice mass balance
(lower left), and out-going longwave radiation (lower right), for Earth over a
single year, as modeled by `poise`. Note that negative values in ice mass balance
represent **potential** melting, i.e. this value is calculated even in the
absence of ice on the surface.

.. figure:: EarthClimateMilankovitch.png
   :width: 600px
   :align: center

Ice sheet evolution for Earth with (upper panels) and without (middle panels)
isostatic depression and rebound of the bedrock. Also shown is the difference
(lower panels). The left panels show the ice sheet height/altitude; the right
panels show the surface density of the ice. Without the bedrock model, the ice
grows taller (in elevation), but there is less ice overall because the surface
does not sink under the weight of the ice.
