Installing VPLanet
==================

There are two ways to install VPLanet: 1) cloning with git, or 2) downloading
the repository with a zip file. VPLanet has been developed on macOS and Linux
platforms, but can also run on Windows.

Linux/macOS Installation
------------------------

To install with git, open a terminal and run

.. code-block:: bash

   git clone https://github.com/VirtualPlanetaryLaboratory/vplanet.git
   cd vplanet
   make opt

To install via zip file, click the "Clone or Download" box on the VPLanet webpage,
select zip, and download it into your preferred directory. Then type

.. code-block:: bash

   unzip vplanet-master.zip
   cd vplanet-master
   make opt

Currently, the installer doesn't add the VPLanet executable to your :code:`PATH` variable, so
you'll have to `do it manually <https://stackoverflow.com/questions/14637979/how-to-permanently-set-path-on-linux-unix>`_.

Windows 10 Installation
--------------------

You must first install `cygwin <https://www.cygwin.com>` and manually select
every package that begins with "gcc." Note that as of their v3.0.7, this option
is not the default. Next click the "Clone or Download" box on the VPLanet webpage,
select zip, and download it into your preferred directory.

Next open the command window (DOS window; open with "cmd") and navigate to the
cygwin directory where you wish to place VPLanet, e.g.,

.. code-block:: bash

    cd c:\cygwin64\home\rory\src
    copy \Users\rory\Downloads\vplanet-master.zip .

Then open a File Explorer window and navigate to the directory with the zip file,
right click, and select "Extract All", type in the destination directory, and
click "Extract."

Next open a cygwin terminal window and type the following commands (substituting your username for "rory"):

.. code-block:: bash

  cd /home/rory/src/vplanet-master/src
  gcc *.c -o vplanet

This will create a file called ./vplanet.exe that can be run inside the cygwin terminal
exactly as in the Linux and macOS instructions.
