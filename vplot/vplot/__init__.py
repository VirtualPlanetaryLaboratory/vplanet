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


# Override matplotlib.figure.Figure
from . import figure


# Set up the matplotlib stylesheet
from . import style


# Import user-facing stuff
from .figure import VPLOTFigure
from .log import Log, LogBody, LogStage
from .output import get_output, Output, Body
from .auto_plot import auto_plot


# User-facing stuff
__all__ = [
    "get_output",
    "auto_plot",
    "Log",
    "LogBody",
    "LogStage",
    "Output",
    "Body",
    "VPLOTFigure",
]
