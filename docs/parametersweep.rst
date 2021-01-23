Parameter Sweep Guide
===================

Parameter sweeps are a very common use of VPLanet software, and the team has
developed a set of python and command line tools to facilitate their completion 
and analysis. In brief, :code:`vspace` builds a set of initial conditions, 
:code:`multi-planet` performs the simulations, and :code:`bigplanet` compresses the data
and streamlines analysis. The following guide explains how to use these
tools with an example based on Earth's internal thermal evolution. 


.. note::

    You need to install :code:`vspace`, :code:`multi-planet`, and :code:`bigplanet` as
    described in their folders.


Initializing Parameter Sweeps with :code:`vspace`
------------------------

The first step is to create an input file for vspace, which is typically called `vspace.in`.
As described in more detail in :code:`vspace`'s `documentation 
<https://github.com/VirtualPlanetaryLaboratory/vplanet/tree/master/vspace>`_, this file 
modifies template files (we use the .in files from `examples/EarthInterior 
<https://github.com/VirtualPlanetaryLaboratory/vplanet/tree/master/examples/EarthInterior>`_) 
and then builds a directory structure with each 
folder containing the .in files for a specific simulation. In this guide we vary 
Tman (initial mantle temperature) and Tcore (initial core temperature).

Here’s the input file for :code:`vspace`:

.. code-block:: bash

    srcfolder ~/vplanet/examples/EarthInterior
    destfolder ParameterSweep
    trialname test

    file   sun.in

    file   earth.in

    dTMan [2500, 3500, n10] tman
    dTCore [5500,6500, n10] tcore

    file   vpl.in

This file directs :code:`vspace` to vary dTMan from 2500 to 3500 in 10 steps and dTCore 
from 5500 to 6500 in 10 steps. The directories will be built in a folder called
ParameterSweep and the individual folders will be called tmanNtcoreN, where the
Ns are integers that ensure unique folder names. Because both dTMan and dTCore 
have an interval of 10, the total number of simulations will be 100.

To build the files, run the following command in the command line:

.. code-block:: bash

    vspace vspace.in

This command will create the folder, ParameterSweep with 100 folders
inside of it, each with their own sun.in, earth.in and vpl.in with the
parameters from the EarthInteror example, but with the dTMan and dTCore changed
based on the vspace file. Now we are ready to run multi-planet.

Running Simulations with :code:`multi-planet` 
-------------------------

multi-planet is the command line tool to run the simulations created with vspace
across the processors on your computer. To run, type the following in the 
command line:

.. code-block:: bash

    multi-planet -c <num_cores> vspace.in

The optional argument -c (or --cores) tells multi-planet the number of cores to run. 
There is another optional argument that creates the HDF5 Files for Bigplanet 
directly after the simulation completes, but we are going to leave it at the default 
setting, which is false. See the :code:`multi-planet` `documentation 
<https://github.com/VirtualPlanetaryLaboratory/vplanet/tree/master/multi-planet>`_ for
more informaition.

.. note::

    The default number of cores multi-planet will use is the maximum number of 
    cores on the machine. 

Checking progress with :code:`mpstatus` 
-------------------------

This example is quick to run (about 2 minutes), but for longer simulations it is often
helpful to know how far along the parameter sweep is. The command mpstatus returns the 
current state of the multi-planet process. To check the current status, type the 
following command:

.. code-block:: bash

    mpstatus vspace.in

This command returns output like the following to the terminal:

.. code-block:: bash

      --Multi-Planet Status--
    Number of Simulations completed: 35
    Number of Simulations in progress: 6
    Number of Simulations remaining: 59

But with the proper numbers shown. 

After multi-planet completes, you may have a large number of directories with gigabytes 
of data. Storing, analyzing, and plotting these data can be tedious as each output file 
from each directory must be opened and read in sequentially. To streamline this process,
use bigplanet.

Compressing Data with :code:`bigplanet`
-------------------------------

The bigplanet command compresses your parameter sweep data into an HDF5 file in which
specific data can be efficiently extracted. Although compression can take some time,
the subsequent analysis will proceed much quicker, especially for large parameter sweeps.
To compress the data, type the following command in the terminal (after multi-planet 
finishes):


