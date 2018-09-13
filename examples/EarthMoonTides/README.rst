IoHeat
==========


Overview
--------

===================   ============
**Date**              08/27/18
**Author**            Rory Barnes
**Modules**           `eqtide <../src/eqtide.html>`_
**Approx. runtime**   <1 second
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/EarthMoonTides>`_
===================   ============



To run this example
-------------------

.. code-block:: bash

   vplanet vpl.in
   python plotEM.py


Expected output
---------------

.. figure:: earthmoontides.png
   :width: 600px
   :align: center

History of the nominal Earth-Moon tidal model. The Earth's rotation slows as the orbit grows and becomes more eccentric. The obliquity rises due to conservation of angular momentum. Both energy and total angular momentum are well-conserved, at until least until just prior to the merge when the model breaks down. 
