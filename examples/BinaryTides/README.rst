Evolution of Tight Stellar Binaries
=====================

Overview
--------

===================   ============
**Date**              07/25/18
**Author**            David Fleming
**Modules**           `eqtide <../src/eqtide.html>`_
                      `stellar <../src/stellar.html>`_
**Approx. runtime**   71 seconds
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/zahn>`_
===================   ============

This script produces a reproduction of Figure 1 of Zahn and Bouchet (1989)
:cite:`ZahnBouchet89` using a coupled **EQTIDE** and **STELLAR** **VPLANET** run.

To run this example
-------------------

.. code-block:: bash

    vplanet vpl.in
    python zahn89.py


Expected output
---------------

Coupled stellar and tidal evolution of a solar twin binary from the pre-main
sequence onward calculated in vplanet using the eqtide and stellar modules.
Orbital eccentricity evolution is given by the red solid line, the orbital
period by the purple dashed line, and the ratio of stellar rotation rate to
binary mean motion (Omega/n) evolution is given by the blue dot dashed curve.
The binary's evolution qualitatively agrees with that of an identical system
presented in Figure 1 of :cite:`ZahnBouchet89`.



.. figure:: zahn.png
   :width: 600px
   :align: center
