satideperts
===========

Overview
--------

===================   ============
**Issue**             `#10 <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/issues/10>`_
**Date**              06/28/18
**Author**            Rodrigo Luger
===================   ============

If EQTIDE is present in any of the body .in files, but *not in the primary* .in file,
a segfault occurs.

Long description
----------------

If EQTIDE is present in the primary file, things run fine, even if saTidePerts is not
set, in which case an informative error is raised. But even if saTidePerts is set in two separate bodies
(neither of which is the primary), a segfault still occurs. Something in the code implicitly expects the star to always
have EQTIDE present (and saTidePerts set), so we need to add a check for that.


Suggested fix
-------------

Not sure yet.
