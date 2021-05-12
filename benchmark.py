import vplanet
import numpy as np
import os
import pytest
import functools


def recursive_getattr(obj, attr, *args):
    _getattr = lambda obj, attr: getattr(obj, attr, *args)
    return functools.reduce(_getattr, [obj] + attr.split("."))


class Benchmark:
    def test_benchmark(self, output, param, value, options):
        assert np.isclose(recursive_getattr(output, param), value, **options)


def benchmark(args):
    for i in range(len(args)):
        if len(args[i]) == 2:
            args[i] = (args[i][0], args[i][1], dict())
        elif len(args[i]) == 3:
            pass
        else:
            raise ValueError("Invalid argument to `benchmark`.")
    return pytest.mark.parametrize("param,value,options", args)
