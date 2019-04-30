import subprocess
proc = subprocess.Popen(['vplanet', 'vpl.in'], stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE)
output = proc.stderr.read()
assert len(output) < 10000, "This bug is still present."
