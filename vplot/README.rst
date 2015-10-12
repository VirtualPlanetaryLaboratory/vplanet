VPLOT
-----
A suite of plotting routines for VPLANET.

Installation
============
Clone the repository and run the ``setup`` script. 
It's seriously that easy. You can also create a ``conf.py`` file in the 
``VPLANET`` output directory to specify custom settings. 
Type ``vplot -h`` for the complete list of options.

Examples
========
Run VPLANET on the built-in example files, and ``cd`` into the ``example``
directory.

Run

.. code-block:: bash  
  
   vplot -y Obl Ecc

to produce the following plot:

.. image:: img/example.png?raw=True
   :alt: example
   :align: center

If you have the ``stellar`` module installed, run ``VPLANET`` on the built-in example
and type

.. code-block:: bash  
  
   vplot

to produce the evolutionary sequence for M dwarfs:

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

    version: 0.1

    conf.py options:
      figheight, figname, figwidth, interactive, legend_all, legend_fontsize, legend_loc, 
      line_styles, linewidth, maxplots, maxylabelsize, short_labels, skip_xzero_log, 
      tight_layout, title, xlabel_fontsize, xlog, xticklabel_fontsize, ylabel_fontsize, 
      ylog, ymargin, yticklabel_fontsize


    Type `vplot -h OPTION_NAME` for info on any option