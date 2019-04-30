import subprocess

# Run vplanet (it will segfault)
try:
    subprocess.check_output(['vplanet', 'vpl.in', '-q'])
except subprocess.CalledProcessError:
    raise AssertionError("This bug is still present.")
