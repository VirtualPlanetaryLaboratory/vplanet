#!/usr/bin/env python
"""Check the Metcalfe et al. (2025) weakened-magnetic-braking suppression law
N*sqrt(Ro_crit - Ro) against the L_X and torque data in their Table 1."""

import argparse
import csv
import math


def fdFloat(sVal):
  """Return a float from a table cell, or None if the cell is blank."""
  sVal = sVal.strip()
  if sVal == "" or sVal == "flat":
    return None
  try:
    return float(sVal)
  except ValueError:
    return None


def flistReadTable(sPath):
  """Return the Table 1 rows as a list of dicts keyed by stripped column name."""
  with open(sPath, newline="") as fp:
    reader = csv.DictReader(fp)
    return [{k.strip(): v for k, v in row.items() if k is not None} for row in reader]


def fdSuppression(fRo, fRoCrit, fExp):
  """Return the normalized dynamo suppression factor, unity at Ro = 0."""
  if fRo >= fRoCrit:
    return 0.0
  return (math.sqrt(fRoCrit - fRo) / math.sqrt(fRoCrit)) ** fExp


def flistLxRatios(listRows, fLxSun, fRoSun):
  """Return (name, Ro, observed/standard L_X ratio) for each star with L_X data."""
  listOut = []
  for dRow in listRows:
    fLx = fdFloat(dRow.get("Lx", ""))
    fLbol = fdFloat(dRow.get("Lbol", ""))
    fRo = fdFloat(dRow.get("Ro", ""))
    if fLx is None or fLbol is None or fRo is None:
      continue
    # Standard model of van Saders & Pinsonneault (2013): L_X ~ L_bol / Ro^2,
    # calibrated on the Sun, with L_bol and Ro already in solar units.
    fLxStd = fLxSun * fLbol / (fRo / fRoSun) ** 2
    listOut.append((dRow["Name"].strip(), fRo, fLx / fLxStd))
  return listOut


def fdChiForModel(listRatios, fRoCrit, fExp, fFloor):
  """Return (log-space residual, best amplitude) for one (Ro_crit, exp) pair."""
  listRes = []
  for _, fRo, fRatio in listRatios:
    if fRatio <= 0:
      continue
    fPsi = max(fdSuppression(fRo, fRoCrit, fExp), fFloor)
    listRes.append(math.log10(fRatio) - math.log10(fPsi))
  if not listRes:
    return None, None
  # The best amplitude in log space is just the mean offset.
  fAmp = sum(listRes) / len(listRes)
  return sum((f - fAmp) ** 2 for f in listRes), 10.0**fAmp


def fdGridSearch(listRatios, fFloor):
  """Return the (Ro_crit, exp, amplitude) minimizing log-space residuals."""
  tBest = (None, None, None, None)
  for iCrit in range(1, 1001):
    fRoCrit = 1.000 + 0.001 * iCrit
    for iExp in range(1, 61):
      fExp = 0.1 * iExp
      fChi, fAmp = fdChiForModel(listRatios, fRoCrit, fExp, fFloor)
      if fChi is not None and (tBest[3] is None or fChi < tBest[3]):
        tBest = (fRoCrit, fExp, fAmp, fChi)
  return tBest


def fnMain():
  parser = argparse.ArgumentParser(description=__doc__)
  parser.add_argument("sTable", help="path to the Metcalfe et al. Table 1 CSV")
  parser.add_argument("sOut", help="path for the text report")
  parser.add_argument("--floor", type=float, default=1e-3,
                      help="minimum suppression factor, avoiding log(0) above Ro_crit")
  args = parser.parse_args()

  listRows = flistReadTable(args.sTable)
  fLxSun, fRoSun = 2.00e27, 1.0
  listRatios = flistLxRatios(listRows, fLxSun, fRoSun)
  fRoCrit, fExp, fAmp, fChi = fdGridSearch(listRatios, args.floor)

  listLines = [
      "Weakened magnetic braking: L_X suppression vs Rossby number",
      "Standard model L_X = L_X,sun * (L_bol/L_bol,sun) * Ro^-2, Sun-calibrated",
      "Suppression law  Psi(Ro) = [sqrt(Ro_crit - Ro)/sqrt(Ro_crit)]^k, floored at %g" % args.floor,
      "Best fit: Ro_crit = %.3f Ro_sun, k = %.2f, amplitude = %.3f" % (fRoCrit, fExp, fAmp),
      "          rms log10 residual = %.3f dex over %d stars" % (
          math.sqrt(fChi / len(listRatios)), len(listRatios)),
      "Paper:    Ro_crit = 1.014 +/- 0.026 Ro_sun",
      "",
      "The amplitude absorbs the offset between a Sun-calibrated standard model",
      "and the paper's active-star calibration; it is not a free physical parameter.",
      "",
      "%-16s %8s %12s %12s %10s" % ("Star", "Ro", "Lx_obs/std", "amp*Psi", "ratio"),
  ]
  listLines.append("")
  listLines.append("Ro_crit fixed at the paper value, scanning exponent and floor:")
  listLines.append("  %6s %8s %10s %10s" % ("k", "floor", "rms(dex)", "amplitude"))
  tBestFix = (None, None, None, None)
  for fExpFix in [1.0, 1.39, 2.0, 3.0]:
    for fFloorFix in [1e-3, 0.01, 0.03, 0.1, 0.2, 0.3]:
      fChiFix, fAmpFix = fdChiForModel(listRatios, 1.014, fExpFix, fFloorFix)
      fRms = math.sqrt(fChiFix / len(listRatios))
      listLines.append("  %6.2f %8.3f %10.3f %10.3f" % (fExpFix, fFloorFix, fRms, fAmpFix))
      if tBestFix[2] is None or fRms < tBestFix[2]:
        tBestFix = (fExpFix, fFloorFix, fRms, fAmpFix)
  listLines.append("  best: k = %.2f, floor = %.3f, rms = %.3f dex" % tBestFix[:3])
  listLines.append("")
  for sName, fRo, fRatio in sorted(listRatios, key=lambda t: t[1]):
    fPred = fAmp * max(fdSuppression(fRo, fRoCrit, fExp), args.floor)
    listLines.append("%-16s %8.3f %12.4f %12.4f %10.2f" % (
        sName if sName else "(unnamed)", fRo, fRatio, fPred, fRatio / fPred))

  with open(args.sOut, "w") as fp:
    fp.write("\n".join(listLines) + "\n")
  print("\n".join(listLines))


if __name__ == "__main__":
  fnMain()