.. code-block:: bash

    bigplanet -c <num_cores> vspace.in

The bigplanet arguments work identically to multi-planet’s with the user able to
specify the number of processors bigplanet can use. This will create an HDF5 file 
that shares the same name as the destfolder from the vspace file, but with ".hdf5" 
appended, e.g. ParameterSweep.hdf5. Now that the HDF5 file exists we can create a 
plot of the data we extracted.

.. note::

    The default number of cores bigplanet will use is the maximum number of 
    cores on the machine. 

Extracting Data with the :code:`bigplanet` Module
------------------------------

With your data compressed, you need to access it later. To accomplish this goal,
BigPlanet is also a python module that can be imported into python scripts for 
extraction of data from the HDF5 files. For this example, let's plot the final 
surface energy flux as a function of the initial mantle and core temperatures. 
Here's what that script looks like

.. code-block:: python

  import bigplanet as bp
  import h5py as h5
  import matplotlib.pyplot as plt
  import vplot as vpl

  HDF5_file = h5.File(‘ParameterSweep.hdf5’, ‘r’)

This loads in the modules and reads in the HDF5 files as HDF5_file. Now to
import the data we want to graph, which is the TCore and the TMan variables we
changed in the vspace file. This step requires the h5py module, which is 
automatically installed when you setup bigplanet.


Next, let's grab the final surface flux values with the ExtractColumn method:

.. code-block:: python

  SurfFluxTot = bp.ExtractColumn(HDF5_File,'earth_SurfEnFluxTotal_final')

This method extracts a particular column from the HDF5 file, returning an array 
in which each element corresponds to the final value of the surface energy flux. 
The second argument to this method is called a "key" and describes a parameter of 
interest. To learn more about keys, consult the `bigplanet documentation 
<https://github.com/VirtualPlanetaryLaboratory/vplanet/tree/master/multi-planet>`_. 
In brief, the key syntax is "body_variable_aggregation", in
which aggregation is some property of a body's variable, e.g. the final value.

Next we want to grab the x and y values for our plot. This step is a bit complicated
because a specific value of x and/or y can be repeated multiple times. In other words,
if we just extracted every value from every simulation and placed it in an array, the
arrays would be multi-valued and the plot could not be built.  To obtain the values 
needed for the plot, use the ``ExtractUniqueValues`` function, like so:

.. code-block:: python

  TCore_uniq = bp.ExtractUniqueValues(HDF5_File,'earth_TCore_initial')
  TMan_uniq = bp.ExtractUniqueValues(HDF5_File,'earth_TMan_initial')

Now we have the values we need for our plot, but the surface energy flux is an array,
not a matrix. You can easilty create a 2D matrix with the shape of TCore and
Tman with the ``CreateMatrix`` function: 

.. code-block:: python

  SurfFLuxTot_Zaxis = bp.CreateMatrix(TCore_uiq,TMan_uniq,SurfFLuxTot)

This method takes 3 arguments, the x-axis vector, the y-axis vectory, and the array 
that must be converted into a 2D matrix.

Now we're ready to plot using matplotlib. Let’s use VPLane's pale blue color and make it 
readable:

.. code-block:: python

  plt.contour(TCore_uniq,TMan_uniq,SurfFLuxTot_Zaxis,colors = vpl.colors.pale_blue)


This should produce the following plot:

.. figure:: parametersweep.png

Finally, the :code:`bigplanet` module also facilitates the creation of files of meta-data
for your parameter sweep with the "WriteOutput" method:

.. code-block:: python

    WriteOutput(inputfile, columns, file="bigplanet.out", delim=" ", header=False, ulysses=False)

where:

*inputfile* is the name of the HDF5 file

*columns* is the list of keys you are extracting (Use the same format as ExtractColumn, ExtractUnits and
ExtractUniqueValues)

*File* is the name of the output file

*delim* is the delimiter for the output file (the default is spaces)

*header* adds the names and units for each column (default is False)

*ulysses* makes the file compatable with `VR Ulysses <https://www.vrulysses.com/>`_ (default is False)

