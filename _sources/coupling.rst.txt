Coupling Modules
================

Coupling VPLanet modules is a powerful way to study phenomena in an
interdisciplinary manner. While developing and validating a new module coupling
can be challenging, many unsolved problems in planetary evolution require
connecting physical models in novel ways. This page provides the basics for
module coupling, but it is important to remember that there is no
one-size-fits-all approach to solving these problems -- every module combination
is unique. Before attempting a new module coupling, we strongly encourage you
to be familiar with developing VPLanet by, for example, adding new options
and/or outputs and fully understanding the code architecture.

Getting Started
---------------

The first step in coupling modules is to determine how they connect. In some
cases, one module is completely dependent on another, such as atmospheric escape
and stellar evolution, or climate and orbital evolution. In those cases, the
escape doesn't influence the star, and the climate doesn't impact the orbit.
These couplings are the simplest to implement as they don't include feedbacks.
An example of a feedback is the evolution of eccentricity due to tidal damping
and planet-planet interactions: As the eccentricity decays due to tides, the
planet's effects on other planets weakens, but the other planets continue to
pump up the tidally damping planet's eccentricity. In practice, implementing
these two types of couplings are similar, but the second case is a bigger task
that will likely require more preparation and development time.

Recall that the module source files, e.g. atmesc.c, **cannot** contain
references to other modules. Subroutines that connect modules must be placed in
the higher level files of body.c, system.c, options.c, output.c, halt.c,
module.c.

Multi-Module Subroutines
------------------------

Most of the work in connecting modules lies in module.c in subroutines that end
with the extension "Multi". The three that you are most likely to use are:
VerifyMulti, ForceBehaviorMulti, PropsAuxMulti, and . The first part of module.c
contains numerous subroutines related to initializing the modules, so coupling
modules does not depend on those functions as they are required for the creation
of an individual module.

The first set of multi-module functions are the VerifyModuleMulti functions,
which perform the checks to ensure that a user who has selected multiple modules
for a body has chosen self-consistent input. As with single-module Verify, the
specifics of multi-nodule verification are unique to the problem. In the
subroutine you create here, you are free to include whatever checks are
necessary. At the end of each function you can add multi-module function for
PropsAux and ForceBehavior. Once you have coded up your function, add a call to
it in the actual VerifyModuleMulti function in module.c. Finally, check
VerifyModuleCompatability and remove any existing exit calls that are associated
with the new module combination.

:note::
  All VerifyModuleMulti functions are called even if only 1 module has been
  selected by the user because some default behavior depends on if other modules
  have *not* been selected.

If your module coupling requires PropsAux and/or ForceBehavior functions that
are unique to the coupling, you can then add them in module.c, too. All
multi-module functions are called *after* the single-module functions, so if
something is set in one of those functions, you can change the setting in the
multi-module functions. Note that you can build as complex a multi-module
function as you like, i.e. connecting all modules, and use the VerifiMulti
framework to add/remove functions to the Control->fnPropsAuxMulti and
Control->fnForceBehaviorMulti function pointer matrices. Just be mindful of the
order in which the VerifyMulti functions are called.

Multi-Module Derivatives
------------------------

In some case, a multi-module combination may necessitate new derivatives that
are not required for single-module runs. VPLanet also includes a framework for
this possibility with the UpdateMulti and AddModulesMulti functions in module.c.
These functions are analogous to those for single module functions. Follow the
example for semi-major axis if EqTide and STELLAR are called for a binary star
system.
