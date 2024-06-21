How to Add a Primary Variable
=============================

This page shows how to introduce a new primary variable into a VPLanet
module. For this tutorial, we add a variable called dPrimaryVariable to a
fictitious module called Physics that is in files called physics.[ch]. For your
new primary variable, replace "PrimaryVariable with the name of your new primary
variable and ""Physics" with the name of the module you are upgrading.

Is the parameter a primary variable?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A "primary variable" is a parameter whose evolution is explicitly tracked by
VPLanet, e.g. its evolution is controlled by an ordinary differential equation.
In other words, adding a primary variable is equivalent to adding a new
governing equation that fundamentally affects an object's evolution.
If that is **not** the case, i.e. it depends only on other primary variables, then
you probably only need to add it to a struct (like BODY), some functions to
calculate its values, `options <option>`_ and/or `outputs <outputs>`_, and a call
in PropsAux and/or ForceBehavior. In many cases just adding an output function
that explicitly calculates the value is sufficient.

If, however, this parameter is indeed a primary variable, this tutorial will
guide you through the steps to add it.

Is the primary variable multi-module?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Some variables can be modified by my multiple modules, while others will be
exclusive to a single module. For example, eccentricity can be affected by
both tidal evolution (EqTide) and/or planet-planet perturbations (SpiNbody or
DistOrb). The setup for the variable is the same, but multi-nodule primary
variables will require additional steps to fully integrate them. For this
tutorial, we focus on a single-modules primary variable. If your primary
variable is multi-module, complete the steps shown below, then look at how a
primary variable like dHecc (a variable related to eccentricity and longitude of
pericenter) are handled, including in module.c.

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
    int iPrimaryVariablePhysics;
    ...
    double *pdDPrimaryVariableDtPhysics;
    ...
  };

The first parameter describes the position of this variable in the UPDATE matrix;
the second is the total number of processes that can modify the primary variable;
the third is the value of the primary variable at a given timestep; and the
final variable is a pointer to the instantaneous derivative. Although not shown
here, don't forget to add the Doxygen comments.

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

  #define VPRIMARYVARIABLE 2000     // Dummy primary variable

where the "V" stands for variable, and the integer ID must be unique.

The final initialization step is to declare the subroutine that returns the
derivative in the module's header file, which we're calling physics.h for this
example.

.. code-block:: bash

  double fdDPrimaryVariableDt(BODY *, SYSTEM *, int *);

.. note::

  All subroutines that return the derivative of a primary variable must have
  the argument list of the example above.


Initialization
~~~~~~~~~~~~~~

With the header files updated, we now turn to the steps necessary in the .c
files. We start with update.c. First, initialize the number of processes that
can modify this variable in the InitializeUpdate function to 0, and create a
block of code that initializes the other pieces of the UPDATE struct.

.. code-block:: bash

  void InitializeUpdate(BODY *body, CONTROL *control, MODULE *module,
                        UPDATE *update, fnUpdateVariable ****fnUpdate) {
    ...
    update[iBody].iNumPrimaryVariable = 0;
    ...
    // Set to -1 to initialize; this is changed if the user requests it
    update[iBody].iPrimaryVariable = -1;
    if (update[iBody].iNumPrimaryVariable) {
      // At least 1 module will use this variable

      // Assign iVar accounting variables
      update[iBody].iPrimaryVariable = iVar;
      update[iBody].iaVar[iVar]      = VPRIMARYVARIABLE;
      update[iBody].iNumEqns[iVar]   = update[iBody].iNumPrimaryVariable;
      // Assign pointer to derivative
      update[iBody].pdVar[iVar]      = &body[iBody].dPrimaryVariable;
      // Allocate memory
      update[iBody].iNumBodies[iVar] =
         malloc(update[iBody].iNumPrimaryVariable * sizeof(int));
      update[iBody].iaBody[iVar] =
         malloc(update[iBody].iNumPrimaryVariable * sizeof(int *));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumPrimaryVariable *
         sizeof(int));
      update[iBody].iaModule[iVar] =
         malloc(update[iBody].iNumPrimaryVariable * sizeof(int));

      // Assign and alloate memory for Runge-Kutta integration
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
        for (iSubStep=0; iSubStep < 4; iSubStep++) {
          control->Evolve.daDerivProc[iSubStep][iBody][iVar] =
              malloc(update[iBody].iNumEqns[iVar] * sizeof(double));
        }
      }

      // Now allocate memory for the number of processes that affect this variable
      iEqn = 0;
      for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
        module->fnFinalizeUpdatePrimaryVariable[iBody][iModule](body, update,
                  &iEqn, iVar, iBody, iFoo);
      }

      (*fnUpdate)[iBody][iVar]        = malloc(iEqn * sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar] = malloc(iEqn * sizeof(double));
      iVar++; // increment iVar for the next primary variable
    }
    ...
  }

