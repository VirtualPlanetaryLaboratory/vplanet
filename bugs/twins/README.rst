❌ twins
=========
===================   ============
**Issue**             `#15 <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/issues/15>`_
**Date**              06/28/18
**Author**            Rodrigo Luger
===================   ============

If two bodies have the same name in the `.in` file, their output properties
literally become entangled and jump back and forth between the values of both.


Long description
----------------

This is a weird one. On more than one occasion I've forgotten to change
the name of a body (`sName`) in its input file when copy-pasting to create a new one. This leads
to a very strange entanglement of their behavior. To showcase this, I created four planet files:
planet1--planet4. Planets 1 and 2 have the same name: `planet`. Planets 3 and 4 are copies
of planets 1 and 2, respectively, so their output is correct. When this system is called with
`vplanet`, notice how the mass of the hydrogen envelope of planets 1 and 2 oscillate back and forth
between the values of 3 and 4!


Suggested fix
-------------

I suspect something wonky is happening in `BodyCopy`, but I'm not sure. The easiest
way to fix this issue is to add a check in `Verify` to ensure the user hasn't set two bodies with the
same name.
