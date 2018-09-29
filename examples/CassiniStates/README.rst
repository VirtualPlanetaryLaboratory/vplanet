Cassini States
==============

Overview
--------

.. todo:: **@deitrr**: This example is broken. The same figure is produced twice.

===================   ============
**Date**              07/25/18
**Author**            Russell Deitrick
**Modules**           eqtide
                      distorb
                      distrot
**Approx. runtime**   54 seconds
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/cassini>`_
===================   ============

This example shows how a planetary system can damp into a Cassini state, in which
a planet's rotational axis, orbital axis, and the local total angular momentum
vector are all coplanar.


To run this example
-------------------

.. code-block:: bash

    vplanet vpl.in
    python makeplot.py <pdf | png>


Expected output
---------------


.. figure:: CassiniStatesEvol.png
   :width: 600px
   :align: center

An Earth-mass planet damping into Cassini state 2 under the influence of tides
and perturbations from an 18 Earth-mass companion planet. This example is
constructed to be similar to Figure 2 of :cite::WinnHolman05.

.. figure:: CassiniStatesSection.png
   :width: 600px
   :align: center

Phase space of the three prograde Cassini states (numbered). The gray curves
show lines of constant Hamiltonian (Equation 5 in :cite::WinnHolman05) and the
black curve shows the separatrix between states 1 and 2. The three possible
Cassini states for this system are denoted by the light gray numbers 1, 2, and
4. This case is constructed to be similar to the illustrative case shown in
Figure 2 of :cite::WinnHolman05, with -g/\alpha ~ 0.75$ and i = 0.5 deg. The
location of the planet's pole after ~400 kyr is shown as the black points. As
the planet's spin is damped by tidal torques, its obliquity sinks into Cassini
state 1.
