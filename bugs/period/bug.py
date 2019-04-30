import subprocess
import vplot

# Run vplanet
try:
    subprocess.check_output(['vplanet', 'vpl.in', '-q'])
except subprocess.CalledProcessError:
    raise AssertionError("This bug is still present.")

# Check the output
output = vplot.GetOutput()
largeP = output.bodies[1]
smallP = output.bodies[2]
assert largeP.EnvelopeMass[-1] > smallP.EnvelopeMass[-1], \
    "This bug is still present."
