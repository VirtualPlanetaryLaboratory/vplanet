VPLanet Architecture
====================

This guide describes the fundamental feature of VPLanet's architecture,
including its flow chart, data structures, and a review of function pointers.

VPLanet's Flow Chart
--------------------

VPLanet has been designed to operate in a linear fashion with very few decision
points. The key steps are 1) read the input options, 2) verify the options, 3)
write the initial conditions to a log file, 4) integrate the system while
outputting results, and 5) write the final conditions to the log file. In
graphical form, here is the flow chart.

.. figure:: VPLanetFlowChart.png

Reading Options
~~~~~~~~~~~~~~~

The first step in the code's execution is to read in the options specified by
the user and a function called ReadOptions. These instructions are in ASCII text
files and are typically called
the "infiles" as the team has traditionally used the extension .in with them.
The infiles contain "options" that are described in the modules and options
page, as well as with the -h and -H command line flags.

At this stage, numerous simple checks
are performed on the input, such as ensuring that arguments to an option are not
unphysical, e.g. eccentricities less than 0. Each of these checks must only
apply to an individual option. More complicated checks are handled
in Verify.

:note::

  Some arrays and matrices are allocated prior to the ReadOptions call.

Verifying Options
~~~~~~~~~~~~~~~~~

Once all the options have been read in, the data are passed to Verify, which is
one of the most complicated aspects of the code. It's in Verify that the options
are checked for self-consistency. For example, if a user inputs mass, radius and
density for a body, it's very unlikely they are all consistent at machine
precision. These sorts of quality control issues are handled in Verify.
Additionally, it is in Verify that most arrays and matrices, including function
pointers, are allocated. The entry point to this process is in
verify.c:VerifyOptions.

Because any given verification step is likely unique, there is not a universal
standard for verification. As a developer, you must determine how best to
confirm that the user has asked for a valid simulation. A primary goal of
VPLanet is to provide a user-friendly code that identifies all inconsistent
input because its interdisciplinary nature makes it easy for a user to request
an unphysical simulation.

That said, many generic problems have already been identified and solution have
been implemented. If you are working on a verification step, you should check
verify.c and all the module files to see if a solution has been presented. Note
that the modules all have subroutines called something like VerifyAtmEsc that
performs verification steps that are unique to that module.

Writing to the Log
~~~~~~~~~~~~~~~~~~

VPLanet includes functionality to thoroughly document each simulation, and one
of the primary methods for this feature is the log file. Writing a log file is
optional, but it is highly recommended for both sanity checks, as well as for
use with its support scripts. If the user elects to write a log file, it will
occur after verification **and** after the integration (if an integration is
requested). In other words the log file provides a snapshot of the initial and
final conditions of the simulation.

The log file records the value of each output in system units (SI). While
options can be input in a wide range of units, recording the state in system
units allows users to determine if they have input something incorrectly, and
also helps developers identify bugs.

:note::

  If a user requests an output with a negative sign, which forces a specific
  unit, then the value in the log file will not be SI, but the custom units
  associated with that output.

For example, a log file entry might look like this:

.. code-block:: bash

  (Age) System Age [sec]: 3.155760e+14

This line provides basic information about a body. In this case it's the age of
the body. The first field, in parentheses, is the output name, i.e. a user could
copy and paste that string into the argument list of saOutputOrder and its value
at each output time would be included in the output file. The next field is a
short description of the output. Next, in brackets, is the units of the output,
and finally the actual value of the parameter is listed. The log file breaks all
these outputs down by body, module, and initial/final conditions.

System Integration
~~~~~~~~~~~~~~~~~~

After verify and possible logging, the code will then begin integrating the
system of equations that are required based on the options. The integration is
broken down into five steps: Auxiliary Properties (sometimes shortened to
AuxProps), One Step, Output, Force Behavior, and Halt.

:note::

  If the user does not select a forward or backward integration, but does elect
  to write a log file, then the code will calculate all the initial conditions
  and write them to the log. This feature can be useful if one just wants to
  assess the current state of a system.

**Auxiliary Properties**: Prior to moving the system one step forward, the code
calculates "helper" variables that make the code easier to parse. While strictly
speaking the code would be faster if these parameters were not calculated
separately, the VPLanet team has found that the code is much easier to read and
debug with this step. There are not strict rules for what should or should not
be an auxiliary property, but given that they are calculated every step, you
should be judicious in their use.

**One Step**: After calculating the auxiliary properties, the code is now ready
to take a step forward. Currently two integration schemes have been implemented
in VPLanet: Euler and 4th order Runge-Kutta. The former is strongly discouraged
and should only be used for testing/educational purposes.

**Output**: After taking one step, the code then compares the new simulation
time to the output interval. If it's time for an output, the WriteOutput
function is called and all outputs are sent to the output file(s), usually
designated as .forward or .backward depending on the direction of evolution that
was selected.

**Force Behavior**: Next the code determines if any fundamental change needs to
be implemented. For example, if all the water is lost from a planet's
atmosphere, then that value should be set to 0 and the function pointer in
fnUpdate should be updated to point to fdTiny, i.e. water loss should no longer
occur. The details of this step are unique to each module and there are no
strict guidelines here. As with Verify, check the module files for methods that
might work for your given problem.

**Halt**: The final step in the integration is to check for halts, which are
conditions that cause the code execution to end. For example, a user may want to
stop the code if all the water is lost from a planet. If this occurs, the code
will write one more output to the .forward/.backward files at the time the
halting condition was triggered. Note that the final conditions will still be
written to the log file in the event of a halt.

Data Structures
---------------

VPLanet consists of 8 data structures (structs) that contain all the informaton needed
to perform a simulation: Control, Body, System, Update, Module, Files, Options,
and Output. Additionally, it uses 3 function pointer arrays/matrices:
fnReadOption, fnWriteOutput, and fnUpdate.

Control
~~~~~~~

The Control struct contains all the variables related to how the code runs. Some
of these variable are housed inside 4 additional structs: Halt, Io, Evolve, and
Units. Many of the variables in the main Control struct are function pointer
arrays that enable individual modules to control the execution.

**Halt**:
