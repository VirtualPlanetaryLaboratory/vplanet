How to Add a Primary Variable
=============================

This documentation shows you how to introduce a new variable into a VPLanet
module. For this tutorial, we add a variable called dPrimaryVariable.

Is the parameter a primary variable?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A "primary variable" is a parameter whose evolution is explicitly tracked by
VPLanet, e.g. its evolution is controlled by an ordinary differential equation.
In other words, adding a primary variable is equivalent to adding a new
governing equation that fundamentally affects a star or planet's evolution.
If that is **not** the case, i.e. it depends only on other primary variables, then
you probably only need to add it to a struct (like BODY), some functions to
calculate its values, `options <option>` and/or `outputs <outputs>`, and a call
in PropsAux and/or ForceBehavior. In many cases just adding an output function
that explicitly calculates the value is sufficient.

If, however, you do want to add a new primary variable, this tutorial will guide
you through the steps to add it.

Is the primary variable multi-module?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Some variables can be modified by my multiple modules, while others will be
exclusive to a single module. For example, eccentricity can be affected by
both tidal evolution (EqTide) and/or planet-planet perturbations (SpiNbody or
DistOrb). The setup for the variable is the same, but multi-nodule primary
variables will require additional steps to fully integrate them. For this
tutorial, we focus on a single-modules primary variable.


Declaration
~~~~~~~~~~~

The first step in creating a primary variable is to add it to the BODY struct in
vplanet.h, i.e.

.. code-block:: bash

  struct BODY {
    ...
    double dPrimaryVariable;  /**< Dummy variable for primary variable tutorial */
    ...
  };

If the new primary variable is part of an existing module, add it to the
corresponding block of code. Note the Doxygen comment.

Next, we add new members to the UPDATE struct, which contains the relevant info
to integrate the primary variable.

.. code-block:: bash

  struct UPDATE {
    ...
    int iPrimaryVariable;
    int iNumPrimaryVariable;
    ...
    double dPrimaryVariable;
    ...
    double *pdDPrimaryVariableDt;
    ...
  };

The first parameter describes the position of this variable in the UPDATE matrix;
the second is the total number of processes that can modify the primary variable;
the third is the value of the primary variable at a given timestep; and the
final variable is a pointer to the instantaneous derivative.

Next, add a new function class for the variable in vplanet.h in the block of
declarations for the other primary variables:

.. code-block:: bash

  typedef void (*fnFinalizeUpdatePrimaryVariabletModule)(BODY *, UPDATE *,
                int *, int, int, int);

and add it to the MODULE struct:

.. code-block:: bash

  struct MODULE {
    ...
    fnFinalizeUpdatePrimaryVariableModule **fnFinalizeUpdatePrimaryVariable;
    ...
  };

The next step is to add a unique identifier in update.h:

.. code-block:: bash

  #define VPRIMARYVARIABLE 2000     // Lost Energy

where the "V" stands for variable, and the integer ID must be unique.

The final initialization step is to declare the subroutine that returns the
derivative in the module's header file. For example, if adding a primary
variable to AtmEsc, add the following line to atmesc.h:

.. code-block:: bash

  double fdDPrimaryVariableDt(BODY *, SYSTEM *, int *).


Initialization
~~~~~~~~~~~~~~

With the header files updated, we now turn to the steps necessary in the .c
files. We start with update.c. First initialize the number of processes that
can modify this variable in update.c:InitializeUpdate and create a block of code
that initializes the other pieces of the UPDATE struct.

