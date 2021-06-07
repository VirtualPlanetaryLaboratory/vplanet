Add a VPLanet Output
====================

To add a new output, complete the following steps.

First, determine if the output is applicable to one module, or multiple modules.
For example, eccentricity is part of the SpiNBody, EqTide, and DistOrb modules,
but the mean anomaly is only used in SpiNBody. If the output should be callable
by multiple modules, then you should modify output.[ch], but if it is only
appropriate for an individual module, then it belongs in module files
(spinbody.[ch] for the previous example).

Next, in the header file, add a OUT macro to give your output a unique ID, e.g.

.. code-block:: bash

  #define OUT_MYNEWOUTPUT   77

Note that the number of the option must lie in the appropriate range for the
module. For new outputs in output.c, the range is 0-1000, for other modules,
consult vplanet.h, specifically the text just before MODULEOUTEND is defined.
Make sure your new outputs name is easily understood and very unlikely to be
duplicated by another variable name.

Then add a block of code in the appropriate InitializeOutput function
(InitializeOutputGeneral in output.c or InitializeOutputModuleName, where
"ModuleName" = AtmEsc, EqTide, etc., in the module's c file) like this:

.. code-block:: bash
  :linenos:

  sprintf(output[OUT_MASS].cName,"Mass");
  sprintf(output[OUT_MASS].cDescr,"Mass");
  sprintf(output[OUT_MASS].cNeg,"Mearth");
  output[OUT_MASS].bNeg = 1;
  output[OUT_MASS].dNeg = 1./MEARTH;
  output[OUT_MASS].iNum = 1;
  output[OUT_MASS].iModuleBit = 1;
  fnWrite[OUT_MASS] = &WriteMass;

These lines instruct VPLanet on the key features of the output. Line 1 contains
the name of the option that will appear in both the log file and/or the argument
list to saOutputOrder. Line 2 is the short description of the output; mass is a
fundamental property, so doesn't need much explanation. This description should
be now more than 10 words long. Line 3 is the units of the output if the user
places a negative sign ("-") in front of it and should be from the list of unit
names used in astropy. (If you must use an alternative unit, please add it to
the file custom_units.py in the vplanet folder that is created after pip
installation.) This feature is only allowed if the
output is positive definite and requires the bNeg flag be set to 1 (Line 4). If
set to 0 (or not set), then including a negative sign will cause VPLanet to exit
and error. The
next line, is a mathematical description of how to convert the units from system
units (SI) to the custom unit. The code *multiplies* the value by this
conversion factor (note that MEARTH is a macro defined in vplanet.h). Line 6
tells VPLanet how many elements are part of the output; mass is a scalar, so
this value is set to 1.

Next on Line 7 is a variable that explains what options
are associated with the output and is a bit more complicated. For ease of use,
VPLanet checks if a selected output is actually appropriate for the selected
modules. This check is done via a bitwise comparison, with each module assigned
a unique bit as defined in vplanet.h.
Outputs that are applicable to all modules, like mass,
can be set to 1 as shown. Otherwise, set the value to equal the sum of module bits,
such as is done for the rotation period:

.. code-block:: bash
  output[OUT_ROTPER].iModuleBit = EQTIDE + DISTROT + STELLAR;

In other words, each module is assigned a unique integer that is a power of 2 so
that all module combinations sum to a unique multiple of 2. When an output is
read in, VPLanet checks that at least one of the appropriate bits in the integer
is set to 1.

The last line in the example is a function pointer to the subroutine that reads
in the output. This function must be defined prior to this line. Finally, not
shown is the cLongDescr field that contains a longer description of the output
parameter that is used with the -H command line option, e.g. the "precession
angle" requires a bit more explanation for a typical user:

.. code-block:: bash
  sprintf(output[OUT_PRECA].cLongDescr,
    "The precession angle is orthogonal to the obliquity and is measured from\n"
    "the vernal equinox. This angle is a \"dog-leg\" angle as shown in Fig. 30\n"
    "of Barnes et al. (2020)."

.. note::
  Any field that is omitted will be set to the default values that are defined
  in output.c:InitializeOutput.

Next, let's turn to the fnWrite subroutine., which assigns the value and unit to
the output in preparation for writing to an output file. These functions
typically look like this:

.. code-block:: bash

  void WriteMass(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS
                 *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

    *dTmp = body[iBody].dMass;

    if (output->bDoNeg[iBody]) {
      *dTmp *= output->dNeg;
      strcpy(cUnit,output->cNeg);
    } else {
      *dTmp /= fdUnitsMass(units->iMass);
      fsUnitsMass(units->iMass,cUnit);
    }
  }

In this function the pointer to dTmp is the value(s) you want to be output. It
can be either a scalar or an array, and can be either calculated here or can
call another function. In this case, we can just use the current value of the
body's mass. The next blocks of code convert the value of the mass from system
units (SI) to the user's preferred units, which depend on if the user selected
the negative option, i.e. the argument to saOutputOrder is either "Mass" or
"-Mass". If the negative sign was not included or is not allowed, then the else
block is executed and the value is converted to the units set by the user by
options such as sUnitMass, sUnitLength, etc. In this case, mass is simple, but
other units like energy or pressure could require numerous operations to convert
to the appropriate units. In some cases, conversion subroutines for common unit
combinations already exist -- see control.c. In both cases, a string that
describes the units is also assigned, with complex unit descriptions also
available in control.c.

With these steps completed, the output parameter is now available in VPLanet.
The code will automatically include it in subsequent compilations and you do
not need to ever explicitly call a function like WriteMass.

If you want to contribute your new feature to the code base, you must also
write or update a unit test to include your new function.
