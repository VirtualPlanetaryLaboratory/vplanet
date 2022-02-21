Proxima Centauri b Atmospheric Escape
============================

Overview
--------

Loss of a hydrogen envelope, surface water and build-up oxygen due to stellar+flares XUV stripping.

===================   ============
**Date**              11/22/21
**Author**            Laura Amaral
**Modules**           AtmEsc
                      FLARE
                      STELLAR
**Approx. runtime**   21 seconds
===================   ============

This example shows the evolution of the planetary parameters of Proxima Centauri b, comparing when the planet is
reached by flares and when only the quiescent XUV luminosity is consider.


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

.. figure:: AtmEscFlareProxCen.png
   :width: 3600px
   :align: center

   Surface water content (top left),  Envelope mass (top center), quiescent+flares XUV luminosity and bolometric
   luminosity ratio (top right), Oxygen content in the atmospher (bottom left), habitable zone location (bottom center) and XUV flux (bottom right).
