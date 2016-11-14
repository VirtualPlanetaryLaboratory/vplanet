"""
dflemin3 Sept 2016

This script extracts/processes data from a suite of VPLANET simulations.
"""

from __future__ import print_function, division, unicode_literals
import os

# Imports
from bigplanet import data_extraction as de
import damping_estimation as dest # Custom package with custom function

### Load in the data ###

# Define root dirctory where all sim sub directories are located
src = "/Users/dflemin3/Desktop/GM_run/"

# Path to the hdf5 dataset
dataset= os.path.join(src,"simulation.hdf5")

# How you wish the data to be ordered (grid for grid simulation suites)
order = "grid"

# Format of the data (default)
fmt = "hdf5"

# Ignore simulations that halted at some point?
remove_halts = False

# Any bodies whose output you wish to ignore?
skip_body = ["primary.in"]

# An optional kwarg that has extract_data_hdf5 output which simulation it's on
# every cadence steps for int cadence
cadence = None

# Compression algorithm to use
compression = "gzip"

data = de.extract_data_hdf5(src=src, dataset=dataset, order=order,
                            remove_halts=remove_halts, compression=compression)

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
df = de.aggregate_data(data, bodies=bodies,new_cols=new_cols,
                       cache=os.path.join(src,"damptime_cache.pkl"),fmt=fmt,
                       **kw)
