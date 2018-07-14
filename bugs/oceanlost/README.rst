❌ oceanlost
============

Overview
--------

===================   ============
**Issue**             `#13 <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/issues/13>`_
**Date**              06/28/18
**Author**            Rodrigo Luger
===================   ============

When EQTIDE and ATMESC are run together for a planet with oceans, once the
planet loses all its water the message `Ocean Lost! Switching dTidalQ to: dTidalQRock.`
is printed to screen at **every timestep.**


Long description
----------------

This leads to very slow run times because of all the outputting to `stdout`!


Suggested fix
-------------

The check for desiccation should only happen as long as the planet has water!