.. code-block:: bash

  void InitializeUpdate(BODY *body, CONTROL *control, MODULE *module,
                        UPDATE *update, fnUpdateVariable ****fnUpdate) {
    ...
    update[iBody].iNumPrimaryVariable = 0;
    ...
    update[iBody].iPrimaryVariable = -1;
    if (update[iBody].iNumPrimaryVariable) {
      update[iBody].iPrimaryVariable = iVar;
      update[iBody].iaVar[iVar]      = VPRIMARYVARIABLE;
      update[iBody].iNumEqns[iVar]   = update[iBody].iNumPrimaryVariable;
      update[iBody].pdVar[iVar]      = &body[iBody].dPrimaryVariable;
      update[iBody].iNumBodies[iVar] =
         malloc(update[iBody].iNumPrimaryVariable * sizeof(int));
      update[iBody].iaBody[iVar] =
         malloc(update[iBody].iNumPrimaryVariable * sizeof(int *));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumPrimaryVariable *
         sizeof(int));
      update[iBody].iaModule[iVar] =
         malloc(update[iBody].iNumPrimaryVariable * sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] =
           &control->Evolve.tmpBody[iBody].dPrimaryVariable;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] =
           malloc(update[iBody].iNumPrimaryVariable * sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] =
           malloc(update[iBody].iNumPrimaryVariable * sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] =
           malloc(update[iBody].iNumPrimaryVariable * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] =
           malloc(update[iBody].iNumPrimaryVariable * sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] =
           malloc(update[iBody].iNumPrimaryVariable * sizeof(int *));
      }

      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdatePrimaryVariable[iBody][iModule](body, update,
                  &iEqn, iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++;
    }
    ...
  }

There's a lot going on here, but all these lines do is setup your new primary
variable.

:note::
  The pdVar variable is a pointer to the update matrix's element that is
  calculated every time step. If you ever assign a new value to this variable,
  you will overwrite the primary variable's derivative!


Next we need to add code to the module file for which this primary variable will
be added, e.g. eqtide.c. First add a new verify function that continues the
initialization process:

.. code-block:: bash

  void VerifyPrimaryVariable(BODY *body, OPTIONS *options, UPDATE *update,
                          double dAge, int iBody) {

    update[iBody].iaType[update[iBody].iPrimaryVariable][0]     = 1;
    update[iBody].iNumBodies[update[iBody].iPrimaryVariable][0] = 1;
    update[iBody].iaBody[update[iBody].iPrimaryVariable][0]     = malloc(
        update[iBody].iNumBodies[update[iBody].iEnvelopeMass][0] * sizeof(int));
    update[iBody].iaBody[update[iBody].iPrimaryVariable][0][0] = iBody;

    update[iBody].pdDPrimaryVariableDt =
        &update[iBody].daDeriv[update[iBody].iEnvelopeMass];
  }

Then add a new line to the AssignDerivative function to set the appropriate
function pointer to the fnUpdate matrix:

.. code-block:: bash

  void AssignMODULEDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                              fnUpdateVariable ***fnUpdate, int iBody) {
    ...
    fnUpdate[iBody][update[iBody].iPrimaryVariable]
            [update[iBody].iPrimaryVariableMODULE] = &fdDPrimaryVariableDt;
    ...
  }

where "MODULE" is the name of the module to which you are adding the primary
variable. The function fdDPrimaryVariableDt is a subroutine that return the
derivative of the primary variable.




Step 3: Working with the variable
Calling your variable
In this example, if you follow the example above your variable can be called like this:

That is, it is part of the body struct and will be visible for every function within Module.src.

PropsAuxModule
After you have declared your variable, you can use it now in Module.src either
in Module.c ->PropsAuxModule. In this example in magmoc.c -> PropsAuxMagmOc
Variables modified here are available during every quarter step of Runge-Kutta numerical integration. If you use your variables to modify primary variables, these should usually go here.


fndForceBehaviorModule

Instead of Module.c->PropsAuxModule, you can also modify the variable in Module.c-> fnForceBehaviorModule. In this example in magmoc.c -> fnForceBehaviorMagmOC
(Note the usage of the prefix ‘fn’, which stands for ‘function’ and follows in philosophy the naming of the variables.)



Modifying your local variable

Example 1:
You can modify your variable either directly within fndForceBehaviorModule and/or PropsAuxModule or you can define your own function fnMyFunction. In this example, that is
magmoc.src-> fndMagMOCtestUpdate, which is defined as a small independent function. In this case, we also call the structure EVOLVE outside of the module to get access to the internal simulation time and to define dMagMOCtest analytically.


The function is called via:  in magmoc.c -> fnForceBehaviorMagmOC

Example 2:

Note that dMagMOCtest is not initialized here. In this example, this is because dMagMOCtest is initialized by reading in an input file at the beginning of the simulation. This function is called via:  in magmoc.c -> fnForceBehaviorMagmOC

