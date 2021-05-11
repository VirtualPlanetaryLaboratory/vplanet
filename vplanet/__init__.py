from . import vplanet_core as core
import sys
import subprocess


__version__ = core.version().decode("utf-8")
__all__ = ["run", "help", "VPLANETError"]


class VPLANETError(RuntimeError):
    pass


def _entry_point():
    """
    VPLANET command line script entry point.

    """
    return core.run(*sys.argv)


def run(infile="vpl.in", verbose=False, quiet=False, spawn=True):
    """
    User-friendly interface to `core.run`.

    NOTE: See this: 
    https://stackoverflow.com/questions/1439533/how-to-catch-exit-in-embedded-c-module-from-python-code

    We need to change the way vplanet exits on error in order to
    return to the Python interpreter when it terminates. The current
    hack is to spawn a subprocess (if `spawn=True`) so we don't
    terminate the current Python session.

    """
    args = ["vplanet", infile]
    if verbose:
        args += ["-v"]
    if quiet:
        args += ["-q"]

    if spawn:
        error = False
        try:
            subprocess.check_output(args)
        except subprocess.CalledProcessError as e:
            error = True
        if error:
            raise VPLANETError("Error running VPLANET.")
    else:
        core.run(*args)


class VPLANETHelp:
    """
    VPLANET help message wrapper.
    
    """

    def __init__(self):
        self._help = subprocess.check_output(["vplanet", "-h"]).decode("utf-8")

    def __repr__(self):
        return self._help

    def __str__(self):
        return self._help


def help():
    """
    Display the VPLANET help message.

    """
    return VPLANETHelp()
