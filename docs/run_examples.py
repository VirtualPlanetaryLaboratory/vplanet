import pathlib
import glob
import os
from importlib.util import spec_from_file_location, module_from_spec
import vplot

# Path to this directory
path = pathlib.Path(__file__).parents[0].absolute()

# gets the list of examples
example_list = sorted(glob.glob(str(path.parents[0] / "examples" / "*" / "makeplot.py")))

print(example_list)


# list of examples that should not be ran in the test
fly_list = ["ParameterSweep"]
no_fly_list = ["ChaoticResonances","SSDistOrbDistRot"]
#"MiniNeptuneEvap"

for example in example_list:
    print(example)

    # Example name
    name = pathlib.Path(example).parents[0].name

    if name not in fly_list:

        # Skip
        continue

    else:

        # Run
        spec = spec_from_file_location(name, example)
        module = module_from_spec(spec)
        spec.loader.exec_module(module)
