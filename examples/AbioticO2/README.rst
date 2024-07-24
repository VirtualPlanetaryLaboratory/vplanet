Accumulation of Atmospheric Oxygen due to Water Photolysis and Hydrogen Escape.
==============

Overview
--------

Track water loss and oxygen build-up due to hydrodynamic atmospheric escape.

===================   ============
**Date**              07/25/18
**Author**            Rodrigo Luger
**Modules**           AtmEsc
                      STELLAR
**Approx. runtime**   10 minutes
===================   ============

Water-rich terrestrial planets orbiting low mass stars may experience sustained
bombardment by XUV photons during the pre-main sequence phase of the host star.
As the planets will likely be in a runaway greenhouse, water can reach the stratosphere
where it is photolyzed. Hydrogen can then escape leaving behind the heavier oxygen.
This script reproduces Figure 7 in `Luger and Barnes (2015) <https://ui.adsabs.harvard.edu/abs/2015AsBio..15..119L/abstract>`_.

To run this example
-------------------

.. note::

    You might need to install :code:`tqdm`:

    .. code-block:: bash

        pip install tqdm


.. code-block:: bash

    python makeplot.py <pdf | png>


Expected output
---------------

.. figure:: AbioticO2.png
   :width: 100%
   :align: center

Water loss and oxygen accumulation for planets orbiting in the habitable
zones of M dwarfs. The extended pre-main sequence phase of low mass
stars leads to vigorous hydrodynamic escape from planets in their
habitable zones, causing the loss of surface water (left) and the
accumulation of atmospheric oxygen (right). See `Luger and Barnes (2015) <https://ui.adsabs.harvard.edu/abs/2015AsBio..15..119L/abstract>`_
for more details.
