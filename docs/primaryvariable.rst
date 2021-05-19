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

  typedef void (*fnFinalizeUpdatePrimaryVariableModule)(BODY *, UPDATE *,
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

:note::

  All subroutines that return the derivative of a primary variable must have
  the argument list of the example above.


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
be added, e.g. eqtide.c. First add a new function that continues the
initialization process:

.. code-block:: bash

  void InitializePrimaryVariable(BODY *body, OPTIONS *options, UPDATE *update,
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

Next add the following block of code to the InitializeUpdateMODULE function in
the module's file:

.. code-block:: bash

  void InitializeUpdateMODULE(BODY *body, UPDATE *update, int iBody) {
    ...
    if (iBody > 0) {
    if (update[iBody].iNumPrimaryVariable == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumPrimaryVariable++;
    ...
  }

Then create the FinalizeUpdate function:

.. code-block:: bash

  void FinalizeUpdatePrimaryVariableMODULE(BODY *body, UPDATE *update,
                                           int *iEqn, int iVar,int iBody,
                                           int iFoo) {
    update[iBody].iaModule[iVar][*iEqn] = MODULE_ID;
    update[iBody].iPrimaryVariableMODULE = *iEqn;
    (*iEqn)++;
    }

where MODULE_ID is the unique integer associated with the module you are
upgrading.

Then add the primary variable to the NullDerivatives function:

.. code-block:: bash

  void NullMODULEDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                             fnUpdateVariable ***fnUpdate, int iBody) {
    ...
    fnUpdate[iBody][update[iBody].iPrimaryVariable]
            [update[iBody].iPrimaryVariableMODULE] = &fndUpdateFunctionTiny;
    ...
  }

The final initialization step is to update the AddModule function:

.. code-block:: bash

  void AddModuleMODULE(CONTROL *control, MODULE *module, int iBody,
                       int iModule) {
    ...
    module->fnFinalizeUpdatePrimaryVariable[iBody][iModule] =
        &FinalizeUpdatePrimaryVariableMODULE;
    ...
  }

Using the New Primary Variable
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

From here, you must add the particular functions that perform the mathematical
calculations associated with the primary variable. At the bare minimum, you must
add the fdDPrimaryVariableDt. Additionally, you may want to take advantage of
the PropsAux function to compute any intermediary parameters that make it easier
to understand the code. You may also need to update the ForceBehavior function.
If your new variable depends on arrays of parameters, you may also need to add
or update the InitializeBody, InitializeUpdate, InitializeTmpBody, and
InitializeTmpUpdate functions. See eqtide.c or distorb.c for examples of how
these functions work. And of course you'll probably want to add `options
<option>` and `outputs <output>`. Finally, add `examples and tests <tests>` to
show off your result and ensure that future upgrades don't destroy your work.
