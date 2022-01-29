# -*- coding: utf-8 -*-
import numbers
import re

import astropy
import astropy.units as u
import numpy as np
from astropy.units.core import Unit, UnitBase, UnitsError, dimensionless_unscaled
from astropy.utils.misc import isiterable

# TODO: There may be other methods in
# https://github.com/astropy/astropy/blob/master/astropy/units/quantity.py
# that we'll need to subclass in order to get `tags` to work


class VPLANETQuantity(u.Quantity):
    """A `~astropy.units.Quantity` represents a number with some associated unit.
    See also: http://docs.astropy.org/en/stable/units/quantity.html
    Parameters
    ----------
    value : number, `~numpy.ndarray`, `Quantity` object (sequence), str
        The numerical value of this quantity in the units given by unit.  If a
        `Quantity` or sequence of them (or any other valid object with a
        ``unit`` attribute), creates a new `Quantity` object, converting to
        `unit` units as needed.  If a string, it is converted to a number or
        `Quantity`, depending on whether a unit is present.
    unit : `~astropy.units.UnitBase` instance, str
        An object that represents the unit associated with the input value.
        Must be an `~astropy.units.UnitBase` object or a string parseable by
        the :mod:`~astropy.units` package.
    dtype : ~numpy.dtype, optional
        The dtype of the resulting Numpy array or scalar that will
        hold the value.  If not provided, it is determined from the input,
        except that any integer and (non-Quantity) object inputs are converted
        to float by default.
    copy : bool, optional
        If `True` (default), then the value is copied.  Otherwise, a copy will
        only be made if ``__array__`` returns a copy, if value is a nested
        sequence, or if a copy is needed to satisfy an explicitly given
        ``dtype``.  (The `False` option is intended mostly for internal use,
        to speed up initialization where a copy is known to have been made.
        Use with care.)
    order : {'C', 'F', 'A'}, optional
        Specify the order of the array.  As in `~numpy.array`.  This parameter
        is ignored if the input is a `Quantity` and ``copy=False``.
    subok : bool, optional
        If `False` (default), the returned array will be forced to be a
        `Quantity`.  Otherwise, `Quantity` subclasses will be passed through,
        or a subclass appropriate for the unit will be used (such as
        `~astropy.units.Dex` for ``u.dex(u.AA)``).
    ndmin : int, optional
        Specifies the minimum number of dimensions that the resulting array
        should have.  Ones will be pre-pended to the shape as needed to meet
        this requirement.  This parameter is ignored if the input is a
        `Quantity` and ``copy=False``.
    Raises
    ------
    TypeError
        If the value provided is not a Python numeric type.
    TypeError
        If the unit provided is not either a :class:`~astropy.units.Unit`
        object or a parseable string unit.
    Notes
    -----
    Quantities can also be created by multiplying a number or array with a
    :class:`~astropy.units.Unit`. See http://docs.astropy.org/en/latest/units/
    """

    # Need to set a class-level default for _equivalencies, or
    # Constants can not initialize properly
    _equivalencies = []

    # Default unit for initialization; can be overridden by subclasses,
    # possibly to `None` to indicate there is no default unit.
    _default_unit = dimensionless_unscaled

    # Ensures views have an undefined unit.
    _unit = None

    __array_priority__ = 10000

    def __new__(
        cls,
        value,
        unit=None,
        tags={},
        dtype=None,
        copy=True,
        order=None,
        subok=False,
        ndmin=0,
    ):

        if unit is not None:
            # convert unit first, to avoid multiple string->unit conversions
            unit = Unit(unit)

        # optimize speed for Quantity with no dtype given, copy=False
        if isinstance(value, u.Quantity):
            if unit is not None and unit is not value.unit:
                value = value.to(unit)
                # the above already makes a copy (with float dtype)
                copy = False

            if type(value) is not cls and not (subok and isinstance(value, cls)):
                value = value.view(cls)

            if dtype is None:
                if not copy:
                    return value

                if value.dtype.kind in "iu":
                    dtype = float

            return np.array(
                value,
                dtype=dtype,
                copy=copy,
                order=order,
                subok=True,
                ndmin=ndmin,
            )

        # Maybe str, or list/tuple of Quantity? If so, this may set value_unit.
        # To ensure array remains fast, we short-circuit it.
        value_unit = None
        if not isinstance(value, np.ndarray):
            if isinstance(value, str):
                # The first part of the regex string matches any integer/float;
                # the second parts adds possible trailing .+-, which will break
                # the float function below and ensure things like 1.2.3deg
                # will not work.
                pattern = (
                    r"\s*[+-]?"
                    r"((\d+\.?\d*)|(\.\d+)|([nN][aA][nN])|"
                    r"([iI][nN][fF]([iI][nN][iI][tT][yY]){0,1}))"
                    r"([eE][+-]?\d+)?"
                    r"[.+-]?"
                )

                v = re.match(pattern, value)
                unit_string = None
                try:
                    value = float(v.group())

                except Exception:
                    raise TypeError(
                        'Cannot parse "{}" as a {}. It does not '
                        "start with a number.".format(value, cls.__name__)
                    )

                unit_string = v.string[v.end() :].strip()
                if unit_string:
                    value_unit = Unit(unit_string)
                    if unit is None:
                        unit = value_unit  # signal no conversion needed below.

            elif (
                isiterable(value)
                and len(value) > 0
                and all(isinstance(v, VPLANETQuantity) for v in value)
            ):
                # Convert all quantities to the same unit.
                if unit is None:
                    unit = value[0].unit
                value = [q.to_value(unit) for q in value]
                value_unit = unit  # signal below that conversion has been done

        if value_unit is None:
            # If the value has a `unit` attribute and if not None
            # (for Columns with uninitialized unit), treat it like a quantity.
            value_unit = getattr(value, "unit", None)
            if value_unit is None:
                # Default to dimensionless for no (initialized) unit attribute.
                if unit is None:
                    unit = cls._default_unit
                value_unit = unit  # signal below that no conversion is needed
            else:
                try:
                    value_unit = Unit(value_unit)
                except Exception as exc:
                    raise TypeError(
                        "The unit attribute {!r} of the input could "
                        "not be parsed as an astropy Unit, raising "
                        "the following exception:\n{}".format(value.unit, exc)
                    )

                if unit is None:
                    unit = value_unit
                elif unit is not value_unit:
                    copy = False  # copy will be made in conversion at end

        value = np.array(
            value,
            dtype=dtype,
            copy=copy,
            order=order,
            subok=False,
            ndmin=ndmin,
        )

        # check that array contains numbers or long int objects
        if value.dtype.kind in "OSU" and not (
            value.dtype.kind == "O" and isinstance(value.item(0), numbers.Number)
        ):
            raise TypeError("The value must be a valid Python or Numpy numeric type.")

        # by default, cast any integer, boolean, etc., to float
        if dtype is None and value.dtype.kind in "iuO":
            value = value.astype(float)

        # if we allow subclasses, allow a class from the unit.
        if subok:
            qcls = getattr(unit, "_quantity_class", cls)
            if issubclass(qcls, cls):
                cls = qcls

        value = value.view(cls)
        value._set_unit(value_unit)

        # Custom tags
        value.tags = tags

        if unit is value_unit:
            return value
        else:
            # here we had non-Quantity input that had a "unit" attribute
            # with a unit different from the desired one.  So, convert.
            return value.to(unit)

    def __array_finalize__(self, obj):
        # If we're a new object or viewing an ndarray, nothing has to be done.
        if obj is None or obj.__class__ is np.ndarray:
            return

        # If our unit is not set and obj has a valid one, use it.
        if self._unit is None:
            unit = getattr(obj, "_unit", None)
            if unit is not None:
                self._set_unit(unit)

        # Copy info if the original had `info` defined.  Because of the way the
        # DataInfo works, `'info' in obj.__dict__` is False until the
        # `info` attribute is accessed or set.
        if "info" in obj.__dict__:
            self.info = obj.info

        # Custom tags
        self.tags = getattr(obj, "tags", {})


class NumpyQuantity(np.ndarray):
    """
    A custom subclass of numpy ndarray with tags.

    """

    def __new__(cls, input_array, tags={}, unit=None):
        # Input array is an already formed ndarray instance
        # We first cast to be our class type
        obj = np.asarray(input_array).view(cls)
        # add the new attribute to the created instance
        obj.tags = tags
        obj.unit = unit
        # Finally, we must return the newly created object:
        return obj

    def __array_finalize__(self, obj):
        # see InfoArray.__array_finalize__ for comments
        if obj is None:
            return
        self.tag = getattr(obj, "tags", {})
        self.unit = getattr(obj, "unit", None)

    def __array_wrap__(self, out_arr, context=None):
        # Call the parent
        return np.ndarray.__array_wrap__(self, out_arr, context)
