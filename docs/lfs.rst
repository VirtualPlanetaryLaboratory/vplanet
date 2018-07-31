Using Git LFS
=============

.. warning::

    I just disabled Git LFS and returned the example png images to the
    git tree. As it turns out, LFS doesn't seem like that good of a deal
    for us. More on this soon...


It's bad practice to commit output with **git**, especially if the file is large
and/or always changing. If for some reason you really need access to
an output file (say, a large data file that you use as input in a simulation),
this should be hosted externally somewhere permanent like `Zenodo <https://zenodo.org>`_.

However, if you **really** need to commit output, there's an alternate, more
practical solution: `Git Large File Storage (LFS) <https://git-lfs.github.com/>`_. The idea here is
to flag certain files or file types (such as images, videos, or :code:`.dat` files)
to tell Git LFS to track them. Once you set that up, Git LFS works behind the scenes
to treat these files separately whenever you change, commit, and push them to the
remote. The files still appear on GitHub, they are still accessible by anyone who
clones the repo, and are otherwise indistinguishable from other files, **except**
they are not version-controlled! This is perfect for output files, which can always
be reproduced from the code (which **is** version controlled), or for input data
files that never change. Git LFS helps to keep your repository size small even
when you commit large files and makes cloning/fetching faster since it can bundle all
LFS files and download them as a single unit.

.. warning::

    For truly large files (50 MB or more) or for **many** intermediate output
    files, you should host them externally as mentioned above. Use LFS with care,
    since anyone who clones the repo will be forced to download all of your
    stuff!

The instructions for installing Git LFS and flagging files to be tracked using LFS
are `here <https://git-lfs.github.com/>`_. It's actually **really** easy, but let
me (**@rodluger**) know if you have any questions. There's also a pretty
good `tutorial on LFS here <https://www.atlassian.com/git/tutorials/git-lfs#clone-respository>`_.

Currently, the repository is set to track all :code:`pdf` and :code:`png` files
using LFS. You can see this in the :code:`.gitattributes` file at the top level
of the repo. Note, however, that these files are also in the :code:`.gitignore`,
so this forces you to think twice before committing output:

.. code-block:: bash

    >>> git add image.png
    The following paths are ignored by one of your .gitignore files:
    image.png
    Use -f if you really want to add them.

If you really want to add the image, do

.. code-block:: bash

    >>> git add -f image.png

You can verify that this file appears to behave like any regular file in the
staging area:

.. code-block:: bash

    >>> git status
    On branch dev
    Your branch is up-to-date with 'origin/dev'.
    Changes to be committed:
      (use "git reset HEAD <file>..." to unstage)

    	new file:   image.png

but, sneakily, Git LFS is taking care of it in the background:

.. code-block:: bash

    >>> git lfs status
    On branch dev
    Git LFS objects to be pushed to origin/dev:

    Git LFS objects to be committed:

    	docs/foo.png (Git: e3b0c44)

    Git LFS objects not staged for commit:
