VPLANET Style Guide
===================

.. contents:: :local:

Naming conventions
------------------

Variable and function names use a "Hungarian style".
Variables names should have the following prefixes:

.. code-block:: C

      b    = Boolean
      i    = Integer
      d    = Double
      s    = String
      ba   = Boolean Array
      ia   = Integer Array
      da   = Double Array
      sa   = String Array
      pb   = Pointer to Boolean
      pi   = Pointer to Integer
      pd   = Pointer to Double
      pab  = Pointer to Boolean Array
      pai  = Pointer to Integer Array
      pad  = Pointer to Double Array
      fnp  = Function Pointer


Function names should have the following prefixes:

.. code-block:: C

    none   = Void function
    fnb    = Boolean Function (returns 0 or 1)
    fni    = Integer Function (returns int)
    fnd    = Double Function (returns double)
    fns    = String function (returns char[])
    fnba   = Boolean Array Function
    fnia   = Integer Array Function
    fnda   = Double Array Function
    fnsa   = String Array Function (returns char[][])

Comments
--------

File headers
~~~~~~~~~~~~

All files should begin with a doxygen-style docstring:

.. code-block:: C

    /**
    @file filename.c
    @brief Brief description of what this file is and what it does.
    @author Author name ([author@email.com](mailto:author@email.com>))
    @date Month DD YYYY

    A more detailed description of the file and its intricacies.
    */

Functions
~~~~~~~~~

All functions should have docstrings describing what they are with
one-liners describing each of the inputs / outputs.

.. code-block:: C

    /**
    This comment describes the point of this function, any potential gotchas or future
    improvements. The argument list should be alphabetical, starting with vplanet structs,
    then any function pointers, then doubles, then integers, then Booleans.

    @param body This describes what the body parameter is all about
    @param control This describes what the control parameter is all about
    ...
    @param iB This describes what the iB parameter is all about
    @return If the function isn't void, describe what it returns.
    */
    void ThisIsAFunction(BODY *body, CONTROL *control, ..., int iB) {
        ...
    }

Macros and struct members
~~~~~~~~~~~~~~~~~~~~~~~~~

Struct members and constants declared using the :code:`#define` directive should
all have inline comments for doxygen following this syntax:

.. code-block:: C

    /**
    \brief Describe what the struct is here.
    */
    struct FOO {
        int iBar;           /**< Inline description of `iBar` */
        ...
    }

and

.. code-block:: C

    #define FOOBAR          /**< Inline description of `FOOBAR` */


Miscellaneous
-------------

    - Do not use the word `test` in any file/folder names *unless* you want it to
      be part of the unit tests, as the unit tester collects and attempts to run
      any file with `test` in it.
    - Use spaces, not tabs, when indenting!
    - Do not use the pound sign "#" in comments, as this refers to a link in **Doxygen**.
    - Use the PEP 8 guide for python scripts: https://www.python.org/dev/peps/pep-0008/
