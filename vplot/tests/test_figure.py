# -*- coding: utf-8 -*-
import vplot as vpl
import matplotlib
import matplotlib.pyplot as plt
import os
import pytest
import numpy as np


# Non-interactive
matplotlib.use("Agg")


# Grab the output
path = os.path.join(
    os.path.dirname(os.path.dirname(__file__)),
    "docs",
    "notebooks",
    "examples",
    "CircumbinaryOrbit",
)
output = vpl.get_output(path=path)


class FigureTester(object):
    def __init__(
        self, xlabel="Simulation Time [yr]", ylabel="", legend_texts=[]
    ):
        self.xlabel = xlabel
        self.ylabel = ylabel
        self.legend_texts = sorted(legend_texts)
        self.fig, self.ax = plt.subplots(1)

    def __enter__(self):
        return self.fig

    def __exit__(self, type, value, traceback):
        self.fig._add_labels()

        # Check labels and legend
        assert self.ax.get_xlabel() == self.xlabel
        assert self.ax.get_ylabel() == self.ylabel
        legend = self.ax.get_legend()
        if self.legend_texts == []:
            assert legend is None
        else:
            assert self.legend_texts == sorted(
                [t.get_text() for t in legend.get_texts()]
            )

        plt.close(self.fig)


def test_basic():
    with FigureTester(ylabel="cbp: Orbital Eccentricity",):
        plt.plot(output.cbp.Time, output.cbp.Eccentricity)


def test_unit_change():
    with FigureTester(
        xlabel="Simulation Time [Myr]", ylabel="cbp: Orbital Eccentricity",
    ):
        plt.plot(output.cbp.Time.to("Myr"), output.cbp.Eccentricity)


def test_two_quantities():
    with FigureTester(
        ylabel="cbp: Angle [deg]",
        legend_texts=[
            "Longitude of ascending node",
            "Longitude of pericenter",
        ],
    ):
        plt.plot(output.cbp.Time, output.cbp.LongA)
        plt.plot(output.cbp.Time, output.cbp.LongP)


def test_two_bodies():
    with FigureTester(
        ylabel="Orbital Eccentricity", legend_texts=["cbp", "earth"],
    ):
        plt.plot(output.cbp.Time, output.cbp.Eccentricity)
        plt.plot(output.earth.Time, output.earth.Eccentricity)


def test_two_quantities_two_bodies():
    with FigureTester(
        ylabel="Angle [deg]",
        legend_texts=[
            "cbp: Longitude of ascending node",
            "earth: Longitude of pericenter",
        ],
    ):
        plt.plot(output.cbp.Time, output.cbp.LongA)
        plt.plot(output.earth.Time, output.earth.LongP)


def test_degrees_radians():
    with FigureTester(
        ylabel="cbp: Angle [deg]",
        legend_texts=[
            "Longitude of ascending node",
            "Longitude of pericenter",
        ],
    ):
        plt.plot(output.cbp.Time, output.cbp.LongA)
        plt.plot(output.cbp.Time, output.cbp.LongP.to("rad"))


def test_mixed_y_units():
    with FigureTester(
        ylabel="Angle [deg]",
        legend_texts=["cbp: Longitude of ascending node", "Angle"],
    ):
        plt.plot(output.cbp.Time, output.cbp.LongA)
        plt.plot(output.cbp.Time, np.ones(len(output.cbp.LongA)))


def test_mixed_xy_units():
    with FigureTester(
        xlabel="Time [yr]",
        ylabel="Angle [deg]",
        legend_texts=["cbp: Longitude of ascending node", "Angle"],
    ):
        plt.plot(output.cbp.Time, output.cbp.LongA)
        plt.plot(
            np.arange(len(output.cbp.Time)), np.ones(len(output.cbp.LongA))
        )


def test_custom_labels():
    with FigureTester(
        xlabel="xlabel", ylabel="ylabel",
    ):
        plt.plot(output.cbp.Time, output.cbp.Eccentricity)
        plt.xlabel("xlabel")
        plt.ylabel("ylabel")


def test_unitless():
    with FigureTester(
        xlabel="", ylabel="",
    ):
        plt.plot(np.linspace(0, 1, 100), np.ones(100))


# TODO: test scatter
