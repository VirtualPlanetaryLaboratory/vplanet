angmom
======

Overview
--------

===================   ============
**Date**              06/28/18
**Author**            Rodrigo Luger
**Modules**           :doc:`eqtide <../src/eqtide.c>`
                      :doc:`stellar <../src/stellar.c>`
**Approx. runtime**   4 seconds
===================   ============

A test of angular momentum conservation for a contracting star.
As the star contracts onto the main sequence, the rotation period
should decrease to conserve angular momentum. This example shows that
this is in fact working in **vplanet**.

To run this example
-------------------

.. code-block:: bash

    vplanet vpl.in
    vplot


Expected output
---------------

.. figure:: https://raw.githubusercontent.com/VirtualPlanetaryLaboratory/vplanet/images/examples/angmom.png
   :width: 600px
   :align: center

   Angular momentum conservation for a contracting/expanding star. The top row shows the
   evolution of the radius of the star, which contracts during the pre-main sequence phase and
   then slowly expands once the star is on the main sequence. To conserve angular momentum, the
   star's rotational period (center) at first decreases then increases again. The bottom plot
   shows the star's rotational frequency.
