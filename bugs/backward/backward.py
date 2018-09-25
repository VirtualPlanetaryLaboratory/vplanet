from vplot import GetOutput
import subprocess
import matplotlib.pyplot as pl
import os
dir_fw = os.path.join(os.path.dirname(os.path.realpath(__file__)), "forward")
dir_bw = os.path.join(os.path.dirname(os.path.realpath(__file__)), "backward")
Myr = 1e6

# Run forward and backward runs
subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=dir_fw)
subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=dir_bw)

# Grab the output
forward = GetOutput(path=dir_fw)
backward = GetOutput(path=dir_bw)

# Plot
fig, ax = pl.subplots(2)
ax[0].set_ylabel('Semi Major Axis [AU]', fontweight='bold')
ax[1].set_ylabel('Eccentricity', fontweight='bold')
ax[1].set_xlabel('Time [Myr]', fontweight='bold')
ax[0].plot(forward.a.Time / Myr, forward.a.SemiMajorAxis,
           label='forward')
ax[1].plot(forward.a.Time / Myr, forward.a.Eccentricity,
           label='forward')
ax[0].plot(300 + backward.a.Time / Myr, backward.a.SemiMajorAxis,
           label='backward')
ax[1].plot(300 + backward.a.Time / Myr, backward.a.Eccentricity,
           label='backward')
ax[0].legend(loc='lower right')
ax[1].legend(loc='lower right')
pl.show()
