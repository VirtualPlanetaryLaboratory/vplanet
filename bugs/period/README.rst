❌ period
==========

===================   ============
**Issue**             `#14 <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/issues/14>`_
**Date**              06/28/18
**Author**            Rodrigo Luger
===================   ============

Setting a planet's orbital period does not automatically set its semi-major axis
when one of the modules controlling the semi-major axis evolution (BINARY DISTORB EQTIDE GALHABIT POISE SPINBODY)
is not present.

Long description
----------------

This was surprisingly hard to debug, since without one of
(BINARY DISTORB EQTIDE GALHABIT POISE SPINBODY) present, there is no way to output the semi-major axis:

.. code-block:: bash

    ERROR: Output parameter SemiMajorAxis requires module(s): BINARY DISTORB EQTIDE GALHABIT POISE SPINBODY


The semi-major axis also does not get recorded anywhere in the log file unless one of these modules
is present. This should not be the case: the semi-major axis and the eccentricity are important for
most modules, and shouldn't require loading one of the orbital evolution modules to be accessible
by the user. Case in point: users who call ATMESC for a planet at a certain orbital period.
If they don't set a semi-major axis, VPLANET just sets it to the default value of 1 AU, but **does
not tell the user anything**.

In the example provided, two planets are instantiated with hydrogen envelopes, one at a period of 1 day
and one at a period of 999999 days. Both lose the same amount of hydrogen, which is clearly wrong.
Thanks to Gijs Mulders for finding this bug.


Suggested fix
-------------

Not sure yet.
