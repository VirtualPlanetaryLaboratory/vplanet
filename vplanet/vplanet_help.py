# -*- coding: utf-8 -*-
import subprocess

_help = subprocess.check_output(["vplanet", "-h"]).decode("utf-8")
_help_verbose = subprocess.check_output(["vplanet", "-H"]).decode("utf-8")


class VPLANETHelp:
    """
    VPLANET help message wrapper.

    """

    def __init__(self, verbose=False):
        if verbose:
            self._help = _help_verbose
        else:
            self._help = _help

    def __repr__(self):
        return self._help

    def __str__(self):
        return self._help
