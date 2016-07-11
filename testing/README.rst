TESTING
-------

The runs in this directory are meant to test individual modules and couplings between
two or more modules. Module owners should create a series of benchmark runs that will
be used in testing whenever new commits are made. 

Running tests
=============

To run all tests, simply use the VPLANET ``Makefile``:

.. code-block:: bash
   
   make test

If all goes well, you'll see the following output:

.. code-block:: bash
   
  Compiling vplanet...
  Running vtest...

  ╔═════════════════════════════════════════════════════════════════╗
  ║                                                                 ║      
  ║   ██╗   ██╗██████╗ ██╗      █████╗ ███╗   ██╗███████╗████████╗  ║
  ║   ██║   ██║██╔══██╗██║     ██╔══██╗████╗  ██║██╔════╝╚══██╔══╝  ║
  ║   ██║   ██║██████╔╝██║     ███████║██╔██╗ ██║█████╗     ██║     ║
  ║   ╚██╗ ██╔╝██╔═══╝ ██║     ██╔══██║██║╚██╗██║██╔══╝     ██║     ║
  ║    ╚████╔╝ ██║     ███████╗██║  ██║██║ ╚████║███████╗   ██║     ║
  ║     ╚═══╝  ╚═╝     ╚══════╝╚═╝  ╚═╝╚═╝  ╚═══╝╚══════╝   ╚═╝     ║
  ║                                                                 ║
  ║     BUILD PASSING: Fri, 17 Jun 2016 14:56:53                    ║
  ║     GIT BRANCH:    vplot                                        ║
  ║     GIT HASH:      e129fff159084b3a1d83ab43e303d45c42e0ac43     ║
  ║                                                                 ║
  ╚═════════════════════════════════════════════════════════════════╝

Any discrepancies between new runs and the benchmark runs result in error messages
printing to the screen:

.. code-block:: bash
   
   Compiling vplanet...
   Running vtest...
   [stellar_atmesc] ERROR: final.star.Luminosity (8.68848e-04 != 1.00087e+00)
   FAILURE: There was 1 error.

Generating tests
================

Create a subdirectory in ``testing/`` whose name is the name of one or more modules
whose features are to be tested; join module names with an underscore. For instance,
the directory ``stellar_atmesc`` tests the coupling between the ``stellar`` and 
``atmesc`` modules. Within that directory, create all ``.in`` files as usual; make sure
to name the main one ``vpl.in``. Run ``vplanet`` and add the extension ``.truth`` to
the log file (in the ``stellar_atmesc`` example, this file is ``stellar_atmesc.log.truth``).
Finally, create a file called ``test.py`` in the same directory. In this file, you may
assign values to two variables: ``params`` and ``tolerance``:

.. code-block:: python

   params = ['final.star.Luminosity', 
             'final.star.LXUVFrac', 
             'final.a.EnvelopeMass']
   tolerance = 1.e-5

The ``params`` variable is a list of parameter names whose values will be compared against
the benchmark values in the ``.log.truth`` file. Each element in the list should follow
the format

.. code-block:: python

   <stage>[.<body name>].<variable name>
  
where ``<stage>`` is one of ``header``, ``initial`` and ``final``, corresponding to the
header, the "Initial System Properties", and the "Final System Properties"
sections of the log file, respectively. The ``<body name>`` attribute is the name of 
the planet/star specified in the ``.in`` files (only availabel for the ``initial`` and 
``final`` stages), and ``<variable name>`` is the name of the variable/parameter whose
value is to be checked. In the example above, the final stellar luminosity, the final 
stellar XUV fraction, and the planet ``a``'s final envelope mass will be checked against
the benchmark value.

The ``tolerance`` variable specifies the maximum (absolute value of the) *fractional* 
difference between the new value and the benchmark value. Differences above this level
will raise an error.

Notes
=====

Developers should run all tests before creating a pull request to the ``master`` branch.
The ``master`` branch should **always** pass every test.

Please disable your tests (by setting ``params = []`` in ``test.py``) while making changes 
to your code that will affect the output
the test is checking. Once you're happy with the new version of your module, re-enable
the relevant tests.

Please keep your test runs **as short as possible**. Runs that take more than 10 seconds
to complete will automatically issue an error. Do not run very long simulations in these
tests; if you'd like to test whether a particular transition/event occurs (such as 
complete evaporation of an envelope at a certain time), begin and end
your simulation in the vicinity of this event.
