import subprocess
try:
    proc = subprocess.run(['vplanet', 'vpl.in'], stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE, timeout=5)
except subprocess.TimeoutExpired:
    assert False, "This bug is still present."
