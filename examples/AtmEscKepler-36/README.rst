Kepler-36 Atmospheric Escape
============================

Overview
--------

Loss of a hydrogen envelope due to stellar XUV stripping.

===================   ============
**Date**              06/28/18
**Author**            Rodrigo Luger
**Modules**           AtmEsc
                      STELLAR
**Approx. runtime**   31 seconds
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/kepler36>`_
===================   ============

Hydrogen-rich planets that are close to their host star can lose significant mass
as the XUV radiation from the host star imparts enough energy for individual atoms
to acheive escape velocity. In this example, **VPLanet** simulates atmmospheric loss
for the case of Kepler-36 b, which is considerably smaller in radius than its very
nearby companions, Kepler-36 c. :cite:`LopezFortney2013` argue that this extreme
radius dichotomy can be explained by XUV winds, and **VPLanet** reproduces that
result, see Figure 3 in Lopez and Fortney (2013).

To run this example
-------------------

.. note::

    You might need to install :code:`tqdm`:

    .. code-block:: bash

        pip install tqdm


.. code-block:: bash

    python makeplot.py <pdf | png>


Expected output
---------------

.. figure:: AtMescKepler-36.png
   :width: 400px
   :align: center

   Percentage of the planet's gaseous enveloped stripped via hydrodynamic
   escape as a function of core mass (top), initial envelope mass (center),
   and initial total mass (bottom), for different initial envelope mass
   fractions (colors). The core mass is the best predictor of the amount of
   gas that is stripped by hydrodynamic escape.
