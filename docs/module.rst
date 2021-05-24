Adding a New Module
===================

So you've decided you want to add a new module to VPLanet? That's great as it is
the best way to improve the code and grow our understanding of planetary and
stellar evolution! This page walks you through the steps necessary to complete
that task. While numerous modules have been implemented to date and can serve
as examples, each new module presents unique challenges, but by following the
established protocols, your new module can be "plug and play" and couple
seamlessly to the other modules.

Initial Steps
-------------

Add a new ID number in vplanet.h:

  #define NEWMODULE 2^x

where NEWMODULE is the name of your module and x is an integer that creates a
unique number for the module. Adjust the values to MODULEOPTEND and MODULEOUTEND.
Add a new struct member to the MODULE struct called *iaModuleName.

At the end of vplanet.h, add #include for your new .h file.

Identify the primary variables, if they are new to the code, follow the `How
to add a primary variable <primaryvariable>` tutorial.

Determine which new options you need and follow the `How to add an option
<option>` tutorial. Add all necessary members to the BODY struct, including new
options, primary variables, and auxiliary properties. Add all new outputs by
following the `How to add an output <output>` tutorial.

Creating the Source Files
-------------------------

For this tutorial, we will follow the format in eqtide.c, which is one of the
more mature and complicated modules in VPLanet.

Memory Allocation
~~~~~~~~~~~~~~~~~

The first function is InitializeControlEqtide which allocated memory for any
array in the CONTROL struct. In this case, we need arrays for each body related
to both rotational and orbital evolution. These arrays set how the evolution
will proceed with regards to tidally locking, or by fixing the orbit for tests.
If your module requires any functionality like this example, don't forget to
modify the CONTROL struct in vplanet.h.

Next up is BodyCopy, which copies the member of the BODY struct to
control->tmpBody for use in the Runge-Kutta substeps. This function exists
because allocating memory at every timestep is an expensive calculation. Instead
we choose to pre-allocate a BODY struct inside CONTROL, which can increase the
speed of the code by over a factor of 10 in some cases. The trade off for this
approach is that all parameters must be copied in the BodyCopy functions.

:note::

  Failure to add a BODY struct member to BodyCopy is a very common mistake! If
  your changes are not working, make sure to check this function.

The next function is InitializeBodyEqtide, which allocates memory in the body
struct based on options. In this case, the BODY struct needs space for the total
number of bodies that can be tidal perturbers.

Next is the InitializeUpdateTmpBodyEqtide function that allocates memory in the
tmpBody struct inside CONTROL.

Option Functions
~~~~~~~~~~~~~~~~

The next blocks of code are the functions for reading options, which were
discussed as part of the initial steps. Note that after InitializeOptions comes
the function ReadoptionsEqtide, which loops over all the options. Make sure your
starting and ending values in the for loop are correct.

After the ReadOptions functions come the Verify functions. As discussed in the
`Code Architecture <architecture>` page, these functions can take on many forms
that depend on the goal of the module. Recall that by the end of Verify, all the
options should be vetted for self-consistency, with errors handled in a friendly
way. After Verify, the code expects everything to be clean and ready for a fast
and accurate numerical integration.

Update and Halt Functions
~~~~~~~~~~~~~~~~~~~~~~~~~

The next sections of the code are for functions related to the UPDATE and HALT
functions. Most of the UPDATE functions should have been filled in when the
primary variables were added.

Next come the Halt functions, which are unique to the new module. All halts must
have the same argument list and must return a 1 (halt the code) or a 0 (do *not*
halt the code). After the individual halt functions, add the CountHalts and
VerifyHalts functions and your halting criteria will be evaluating at each
time step.


Output Functions
~~~~~~~~~~~~~~~~

The output function format is similar to the options. However, note that there
are two categories of output files, the evolution (.forward) files and the log
file. EqTide breaks the logging into two functions because some outputs depend
on the number of perturbers. Please use these functions as a template if your
module contains similar functionality.

AddModule
~~~~~~~~~

The AddModule function contains the assignment of the function pointer matrices
that enable the module to be integrated into the main code. Essentially, all the
work you finished above to track options, outputs, variables, etc. comes
together in this function.

Module Functions
~~~~~~~~~~~~~~~~

The final block of functions contain all the subroutines necessary to actually
compute the physics and chemistry for your module. These functions must be
tailored to your specific needs, but do remember that all primary variable
function must contain the same argument list (BODY*,SYSTEM*,int).

The Header File
~~~~~~~~~~~~~~~

At the same time you are developing the .c file, you'll want to update the .h
file. These are all pretty standard across the modules, so use them as a
template. In these files you define numbers for the options and outputs, as well
as define prototype functions for all subroutines that can be called from other
files. You can also define any other macros unique to your module here.

Updating module.c
-----------------

To finish your new module, you must also update several subroutines in module.c.
First, allocate and initialize the members of the MODULE struct in
InitializeModule. Then update FinalizeModule to increment the iNumModules
variable. Next, add a block of text for your module to AddModules that is
similar to the blocks already present. Then add the appropriate block of text to
ReadModules so that the code can actually find your new module! Next add
lines to PrintModuleList and InitializeBodyModules. The final step is to write
any `multi-module PropsAux and ForceBehavior functions, <coupling>` if
necessary. Congratulations! You've now written a new, bug-free module!
