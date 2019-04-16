Cassini States
==============

Overview
--------

Demonstration of tidal damping can drive a planet's obliquity to a constant value,
often called a a Cassini state.

===================   ============
**Date**              07/25/18
**Author**            Russell Deitrick
**Modules**           EqTide
                      DistOrb
                      DistRot
**Approx. runtime**   54 seconds
===================   ============

A damped orbital-rotational system can enter a "Cassini state," in which
a planet's rotational axis, orbital axis, and the local total angular momentum
vector are all coplanar. In this case, we consider a system in which tides from the
star damp the rotation rate, obliquity, semi-major axis and orbital eccentricity.
Tides tend to damp the rotational axis so that it is perpendicular to the orbital
plane, but perturbations from other planets drive the obliquity to higher values.
Over time, the systems settles in a damped-drived state in which the obliquity is
non-zero, but also not oscillating. This example is modeled after Winn & Holman (2005).

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