There's a lot going on here, but all these lines do is setup your new primary
variable.

.. note::
  The pdVar variable is a pointer to the update matrix's element
  (fnUpdate[iBody][iVar][iEqn]) that is calculated every time step. If you ever
  assign a new value to this variable, you will overwrite the primary variable's
  derivative (which would be bad!).

We then need to add some lines to ```module.c```. In InitializeModule, add

.. code-block:: bash  

  module->fnFinalizeUpdatePhysics =
        malloc(iNumBodies * sizeof(fnFinalizeUpdatePhysicsModule));

and in FinalizeModule, add

.. code-block:: bash

  module->fnFinalizeUpdatePhysics[iBody] =
        malloc(iNumModules * sizeof(fnFinalizeUpdatePhysicsModule));

and

.. code-block:: bash

  module->fnFinalizeUpdatePhysics[iBody][iModule]   = &FinalizeUpdateNULL; 

Please add them next to the other similar statements

Next we need to add code to the module file, physics.c in this example, for
which this primary variable will be added. First add a new function that
continues the initialization process:

.. code-block:: bash

  void InitializePrimaryVariablePhysics(BODY *body, OPTIONS *options,
                          UPDATE *update, double dAge, int iBody) {

    // "Type" of update; 1 is an ordinary differential equation
    update[iBody].iaType[update[iBody].iPrimaryVariable]
                  [update[iBody].iPrimaryVariablePhysics] = 1;
    // here we assume only one body is affecting the way the variable is updated
    update[iBody].iNumBodies[update[iBody].iPrimaryVariable]
                  [update[iBody].iPrimaryVariablePhysics] = 1;
    // allocate memory
    update[iBody].iaBody[update[iBody].iPrimaryVariable]
                  [update[iBody].iPrimaryVariablePhysics] = malloc(
                  update[iBody].iNumBodies[update[iBody].iPrimaryVariable]
                  [update[iBody].iPrimaryVariablePhysics] * sizeof(int));
    // Assign body(ies) that affect this derivative
    update[iBody].iaBody[update[iBody].iPrimaryVariable]
                  [update[iBody].iPrimaryVariablePhysics][0] = iBody;
    // Assign pointer the derivative
    update[iBody].pdDPrimaryVariableDtPhysics =
                  &update[iBody].daDeriv[update[iBody].iPrimaryVariable]
                  [update[iBody].iPrimaryVariablePhysics];
  }

Then add a new line to the AssignDerivative function to set the appropriate
function pointer to the fnUpdate matrix:

.. code-block:: bash

  void AssignPhysicsDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                              fnUpdateVariable ***fnUpdate, int iBody) {
    ...
    fnUpdate[iBody][update[iBody].iPrimaryVariable]
            [update[iBody].iPrimaryVariablePhysics] = &fdDPrimaryVariableDtPhysics;
    ...
  }

where "Physics" is the  module name. The function fdDPrimaryVariableDt is a
subroutine that return the derivative of the primary variable.

Next add the following block of code to the InitializeUpdatePhysics function in
the module's file (physics.c):

.. code-block:: bash

  void InitializeUpdatePhysics(BODY *body, UPDATE *update, int iBody) {
    ...
    if (update[iBody].iNumPrimaryVariable == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumPrimaryVariable++;
    ...
  }

Then create the FinalizeUpdate function:

.. code-block:: bash

  void FinalizeUpdatePrimaryVariablePhysics(BODY *body, UPDATE *update,
                                           int *iEqn, int iVar,int iBody,
                                           int iFoo) {
    update[iBody].iaModule[iVar][*iEqn] = PHYSICS;
    update[iBody].iPrimaryVariablePhysics = *iEqn;
    (*iEqn)++;
    }

where PHYSICS is the unique integer associated with the module you are
upgrading.

Then add the primary variable to the NullDerivatives function:

.. code-block:: bash

  void NullDerivativesPhysics(BODY *body, EVOLVE *evolve, UPDATE *update,
                             fnUpdateVariable ***fnUpdate, int iBody) {
    ...
    fnUpdate[iBody][update[iBody].iPrimaryVariable]
            [update[iBody].iPrimaryVariableMODULE] = &fndUpdateFunctionTiny;
    ...
  }

The final initialization step is to update the AddModule function:

.. code-block:: bash

  void AddModulePhysics(CONTROL *control, MODULE *module, int iBody,
                       int iModule) {
    ...
    module->fnFinalizeUpdatePrimaryVariable[iBody][iModule] =
        &FinalizeUpdatePrimaryVariablePhysics;
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
<option>`_ and `outputs <output>`_. Finally, add `examples and tests <tests>`_ to
show off your result and ensure that future upgrades don't destroy your work.
