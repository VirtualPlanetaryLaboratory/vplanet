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
git add -f .nojekyll > /dev/null 2>&1
git add -f *.html > /dev/null 2>&1
git add -f *.js > /dev/null 2>&1
git add -f _sources > /dev/null 2>&1
git add -f _static > /dev/null 2>&1
git add -f _images > /dev/null 2>&1
git add -f examples > /dev/null 2>&1
git add -f bugs > /dev/null 2>&1
git add -f src > /dev/null 2>&1
git add -f $COVERAGE_HASH > /dev/null 2>&1
git add -f docs.log > /dev/null 2>&1
git -c user.name='sphinx' -c user.email='sphinx' commit -m "rebuild gh-pages at ${rev}"
git push -q -f https://$GITHUB_USER:$GITHUB_API_KEY@github.com/$TRAVIS_REPO_SLUG HEAD:gh-pages

# Return to the top level
cd $TRAVIS_BUILD_DIR
