# -*- coding: utf-8 -*-
import logging
import os
import re
import warnings
from glob import glob

import astropy.units as u
import numpy as np

from .custom_units import custom_units
from .logger import logger
from .quantity import VPLANETQuantity as Quantity


def get_param_unit(param, file, line):
    """
    Grab the parameter unit from a line in the log file.

    Returns an `astropy.units` unit.
    """
    groups = re.findall(r"\[(.*?)\]", param)
    if len(groups):
        unit_str = groups[-1]

        with warnings.catch_warnings(record=True) as w:
            unit = u.Unit("")
            try:
                unit = u.Unit(unit_str)
                assert len(w) == 0
            except ValueError:
                logger.error(
                    "Error processing line {} of {}: ".format(line, file)
                    + "Cannot interpret unit `{}`.".format(unit_str)
                )
            except AssertionError:
                logger.warn(
                    "Error processing line {} of {}: ".format(line, file)
                    + str(w[0].message)
                )

        return unit
    else:
        # Dimensionless
        return u.Unit("")


def get_param_name(param, file, line):
    """
    Grab the parameter name from a line in the log file.

    Returns a string.
    """
    # Replace any bad characters
    repl = [("#", "")]
    for a, b in repl:
        param = param.replace(a, b)

    # Search for a match
    match = re.search(r"\((.*?)\)", param)
    if match:
        param = match.groups()[0]
        # If the param name starts with a number,
        # add an underscore so we can make it a
        # valid class property name
        if any(param.startswith(str(n)) for n in range(10)):
            param = "_" + param
        return param
    else:
        # There is no (ParameterName) here.
        # This is therefore not a variable, but a setting.
        # If there are spaces, get rid of them and force title case
        if " " in param:
            param = param.title().replace(" ", "")
        return param


def get_param_value(val, unit, file, line, units=True):
    """
    Grab the parameter value from a line in the log file.

    Returns an int, float (with units), bool, None (if no value
    was provided) or a string (if processing failed).
    """
    # Remove spaces
    val = val.lstrip().rstrip()

    # Is there a value?
    if val == "":
        return None

    # Check if int, float, or bool
    int_chars = "-0123456789"
    float_chars = int_chars + ".+e"
    if all([c in int_chars for c in val]):
        try:
            val = int(val)
        except ValueError:
            logger.error(
                "Error processing line {} of {}: ".format(line, file)
                + "Cannot interpret value as integer."
            )
            # Return unprocessed string
            return val
    elif all([c in float_chars for c in val]):

        if units:

            try:
                val = Quantity(float(val) * unit)
            except ValueError:
                logger.error(
                    "Error processing line {} of {}: ".format(line, file)
                    + "Cannot interpret value as float."
                )
                # Return unprocessed string
                return val

        else:

            val = float(val)

    elif (val.lower() == "true") or (val.lower() == "yes"):
        val = True
    elif (val.lower() == "false") or (val.lower() == "no"):
        val = False
    elif val.lower() == "inf":
        val = np.inf
    elif val.lower() == "-inf":
        val = -np.inf
    elif val.lower() == "nan":
        val = np.nan
    return val


class Log(object):
    """
    A class containing the information of a `vplanet` `.log` output file.

    """

    def __init__(self):
        self.sysname = ""
        self._header = LogStage()
        self._header._name = "Header"
        self._initial = LogStage()
        self._initial._name = "Initial"
        self._final = LogStage()
        self._final._name = "Final"
        self._body_names = []
        self.path = ""

    @property
    def header(self):
        """
        Container for the information in the header of the log file,
        an instance of :py:class:`LogStage`.

        """
        return self._header

    @property
    def initial(self):
        """
        Container for the information about the initial conditions of the system,
        an instance of :py:class:`LogStage`.

        """

        return self._initial

    @property
    def final(self):
        """
        Container for the information about the final conditions of the system,
        an instance of :py:class:`LogStage`.

        """
        return self._final

    def __repr__(self):
        return "<vplanet.Log: %s>" % self.sysname

    @property
    def members(self):
        """A list of all the properties of this object."""
        keys = list(self.__dict__.keys())
        keys += ["header", "initial", "final"]
        return [key for key in keys if not key.startswith("_")]


class LogStage(object):
    """A class containing the information of a particular stage of
    a ``vplanet`` ``.log`` output file.

    """

    def __init__(self):
        self._name = ""

    def __repr__(self):
        return "<vplanet.LogStage: %s>" % self._name

    @property
    def members(self):
        """A list of all the properties of this object."""
        keys = list(self.__dict__.keys())
        return [key for key in keys if not key.startswith("_")]


class LogBody(object):
    """A class containing all of the properties of a body at a certain
    stage in a ``vplanet`` ``.log`` output file.

    """

    def __init__(self):
        self._name = ""

    def __repr__(self):
        return "<vplanet.LogBody: %s>" % self._name

    @property
    def members(self):
        """A list of all the properties of this object."""
        keys = list(self.__dict__.keys())
        return [key for key in keys if not key.startswith("_")]


