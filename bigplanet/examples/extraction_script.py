"""
dflemin3 Sept 2016

This script extracts/processes data from a suite of VPLANET simulations.
"""

from __future__ import (absolute_import, division, print_function,
                        unicode_literals)
import os
from bigplanet import data_extraction as de

### Load in the data ###

# Define root dirctory where all sim sub directories are located
src = "/Users/dflemin3/Desktop/GM_run/"

# Path to the hdf5 dataset
dataset= os.path.join(src,"simulation.hdf5")

# How you wish the data to be ordered (grid for grid simulation suites)
order = "none"

# Format of the data (default)
fmt = "hdf5"

# Ignore simulations that halted at some point?
remove_halts = False

# Any bodies whose output you wish to ignore?
skip_body = ["primary.in"]

# Any parameters not in a body's .forward files that you want?
var_from_log = {"secondary" : ["Mass"], "cbp" : ["Mass"]}

# An optional kwarg that has extract_data_hdf5 output which simulation it's on
# every cadence steps for int cadence
cadence = 100

# Compression algorithm to use
compression = "gzip"

data = de.extract_data_hdf5(src=src, dataset=dataset, order=order,
                            remove_halts=remove_halts, compression=compression,
                            var_from_log=var_from_log)

### Make a dataframe of initial conditions! ###

# Define the bodies and body variables to extract using a dictionary
bodies = {'cbp':["Eccentricity","SemimajorAxis","Mass"],
          'secondary':['SemimajorAxis','Eccentricity', "Mass"]}

# Define the new value (dataframe column) to produce for a given body.  The new column
# and how to calculate it are given as a dictionary for each body.

new_cols = {}

# Define any keyword arguments trivial might need
kw = {"key" : "Eccentricity"}

# Extract and save into a cache (or read from it if it already exists)
# Note ind=0 makes it clear that we want initial conditions stored for all non-new_cols variables
df = de.aggregate_data(data, bodies=bodies,new_cols=new_cols,
                       cache=os.path.join(src,"trivial_cache.pkl"),fmt=fmt,
                       **kw)
