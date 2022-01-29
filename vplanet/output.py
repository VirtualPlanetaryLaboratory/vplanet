# -*- coding: utf-8 -*-
import os
import re
import warnings

import astropy.units as u
import numpy as np

from .custom_units import custom_units
from .log import get_log
from .logger import logger
from .quantity import NumpyQuantity
from .quantity import VPLANETQuantity as Quantity


class Output(object):
    """A class containing all of the information of a ``vplanet`` run."""

    def __init__(self):
        self.sysname = ""
        self.bodies = []

    def __getitem__(self, i):
        return self.bodies[i]

    def __len__(self):
        return len(self.bodies)

    def __repr__(self):
        return "<vplanet.Output: %s>" % self.sysname

    @property
    def members(self):
        """A list of all the properties of this object."""
        keys = list(self.__dict__.keys())
        return [key for key in keys if not key.startswith("_")]


class Body(object):
    """A class containing the parameter arrays of a body in a ``vplanet`` run.

    These are populated from either a ``.forward``, a ``.backward``, or a
    ``.Climate`` file.

    """

    def __init__(self):
        self._name = ""
        self._params = []

    def __getitem__(self, i):
        return self._params[i]

    def __len__(self):
        return len(self._params)

    def __repr__(self):
        return "<vplanet.Body: %s>" % self._name

    @property
    def members(self):
        """A list of all the properties of this object."""
        keys = list(self.__dict__.keys())
        return [key for key in keys if not key.startswith("_")]


def get_param_descriptions():
    """ """

    # Get the help message
    from .wrapper import help

    help_msg = str(help())

    # Get only the output params
    stroutput = help_msg.split("These options follow the argument saOutputOrder.")[1]
    stroutput = [x for x in stroutput.split("\n") if len(x)]
    description = {}
    for out in stroutput:
        if out.startswith("[-]"):
            try:
                n, d, _ = re.search(r"\[-\](.*) -- (.*). \[(.*)\]", out).groups()
            except AttributeError:
                raise ValueError(
                    "Error parsing the following line in the log file: {}".format(out)
                )
            description.update({n: d})
        else:
            try:
                n, d = re.search("(.*) -- (.*).", out).groups()
            except AttributeError:
                raise ValueError(
                    "Error parsing the following line in the log file: {}".format(out)
                )
            description.update({n: d})

    # Format the entries a bit
    for key, value in description.items():

        # Remove periods
        if value.endswith("."):
            description[key] = value[:-1]

        # Change "Time Rate of Change" to "Rate of Change"
        if value.lower().startswith("time rate of change"):
            description[key] = value[5:]

        # Remove "Body's"
        if value.lower().startswith("body's"):
            description[key] = value[7:]

    return description


def get_params(outputorder, file, units=True, body=None):
    """ """
    # Get parameter descriptions from the vplanet help
    description = get_param_descriptions()

    # Get params and units
    params_and_units = re.findall(r"(.*?)\[(.*?)\]", outputorder)

    # Populate the params
    params = []
    for j, param in enumerate(params_and_units):

        # Grab the array in the fwfile/bwfile
        array = []
        for line in file:
            array.append(float(line.split()[j]))

        # Get the name and units
        name = param[0].replace(" ", "")
        unit_str = param[1].replace(" ", "")

        # If the param name starts with a number,
        # add an underscore so we can make it a
        # valid class property name
        if any(name.startswith(str(n)) for n in range(10)):
            name = "_" + name

        # Process the unit
        if units:
            with warnings.catch_warnings(record=True) as w:
                try:
                    unit = u.Unit(unit_str)
                    assert len(w) == 0
                except ValueError as e:
                    logger.error(
                        "Error processing unit {}: ".format(unit_str) + (str(e))
                    )
                    unit = u.Unit("")
                except AssertionError:
                    logger.warn(
                        "Error processing unit {}: ".format(unit_str)
                        + str(w[0].message)
                    )
                    unit = u.Unit("")

            # Make it into an astropy quantity with units
            array = Quantity(np.array(array), unit=unit)
            physical_type = unit.physical_type

        else:

            # Keep it as a numpy array with tags. We'll
            # still keep track of the unit, but it's only
            # a passive tag!
            array = NumpyQuantity(np.array(array))
            array.unit = unit_str
            physical_type = None

        # Tag it for plotting
        array.tags = dict(
            name=name,
            description=description.get(name, name),
            body=body,
            physical_type=physical_type,
        )

        # Add to the list
        params.append(array)

    return params


