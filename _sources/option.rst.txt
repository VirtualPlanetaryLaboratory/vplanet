Add a VPLanet Option
====================

To add a new option, complete the following steps.

First, determine if the option is applicable to one module, or multiple modules.
For example, eccentricity is part of the SpiNBody, EqTide, and DistOrb modules,
but the mean anomaly is only used in SpiNBody. If the option should be
available to multiple modules, then you should modify options.[ch], but if it is
only appropriate for an individual module, then it belongs in module files
(spinbody.[ch] for the previous example).

Next, in the header file, add a OPT macro to give your output a unique ID, e.g.

.. code-block:: bash

  #define OPT_MYNEWOPTION   77

Note that the number of the option must lie in the appropriate range for the
module. For new options in option.c, the range is 0-1000, for other modules,
consult vplanet.h, specifically the text just before MODULEOPTEND is defined.
Make sure your new outputs name is easily understood and very unlikely to be
duplicated by another variable name.

Then add a block of code in the appropriate InitializeOptions function
(InitializeOptionsGeneral in output.c or InitializeOptionsModuleName, where
"ModuleName" = AtmEsc, EqTide, etc., in the module's c file) like this:

.. code-block:: bash
  :linenos:

  sprintf(options[OPT_STOPTIME].cName,"dStopTime");
  sprintf(options[OPT_STOPTIME].cDescr,"Integration Stop Time");
  sprintf(options[OPT_STOPTIME].cDefault,"10 years");
  options[OPT_STOPTIME].dDefault = 10*YEARSEC;
  sprintf(options[OPT_STOPTIME].cDimension,"time");
  options[OPT_STOPTIME].iType = 2;
  options[OPT_STOPTIME].bNeg = 1;
  sprintf(options[OPT_STOPTIME].cNeg,"Years");
  options[OPT_STOPTIME].dNeg = YEARSEC;
  options[OPT_STOPTIME].iFileType = 2;
  options[OPT_STOPTIME].iModuleBit = 0;
  fnRead[OPT_STOPTIME] = &ReadStopTime;

These lines instruct VPLanet on the key features of the option. Line 1 is the
name of the option that can appear in the input files. Line 2 is a short
description of the option that will appear in the help message. Lines 3-4 are
for the default value the option will be given if it is not included in an input
file. The string is human readable and appears in the help file, the double is
the mathematical representation of the string. Line 5 is the dimension (units)
of the option (only applicable to doubles). Line 6 is the type or cast of the
option (0 = boolean, 1 = integer, 2 = double, 4 = string, 10 = array of bools,
11 = array of ints, 12 = array of doubles, 13 = array of strings). Line 7 is a
flag for permitting the user to use a negative sign ("-") in front of the
argument to select a custom unit, which is only permitted for positive-definite
parameters. If set to true (1), then the next two lines define the alternative
units' qualitative description and quantitative value. Line 10 sets what file(s)
the option can be set in (0 = primary only, 1 = body file only, 2 = any file).

Next on Line 11 is a variable that explains what options are associated with the
option and is a bit more complicated. For ease of use, VPLanet checks if each
option is actually appropriate for the selected modules. This check is done via
a bitwise comparison, with each module assigned a unique bit as defined in
vplanet.h. Options that are applicable to all modules, like stop time,
should be set to 0 as shown. Otherwise, set the value to equal the sum of module
bits, such as is done for the inclination:

.. code-block:: bash

    options[OPT_INC].iModuleBit = DISTORB + SPINBODY;

In other words, each module is assigned a unique integer that is a power of 2 so
that all module combinations sum to a unique multiple of 2. When an option is
read in, VPLanet checks that either iModuleBit = 0, or that at least one of the
appropriate bits in the integer is set to 1.

The last line in the example is a function pointer to the subroutine that reads
in the option. This function must be defined prior to this line. Finally, not
shown is the cLongDescr field that contains a longer description of the output
parameter that is used with the -H command line option, e.g. the verbosity
benefits from a bit more explanation:

.. code-block:: bash

  sprintf(options[OPT_VERBOSE].cLongDescr,
  "Set how much text is written to the screen. 0 = no output, 1 = only\n"
  "errors, 2 = progress updates, 3 = statements about input choices,  4 =\n"
  "information about unit choices, 5 = all possible output. Note that levels\n"
  "0 and 5 can be set at execution with the -q and -v options, respectively."

.. note::

  Any field that is omitted will be set to the default values that are defined
  in output.c:InitializeOutput.

Now let's look at the subroutine that actually reads in the option, sticking
with the stop time:

.. code-block:: bash
  :linenos:

  void ReadStopTime(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,
                    SYSTEM *system,int iFile) {
    /* This parameter can exist in any file, but only once */
    int lTmp=-1;
    double dTmp;

    AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,
                    control->Io.iVerbose);
    if (lTmp >= 0) {
      /* Option was found */
      CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,
                       control->Io.iVerbose);
      if (dTmp < 0) {
        control->Evolve.dStopTIme = dTmp*dNegativeDouble(*options,
                                    files->Infile[iFile].cIn,
                                    control->Io.iVerbose);
      } else {
        /* Convert stop time to cgs */
        control->Evolve.dStopTime = dTmp*fdUnitsTime(control->Units[iFile].iTime);
        }
      UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
    } else {
      AssignDefaultDouble(options,&control->Evolve.dStopTime,files->iNumInputs);
    }
  }

The variable lTmp will represent the line number the option was found on (or,
if not found, it remains -1), and dTmp holds that value as shown in the input
file. Line 7 searches the input file for the option and, if found, populates the
lTmp and dTmp variables.

Next the function enters and if-then block based on whether or not the option
was found. If it was, then the first thing to check is if the option is OK to be
in this file. This feature is something the developer must decide. In this case,
it does not make sense for multiple instances of dStopTime to appear in multiple
files (not that the AddOptionDouble automatically scans for duplications inside
a single file and errors if more than one is found), so we call
CheckDuplication, which will exit if duplications are present. If the file
should not be in the primary input file, call the NotPrimaryInput function, such
as for the semi-major axis:

.. code-block:: bash

    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);

Moving on, line 13 then starts a new if-then block to perform basic checks on
the validity of the argument. In this case, the option has an explicit negative
option, so the value is converted to the appropriate units and assigned to
appropriate struct. If, on the other hand, the argument to the option is
positive then the argument is converted into the units specified by the user,
either by setting sUnitTime, or the default time units. In both cases, these
operations convert the units to system units, which are SI.

After assigning the user input to the appropriate struct, UpdateFoundOption is
called to perform bookkeeping on where the option was found and in which file.
Finally, if the option was not set, the default value is assigned. With these
steps completed, the option is now available in VPLanet. The code will
automatically include it in subsequent compilations and you do not need to
explicitly call a function like ReadStopTime.

While this completes the procedure for the stop time, a second step is often
required that we call verify. This step involves checking for consistency
*across* options. For example, if a user inputs semi-major axis and orbital
period, then there is a high likelihood the two values are not equivalent to
machine precision. Thus, in verify.c there is a function called VerifyOrbit that
handles these sorts of issues. In general, there is not universal algorithm to
resolve multi-option inconsistencies and so the developer must write a new
verify function to ensure that all the input is self-consistent.

If you want to contribute your new feature to the code base, you must also
write or update a unit test to include your new function.
