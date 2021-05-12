# -*- coding: utf-8 -*-
from astropy import units as u

# import Angle just so we have a more or less complete list of Quantity
# subclasses loaded - matplotlib needs them all separately!
# NOTE: in matplotlib >=3.2, subclasses will be recognized automatically,
# and once that becomes our minimum version, we can remove this,
# adding just u.Quantity itself to the registry.
from astropy.coordinates import Angle  # noqa
from matplotlib import units


# Get all subclass for Quantity, since matplotlib checks on class, not subclass.
def all_issubclass(cls):
    return {cls}.union([s for c in cls.__subclasses__() for s in all_issubclass(c)])


class quantity_support(units.ConversionInterface):

    _all_issubclass_quantity = all_issubclass(u.Quantity)

    def __init__(self):

        # Keep track of original converter in case the context manager is
        # used in a nested way.
        self._original_converter = {}

        for cls in self._all_issubclass_quantity:
            self._original_converter[cls] = units.registry.get(cls)
            units.registry[cls] = self

    @staticmethod
    def axisinfo(unit, axis):
        # We set the axes labels in `VPLANETFigure` instead
        return None

    @staticmethod
    def convert(val, unit, axis):
        if isinstance(val, u.Quantity):
            return val.to_value(unit)
        elif hasattr(val, "__len__") and len(val) == 0:
            return []
        elif isinstance(val, list) and isinstance(val[0], u.Quantity):
            return [v.to_value(unit) for v in val]
        else:
            return val

    @staticmethod
    def default_units(x, axis):
        if hasattr(x, "unit"):
            return x.unit
        return None

    def __enter__(self):
        return self

    def __exit__(self, type, value, tb):
        for cls in self._all_issubclass_quantity:
            if self._original_converter[cls] is None:
                del units.registry[cls]
            else:
                units.registry[cls] = self._original_converter[cls]
