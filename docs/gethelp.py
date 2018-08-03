import subprocess
BOLD = r"\x1b[1m"
REGULAR = r"\x1b[0m"

module_info = \
"""
Modules and Options
===================

.. contents:: :local:


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

proc = subprocess.Popen(['vplanet', '-h'], stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE)
output = str(proc.stdout.read())
output = output.replace(REGULAR + BOLD, "")
output = output.replace(BOLD, '**')
output = output.replace(REGULAR, '**')
output = output.replace("----- Command Line Options -----",
                        "Command Line Options\\n~~~~~~~~~~~~~~~~~~~~\\n====================================   ============================================================================================================")
output = output.replace("----- Input File -----",
                        "====================================   ============================================================================================================\\nInput File\\n~~~~~~~~~~")
output = output.replace("----- Input Options -----",
                        "Input Options\\n~~~~~~~~~~~~~\\n====================================   ============================================================================================================")
output = output.replace("----- Output Options -----",
                        "====================================   ============================================================================================================\\n\\nOutput Options\\n~~~~~~~~~~~~~~")
output = output.replace("These options follow the argument saOutputOrder.", "These options follow the argument :code:`saOutputOrder`.\\n====================================   ============================================================================================================\\n")
output = output.split("\\n")[:-1]
tag = False
with open('help.rst', 'w') as f:
    print(module_info, file=f)
    print("Options Lookup", file=f)
    print("--------------", file=f)
    print("", file=f)
    print("The contents of this page can be generated by " +
          "typing :code:`vplanet -h` in a terminal.", file=f)
    print("", file=f)
    print(".. contents:: :local:", file=f)
    print("", file=f)
    for line in output:
        if "Command Line Options" in line:
            tag = True
        if tag:
            if len(line) > 1:

                if "Input File" in line:
                    print("", file=f)
                    print(line, file=f)
                elif "Input Options" in line:
                    print("", file=f)
                    print(line, file=f)
                elif "Output Options" in line:
                    print("", file=f)
                    print(line, file=f)
                elif "These options follow" in line:
                    print(line, file=f)
                    print("", file=f)
                elif "~~~" in line:
                    print(line, file=f)
                    print("", file=f)
                else:
                    print(line.replace("--", "                                 "), file=f)
            #if line.startswith('**'):
            #    print("", file=f)
    print("====================================   ============================================================================================================", file=f)
