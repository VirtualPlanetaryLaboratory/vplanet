from . import vplanet_core as core
import sys


__version__ = core.version().decode("utf-8")


def _entry_point():
    """
    VPLANET command line script entry point.

    """
    return core.run(*sys.argv)


def run(infile="vpl.in", verbose=False, quiet=False):
    """
    User-friendly interface to `core.run`.

    NOTE: See this: 
    https://stackoverflow.com/questions/1439533/how-to-catch-exit-in-embedded-c-module-from-python-code

    We need to change the way vplanet exits on error in order to
    return to the Python interpreter when it terminates.

    """
    args = []
    if verbose:
        args += ["-v"]
    if quiet:
        args += ["-q"]
    core.run("vplanet", infile, *args)
