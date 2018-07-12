gl581
=====

Overview
--------

===================   ============
**Date**              09/01/15
**Author**            Rory Barnes
**Modules**           :doc:`eqtide <../src/eqtide.c>`
                      :doc:`stellar <../src/stellar.c>`
                      :doc:`distorb <../src/distorb.c>`
                      :doc:`distrot <../src/distrot.c>`
**Approx. runtime**   4 seconds
===================   ============

The original fully-coupled M-dwarf planetary system.

To run this example
-------------------

.. code-block:: bash

    vplanet vpl.in
    vplot

Expected output
---------------

.. figure:: https://raw.githubusercontent.com/VirtualPlanetaryLaboratory/vplanet/images/examples/gl581.png
   :width: 600px
   :align: center

   Evolution of the `Gl581 system <https://en.wikipedia.org/wiki/Gliese_581>`_, an M dwarf hosting
   at least three close-in planets. Shown here is the evolution of planets *c* and *d* under
   the influence of tidal and planet-planet interactions over one hundred thousand years.
