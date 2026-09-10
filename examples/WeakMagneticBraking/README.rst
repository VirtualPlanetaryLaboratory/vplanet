Weakened Magnetic Braking
=========================

Overview
--------

The collapse of the global stellar dynamo, and its effect on spin-down and XUV
emission.

===================   ============
**Date**              08/28/26
**Author**            Rory Barnes
**Modules**           STELLAR
**Approx. runtime**   20 seconds
===================   ============

Old, slowly rotating stars spin down more slowly than standard models predict.
`van Saders et al. [2016]
<https://ui.adsabs.harvard.edu/abs/2016Natur.529..181V/abstract>`_ first
identified this weakened magnetic braking (WMB) in Kepler asteroseismic targets,
and `Metcalfe et al. [2025]
<https://ui.adsabs.harvard.edu/abs/2025arXiv250903717M/abstract>`_ traced it to
the collapse of the global dynamo as the Rossby number approaches a critical
value for dynamo excitation.

This example evolves a 1 solar-mass star for 10 Gyr under four prescriptions,
all using the Baraffe et al. (2015) stellar evolution grid and the Matt et al.
(2015) wind braking torque:

- ``none``: standard spin-down, no WMB.
- ``vansaders``: braking ceases entirely once Ro exceeds the critical value.
- ``metcalfe``: the Metcalfe et al. (2025) suppression factor
  ``N*sqrt(Ro_crit - Ro)`` applied literally, with their fitted ``N = 0.354``.
- ``metcalfenorm``: the same functional form renormalized so the suppression is
  unity at ``Ro = 0``, i.e. ``bNormalizeWeakMagBraking 1``.

The suppression factor multiplies both the wind braking torque and the XUV
luminosity, because Metcalfe et al. attribute the torque deficit to a drop in
Poynting flux that starves coronal heating and the wind alike. The XUV is
floored at ``dXUVWeakMagBrakingFloor`` times the Ribas et al. (2005)
prediction, representing the basal emission from the unstructured field that
survives the collapse of the large-scale dipole.

Rossby numbers are reported in units of the solar Rossby number
(``RossbyNumberNorm``). Absolute Rossby numbers are not portable between
studies, because every author adopts a different definition of the convective
turnover timescale; VPLanet uses the Cranmer & Saar (2011) fit, for which the
solar value is 2.168.

The normalization question
--------------------------

Metcalfe et al. fit ``N = 0.354`` against a vertical axis whose normalization is
not stated explicitly. Taken literally, the suppression factor is 0.354 even for
the most active stars, where the paper says standard spin-down models work. The
two ``metcalfe`` runs bracket the ambiguity, and the 10 Gyr endpoint
discriminates between them: Metcalfe et al. predict that a solar model with WMB
reaches Ro ~ 1.1 Ro_sun at the main-sequence turnoff, against Ro ~ 1.8 for a
standard model.

============================  ===============  ==============  ===============
Model                         Prot at 4.57 Gyr  Ro at 10 Gyr   Psi at 4.57 Gyr
============================  ===============  ==============  ===============
``none``                      29.4 d           1.80           1.00
``vansaders``                 28.8 d           1.16           0.00
``metcalfe`` (literal)        14.9 d           0.85           0.25
``metcalfenorm``              24.1 d           1.08           0.45
============================  ===============  ==============  ===============

The standard run recovers the predicted Ro = 1.8 at the turnoff, and both the
hard cutoff and the renormalized suppression land near the predicted 1.1. The
literal form does not: suppressing the torque by a factor of three from birth
leaves the star spinning too fast to ever reach the Rossby number at which WMB
is supposed to matter, and it arrives at 4.57 Gyr with a rotation period of
14.9 days rather than the solar 25.4 days.

To run this example
-------------------

.. code-block:: bash

    vplanet vpl.in
    python makeplot.py

Expected output
---------------

.. figure:: WeakMagneticBraking.png
   :width: 600px
   :align: center

   Rotation period, Rossby number, suppression factor, and XUV luminosity for
   the four prescriptions. The horizontal line in the Rossby panel marks the
   critical value for dynamo excitation; the dashed line in the rotation panel
   marks the solar rotation period at the solar age.
