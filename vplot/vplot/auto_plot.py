# -*- coding: utf-8 -*-
from . import logger
from .output import GetOutput
import matplotlib.pyplot as plt
import numpy as np


def auto_plot(
    path=".",
    sysname=None,
    group="param",
    bodies=[],
    params=[],
    show=True,
    **kwargs
):
    """Automatically plot the results of a :py:obj:`vplanet` run.

    Args:
        sysname (str, optional): System name. This is determined automatically,
            unless there are multiple runs in the same :py:obj:`path`. Defaults
            to None.
        path (str, optional): Path to the directory containing the results of
            the :py:obj:`vplanet` run. Defaults to the current directory.
        group (str, optional): How to group plots. Options are "param"
            (one plot per parameter), "type" (one plot per physical type, such
            as angle, length, etc.), or "none" (one plot per column in the output
            file). Defaults to "param".
        bodies (list, optional): Which bodies to generate plots for. These
            should be specified as a list of strings (case-insensitive). Defaults
            to ``[]``, in which case all available bodies are plotted.
        params (list, optional): Which parameters to generate plots for. These
            should be specified as a list of strings (case-insensitive). Defaults
            to ``[]``, in which case all available parameters are plotted.
        show (bool, optional): Show the plots? Defaults to True. If False,
            returns the figures instead.
        kwargs (optional): Extra keyword arguments passed directly to
            :py:class:`vplot.VPLOTFigure`.

    Returns:
        If :py:obj:`show` is False, returns a list of figures.
    """
    # Parse kwargs
    group_allowed = ["type", "param", "none"]
    group = str(group).lower()
    assert group in group_allowed, "Keyword `group` must be one of {}.".format(
        ", ".join(group_allowed)
    )

    if type(bodies) is str:
        bodies = [bodies]
    assert type(bodies) is list, "Kewyord `bodies` must be a list."
    for i, body in enumerate(bodies):
        assert type(body) is str, "Items in `bodies` must be strings."
        bodies[i] = body.lower()
    body_names = bodies

    if type(params) is str:
        params = [params]
    assert type(params) is list, "Kewyord `params` must be a list."
    for i, param in enumerate(params):
        assert type(param) is str, "Items in `params` must be strings."
        params[i] = param.lower()
    param_names = [param for param in params if param.lower() != "time"]

    # Grab the output
    output = GetOutput(sysname=sysname, path=path)

    # Grab all params
    params = []
    time = None
    for body in output.bodies:
        if len(body_names) == 0 or body._name.lower() in body_names:
            for param in body._params:
                if param.tags.get("name", "").lower() == "time":
                    if time is None:
                        time = param
                    else:
                        assert np.array_equal(
                            time.to("yr"), param.to("yr")
                        ), "Mismatch in the time arrays for two of the bodies."
                elif (
                    len(param_names) == 0
                    or param.tags.get("name", "").lower() in param_names
                ):
                    params.append(param)

    if len(params) == 0:
        logger.error("No parameters found for plotting.")
        return
    elif time is None:
        logger.error("No Time array found for any of the bodies.")
        return

    # One plot per physical type
    figs = []
    if group == "type":

        physical_types = list(set([p.unit.physical_type for p in params]))
        for physical_type in physical_types:
            arrays = [
                param
                for param in params
                if param.unit.physical_type == physical_type
            ]
            fig, ax = plt.subplots(1, **kwargs)
            for array in arrays:
                ax.plot(time, array)
            figs.append(fig)

    # One plot per parameter name (multiple bodies)
    elif group == "param":

        parameter_names = list(set([p.tags.get("name", None) for p in params]))
        for parameter_name in parameter_names:
            arrays = [
                param
                for param in params
                if param.tags.get("name", None) == parameter_name
            ]
            fig, ax = plt.subplots(1, **kwargs)
            for array in arrays:
                ax.plot(time, array)
            figs.append(fig)

    # One plot per parameter
    else:

        for param in params:
            fig, ax = plt.subplots(1, **kwargs)
            ax.plot(time, param)
            figs.append(fig)

    if show:
        plt.show()
    else:
        plt.close("all")
        return figs
