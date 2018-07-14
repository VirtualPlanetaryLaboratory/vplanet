"""Figure out who needs to work on their docs!"""
import glob
import os
import numpy as np

# Get all the module names
modules = np.array([os.path.basename(x)[:-2] for x in glob.glob("../src/*.c")])
errors = np.array([0 for module in modules])

# Open the log
with open("docs.log", "r") as f:
    log = f.read()

# Count number of errors per module
for i, module in enumerate(modules):
    errors[i] = log.count(module + ".c") + log.count(module + ".h")

# Sort by number of errors
idx = np.argsort(errors)[::-1]
errors = errors[idx]
modules = modules[idx]

# Print
for module, err in zip(modules, errors):
    print("%10s: %d errors" % (module, err))
