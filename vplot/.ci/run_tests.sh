#!/bin/bash

# Load the environment
if [[ -n $CONDA ]]; then
    . $CONDA/etc/profile.d/conda.sh
    conda activate vplot
fi

# Install dependencies
pip install -U parameterized nose pytest pytest-cov
pip install -U coverage-badge

# Run tests
py.test -v -s tests --junitxml=junit/test-results.xml \
        --cov=vplot --cov-report html:coverage \
        --cov-config=.ci/.coveragerc tests

# Generate coverage badge
coverage-badge -o coverage/coverage.svg