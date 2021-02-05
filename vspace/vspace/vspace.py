from __future__ import print_function
import os
import sys
import subprocess as sb
import numpy as np
import re
import vspace_hyak
import itertools as it
import matplotlib.pyplot as plt
import subprocess as sub
import argparse

def SearchAngleUnit(src,flist):
  for fcurr in flist:
    fread = open(src+'/'+fcurr).read()
    if 'sUnitAngle' in fread:
      angUnit = fread.split()[np.where(np.asarray(fread.split())=='sUnitAngle')[0][0]+1]

  return angUnit


parser = argparse.ArgumentParser(description="Create Vplanet parameter sweep")
parser.add_argument("-f","--force", action="store_true", help="forces override of vspace file creation")
parser.add_argument("InputFile",type = str, help="Name of the vspace input file")
args = parser.parse_args()
inputf = args.InputFile
forced = args.force

try:
  f = open(inputf,'r')
except IOError:
  print ("%s is not a valid file name. Please reenter." %inputf)


lines = f.readlines()
f.close()

src = None
dest = None
trial = 'default'
fnum = 0   #number of files to be copied/edited
flist = [] #list of files
fline = [] #list of line numbers associated with each file
iter_var = []  #list that will contain all iteration variables.
iter_file = []  #which file each variable belongs to
iter_name = []  #the name of each variable
prefix = []
numtry = 1     #number of trials to be generated
numvars = 0   #number of iter_vars
angUnit = 0   #angle unit used for sine and cosine sampling
mode = 0 #sampling mode (0 = grid, 1 = random (also need to set randsize for random))
randsize = 0   #size of random sample

for i in range(len(lines)):
  if lines[i].split() == []:
    pass  #nothing on this line
  elif lines[i].split()[0] == 'srcfolder':
    src = lines[i].split()[1]
    if '~' in src:
        src = os.path.expanduser(src)

  elif lines[i].split()[0] == 'destfolder':
    dest = lines[i].split()[1]
    if '~' in dest:
        dest = os.path.expanduser(dest)
    if os.path.isdir(dest) == True and forced == True:
        sub.run(['rm', '-rf', dest])
        if os.path.isfile(dest + '.hdf5') == True:
            sub.run(['rm', dest + '.hdf5'])
        if os.path.isfile('.'+ dest + '_hdf5') == True:
            sub.run(['rm', '.'+ dest + '_hdf5'])
        if os.path.isfile('.'+ dest) == True:
            sub.run(['rm', '.'+ dest])
    if os.path.isdir(dest) == True:
        reply = None
        question = "Destination Folder " + dest + " already exists. Would you like to override it? \nWARNING: This will delete " + dest + ", as well as any checkpoint files and HDF5 files if applicable."
        while reply not in ('y','n'):
            reply = str(input(question+' (y/n): ')).lower().strip()
            if reply[:1] == 'y':
                sub.run(['rm', '-rf', dest])
                if os.path.isfile(dest + '.hdf5') == True:
                    sub.run(['rm', dest + '.hdf5'])
                if os.path.isfile('.'+ dest + '_hdf5') == True:
                    sub.run(['rm', '.'+ dest + '_hdf5'])
                if os.path.isfile('.'+ dest) == True:
                    sub.run(['rm', '.'+ dest])
            if reply[:1] == 'n':
                exit()


  elif lines[i].split()[0] == 'trialname':
    trial = lines[i].split()[1]
  elif lines[i].split()[0] == 'samplemode':
    modename = lines[i].split()[1]
    if modename.startswith('g') or modename.startswith('G'):
      mode = 0
    elif modename.startswith('r') or modename.startswith('R'):
      mode = 1
    else:
      raise IOError("samplemode must be grid or random")
  elif lines[i].split()[0] == 'seed':
    if np.float(lines[i].split()[1]).is_integer():
      np.random.seed(np.int(lines[i].split()[1]))
    else:
      raise IOError("Attempt to pass non-integer value to seed")
  elif lines[i].split()[0] == 'file':
    flist.append(lines[i].split()[1])
    fline.append(i)
  elif lines[i].split()[0] == 'sUnitAngle':
    angUnit = lines[i].split()[1]
  elif lines[i].split()[0] == 'randsize':
    if np.float(lines[i].split()[1]).is_integer():
      randsize = np.int(lines[i].split()[1])
    else:
      raise IOError("Attempt to pass non-integer value to randsize")

