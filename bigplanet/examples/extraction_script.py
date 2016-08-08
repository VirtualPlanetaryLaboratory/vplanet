"""
dflemin3 Aug 2016

This script extracts/processes data from a suite of VPLANET simulations.
"""

# Imports
from bigplanet import data_extraction as de
import damping_estimation as dest

### Load in the data ###

# Path to the root directory where all the simulation subdirectories live
src = "/Users/dflemin3/Desktop/GM_run/"

# Define name for the dataset (hdf5 approach)
dataset = src + "simulation.hdf5"

# Define extract_data params
fmt = "hdf5"
order = "grid"
remove_halts = False

data = de.extract_data_hdf5(src=src, dataset=dataset, order=order, remove_halts=remove_halts)

### Make a dataframe of initial conditions! ###

# Define the bodies and body variables to extract using a dictionary
bodies = {'cbp':["FreeEcc","R0"],'secondary':['SemimajorAxis','Eccentricity']}

# Define the new value (dataframe column) to produce for a given body.  The new column
# and how to calculate it are given as a dictionary for each body.

# New column for the cbp is "InitEcc" and is calculated using the function "trivial"
new_cols = {"cbp" : {"DampTime" : dest.find_damping_time}}

# Define any keyword arguments trivial might need
kw = {"key" : "Eccentricity"}

# Extract and save into a cache (or read from it if it already exists)
# Note ind=0 makes it clear that we want initial conditions stored for all non-new_cols variables
df = de.aggregate_data(data, bodies=bodies,new_cols=new_cols,cache=src+"damptime_cache.pkl",fmt=fmt,**kw)
