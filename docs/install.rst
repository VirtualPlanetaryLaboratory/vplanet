Installing VPLanet
==================

Users
----------

From source:

.. code-block:: bash

   git clone https://github.com/VirtualPlanetaryLaboratory/vplanet
   cd vplanet
   python setup.py install


From pip (coming soon):

.. code-block:: bash

   python -m pip install vplanet
   
   
Developers
----------

First time:

.. code-block:: bash

   git clone https://github.com/VirtualPlanetaryLaboratory/vplanet
   cd vplanet
   python setup.py develop
   
After changes to the C source:

.. code-block:: bash

   python setup.py clean --all
   python setup.py develop
