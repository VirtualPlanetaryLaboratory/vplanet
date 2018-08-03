❌ Distrot with N-Body
=======================


Overview
--------

===================   ============
**Date**              07/31/18
**Author**            Russell Deitrick
**Modules**           `distrot <../src/distrot.html>`_
**Approx. runtime**   < 1 second
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/distrot_nbody>`_
===================   ============


This quick example demonstrates the use of externally generated orbital data
(from another N-body model, for example) as input for the obliquity model
DistRot. IMPORTANT: At present, the orbital data must conform to a specific
format and set of units with header-less columns (space separated) arranged
as follows:

Time(years) SemiMajorAxis(au) Eccentricity Inclination(deg) ArgumentPericenter(deg) LongitudeAscendingNode(deg) MeanAnomaly(deg)

MeanAnomaly is not currently used, but the data column must be present.

.. todo::

    **@deitrr**: Please add a description and output images to the **distrot_nbody** example.
    Make the input files pretty by aligning the options and commenting each line.


To run this example
-------------------

.. code-block:: bash

    vplanet vpl.in
    vplot --nolog

Expected output
---------------