for i in range(len(lines)):
  if lines[i].split() == []:
    pass  #nothing on this line
  elif lines[i].split()[0] == 'file':
    fnum += 1

  if re.search('\[',lines[i]) != None:
    spl = re.split('[\[\]]',lines[i])
    name = lines[i].split()[0]
    values = spl[1].split(',')
    if len(spl) == 3:
      prefix.append(spl[2].strip())
    else:
      raise IOError("Please provide a short prefix identifying each parameter to be iterated (to be used in directory names): <option> [<range>] <prefix>. Prefix is missing for '%s' for '%s'"%(name,flist[fnum-1]))
    if mode == 0:
      if len(values) != 3:
        raise IOError("Attempt to iterate over '%s' for '%s', but incorrect number of values provided. Syntax should be [<low>, <high>, <spacing>], [<low>, <high>, n<number of points>], or [<low>, <high>, l<number of points>] (log spacing) "%(name,flist[fnum-1]))

    for j in range(len(values)):
      values[j] = values[j].strip()  #remove any leading white spaces

    if mode == 1:
      if len(values) != 3:
        if values[2][0] == 'g':
          if len(values) >= 4 and len(values) < 6:
            if values[3][:3] == 'min':
              min_cutoff = np.float(values[3][3:])
            elif values[3][:3] == 'max':
              max_cutoff = np.float(values[3][3:])
            else:
              raise IOError("Incorrect syntax in Gaussian/normal distribution cutoff for '%s' for '%s'. Correct syntax is [<center>, <width>, g, min<value>], [<center>, <width>, g, max<value>],[<center>, <width>, g, min<value>, max<value>], or [<center>, <width>, g, max<value>, min<value>]"%(name,flist[fnum-1]))
          if len(values) == 5:
            if values[4][:3] == 'min':
              min_cutoff = np.float(values[4][3:])
            elif values[4][:3] == 'max':
              max_cutoff = np.float(values[4][3:])
            else:
              raise IOError("Incorrect syntax in Gaussian/normal distribution cutoff for '%s' for '%s'. Correct syntax is [<center>,<width>,g,min<value>], [<center>,<width>,g,max<value>],[<center>,<width>,g,min<value>,max<value>], or [<center>,<width>,g,max<value>,min<value>]"%(name,flist[fnum-1]))
          if len(values) >= 6:
             raise IOError("Incorrect syntax in Gaussian/normal distribution cutoff for '%s' for '%s'. Correct syntax is [<center>,<width>,g,min<value>], [<center>,<width>,g,max<value>],[<center>,<width>,g,min<value>,max<value>], or [<center>,<width>,g,max<value>,min<value>]"%(name,flist[fnum-1]))
        else:
          raise IOError("Attempt to draw from uniform distributions of '%s' for '%s', but incorrect number of values provided. Syntax should be [<low>,<high>,<type of distribution>], where valid types of distributions are u (uniform), s (uniform in sine), or c (uniform in cosine)."%(name,flist[fnum-1]))

    #user set linear spacing of data
    if values[2][0] == 'n':
      values[2] = values[2][1:]
      if np.float(values[2]).is_integer():
        array = np.linspace(np.float(values[0]),np.float(values[1]),int(values[2]))
      else:
        raise IOError("Attempt to iterate over '%s' for '%s', but number of points provided not an integer value"%(name,flist[fnum-1]))

    #user set log spacing of data
    elif values[2][0] == 'l':
      values[2] = values[2][1:]
      if np.float(values[0]) < 0:
        if np.float(values[2]).is_integer():
          array = -np.logspace(np.log10(-np.float(values[0])),np.log10(-np.float(values[1])),int(values[2]))
        else:
          raise IOError("Attempt to iterate over '%s' for '%s', but number of points provided not an integer value"%(name,flist[fnum-1]))
      else:
        if np.float(values[2]).is_integer():
          array = np.logspace(np.log10(np.float(values[0])),np.log10(np.float(values[1])),int(values[2]))
        else:
          raise IOError("Attempt to iterate over '%s' for '%s', but number of points provided not an integer value"%(name,flist[fnum-1]))

    #user wants to randomly sample a normal/gaussian distribution
    elif values[2][0] == 'g':
      if mode == 1:
        array = np.random.normal(loc=np.float(values[0]),scale=np.float(values[1]),size=np.int(randsize))
        if 'min_cutoff' in vars() and 'max_cutoff' not in vars():
          for ll in np.arange(len(array)):
            while array[ll] < min_cutoff:
              array[ll] = np.random.normal(loc=np.float(values[0]),scale=np.float(values[1]),size=1)
        elif 'min_cutoff' not in vars() and 'max_cutoff' in vars():
          for ll in np.arange(len(array)):
            while array[ll] > max_cutoff:
              array[ll] = np.random.normal(loc=np.float(values[0]),scale=np.float(values[1]),size=1)
        elif 'min_cutoff' in vars() and 'max_cutoff' in vars():
          for ll in np.arange(len(array)):
            while array[ll] < min_cutoff or array[ll] > max_cutoff:
              array[ll] = np.random.normal(loc=np.float(values[0]),scale=np.float(values[1]),size=1)
      else:
        raise IOError("Attempt to draw from a random distribution in grid mode for '%s' for '%s'"%(name,flist[fnum-1]))

    #user wants to randomly sample a uniform distribution
    elif values[2][0] == 'u':
      if mode == 1:
        array = np.random.uniform(low=np.float(values[0]),high=np.float(values[1]),size=np.int(randsize))
      else:
        raise IOError("Attempt to draw from a random distribution in grid mode for '%s' for '%s'"%(name,flist[fnum-1]))

    #user wants to randomly sample a uniform distribution of the SINE of an angle
    elif values[2][0] == 's':
      if mode == 1:
        if angUnit == 0:
          angUnit = SearchAngleUnit(src,flist)
        if angUnit.startswith('d') or angUnit.startswith('D'):
          array = np.arcsin(np.random.uniform(low=np.sin(np.float(values[0])*np.pi/180.0),high=np.sin(np.float(values[1])*np.pi/180.0),size=np.int(randsize)))*180/np.pi
        elif angUnit.startswith('r') or angUnit.startswith('R'):
          array = np.arcsin(np.random.uniform(low=np.sin(np.float(values[0])),high=np.sin(np.float(values[1])),size=np.int(randsize)))
        else:
          raise IOError("Cannot randomly sample sin(%s): improper angle units set"%name)
      else:
        raise IOError("Attempt to draw from a random distribution in grid mode for '%s' for '%s'"%(name,flist[fnum-1]))

    #user wants to randomly sample a uniform distribution of the COSINE of an angle
    elif values[2][0] == 'c':
      if mode == 1:
        if angUnit == 0:
          angUnit = SearchAngleUnit(src,flist)
        if angUnit.startswith('d') or angUnit.startswith('D'):
          array = np.arccos(np.random.uniform(low=np.cos(np.float(values[0])*np.pi/180.0),high=np.cos(np.float(values[1])*np.pi/180.0),size=np.int(randsize)))*180/np.pi
        elif angUnit.startswith('r') or angUnit.startswith('R'):
          array = np.arccos(np.random.uniform(low=np.cos(np.float(values[0])),high=np.cos(np.float(values[1])),size=np.int(randsize)))
        else:
          raise IOError("Cannot randomly sample cos(%s): improper angle units set"%name)
      else:
        raise IOError("Attempt to draw from a random distribution in grid mode for '%s' for '%s'"%(name,flist[fnum-1]))

    #user set the spacing size of data
    else:
      if np.float(values[0]) > np.float(values[1]) and np.float(values[2]) > 0:
        raise IOError("Attempt to iterate over '%s' for '%s', but start value > end value and spacing is positive"%(name,flist[fnum-1]))
      else:
        array = np.arange(np.float(values[0]),np.float(values[1])+np.float(values[2]),np.float(values[2]))

    #add to iterables
    iter_var.append(array)
    iter_file.append(fnum-1)
    iter_name.append(name)
    numtry *= len(array)
    numvars += 1

