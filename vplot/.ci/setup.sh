#!/bin/bash
set -e

# Display machine info
lscpu

# Set up conda
sudo chown -R $USER $CONDA
. $CONDA/etc/profile.d/conda.sh
conda create --yes --quiet --name vplot python=3.7.3 pip

# Activate conda & install base dependencies
. $CONDA/etc/profile.d/conda.sh
conda activate vplot
conda install -y -q numpy matplotlib
pip install -U pip
pip install -U setuptools
pip install -U -r requirements.txt

# Install vplanet & add to path
git clone https://github.com/VirtualPlanetaryLaboratory/vplanet.git
pushd vplanet
make
VPLANETPATH=pwd
echo "##vso[task.prependpath]$(pwd)"
popd

# Install vplot
python setup.py develop