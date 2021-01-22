Parameter Sweep Guide
===================

Parameter sweeps are a very common use of the Vplanet software. A Parameter sweep
ultizes vspace, multi-planet and bigplanet in conjunction to run a multitude of
simulations.

The following guide will show you how to do the entire process from setting up
the vspace file to create a contour plot using bigplanet’s HDF5 data structure.


Vspace File Setup
------------------------

The first thing to run a parameter sweep is to create a vspace file. In this
guide we are going to run a vspace using the Earth Interior in the example
folder as a template, but have two following parameters be sweeped: Tman and
Tcore, which are a part of the Thermint module and control the temperature of
the mantle and the core of the planet.

Here’s what the vspace file, called paramexample should look like:

.. code-block:: bash

    srcfolder ~/vplanet/examples/EarthInterior

    destfolder ParameterSweep

    trialname test


    file   sun.in


    file   earth.in

    dTMan [2500, 3500, n10] tman

    dTCore [5500,6500, n10] tcore


    file   vpl.in

Where:
srcfolder is the folder vspace uses as a template. Anything changed in the
file section will be overwritten
Destfolder is the name of the folder where the simulations will be held
Trialname is the name of each folder (in this case the first folder will be
named test_tman0_tcore0 because we are changing both the Tman and Tcore
variables for each simulation

We include each of the files by using ``file inputfilename`` regardless of
whether or not they are being changed. This is good practice to note what all
the input files are for the given simulation.

dTMan is the temperature of the mantle, and it will change from 2500 to 3500
and the interval will be 10, as noted with the n10
dTCore is the temperature of the core, and it will change from 5500 to 6500 and
the interval will be 10, as noted with the n10.

Because both dTMan and dTCore have an interval of 10, the total number of
simulations will be 100.

Once the vspace file is complete, run the following command in the command line:

.. code-block:: bash

    vspace paramexample

This command will create the folder, ParameterSweepExample with 100 folders
inside of it, each with their own sun.in, earth.in and vpl.in with the
parameters from the EarthInteror example (with the dTMan and dTCore changed
based on the vspace file). Now we are ready to run multi-planet

Multi-planet Guide
-------------------------

The next step is to use multi-planet, another tool of vplanet, to run the
simulations in a quick and efficient manner, by utilizing multiple processors
on a machine. To run using multi-planet, type the following in the command line:

.. code-block:: bash

    multi-planet -c 4 paramexample

This uses the optional argument -c or --cores to tell multi-planet the number
of cores to run. The default is the maximum number of cores on the machine,
but for this example we are going to use 4 cores. There is another optional
argument that creates the HDF5 Files for Bigplanet directly after the
simulation completes, but we are going to leave it at the default setting,
which is false.

Even though this example is rather quick to run (each simulation in the
parameter sweep only takes 1 second to run), for longer simulations there may
be a time where you want to check the status of multi-planet. The command
mpstatus does exactly this. Type the following command below:

.. code-block:: bash

    mpstatus paramexample

This command shows the status of the multi-planet that is running with that
particular input file, paramexample. It should output the following to the
terminal:

.. code-block:: bash

      --Multi-Planet Status--
    Number of Simulations completed:
    Number of Simulations in progress:
    Number of Simulations remaining:

But with the proper numbers shown. Now that Multi-planet is complete, let's
extract the data using Bigplanet.

Bigplanet Usage Guide
-------------------------------

The last (and final) step is to use bigplanet to extract all the data needed
in HDF5 format for easy access. To do so, type the following command in the
terminal after multi-planet finishes:


.. code-block:: bash

    bigplanet -c 4 paramexample

The bigplanet arguments work identical to multi-planet’s with the user able to
specify the number of processors bigplanet can use. In this example we are
going to use 4 cores. This will create an HDF5 file that shares the same name
as the destfolder from the vspace file which was ParameterSweep in the example.
Now that the HDF5 file exists we can create a plot of the data we extracted.

BIgplanet Module
++++++++++++++

Bigplanet is multipurpose, as it also is a module that can be imported into
python scripts for extraction of data from the HDF5 files. To start we are
going to import bigplanet (and the other needed modules) and load in the HDF5
file:

.. code-block:: python

  import bigplanet as bp
  import h5py as h5
  import matplotlib.pyplot as plt
  import vplot as vpl

  HDF5_file = h5.File(‘ParameterSweep.hdf5’, ‘r’)

This loads in the modules and reads in the HDF5 files as HDF5_file. Now to
import the data we want to graph, which is the TCore and the TMan variables we
changed in the vspace file.

.. code-block:: python

  TCore = bp.ExtractColumn(HDF5_File,'earth_TCore_initial')
  TMan = bp.ExtractColumn(HDF5_File,'earth_TMan_initial')

  SurfFLuxTot = bp.ExtractColumn(HDF5_File,'earth_SurfEnFluxTotal_initial')

The first method we called, ``bp.ExtractColumns``, extracts the particular column
from the HDF5 file. We want the initial Tcore and TMan, which are found in the
earth portion of the earth.log file.
We also grab the SurfFLuxTot as that will be our z variable for the contour
plot later on.

Next we want to grab the Unique Values from our x and y axis to make the contour
plot. We do that by calling the ``ExtractUniqueValues`` function, like so:

.. code-block:: python

  TCore_uiq = bp.ExtractUniqueValues(HDF5_File,'earth_TCore_initial')
  TMan_uniq = bp.ExtractUniqueValues(HDF5_File,'earth_TMan_initial')

Once that is done, we need to create a 2D matrix from the shape of TCore, and
Tman using the data from SurfEnFluxTotal. This easily can be done by calling
the ``CreateMatrix`` function, which takes in the xaxis,yaxis and the zarray we
want to be converted into a 2d matrix.

.. code-block:: python

  SurfFLuxTot_Zaxis = bp.CreateMatrix(TCore_uiq,TMan_uniq,SurfFLuxTot)

And finally we can plot the data with the ``plt.contour`` command. Let’s use the
VPLanet official colors of blue. We can do that by setting the colors to be
vpl.colors.darkblue.

.. code-block:: python

  plt.contour(TCore_uiq,TMan_uniq,SurfFLuxTot_Zaxis,colors = vpl.colors.orange)

This should produce the following plot:
