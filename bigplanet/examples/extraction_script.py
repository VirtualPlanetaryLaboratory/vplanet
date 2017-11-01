"""
dflemin3 July 2017

This script extracts/processes data from a suite of VPLANET simulations.

Use this as a template for all your future data processing needs.

"""

from __future__ import (absolute_import, division, print_function,
                        unicode_literals)
import os
from bigplanet import data_extraction as de

### Load in the data ###

# Define root dirctory where all sim sub directories are located
root_dir = "/Users/dflemin3/Desktop/GM_run" # Master sim directory
src = os.path.join(root_dir,"Data") # Where allll the sub dirs are

# How you wish the data to be ordered (grid for grid simulation suites)
order = "none"

# Format of the data (default)
fmt = "hdf5"  # For backwards compatitiblity

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
compression = None #"gzip"

# Use all processors? Best if used on a cluster
parallel = True

# Path to HDF5 dataset to make.  In general, doesn't have to be where data
# lives. cache_dir is where we'll save all the products
if not parallel:
    cache_dir = os.path.join(root_dir,"Serial")
    dataset = os.path.join(cache_dir,"simulation")
else:
    cache_dir = os.path.join(root_dir,"Parallel")
    dataset = os.path.join(cache_dir,"simulation")

# Extract the data!
data = de.extract_data_hdf5(src=src, dataset=dataset, order=order,
                            remove_halts=remove_halts, compression=compression,
                            var_from_log=var_from_log, cadence=cadence, parallel=parallel)

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
                       cache=os.path.join(cache_dir,"trivial_cache.pkl"),fmt=fmt,
                       **kw)
