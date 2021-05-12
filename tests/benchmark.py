import vplanet
import numpy as np
import os
import pytest
import functools
import inspect
import astropy.units as u


def recursive_getattr(obj, attr, *args):
    _getattr = lambda obj, attr: getattr(obj, attr, *args)
    return functools.reduce(_getattr, [obj] + attr.split("."))


class Benchmark:
    def test_benchmark(self, vplanet_output, param, value, unit, param_options):
        assert np.isclose(
            recursive_getattr(vplanet_output, param), value * unit, **param_options
        )


def benchmark(args_dict):
    args = []
    for param, v in args_dict.items():
        value = v.pop("value", 0.0)
        unit = v.pop("unit", u.Unit(""))
        marks = v.pop("marks", None)
        if marks is not None:
            args.append(pytest.param(param, value, unit, v, marks=marks))
        else:
            args.append(pytest.param(param, value, unit, v))
    return pytest.mark.parametrize("param,value,unit,param_options", args)
