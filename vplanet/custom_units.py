# -*- coding: utf-8 -*-
import astropy.units as u

# Aliases
custom_units = [
    #
    # Unitful quantities:
    #
    u.def_unit("TO", 1.39e21 * u.kg),  # Terrestrial ocean
    u.def_unit("LSUN", 3.846e26 * u.W),  # Solar luminosity
    #
    # Aliases that need to be standardized in VPLANET:
    #
    u.def_unit("sec", u.second),
    u.def_unit("years", u.year),
    u.def_unit("days", u.day),
    u.def_unit("bars", u.bar),
    u.def_unit("ergs", u.erg),
    u.def_unit("Earth Radii", u.Rearth),
    u.def_unit("Earth radii", u.Rearth),
    u.def_unit("Earthradii", u.Rearth),
    u.def_unit("Earth Masses", u.Mearth),
    u.def_unit("Degrees", u.deg),
    #
    # Non-standard quantities we'll interpret as unitless:
    #
    u.def_unit("F/F_Earth", u.Unit("")),
    u.def_unit("Initial Primordial Earth Number", u.Unit("")),
    u.def_unit("EMAGMOM", u.Unit("")),
    u.def_unit("EMAGPAUSERAD", u.Unit("")),
]

# Register the units
u.add_enabled_units(custom_units)

# Make them available as regular units
for unit in custom_units:
    name = unit.name.replace("/", "_").replace(" ", "")
    setattr(u, name, unit)