def get_arrays(log, units=True):
    """ """
    # Initialize
    output = Output()

    # Get basic system info
    output.log = log
    output.sysname = log.sysname
    output.path = log.path

    # Grab body properties
    for i, body_name in enumerate(log._body_names):

        # Create a body
        body = Body()
        body._name = body_name
        body.name = body_name

        # Grab the input file name
        body.infile = getattr(log.header, "BodyFile%d" % (i + 1))

        # Grab the output file names
        body.fwfile = "%s.%s.forward" % (output.sysname, body._name)
        if not os.path.exists(os.path.join(output.path, body.fwfile)):
            body.fwfile = ""
        body.bwfile = "%s.%s.backward" % (output.sysname, body._name)
        if not os.path.exists(os.path.join(output.path, body.bwfile)):
            body.bwfile = ""
        body.climfile = "%s.%s.Climate" % (output.sysname, body._name)
        if not os.path.exists(os.path.join(output.path, body.climfile)):
            body.climfile = ""

        # Grab the forward arrays. Note that they may not exist for this body
        try:
            with open(os.path.join(output.path, body.fwfile), "r") as f:
                fwfile = f.readlines()
        except IOError:
            fwfile = [""]

        # Grab the backward arrays. Note that they may not exist for this body
        try:
            with open(os.path.join(output.path, body.bwfile), "r") as f:
                bwfile = f.readlines()
        except IOError:
            bwfile = [""]

        # TODO: Add support for *both* fwfile and bwfile at the same time?
        if fwfile != [""] and bwfile != [""]:
            logger.error(
                "Both a fwfile and a bwfile were detected. "
                + "Currently, vplanet can only handle one at a time. "
                + "Continuing, but ignoring the bwfile..."
            )

        # Now grab the output order and the params
        outputorder = getattr(log.initial, body._name).OutputOrder
        if fwfile != [""]:
            body._params = get_params(outputorder, fwfile, units=units, body=body._name)
        elif bwfile != [""]:
            body._params = get_params(outputorder, bwfile, units=units, body=body._name)

        # Climate file
        if body.climfile != "":
            # Grab the climate arrays...
            try:
                with open(os.path.join(output.path, body.climfile), "r") as f:
                    climfile = f.readlines()
            except IOError:
                raise Exception("Unable to open %s." % body.climfile)

            # ... and the grid order
            try:
                gridorder = getattr(log.initial, body._name).GridOutputOrder
                body._gridparams = get_params(
                    gridorder, climfile, units=units, body=body._name
                )
            except:
                logger.error(
                    "Unable to obtain grid output parameters from %s." % body.climfile
                )
                body._gridparams = []
        else:
            body._gridparams = []

        # Add the body
        output.bodies.append(body)

    return output


def get_output(path=".", sysname=None, units=True):
    """Parse all of the output from a :py:obj:`vplanet` run.

    Args:
        sysname (str, optional): System name. This is determined automatically,
            unless there are multiple runs in the same :py:obj:`path`. Defaults
            to None.
        path (str, optional): Path to the directory containing the results of
            the :py:obj:`vplanet` run. Defaults to the current directory.
        units (bool, optional): Whether or not the quantities returned by this
            method have astropy units. Default is True.

    Returns:
        A :py:class:`Output` instance containing all the information from the
        ``.log``, ``.forward``, and ``.backward`` output files.
    """
    # Get the log file and the arrays
    log = get_log(sysname=sysname, path=path, units=units)
    output = get_arrays(log, units=units)

    for body in output.bodies:

        # Make the body accessible as an attribute
        setattr(output, body._name, body)

        # Make all the arrays accessible as attributes
        for array in getattr(output, body._name)._params:
            setattr(getattr(output, body._name), array.tags["name"], array)

        # Grid params
        if len(getattr(output, body._name)._gridparams):

            # Get the time array
            iTime = np.argmax(
                [
                    array.tags["name"] == "Time"
                    for array in getattr(output, body._name)._gridparams
                ]
            )
            Time = getattr(output, body._name)._gridparams[iTime]

            # Get 2d array dimensions
            J = np.where(Time[1:] > Time[:-1])[0][0] + 1
            _, r = divmod(len(Time), J)
            if r != 0:
                raise Exception(
                    "Irregular time grid detected for {}. VPLANET is confused!".format(
                        body._name
                    )
                )

            for array in getattr(output, body._name)._gridparams:
                if array.tags["name"] != "Time":
                    # We don't want to overwrite the time array!
                    setattr(getattr(output, body._name), array.tags["name"], array)

    return output
