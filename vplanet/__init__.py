# -*- coding: utf-8 -*-

# Grab version from the C extension
from .vplanet_core import version

__version__ = version().decode("utf-8")

# Grab the help info


# Import the rest of the user-facing stuff
from .log import Log, LogBody, LogStage

# Import the logger
from .logger import logger
from .output import Body, Output, get_output
from .quantity import VPLANETQuantity as Quantity

# Import the main interface
from .wrapper import VPLANETError, help, run
