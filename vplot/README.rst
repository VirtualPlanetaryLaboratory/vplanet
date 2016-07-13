VPLOT
-----
A suite of plotting routines for ``VPLANET``.

Installation
============
PD: type "python setup.py install"

Clone the repository and run the ``setup`` script. 
It's seriously that easy. You can also edit the ``vplot_config.py`` to specify custom 
settings. This file is automatically created in the *cwd* when you run ``vplot``.
Type ``vplot -h`` for the complete list of options.

Examples
========
Run ``vplanet`` on the built-in example files, and ``cd`` into the ``examples/gl581``
directory.

Run

.. code-block:: bash  
  
   vplot -y Obl Ecc

to produce the following plot:

.. image:: img/example.png?raw=True
   :alt: example
   :align: center

To produce the evolutionary sequence for M dwarfs, ``cd`` into ``examples/main_seq``
directory and run

.. code-block:: bash  
  
   vplot

to produce the evolutionary sequence for M dwarfs. If you want a main-sequence-like
plot, try running

.. code-block:: bash  
  
   vplot -x Temperature -y Luminosity

You'll see something like this:

.. image:: img/mdwarfs.png?raw=True
   :alt: mdwarfs
   :align: center

Quick-and-dirty docs
====================
.. code-block:: bash

    VPLOT
    -----
    usage: vplot  [-h [OPTION_NAME]] [-b [BODIES [BODIES ...]]] 
                  [-x XAXIS] [-y [YAXIS [YAXIS ...]]] [-a ALPHA]

    optional arguments:
      -h [OPTION_NAME]          Show this help message or the docstring for OPTION_NAME
      -b BODIES [BODIES ...]    Bodies to plot; should match names of .in files in cwd
      -x XAXIS                  Parameter to plot on the x-axis
      -y YAXIS [YAXIS ...]      Parameter(s) to plot on the y-axis
      -a ALPHA                  Parameter to control line alpha

    version: 0.1.1

    vplot_config.py options:
      figheight, figname, figwidth, interactive, legend_all, legend_fontsize, legend_loc, 
      line_styles, linewidth, maxplots, maxylabelsize, short_labels, skip_xzero_log, 
      tight_layout, title, xlabel_fontsize, xlog, xticklabel_fontsize, ylabel_fontsize, 
      ylog, ymargin, yticklabel_fontsize


    Type `vplot -h OPTION_NAME` for info on any option
