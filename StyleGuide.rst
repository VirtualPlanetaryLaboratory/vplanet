VPLANET Style Guide
===================

General remarks
---------------

    - Do not use the word `test` in any file/folder names *unless* you want it to
      be part of the unit tests, as the unit tester collects and attempts to run
      any file with `test` in it.
    - Use spaces, not tabs, when indenting!
    - 


Variable and function names use a "Hungarian style."

All files should begin with a doxygen-style docstring:

.. code-block:: C

    /**
        @file filename.c
        @brief Brief description of what this file is and what it does.

        A more detailed description of the file and its intricacies.
    */

Subroutine names can begin with a prefix that describes the returned parameter
`[none]` = void function
`fnb` = Boolean Function (returns `0` or `1`)
`fni` = Integer Function (returns `int`)
`fnd` = Double Function (returns `double`)
`fns` = String function (returns `char[]`)
`fnba` = Boolean Array Function
`fnia` = Integer Array Function
`fnda` = Double Array Function
`fnsa` = String Array Function (returns `char[][]`)

Subroutines should be written as:

.. code-block:: C

    /**
    This comment describes the point of this function, any potential gotchas or future
    improvements. Single line comments with // are OK. The argument list should be
    alphabetical, starting with vplanet structs, then any function pointers, then doubles,
    then integers, then Booleans.

    @param body This describes what the body parameter is all about
    @param control This describes what the control parameter is all about
    ...
    @param iB This describes what the iB parameter is all about
    @return If the function isn't void, describe what it returns.
    */
    void ThisIsAFunction(BODY *body, CONTROL *control, ..., fnUpdate, double dA, double dB, ..., int iA, int iB) {

      /*
      Use spaces instead of tabs to indent code. Tabs can cause conflicts. You should be able
      to modify your text editor to do this. Two spaces per logic level.
      */

      /*
      All variables must be declared at the top of the subroutine in the same order as the
      argument list.
      */

      // Include comments for each variable name, eg:
      BODY *tmpBody;                        // Copy of body struct for RK4 calculations
      fnUpdateVariable *fnFunctionPointer;  // Function pointer to Derivative dx/dt
      double dAA, *daA;                     // Astrometric Anomaly; vector of albedos
      int iAA, *iaA;                        // index of Asteroid Albedo; vector of Asteroid body numbers

      /*
      Prefixes for variables:

      b = Boolean
      i = Integer
      d = Double
      s = String
      ba = Boolean Array
      ia = Integer Array
      da = Double Array
      sa = String Array
      pb = Pointer to Boolean
      pi = Pointer to Integer
      pd = Pointer to Double
      pab = Pointer to Boolean Array
      pai = Pointer to Integer Array
      pad = Pointer to Double Array
      fnp = Function Pointer
      */

      // Unbracketed, single line if/then, for, and while loops are OK, but you should comment them!
      // [Either way it's work.]

      if (...) {
        // Code here
      } else if (...) {
        // Code here
      } else {
        // Code here
      }

      while (...) {
        // Code here
      }

      do {
        // Code here
      } while (...);

      for (...; ...; ...) {
        // Code here
      }

      switch (...) {
        case 1: {
          // When you need to declare a variable in a switch, put the block in braces
          int var;
          break;
        }
        case 2:
          // Code here
          break;
        default:
          break;
      }

      // The meat of the subroutine is next, with appropriate comments.
      // Make sure to free all malloc'd vectors/matrices!

    }


In `vplanet.h`, use the **Doxygen** commenting style:

Single line comment:

.. code-block:: C

    double dAge;           /**< Body's Age */

Multi-line comment:

.. code-block:: C

    /*! Points to the element in UPDATE's daDerivProc matrix that contains the
        semi-major axis' derivative due to EQTIDE. */
    double *pdDsemiDtEqtide;

Do not use the pound sign "#" in comments, as this refers to a link in **Doxygen**.


Auxiliary Python Code
---------------------

This applies to `vplot`, `vspace`, `bigplanet`, etc.

Use the PEP 8 guide for python: https://www.python.org/dev/peps/pep-0008/
