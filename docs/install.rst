Installing VPLanet
==================

VPLanet will run on macOS 10+, most recent Linux distributions, and Windows 10
(via their free `Ubuntu Linux interface
<https://www.microsoft.com/en-us/p/ubuntu/9nblggh4msv6?activetab=pivot:overviewtab>`_).
If you plan on developing VPLanet us the Developers


Users
----------

You can install VPLanet and its support scripts (vplot, vspace, multiplanet, and
bigplanet) using `pip <https://pip.pypa.io/en/stable>`_:

.. code-block:: bash

   python -m pip install vplanet

This will place executables of the most up-to-date, stable versions of these
codes in your path (after ensuring all dependency checks are satisfied). This
command will **not** download the repository. Please note that VPLanet requires
Python 3.6 or greater.

Developers
----------

To install VPLanet as a developer, you must clone the repository and then
install the software. The first time you install VPLanet, use the following
commands:

.. code-block:: bash

   git clone https://github.com/VirtualPlanetaryLaboratory/vplanet
   cd vplanet
   python setup.py develop

After changes to the C source, make sure to re-install:

.. code-block:: bash

   python setup.py develop


.. note::

   If you are running macOS Catalina or later, you may need to force the operating
   system to set the location of C libraries:

   .. code-block:: bash

      export CPATH=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include

   Thanks to Chris Colose for identifying and solving this problem.
