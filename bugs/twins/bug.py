import subprocess
import numpy as np
import vplot

# Run vplanet
try:
    subprocess.check_output(['vplanet', 'vpl.in', '-q'])
except subprocess.CalledProcessError:
    raise AssertionError("This bug is still present.")

# Check
output = vplot.GetOutput()
assert not np.all([output.bodies[2].EnvelopeMass ==
                   output.bodies[1].EnvelopeMass]), \
                   "This bug is still present."
