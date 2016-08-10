# -*- coding: utf-8 -*-
"""
@author: dflemin3 2016
"""

__version__ = "0.12"
__author__ = "David Fleming (dflemin3@uw.edu)"
__copyright__ = "Copyright 2016 David Fleming"

from . import bigplot
from . import data_extraction

from .data_extraction import extract_data_hdf5, aggregate_data, Dataset, reduce_dimensions
from .bigplot import plot_red_dim, plot_red_dim_contour, red_dim_grid
