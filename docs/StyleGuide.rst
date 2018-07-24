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


Fancy stuff
~~~~~~~~~~~

We are currently using `breathe <http://breathe.readthedocs.io/en/latest/>`_ to
link `DOXYGEN <http://www.doxygen.org>`_ documentation to `sphinx <http://sphinx-doc.org/>`_
html output. This means you should familiarize yourself with both the
`DOXYGEN markup <https://www.stack.nl/~dimitri/doxygen/manual/commands.html>`_
and the `reStructuredText markup <http://www.sphinx-doc.org/en/master/usage/restructuredtext/index.html>`_.

The things you can do with DOXYGEN + breathe fairly limited, so it's useful to
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

The DOXYGEN citation functionality is pretty lame, so I'm using rst to add citations
to papers relevant to this module. The cool thing about this is that it automatically
adds entries to the `html bibliography <zzreferences.html>`_. Note that in order for
this to work, the references must be in the :code:`docs/vplanet.bib` file in the repo.

Miscellaneous
-------------

    - Do not use the word `test` in any file/folder names *unless* you want it to
      be part of the unit tests, as the unit tester collects and attempts to run
      any file with `test` in it.
    - Use spaces, not tabs, when indenting!
    - Do not use the pound sign "#" in comments, as this refers to a link in **Doxygen**.
    - Use the PEP 8 guide for python scripts: https://www.python.org/dev/peps/pep-0008/