# -*- coding: utf-8 -*-
from vplanet import Quantity
import astropy.units as u
import numpy as np


def test_quantity():

    # A simple position quantity
    x = Quantity(1 * u.m)
    x.tags = dict(name="position")

    # Check that tags are preserved during operations
    assert x.tags["name"] == "position"
    assert x.to("km").tags["name"] == "position"
    assert (2 * x).tags["name"] == "position"
    assert np.abs(x).tags["name"] == "position"
    assert np.ones_like(x).tags["name"] == "position"
