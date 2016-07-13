"""

dflemin3 June 2016

This script extracts data from a suite of VPLANET simulations.

For a given users' purposes, one must change "src" to where the root data
dir is located and "archiveName" to whatever the user wishes.  Also, user
must make sure gpl_data_extraction.py is in their python path.  I've added
a sys hack to make that work.

"""

# Imports
import os
import sys
import numpy as np


from bigplanet import data_extraction as de

### Load in the data ###

# Set some archive name
src = "/Users/dflemin3/Desktop/GM_run"
archiveName = src + "/" + "GM_suite.npz"

# Archive doesn't exit: load data, make it
if (not os.path.exists(archiveName)):
    data = de.extract_data(src)
    
    print("Creating archive:",archiveName)
    np.savez(archiveName,data=data)
