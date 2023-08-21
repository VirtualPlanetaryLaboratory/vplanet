"""
Reproduces the T1-e curve from Figure 2 of Gialluca et al (2023, in prep)
and the T1-b curve from that Figure except with 10 TO initial instead of 200 TO

Megan Gialluca, UW, 2023

"""

import sys
import os
import pathlib

import matplotlib.pyplot as plt
import numpy as np

import vplanet

# Get Path
#path = os.getcwd()
path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))
from get_args import get_args

model = vplanet.run(path / 'vpl.in', quiet=True)

