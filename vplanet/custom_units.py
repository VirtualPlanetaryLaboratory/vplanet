# -*- coding: utf-8 -*-
import astropy.units as u

# Aliases
custom_units = [
    u.def_unit("sec", u.second),  # Alias for second
    u.def_unit("years", u.year),  # Alias for year
    u.def_unit("days", u.day),  # Alias for day
    u.def_unit("bars", u.bar),  # Alias for bar
    u.def_unit("ergs", u.erg),  # Alias for erg
    u.def_unit("TO", 1.39e21 * u.kg),  # Terrestrial ocean
    u.def_unit("LSUN", 3.846e26 * u.W),  # Solar luminosity
    u.def_unit("F/F_Earth", u.Unit("")),  # TODO: Unit in Binary
]

# Register the units
u.add_enabled_units(custom_units)

# Make them available as regular units
for unit in custom_units:
    name = unit.name.replace("/", "_")
    setattr(u, name, unit)
