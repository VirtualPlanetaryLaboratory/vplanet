#!/bin/bash

# Exit on errors
set -o errexit -o nounset

# Check that we are on Bitbucket Pipelines
if [ -z "$BITBUCKET_BUILD_NUMBER" ]
then
    exit 0
fi

# Begin
branch=$(git branch | sed -n -e 's/^\* \(.*\)/\1/p')
echo "Building docs from ${branch} branch..."

# Get git hash
rev=$(git rev-parse --short HEAD)

# Copy the html folder to a temporary location, initialize
# a new git repo there, add the necessary files, and force-push
# to bitbucket pages.
cd .build
cp -r html tmp_html
cd tmp_html
git init
touch .nojekyll
git add -f .nojekyll
git add -f *.html
git add -f *.js
git add -f _sources
git add -f _static
git add -f _images
git -c user.name='pipelines' -c user.email='pipelines' commit -m "rebuild docs at ${rev}"
git push -f https://bitbucket.org/rodluger/rodluger.bitbucket.io.git HEAD:master

# Remove the temporary directory
cd ..
rm -rf tmp_html
