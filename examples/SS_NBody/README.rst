N-body Orbital Evolution of the Solar System
============

Overview
--------

Orbital evolution from an N-body model.

===================   ============
**Date**              10/30/18
**Author**            Hayden Smotherman
**Modules**           SpiNBody
**Approx. runtime**   27 seconds
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/spinbody>`_
===================   ============

This example uses **SpiNBody** to model the orbits of the solar system planets and compares
the results to a similar simulation run by HNBody v1.0.10. The output files for
hnbody are generated using the HNBody input script :code:`SpiNBody_Comparison.hnb`.

To run this example
-------------------

.. code-block:: bash

    # Run the Solar System example
    vplanet vpl.in
    # Run hnbody
    hnbody SpiNBody_Comparison.hnb
    # Make the plots
    python makeplots.py

Expected output
---------------

.. figure:: SpiNBody_Earth.png
   :width: 600px
   :align: center

   Evolution of Earth's orbital elements in a Solar System simulation over the next
   1000 years. Blue is VPlanet results. Red is HNBody results.


.. figure:: SpiNBody_Conservation.png
   :width: 600px
   :align: center

   Conservation of orbital angular momentum (top) and orbital energy (bottom) for
   the entire Solar System simulation.
