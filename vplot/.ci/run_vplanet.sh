#!/bin/bash

# Load the environment
if [[ -n $CONDA ]]; then
    . $CONDA/etc/profile.d/conda.sh
    conda activate vplot
fi

# Run all the examples
for f in docs/notebooks/examples/*/vpl.in 
do 
    pushd ${f%/*}
        vplanet vpl.in
    popd
done