#!/usr/bin/env python
# -*- coding: utf-8 -*-
# setup script stolen shamelessly from Rod Luger

from __future__ import print_function
import os

print("\x1b[01m%s\x1b[39;49;00m" % "Setting up Multi-Planet...")

# Get home directory
home = os.path.expanduser('~')

# Command we're adding to the user profile
path = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'bin')
pythonpath = os.path.join(os.path.dirname(os.path.realpath(__file__)))
cmd = '\n\n# Added by Multi-Planet 0.1.0 setup script\nexport PATH=%s:$PATH\nexport PYTHONPATH=${PYTHONPATH}:%s' % (path, pythonpath)

# Setup success?
success = False

for file in [foo for foo in ['.bash_profile', '.profile', '.bashrc'] if foo in os.listdir(home)]:

  with open(os.path.join(home, file), 'r') as f:
    contents = f.read()

  # Check if the path is already there
  if cmd in contents:
    success = True
    break

  # We're going to add it
  else:
    with open(os.path.join(home, file), 'a') as f:
      print(cmd, file = f)
    success = True

if success == False:
  print("Error adding \x1b[01mmulti-planet\x1b[39;49;00m to your PATH.")
  print("You will have to add `%s` manually." % path)
else:
  print("Success. Navigate to the Multi-Planet directory and read the README.")
  print("You may need to restart your terminal.")
