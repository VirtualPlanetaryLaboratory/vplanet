TIP OF THE DAY
--------------

Once a *VPLANET* run is complete, open up Python in the run directory and type

.. code-block:: python
   
   import vplot
   output = vplot.GetOutput()

to get an object that contains all the output information contained in the *.forward* and *.Climate* files in the `cwd`. For instance, to access the array of obliquity of `Earth` in `poise_example`, simply type

.. code-block:: python
   
   output.Earth.Obliquity

Units are stored in ``output.Earth.unit``; a description of the parameter is stored in ``output.Earth.description``.