fline.append(i+1)
#error handling
if src is None:
  raise IOError("Name of source folder not provided in file '%s'. Use syntax 'srcfolder <foldername>'"%inputf)
if dest is None:
  raise IOError("Name of destination folder not provided in file '%s'. Use syntax 'destfolder <foldername>'"%inputf)
if flist == []:
  raise IOError("No files-to-be-copied provided in file '%s'. Use syntax 'file <filename>'")

if mode == 1:
  if randsize == 0:
    raise IOError("Must set randsize > 0 for random sampling mode")

if not os.path.exists(src):
  raise IOError("Source folder '%s' does not exist"%src)

if re.search('\/',dest) is not None:
  dest_parent = '/'.join(dest.split("/")[0:-1])
  if not os.path.exists(dest_parent):
    raise IOError("Destination's parent folder '%s' does not exist"%dest_parent)
if not os.path.exists(dest):
  os.system('mkdir '+dest)

if numvars == 0:
  for i in range(fnum):
    #read in file to be copied
    try:
      fIn = open(os.path.join(src,flist[i]),'r')
    except IOError:
      print("%s is not a valid file name. Please reenter." % (os.path.join(src,flist[i])))

    #find the lines in 'inputf' that correspond to this file
    slines = lines[fline[i]+1:fline[i+1]]
    slines = [slines[j] for j in range(len(slines)) if slines[j].split() != []]
    sflag = np.zeros(len(slines)) - 1  #bookkeeping (has option been found in file?)
    spref = [slines[j].split()[0] for j in range(len(slines))] #option name
    dlines = fIn.readlines()
    fIn.close()

    #create file out
    fOut = open(os.path.join(dest,flist[i]),'w')

    for j in range(len(dlines)):
      for k in range(len(spref)):
        if dlines[j].split() != []:
          if dlines[j].split()[0] == spref[k]:
            sflag[k] = 1   #option in file to be copied matched with option from inputf
            dlines[j] = slines[k]
            if dlines[j][-1] != '\n' and j < (len(dlines)-1):
             dlines[j] = dlines[j]+'\n'  #add a newline, just in case
          elif dlines[j].split()[0] == 'rm':
            #remove an option by placing a comment!
            if dlines[j].split()[1] == spref[k]:
              dlines[j] = '#'+dlines[j]
              sflag[k] = 1

      fOut.write(dlines[j])  #write to the copied file

    for k in range(len(spref)):
      #check if any options were not already present in the copied file, then write them
      if sflag[k] < 0:
        fOut.write('\n'+slines[k])

    fOut.close()

