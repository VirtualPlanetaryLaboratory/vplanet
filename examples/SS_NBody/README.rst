N-body Orbital Evolution of the Solar System
============

Overview
--------

Orbital evolution from an N-body model.

===================   ============
**Date**              10/30/18
**Author**            Hayden Smotherman
**Modules**           `spinbody <../src/spinbody.html>`_
**Approx. runtime**   20 min (VPLanet)
                      10 Min (HNBody)
===================   ============

This example uses SpiNBody to model the orbits of the solar system planets and compares
the results to a similar simulation run by HNBody v1.0.10. The output files for
hnbody are generated using the HNBody input script :code:`SpiNBody_Comparison.hnb`.

To run this example
-------------------

.. code-block:: bash

    # Run the Solar System example
    vplanet vpl.in
    # Run hnbody
    hnbody ss.hnb
    # Make the plots
    python makeplot.py <pdf | png>

Expected output
---------------

.. figure:: SS_NBody.png
   :width: 600px
   :align: center

   Evolution of Earth's orbital elements in a Solar System simulation over the next
   100,000 years. Black is **VPLanet** results; red is **HNBody**. The lower right panel shows the energy and angular   momentum conservation for **VPLanet** only.