def get_log(path=".", sysname=None, ext="log", units=True):
    """ """
    # Just in case!
    if ext.startswith("."):
        ext = ext[1:]

    # Look for the log file
    if sysname is None:
        lf = glob(os.path.abspath(os.path.join(path, "*.%s" % (ext))))
        if len(lf) > 1:
            raise Exception(
                "There are multiple log files matching {}.".format(
                    os.path.abspath(os.path.join(path, "*.%s" % (ext)))
                )
            )
        elif len(lf) == 0:
            raise Exception(
                "Could not find a log file matching {}.".format(
                    os.path.abspath(os.path.join(path, "*.%s" % (ext)))
                )
            )
        else:
            lf = lf[0]
            sysname = os.path.basename(lf).split(".%s" % ext)[0]
    else:
        lf = glob(os.path.join(path, "%s.%s" % (sysname, ext)))
        if len(lf) == 0:
            raise Exception(
                "Could not find a log file matching {}.".format(
                    os.path.abspath(os.path.join(path, "%s.%s" % (sysname, ext)))
                )
            )
        else:
            lf = lf[0]

    # Grab the contents
    with open(lf, "r") as f:
        lines = f.readlines()

    # Shorten the file name for logging
    lf = os.path.basename(lf)

    # Remove newlines and blank lines
    header = []
    initial = []
    final = []
    stage = 0
    for i, line in enumerate(lines):
        line = line.replace("\n", "")
        if "INITIAL SYSTEM PROPERTIES" in line:
            stage = 1
        elif "FINAL SYSTEM PROPERTIES" in line:
            stage = 2
        if len(line):
            if stage == 0:
                if ("Log file" not in line) and ("FORMATTING" not in line):
                    header.append((i + 1, line))
            elif stage == 1:
                if ("SYSTEM PROPERTIES" not in line) and ("PARAMETERS" not in line):
                    initial.append((i + 1, line))
            elif stage == 2:
                if ("SYSTEM PROPERTIES" not in line) and ("PARAMETERS" not in line):
                    final.append((i + 1, line))

    # Instantiate a `Log` object
    log = Log()
    log.sysname = sysname
    log.path = os.path.abspath(path)

    # Process the header
    for i, line in header:
        try:
            line_items = line.split(":")
            name_and_unit = line_items[0]
            value = ":".join(line_items[1:])
            unit = get_param_unit(name_and_unit, lf, i)
            name = get_param_name(name_and_unit, lf, i)
            value = get_param_value(value, unit, lf, i, units=units)
            setattr(log.header, name, value)
        except Exception as e:
            raise ValueError("Error processing line {} of {}: ".format(i, lf) + str(e))

    # Process the initial conditions
    body = "system"
    logbody = LogBody()
    logbody._name = body
    setattr(log.initial, body, logbody)
    for i, line in initial:
        if "BODY:" not in line:
            try:
                line_items = line.split(":")
                name_and_unit = line_items[0]
                value = ":".join(line_items[1:])
                unit = get_param_unit(name_and_unit, lf, i)
                name = get_param_name(name_and_unit, lf, i)
                value = get_param_value(value, unit, lf, i, units=units)
                setattr(getattr(log.initial, body), name, value)
            except Exception as e:
                raise ValueError(
                    "Error processing line {} of {}: ".format(i, lf) + str(e)
                )
        else:
            try:
                match = re.search(r"BODY:\s(.*?)\s-", line)
                body = match.groups()[0]
            except:
                raise ValueError(
                    "Error processing line {} of {}: ".format(i, lf)
                    + "Cannot understand body name."
                    + line
                )
            logbody = LogBody()
            logbody._name = body
            setattr(log.initial, body, logbody)
            log._body_names.append(body)

    # Process the final conditions
    body = "system"
    logbody = LogBody()
    logbody._name = body
    setattr(log.final, body, logbody)
    for i, line in final:
        if "BODY:" not in line:
            try:
                line_items = line.split(":")
                name_and_unit = line_items[0]
                value = ":".join(line_items[1:])
                unit = get_param_unit(name_and_unit, lf, i)
                name = get_param_name(name_and_unit, lf, i)
                value = get_param_value(value, unit, lf, i, units=units)
                setattr(getattr(log.final, body), name, value)
            except Exception as e:
                raise ValueError(
                    "Error processing line {} of {}: ".format(i, lf) + str(e)
                )
        else:
            try:
                match = re.search(r"BODY:\s(.*?)\s-", line)
                body = match.groups()[0]
            except:
                raise ValueError(
                    "Error processing line {} of {}: ".format(i, lf)
                    + "Cannot understand body name."
                    + line
                )
            logbody = LogBody()
            logbody._name = body
            setattr(log.final, body, logbody)

    return log
