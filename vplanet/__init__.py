# -*- coding: utf-8 -*-

# Grab version from the C extension
from .vplanet_core import version

__version__ = version().decode("utf-8")

# Grab the help info


# Import the main interface
from .wrapper import run, help, VPLANETError

# Import the logger
from .logger import logger

# Import the rest of the user-facing stuff
from .log import Log, LogBody, LogStage
from .output import get_output, Output, Body
from .quantity import VPLANETQuantity as Quantity
