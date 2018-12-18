BRANCH=$(git branch | grep \* | cut -d ' ' -f2)
if [ $BRANCH == "test" ]
then
    git pull --no-commit --no-ff private master && \
    git checkout $BRANCH README.md && \
    git checkout $BRANCH docs/developers.rst && \
    git checkout $BRANCH docs/genexamples.py && \
    git commit -m "Merging changes from private branch" && \
    echo "Success! Please push your changes to origin dev." ||
    echo "Merge was not performed."
else
    echo "Merging from private must be done on the dev branch."
fi