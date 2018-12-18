#!/bin/bash
set -e

# Make the docs
cd $TRAVIS_BUILD_DIR/docs
make html
python scoreboard.py
cp sphinx_rtd_theme/static/css/theme.css .build/html/_static/css/theme.css

# Copy coverage stats over
cp -r $TRAVIS_BUILD_DIR/gcov/html $TRAVIS_BUILD_DIR/docs/.build/html/$COVERAGE_HASH

# Begin
branch=$(git branch | sed -n -e 's/^\* \(.*\)/\1/p')
echo "Building docs from ${branch} branch..."

# Get git hash
rev=$(git rev-parse --short HEAD)

# Initialize a new git repo in the build dir,
# add the necessary files, and force-push
# to the gh-pages branch
cd $TRAVIS_BUILD_DIR/docs/.build/html
git init
touch .nojekyll
git add -f .nojekyll
git add -f *.html
git add -f *.js
git add -f _sources
git add -f _static
git add -f _images
git add -f examples
git add -f bugs
git add -f src
git add -f docs.log
git -c user.name='sphinx' -c user.email='sphinx' commit -m "rebuild gh-pages at ${rev}"
git push -q -f https://$GITHUB_USER:$GITHUB_API_KEY@github.com/$TRAVIS_REPO_SLUG HEAD:gh-pages

# Return to the top level
cd $TRAVIS_BUILD_DIR