elif numvars >= 1:
  count = 0  #suffix of directory to be generated
  iter_file = np.array(iter_file)  #convert to numpy array for useful methods
  iter_name = np.array(iter_name)
  iterables0 = [x for x in iter_var]  #create list of iterables

  if mode == 0:
    #iterate over all possible combinations
    histf = open(dest+'/grid_list.dat','w')
    for tup in it.product(*iterables0):
      if count == 0:
        header = 'trial '

      current_line = ''
      destfull = os.path.join(dest,trial)  #create directory for this combination
      for ii in range(len(tup)):
        n = len(str(len(iter_var[ii])-1)) #number of digits to pad directory index
        index0 = np.where(iter_var[ii]==tup[ii])[0]
        destfull += prefix[ii]+np.str(index0[0]).zfill(n)
        if count == 0:
          header += flist[iter_file[ii]][:-3]+'/'+iter_name[ii]+' '
        current_line += prefix[ii]+np.str(index0[0]).zfill(n)
        if ii != len(tup)-1:
          destfull += '_'
          current_line += '_'

      for ii in range(len(tup)):
        current_line += ' '+'%f'%tup[ii]

      if count == 0:
        histf.write(header+'\n')
      histf.write(current_line+'\n')

      if not os.path.exists(destfull):
         os.system('mkdir '+destfull)

      for i in range(fnum):
        #read in file to be copied
        try:
          fIn = open(os.path.join(src,flist[i]),'r')
        except IOError:
          print("%s is not a valid file name. Please reenter." % (os.path.join(src,flist[i])))

        #find the lines in 'inputf' that correspond to this file
        slines = lines[fline[i]+1:fline[i+1]]
        slines = [slines[j] for j in range(len(slines)) if slines[j].split() != []]
        sflag = np.zeros(len(slines)) - 1   #bookkeeping (has option been found in file?)
        spref = [slines[j].split()[0] for j in range(len(slines))]   #option name
        dlines = fIn.readlines()
        fIn.close()

        #create file out
        fOut = open(os.path.join(destfull,flist[i]),'w')

        for j in range(len(dlines)):
          for k in range(len(spref)):
            if dlines[j].split() != []:
              if dlines[j].split()[0] == spref[k]:
                sflag[k] = 1   #option in file to be copied matched with option from inputf
                dlines[j] = slines[k]
                for m in range(len(iter_file)):
                  if iter_file[m] == i and iter_name[m] == dlines[j].split()[0]:
                    dlines[j] = dlines[j].split()[0] + ' ' +str(tup[m])
                if dlines[j][-1] != '\n' and j < (len(dlines)-1):
                  dlines[j] = dlines[j]+'\n'  #add a newline, just in case
              elif slines[k].split()[0] == 'rm':
                #remove an option by placing a comment!
                if dlines[j].split()[0] == slines[k].split()[1]:
                  dlines[j] = '#'+dlines[j]
                  sflag[k] = 1

          fOut.write(dlines[j])  #write to the copied file

        for k in range(len(spref)):
          #check if any were not already present in the copied file, then write them
          if sflag[k] < 0:
            if slines[k].split()[0] == 'rm':
              raise IOError("No option '%s' to be removed in file %s." % (slines[k].split()[1],flist[i]))
            else:
              tmp = slines[k]
              for m in range(len(iter_file)):
                if iter_file[m] == i and iter_name[m] == slines[k].split()[0]:
                  tmp = slines[k].split()[0] + ' ' +str(tup[m])
              if tmp[-1] != '\n':
                tmp = tmp+'\n'
              fOut.write('\n'+tmp)

      fOut.close()
      count += 1    #move to next combination
    histf.close()

  else:
    #random draw, iterate linearly
    n = len(str(randsize-1)) #number of digits to pad directory index
    histf = open(dest+'/rand_list.dat','w')
    for count in np.arange(randsize):
      tup = []
      if count == 0:
        header = 'trial '
      current_line = 'rand_'+np.str(count).zfill(n)+' '
      for ii in np.arange(len(iterables0)):
        tup.append(iterables0[ii][count])
        if count == 0:
          header += flist[iter_file[ii]][:-3]+'/'+iter_name[ii]+' '
        current_line += '%f'%iterables0[ii][count]+' '

      if count == 0:
        histf.write(header+'\n')
      histf.write(current_line+'\n')

      destfull = os.path.join(dest,trial)  #create directory for this combination
      destfull += 'rand_'+np.str(count).zfill(n)
      if not os.path.exists(destfull):
        os.system('mkdir '+destfull)

      for i in range(fnum):
        #read in file to be copied
        try:
          fIn = open(os.path.join(src,flist[i]),'r')
        except IOError:
          print("%s is not a valid file name. Please reenter." % (os.path.join(src,flist[i])))

        #find the lines in 'inputf' that correspond to this file
        slines = lines[fline[i]+1:fline[i+1]]
        slines = [slines[j] for j in range(len(slines)) if slines[j].split() != []]
        sflag = np.zeros(len(slines)) - 1   #bookkeeping (has option been found in file?)
        spref = [slines[j].split()[0] for j in range(len(slines))]   #option name
        dlines = fIn.readlines()
        fIn.close()

        #create file out
        fOut = open(os.path.join(destfull,flist[i]),'w')

        for j in range(len(dlines)):
          for k in range(len(spref)):
            if dlines[j].split() != []:
              if dlines[j].split()[0] == spref[k]:
                sflag[k] = 1   #option in file to be copied matched with option from inputf
                dlines[j] = slines[k]
                for m in range(len(iter_file)):
                  if iter_file[m] == i and iter_name[m] == dlines[j].split()[0]:
                    dlines[j] = dlines[j].split()[0] + ' ' +str(tup[m])
                if dlines[j][-1] != '\n' and j < (len(dlines)-1):
                  dlines[j] = dlines[j]+'\n'  #add a newline, just in case
              elif slines[k].split()[0] == 'rm':
                #remove an option by placing a comment!
                if dlines[j].split()[0] == slines[k].split()[1]:
                  dlines[j] = '#'+dlines[j]
                  sflag[k] = 1

          fOut.write(dlines[j])  #write to the copied file

        for k in range(len(spref)):
          #check if any were not already present in the copied file, then write them
          if sflag[k] < 0:
            if slines[k].split()[0] == 'rm':
              raise IOError("No option '%s' to be removed in file %s." % (slines[k].split()[1],flist[i]))
            else:
              fOut.write('\n'+slines[k])

      fOut.close()

    histf.close()

    for ii in np.arange(len(iterables0)):
      plt.figure(figsize=(8,8))
      plt.hist(iterables0[ii],histtype='stepfilled',color='0.5',edgecolor='None')
      plt.xlabel(iter_name[ii])
      plt.ylabel('Number of trials')
      plt.savefig(dest+'/hist_'+flist[iter_file[ii]][:-3]+'_'+iter_name[ii]+'.pdf')
      plt.close()

# Just do this block if you want to
if(False):
    # Now that all the simulation directories have been populated,
    # Make the submission scripts for hyak
    # Parse input file

    # TODO: allow the input file to include flags to set default things for
    # the .pbs script and for whether or not to run this section

    # Parallel or parallel_sql? Always use parallel_sql!
    para = "parallel_sql"

    destfolder, trialname, infiles, src = vspace_hyak.parseInput(infile=inputf)

    # Make command list and .sh files to run the scripts
    vspace_hyak.makeCommandList(simdir=destfolder,infile=inputf,para=para)

    # Make the submission script
    vspace_hyak.makeHyakVPlanetPBS(script="run_vplanet.pbs",taskargs="vplArgs.txt",
                           walltime="00:30:00",para=para,
                           simdir=destfolder,logdir=destfolder)
