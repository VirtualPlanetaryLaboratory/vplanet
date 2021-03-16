# -*- coding: utf-8 -*-
from .quantity import VPLOTQuantity
import matplotlib
import matplotlib.pyplot
from matplotlib.figure import Figure
from matplotlib.axes import Axes
import astropy.units as u
import sys
import numpy as np


def _get_array_info(array, max_label_length=40):
    if hasattr(array, "unit") and hasattr(array, "tags"):
        if array.unit.physical_type != array.tags["physical_type"]:
            # The physical type of this array changed, so this is
            # no longer the original VPLANET quantity!
            unit = str(array.unit)
            if unit == "":
                unit = None
            body = None
            label = None
            physical_type = array.unit.physical_type.title()
            if physical_type == "Dimensionless":
                physical_type = None
        else:
            unit = str(array.unit)
            if unit == "":
                unit = None
            body = array.tags.get("body", None)
            label = array.tags.get("description", None)
            if len(label) > max_label_length:
                label = array.tags.get("name", None)
            physical_type = array.unit.physical_type.title()
            if physical_type == "Dimensionless":
                physical_type = None
    else:
        unit = None
        body = None
        label = None
        physical_type = None
    return unit, body, label, physical_type


class VPLOTFigure(Figure):
    """A ``vplot`` figure object, a subclass of :py:class:`matplotlib.figure.Figure`.

    This class adds certain functionality to the default ``matplotlib`` figure
    object, in particular the ability to recognize when ``vplanet`` quantities
    are plotted. When showing / drawing / saving the figure, this class will
    automatically label the axes and/or add a legend with the appropriate
    parameter names, types, units, and corresponding ``vplanet`` body. All of
    these can be overridden by setting the axes or legend labels explcitly.

    This class accepts all args and kwargs as :py:class:`matplotlib.figure.Figure`,
    with support for the following additional keywords:

    Args:
        max_label_length (int, optional): If the parameter description is longer
            than this value, the axis will be labeled with the shorter parameter
            name instead. Default 40.
        mpl_units (bool, optional): Enable matplotlib units functionality? Default
            is True. This allows quantities of the same physical type but different
            units (such as ``yr`` and ``Gyr``) to be plotted on the same axis;
            this class will handle unit conversions as needed. An error will be
            raised if the unit conversion fails.
        xlog (bool, optional): Set the x axis scale to be logarithmic? Default False.
        ylog (bool, optional): Set the y axis scale to be logarithmic? Default False.

    """

    def __init__(
        self,
        *args,
        max_label_length=40,
        mpl_units=True,
        xlog=False,
        ylog=False,
        **kwargs
    ):

        # Parameters
        self.max_label_length = max_label_length
        self.xlog = xlog
        self.ylog = ylog

        # Enable astropy/matplotlib quantity support? (Recommended)
        if mpl_units:
            from .quantity_support import quantity_support

            quantity_support()

        super().__init__(*args, **kwargs)

        # Watch the axes
        self._update_on_draw = True
        self.add_axobserver(self._ax_observer)

    def _ax_observer(self, *args):

        # Force an update next time we draw
        self._update_on_draw = True

        # HACK: Override `ax.scatter` so that we preserve the
        # metadata in the Quantity arrays, as `scatter` converts
        # them to numpy masked arrays. I couldn't find a
        # simple way to subclass `Axes` or `Subplots` to directly
        # replace the `scatter` method, so we'll go with this for now.

        for ax in self.axes:

            if hasattr(ax.scatter, "__vplot__"):
                continue

            old_scatter = ax.scatter

            def new_scatter(x, y, *args, **kwargs):
                collection = old_scatter(x, y, *args, **kwargs)

                def get_data():
                    return VPLOTQuantity(x), VPLOTQuantity(y)

                get_data.__vplot__ = True

                collection.get_data = get_data

                return collection

            ax.scatter = new_scatter

    def _add_labels(self):

        # Get the labels for each axis
        for k, ax in enumerate(self.axes):

            # Skip if there's no data to parse
            if len(ax.lines) == 0 and len(ax.collections) == 0:
                continue

            # Check if there are labels already
            xlabel_exists = not (
                ax.get_xlabel() is None or ax.get_xlabel() == ""
            )
            ylabel_exists = not (
                ax.get_ylabel() is None or ax.get_ylabel() == ""
            )
            legend_exists = ax.get_legend() is not None

            # Skip if the user already set these
            if xlabel_exists and ylabel_exists and legend_exists:
                continue

            # Get info on all lines in the axis
            xunits = []
            xlabels = []
            xtypes = []
            yunits = []
            ylabels = []
            ytypes = []
            bodies = []
            for line in ax.lines + ax.collections:

                if not (hasattr(line, "get_data")):
                    continue

                # Get the data
                x, y = line.get_data()

                # Grab the x metadata
                unit, _, label, physical_type = _get_array_info(
                    x, self.max_label_length
                )
                xunits.append(unit)
                xlabels.append(label)
                xtypes.append(physical_type)

                # Grab the y metadata
                unit, body, label, physical_type = _get_array_info(
                    y, self.max_label_length
                )
                yunits.append(unit)
                ylabels.append(label)
                ytypes.append(physical_type)
                bodies.append(body)

            # Figure out the x physical type
            if len(set(xtypes)) == 1:
                xtype = xtypes[0]
            elif len(set(xtypes)) == 2 and None in xtypes:
                # Allow unitless quantities to be shown on the same
                # axis as unitful quantities, since matplotlib.units allows it
                xtype = [xtype for xtype in xtypes if xtype is not None][0]
            else:
                raise ValueError(
                    "Axis #{} contains quantities with different physical types: {}".format(
                        k + 1, ", ".join(xtypes)
                    )
                )

            # Figure out the y physical type
            if len(set(ytypes)) == 1:
                ytype = ytypes[0]
            elif len(set(ytypes)) == 2 and None in ytypes:
                # Allow unitless quantities to be shown on the same
                # axis as unitful quantities, since matplotlib.units allows it
                ytype = [ytype for ytype in ytypes if ytype is not None][0]
            else:
                raise ValueError(
                    "Axis #{} contains quantities with different physical types: {}".format(
                        k + 1, ", ".join(ytypes)
                    )
                )

            # Figure out the x unit
            if len(set(xunits)) == 1:
                if xunits[0] is None:
                    xunit = None
                else:
                    xunit = str(xunits[0])
            elif len(set(xunits)) > 1:
                xunit = None
                for xunit in set(xunits):
                    if xunit is not None:
                        # A hacky way to figure out the actual unit
                        if ax.convert_xunits(1 * u.Unit(xunit)) == 1:
                            break

            # Figure out the y unit
            if len(set(yunits)) == 1:
                if yunits[0] is None:
                    yunit = None
                else:
                    yunit = str(yunits[0])
            elif len(set(yunits)) > 1:
                yunit = None
                for yunit in set(yunits):
                    if yunit is not None:
                        # A hacky way to figure out the actual unit
                        if ax.convert_yunits(1 * u.Unit(yunit)) == 1:
                            break

            # Are we dealing with single bodies/quantity types?
            single_body = len(set(bodies)) == 1 and bodies[0] is not None
            single_xparam = len(set(xlabels)) == 1 and xlabels[0] is not None
            single_yparam = len(set(ylabels)) == 1 and ylabels[0] is not None

            # Add the x axis label
            if not xlabel_exists:

                xlabel = ""

                if single_xparam:
                    xlabel += "{}".format(xlabels[0])
                elif xtype is not None:
                    xlabel += "{}".format(xtype)

                if xunit is not None:
                    xlabel += " [{}]".format(xunit)

                if xlabel.endswith(": "):
                    xlabel = xlabel[:-2]

                ax.set_xlabel(xlabel)

            # Add the y axis label
            if not ylabel_exists:

                ylabel = ""

                if single_body:
                    ylabel += "{}: ".format(bodies[0])

                if single_yparam:
                    ylabel += "{}".format(ylabels[0])
                elif ytype is not None:
                    ylabel += "{}".format(ytype)

                if yunit is not None:
                    ylabel += " [{}]".format(yunit)

                if ylabel.endswith(": "):
                    ylabel = ylabel[:-2]

                ax.set_ylabel(ylabel)

            # Add the legend
            if not legend_exists:

                make_legend = False

                for k, line in enumerate(ax.lines + ax.collections):
                    if (
                        line.get_label() is None
                        or line.get_label() == ""
                        or line.get_label().startswith("_line")
                        or line.get_label().startswith("_collection")
                    ):

                        label = ""

                        if not single_body and bodies[k] is not None:
                            label += "{}: ".format(bodies[k])

                        if not single_yparam:
                            if ylabels[k] is not None:
                                label += "{}".format(ylabels[k])
                            elif ytype is not None:
                                label += "{}".format(ytype)

                        if label.endswith(": "):
                            label = label[:-2]

                        if label != "":
                            line.set_label(label)
                            make_legend = True

                if make_legend:
                    ax.legend(loc="best")

    def _format_axes(self):
        for ax in self.axes:

            # Force time axis margins to be zero
            if "Time" in ax.get_xlabel():
                ax.margins(0, ax.margins()[1])

            # Make axes logarithmic?
            if self.xlog:
                ax.set_xscale("log")
            else:
                ax.set_xscale("linear")
            if self.ylog:
                ax.set_yscale("log")
            else:
                ax.set_yscale("linear")

    def draw(self, *args, **kwargs):
        if self._update_on_draw:
            self._add_labels()
            self._format_axes()
            self._update_on_draw = False
        super().draw(*args, **kwargs)




# HACK: Override `Figure` so this will work seamlessly in the background
matplotlib.figure.Figure = VPLOTFigure

# HACK: We need to explicitly override `plt.figure` since its default
# kwarg for `FigureClass` is `matplotlib.figure.Figure`. This default
# value is parsed on **import**, so if the user imported `pyplot`
# before `vplot`, the default figure class will still be the old one.
mpl_figure = matplotlib.pyplot.figure


def figure_wrapper(*args, FigureClass=VPLOTFigure, **kwargs):
    return mpl_figure(*args, FigureClass=VPLOTFigure, **kwargs)


matplotlib.pyplot.figure = figure_wrapper
