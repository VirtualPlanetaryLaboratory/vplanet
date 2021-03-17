#!/bin/bash
set -e

# Load the environment
if [[ -n $CONDA ]]; then
    . $CONDA/etc/profile.d/conda.sh
    conda activate vplot
fi

# Force an update of the JSON file
python -c "import vplot; vplot.output.get_param_descriptions()"

# Don't build on PRs
if [[ -n $BUILDREASON ]] && [[ $BUILDREASON != "PullRequest" ]]; then

    # Check if the JSON file changed
    if [[ $(git diff --name-only) == *"vplot/params.json"* ]]; then

        # Clone the repo, update the file, and push it back
        mkdir tmp_vplot
        git clone https://github.com/VirtualPlanetaryLaboratory/vplot.git tmp_vplot
        pushd tmp_vplot
            git checkout $GHBRANCH
            git pull
            cp ../vplot/params.json vplot/params.json
            git add vplot/params.json
            git -c user.name='rodluger' -c user.email='rodluger@gmail.com' \
                commit -m "update parameter descriptions"
            git push https://$GHUSER:$GHKEY@github.com/VirtualPlanetaryLaboratory/vplot HEAD:$GHBRANCH
        popd
        rm -rf tmp_vplot

    fi

fi