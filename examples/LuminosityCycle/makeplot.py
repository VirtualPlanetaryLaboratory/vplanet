import pathlib
import sys

import matplotlib.pyplot as plt
import vplot

import vplanet

# Path hacks
path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))
from get_args import get_args

# Run vplanet
out = vplanet.run(path / "vpl.in", units=False)

fig = plt.figure(figsize=(6.5, 8))
plt.subplot(2, 1, 1)
plt.plot(
    out.sinewave.Time,
    out.sinewave.Luminosity,
    color=vplot.colors.red,
    label="SineWave",
)
plt.plot(
    out.none.Time,
    out.none.Luminosity,
    color=vplot.colors.pale_blue,
    label="None",
)
plt.legend(loc="best", title="Stellar Model")
plt.ylabel(r"Luminosity (L$_\odot$)")
plt.xlabel("Time (yr)")

plt.subplot(2, 1, 2)
plt.plot(out.sinewave.Time, out.sinewave.Temperature, color=vplot.colors.red)
plt.plot(out.none.Time, out.none.Temperature, color=vplot.colors.pale_blue)
plt.ylabel("Effective Temperature (K)")
plt.xlabel("Time (yr)")

# Save the figure
ext = get_args().ext
fig.savefig(path / f"LuminosityCycle.{ext}", bbox_inches="tight", dpi=300)
