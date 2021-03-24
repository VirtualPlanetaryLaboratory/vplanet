import subprocess
BOLD = r"\x1b[1m"
REGULAR = r"\x1b[0m"

module_info = \
"""
Modules and Options
===================

.. contents::
   :local:
   :depth: 2

.. raw:: html

    <div class="admonition note">
    <p class="first admonition-title">Options Search</p>
    <p></p>
    <div class="last"><div role="search">
    <form id="options-search-form" class="wy-form" action=""
        onsubmit="location.hash = '#' + getElementById('option_textbox').value.toLowerCase();">
        <div style="margin-bottom:1em;">
        Option name: &nbsp;<input type="text" id="option_textbox" placeholder="e.g., bAccuracyMode"
        style = "height: 1.75em; width: 20em;">
        </div>
    </form>
    </div></div></div>

Module Lookup
-------------

+-----------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Module name                             | Description                                                                                                                                                      |
+=========================================+==================================================================================================================================================================+
| :doc:`atmesc </src/atmesc>`             | Models atmospheric escape of terrestrial or gaseous planets.                                                                                                     |
+-----------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| :doc:`binary </src/binary>`             | Models the orbital evolution of binary stars and circumbinary planets.                                                                                           |
+-----------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| :doc:`distorb </src/distorb>`           | Models orbital evolution of planetary systems using the classical disturbing function.                                                                           |
+-----------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| :doc:`distrot </src/distrot>`           | Models the evolution of planetary obliquity.                                                                                                                     |
+-----------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| :doc:`eqtide </src/eqtide>`             | Models the tidal evolution of multi-body systems.                                                                                                                |
+-----------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| :doc:`flare </src/flare>`               | Models the emission of stellar flares.                                                                                                                           |
+-----------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| :doc:`galhabit </src/galhabit>`         | Models the galactic habitat of a system.                                                                                                                         |
+-----------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| :doc:`poise </src/poise>`               | Models planetary climate using an energy balance model.                                                                                                          |
+-----------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| :doc:`radheat </src/radheat>`           | Models radiogenic heating of planetary interiors.                                                                                                                |
+-----------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| :doc:`spinbody </src/spinbody>`         | Models orbital evolution using an N-body integrator.                                                                                                             |
+-----------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| :doc:`stellar </src/stellar>`           | Models the evolution of stars before and during the main sequence.                                                                                               |
+-----------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| :doc:`thermint </src/thermint>`         | Models the thermal evolution of terrestrial planet interiors.                                                                                                    |
+-----------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+

"""

proc = subprocess.Popen(['vplanet', '-H'], stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE)
output = str(proc.stdout.read())
output = output.split('\\n')[7:-1]

with open('help.rst', 'w') as f:
    print(module_info, file=f)
    print("Options Lookup", file=f)
    print("--------------", file=f)
    print("", file=f)
    print("The contents of this page can be generated by " +
          "typing :code:`vplanet -H` in a terminal.", file=f)
    print("", file=f)
    print("", file=f)
    for line in output:
        print(line, file=f)