Input/Output of your local variable

You can read in the initial value for your local variable and/or write the variable during your simulation to the output file and from the input file that are always needed with VPLANET. For this you have to define new read and write functions inside your module:

Set pointer
VPLANET operates with pointers. Thus, you also have to define a pointer if you want your variable to interact with routines inside and outside of the module (as in read,write). This is done in Module.h and in our case in magmoc.h



You notice here that the module has a range of addresses reserved. So you need to choose one number within this range, which is not yet occupied. For OPT_VARIABLE (input) and OUT_VARIABLE (output).



Write Function:
In all modules you can find examples of write functions for variables. It is advisable to copy one of those and adapt it for your own needs. In this example:  magmoc.src-> WriteMagMOCtest



dTmp  takes the value from your variable.
Further you need to declare the function in Module.h. In this example in  magmoc.h like this:



You need to call the read function in  Module.c -> InitializeOutputModule and tell it how you want to write the variable in Module.c. Again, we advise you to use similar functions of other modules and to adapt them to your needs. In this example in magmoc.c->InitializeOutputMagmOc




Read Function:
In all modules you can find examples of read functions for variables. It is advisable to copy one of those and adapt it for your own needs. In this example:  magmoc.src-> ReadMagMOCtest



dTmp  takes the value from the input file.
Further you need to declare the function in Module.h. In this example in  magmoc.h like this:



The read function needs also to be called within the function InitializeOptionsModule. In this example it is in magmoc.c -> InitializeOptionsMagmOc

Again, we advise to adapt usage from other similar variables.


The read in file is in our case g.in and placed in the runs Folder. It calls the necessary modules, here e.g. magmoc:



And specifies the start value for our example variable: dMagMOCtest



Further, the file  g.in specifies which variables are written out:


Please note the usage of the “$” sign. This is a ‘continue line’ statement here, because the string is otherwise truncated due its length.

Step 4: Working with primary variables

Here, we need more steps. You also need to modify update.h and update.c if you want to integrate your variable during the simulation.




Define in structure update
So far the variable has been declared as part Struct body. But now you also have to declare as part of Struct update. That is, the variable has to be declared twice in vplanet.h

in
Vplanet.h-> struc UPDATE
Set pointer to iaVar (array to keep track of primary variables)


In update.h you set again a pointer to a generic address for Vvariable, here VMAGMOCTEST that will be later set in iaVar. iaVar is an array that keeps track on which primary variable is associated with which module (accounting level)





Further you have to declare in vplanet.h the iVariable and iNumVariable, here iMagMOCtest and iNumMagMOCtest


Define structure
In update.c->InitializeUpdate: Initialize here with 0, because we assume when we start that 0 modules are modifying these properties.)
Important: Here you define a structure for your variable that take the general properties of your variable. This will become apparent in the next example.


Tipp: “Just” copy another structure and adopt accordingly. We assume here that nothing is yet associated with iMagMOCtest. We set to -1 because iVar (needed at 2nd level to loop over all primary variables in Evolve.c can start at 0 already.)


Careful: update.pdVar[ivar] instantly resets the actual variable you want to integrate.




The variable dMagMOCtest is now passed over to -> Evolve via rungekutta numerical integration. Further, a function fnFinalizeUpdateMagMOCtest is called at the end of the initialization. Generally, fnFinalizeUpdateVariable has to be defined in module.c and module.h

Caution! Make sure that your iVariable is not changed anywhere else. You need the assignment to the proper iVar to assess the value subsequently, everywhere else. Especially, when you want to integrate.


Be also careful about iNumVarieble or here  iNumMagMOCtest

In this example, it has to be defined in magmoc.c-> AddModuleMagmOc




iaVar is needed in the engine “Evolve” and pointing to pdVar, which is defined in the local modules.


CAREFUL: Verify <.> Initialize. Patrick has swapped this. Patrick has assumed initial values in initialize. Rory does it in Verify. He uses Initialize for memory allocation. Point out the differences in the documentation.

fnFinalizeUpdate
Declare  vplanet.h -> fnFinalizeUpdateMagMOCtestModule
(pointer to pointer in struc module)

