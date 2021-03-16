# -*- coding: utf-8 -*-
__author__ = "Rodrigo Luger (rodluger@gmail.edu)"
__copyright__ = "Copyright 2018, 2019 Rodrigo Luger"

# Import the version
from .vplot_version import __version__
import os

_path = os.path.dirname(os.path.abspath(__file__))


# Set up the logger
import logging

logger = logging.getLogger("vplot")
ch = logging.StreamHandler()
ch.setLevel(logging.ERROR)
formatter = logging.Formatter("%(levelname)s:%(message)s")
ch.setFormatter(formatter)
logger.addHandler(ch)

# copy mplstyle sheet to correct location
#import shutil as sh
#file = 'style/vplot.mplstyle'
#location = os.path.expanduser('~/.config/matplotlib/mpl_configdir/stylelib')
#sh.copy(file,location)

# Override matplotlib.figure.Figure
from . import figure


# Set up the matplotlib stylesheet
from . import style


# Import user-facing stuff
from .figure import VPLOTFigure
from .log import Log, LogBody, LogStage
from .output import GetOutput, Output, Body
from .auto_plot import auto_plot
from .plot import colors, make_pretty, plot

# User-facing stuff
__all__ = [
    "GetOutput",
    "auto_plot",
    "Log",
    "LogBody",
    "LogStage",
    "Output",
    "Body",
    "VPLOTFigure",
    "colors",
    "make_pretty"
    "plot"
]
