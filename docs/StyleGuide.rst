VPLanet Contributors Guide
==========================

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

    fv   = Void function
    fb    = Boolean Function (returns 0 or 1)
    fi    = Integer Function (returns int)
    fd    = Double Function (returns double)
    fs    = String function (returns char[])
    fba   = Boolean Array Function
    fia   = Integer Array Function
    fda   = Double Array Function
    fsa   = String Array Function (returns char[][])

EXCEPT the following when they have either a variable or module name after them:
BodyCopy, PropsAux, ForceBehavior, CountHalts, Read, Write, AddModule, Verify,
InitializeOptions, InitializeOutput, InitializeBody, InitializeUpdate,
InitializeControl, InitializeHalts, InitializeModule, InitializeConstants.

C Code formatting
-----------------

1. Use no tab characters and indent blocks by 2 spaces.
2. All conditional blocks should have a { at the end of the line.
3. The closing } is on the line following the conditional block.
4. Single line conditionals must be surrounded by {}.
5. All functions shall return a variable, never an algebraic expression.
6. All lines shall be less than 80 characters.
7. For expressions that span multiple lines, the second and following lines
   shall have additional indents.
8. Lines that continue to the next must end with a "\" in C files.
9. Variables cannot be assigned a value at declaration.
10. No unused variables in a function.

.. note::

  The top-level directory contains the file .clang-format that contains
  instructions for a program to enforce some C code formatting with the `clangformat
  <https://clang.llvm.org/docs/ClangFormat.html>`_ package. You can code up your
  changes and then run this command afterwards to fix many (but not all) formatting
  problems.

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


Fancy stuff
~~~~~~~~~~~

We are currently using `breathe <http://breathe.readthedocs.io/en/latest/>`_ to
link `DOXYGEN <http://www.doxygen.org>`_ documentation to `sphinx <http://sphinx-doc.org/>`_
html output. This means you should familiarize yourself with both the
`DOXYGEN markup <https://www.stack.nl/~dimitri/doxygen/manual/commands.html>`_
and the `reStructuredText markup <http://www.sphinx-doc.org/en/master/usage/restructuredtext/index.html>`_.

The things you can do with DOXYGEN + breathe are fairly limited, so it's useful to
know that you can always take advantage of reStructuredText commands by escaping
a paragraph with the :code:`\rst` command. For instance, check out the header in the atmesc
source:

.. code-block:: C

    /**
    @file atmesc.c
    @brief Subroutines that control the integration of the
    atmospheric escape model.
    @author Rodrigo Luger ([rodluger@gmail.com](mailto:rodluger@gmail.com>))
    @date May 12 2015

    @par Description
    \rst
        This module defines differential equations controlling the evolution
        of planetary atmospheres under intense extreme ultraviolet (XUV)
        stellar irradiation. The `atmesc <atmesc.html>`_ module implements energy-limited
        and diffusion-limited escape for hydrogen/helium atmospheres and water
        vapor atmospheres following
        :cite:`Luger2015`, :cite:`LugerBarnes2015`, and :cite:`LehmerCatling17`.
    \endrst
    */

The DOXYGEN citation functionality is pretty limited, but should improve. For now
you can just copy/past the URL into the comments.


Code Coverage
-------------

We use `gcov <https://gcc.gnu.org/onlinedocs/gcc/Gcov.html>`_ and
`lcov <http://ltp.sourceforge.net/coverage/lcov.php>`_ to check code
coverage by the test scripts. In general, there will always exist
lines of code that you don't *want* to have executed -- such as lines
whose only purpose is to catch an error, print a message to screen, and
terminate the program. But **gcov** doesn't know that, so if your tests
don't hit those lines, your coverage goes down. The way around this is to
use the :code:`LCOV_EXCL_LINE`, :code:`LCOV_EXCL_START`, and
:code:`LCOV_EXCL_END` commands to explicitly markup lines you don't want
included in the coverage report. For instance, consider this snippet
of code inside :code:`ReadXFrac`, a function that reads in the user's value
for the XUV radius fraction in :code:`atmesc`:

.. code-block:: C
      :linenos:

      AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,
                      control->Io.iVerbose);
      if (lTmp >= 0) {
        NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,
                        control->Io.iVerbose);
        if (dTmp < 0) {
          if (control->Io.iVerbose >= VERBERR) {
    	       fprintf(stderr,"ERROR: %s must be >= 0.\n",options->cName);
           }
           LineExit(files->Infile[iFile].cIn,lTmp);
        }
        body[iFile-1].dXFrac = dTmp;
        UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
      }

Lines 5, 6, and 7 are only ever executed if the user provides a negative
XUV fraction. You *could* in principle add a test that checks whether the
error handling is working, but that seems silly. It's easiest to just mark
these lines so that **gcov** and **lcov** know to skip them. You can either
mark each line:

.. code-block:: C
      :linenos:

      AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,
                      control->Io.iVerbose);
      if (lTmp >= 0) {
        NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,
                        control->Io.iVerbose);
        if (dTmp < 0) {
          if (control->Io.iVerbose >= VERBERR) { // LCOV_EXCL_LINE
    	       fprintf(stderr,"ERROR: %s must be >= 0.\n",options->cName); // LCOV_EXCL_LINE
           }
           LineExit(files->Infile[iFile].cIn,lTmp); // LCOV_EXCL_LINE
        }
        body[iFile-1].dXFrac = dTmp;
        UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
      }

or use a block:

.. code-block:: C
      :linenos:

      AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,
                      control->Io.iVerbose);
      if (lTmp >= 0) {
        NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,
                        control->Io.iVerbose);
        if (dTmp < 0) {
          // LCOV_EXCL_START
          if (control->Io.iVerbose >= VERBERR) {
    	       fprintf(stderr,"ERROR: %s must be >= 0.\n",options->cName);
           }
           LineExit(files->Infile[iFile].cIn,lTmp);
           // LCOV_EXCL_STOP
        }
        body[iFile-1].dXFrac = dTmp;
        UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
      }

Miscellaneous
-------------

    - Do not use the word `test` in any file/folder names *unless* you want it to
      be part of the unit tests, as the unit tester collects and attempts to run
      any file with `test` in it.
    - Use spaces, not tabs, when indenting!
    - Do not use the pound sign "#" in comments, as this refers to a link in **Doxygen**.
    - Use the PEP 8 guide for python scripts: https://www.python.org/dev/peps/pep-0008/