And set in structure module

The pointer to the pointer







Set fnFinalizeUpdateMagMOCtest to set in magmoc.c -> AddModuleMagmOc and to point to function FinalizeUpdateMagMOCtest.




Allocate necessary memory in module.c-> InitializeModule -> fnFinalizeUpdateVariable






The same in module.c-> FinalizeModule -> fnFinalizeUpdateVariable






Further:
 /* Initialize all module functions pointers to point to their respective NULL function. The modules that need actual function will replace them in AddModule. */

Last but not least, define what the Finalize function is doing (which equation is called for the variable in the iEqn direction). Here:


Note, here we introduce a new variable iVariableModule for bookkeeping. It is not the same as iVariable. This new variable has also to be declared in vplanet.h in the same place as iVariable and INumVariable.


That is in structure declaration for Vplanet.h-> UPDATE:


Q to Patrick: Why is in module.h->void PropsAuxMagmOcAtmEsc but not PropsAuxMagmOc. And why are these separate? And why is it not defined?
Is there for the case that something needs to be done both to MagmOc and AtmEsc
Test case:
Can MagmOc be run by itself? For a planet far away from their star.
Define derivative fdDVariable

Your primary variable is integrated in evolve.c  Thus you need to formulate a derivative and associate it with your primary variable.



You should define your derivative in module.c. Here a very simple example:  magmoc.c -> fdDMagMOCtest

And of course also declare in module.h. Here  magmoc.c -> fdDMagMOCtest


Caution: If you first started using the variable as a module specific variable, you have to make sure that the primary variable is changed via the derivative and not somewhere else - unless you really want this. E.g. when a certain threshold is reached. In that case, make sure that the interactions between the integration of the primary variable and functions in your module are consistent.

Here, for example. The call to fndMagMOCtestUpdate has been removed.

Then, assign the address of the derivative to the primary variable via iVariable. Here, to iMagMOCtest in
magmoc.c->  AssignMagmOcDerivatives:



You also have to define a VerifyVariable function in module.c In this example,
magmoc.c -> VerifyMagMOCtest.


This you will need before you start the actual integration. (Only for MagMOC for other this is actually Initialize) TBD

This is called in module.c -> VerifyMagMOC-> VerifyMagMOCtest


And one has to define: pdDMagMOCtest in Vplanet.h -> UPDATE:




Remember!
update[iBody].iMagMOCtest  = iVar (is a unique integer for every primary variable, if not set -1)
Is iVar properly set? Why is address not associated
fnUpdate[iBody][update[iBody].iMagMOCtest][0] = &fdDMagMOCtest
(Check this print update[iBody].iMagMOCtest )
Print fnUpdate[iBody][0][0] = is ok

Further, you have to set iNumVariable (here iNumMagMOCtest) to make sure that the structure is actually filled in update.c.


 You can two derivates to fnUpdate[iBody][update[iBody].iMagMOCtest][1] = &fdDMagMOCtest ?! And you actually need to with a dynamical time step.

You have to do this in your module e.g. in this example: magmoc.c -> InitializeUpdateMagmOc




Q: Why is the last line not inside the if statement? In other words, the second variable is always increased by one and the first only of iNumMagMOCtest is 0.
I NumVars - iNumVariable
NumVars tells you that you need for the variable have a place in the matrix. However, you can have different equations that modify variable and fot bookkeeping that we have iNumMagMOCtest for example.

Q: Difference between iBody in function ForceBehavour and iaBody in definition of primary variable derivatives
iBody is generic counter for the body you are “working in” - iaBody is an array of all the bodies indices in the proper order . E.g. tides can affect multiple bodies.

Look at VerifyEqTide!
General note:
We have three levels in the code where pointer are used to get from the top to the bottom dimension:

Level 1 (top): The general infrastructure. Modules and simulation routine. (e.g. body & evolve)
Level 2 (middle): The book-keeping level. Here an overview is kept of how many modules and primary variables there are. Checks are in place to ensure that modules are complete and do not interfere with each other. (e.g. iVar )
Level 3 (bottom): The actual physics and calculation level. Here the variables and auxiliary variables and functions are defined.(e.g.  pdVar )
