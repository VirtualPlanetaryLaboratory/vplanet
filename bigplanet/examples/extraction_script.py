"""

dflemin3 July 2016

This script extracts/processes data from a suite of VPLANET simulations.

For a given users' purposes, one must change "src" to where the root data
dir is located and "archiveName" to whatever the user wishes.

"""

# Imports
from bigplanet import data_extraction as de

### Load in the data ###

# Path to the root directory where all the simulation subdirectories live
src = "/Users/dflemin3/Desktop/GM_run/"

# Define name for the dataset (hdf5 approach)
dataset = src + "simulation.hdf5"

# Define a data format (overkill here, but useful in general)
fmt = "hdf5"
order = "grid"

data = de.extract_data_hdf5(src=src, dataset=dataset, order=order)
