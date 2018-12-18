BRANCH=$(git branch | grep \* | cut -d ' ' -f2)
if [ $BRANCH == "dev" ]
then
    git pull --no-commit --no-ff public master && \
    git checkout $BRANCH README.md && \
    git checkout $BRANCH docs/developers.rst && \
    git checkout $BRANCH docs/genexamples.py && \
    git commit -m "Merging changes from public branch" && \
    echo "Success! Please push your changes to origin dev and issue a pull request for master." ||
    echo "Merge was not performed."
else
    echo "Merging from public must be done on the dev branch."
fi