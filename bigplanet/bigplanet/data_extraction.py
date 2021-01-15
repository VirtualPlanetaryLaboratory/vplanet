"""
David Fleming (dflemin3) Spring 2016

Functions for processing results of VPLANET simulations from an ensemble of
simulations

"""

from __future__ import (absolute_import, division, print_function,
                        unicode_literals)
import numpy as np
import os
import sys
import pandas as pd
import h5py
import pickle
import multiprocessing as mlt

# Tell module what it's allowed to import
__all__ = ["reduce_dimensions",
           "aggregate_data",
           "Dataset",
           "extract_data_hdf5",
           "data_from_dir_hdf5",
           "add_column"]


def find_hdf5_files(path):
    """
    Find all the *.hdf5 datasets in a directory.  If there's multiple, return
    list of paths to each one.  If there's one, return just path to lonely
    dataset.  Returns None if none are found.  It also let's you know if there
    are multiple because len/iterating on strings and lists can be a pain.

    Parameters
    ----------
    path : str
        path to dataset to look in

    Returns
    -------
    bool : bool
        Whether or not there are multiple hdf5 files
    dataset : str or list
        path(s) to *.hdf5 dataset(s) in the directory
    """

    # Since this is something like /path/to/dataset/dir/dataset_name
    # I just want /path/to/dataset/dir
    path = path.split("/")[:-1]
    path = os.path.join('/', *path)

    ret = []
    for f in os.listdir(path):
        if f.endswith(".hdf5"):
            ret.append(os.path.join(path,f))

    # None found!
    if len(ret) == 0:
        return False, None
    # Found one!
    elif len(ret) == 1:
        return False, ret[0]
    # Found a bunch!
    else:
        return True, ret
# end function


def find_logfile(direct):
    """
    Find path to logfile in given directory.

    Parameters
    ----------
    direct : str
        path to directory where logfile lives

    Returns
    -------
    logfile : str
        complete path to direct's logfile, if it exists
    """

    logcount = 0
    for f in os.listdir(direct):
        if f.endswith(".log"):
            logfile = os.path.join(direct,f)
            logcount = logcount + 1

    # Ensure there's just one logfile
    assert logcount == 1, "ERROR: There can only be one (log)! Number of logs: %d" % logcount

    return logfile
# end function


def parse_log(logfile, body, param):
    """"
    Parse log file for one of a body's initial conditions and return it.  This
    is useful for when a parameter is not outputted but one needs it later on
    for bigplanet analysis.

    Parameters
    ----------
    logfile : str
        path to the logfile
    body : str
        name of body whose variable you're looking for
    param : str
        body's parameter

    Returns
    -------
    param : float
        initial condition of interest
    """

    # Open log file and get lines
    with open(logfile) as handle:
        lines = handle.readlines()

    # Loop over all lines in the input file looking for body string of the form
    # ----- BODY: planet ----
    to_find_body = "BODY: %s" % body
    for ii in range(0,len(lines)):

        line = lines[ii].strip(' \t\n\r') # Remove whitespace

        # Found it? If so, now loop over lines below it looking for param
        # of the form (param)
        if line.find(to_find_body) != -1:
            to_find_param = "(%s)" % param
            found_it = False
            while not found_it:

                # At the end of the file?
                if ii == len(lines) - 1:
                    raise RuntimeError("Couldn't find %s in %s." % (param, logfile))

                line = lines[ii].strip(' \t\n\r') # Remove whitespace

                # Found it? Done!
                if line.startswith(to_find_param):
                    words = line.split()
                    return float(words[-1])

                # On to the next line
                ii = ii + 1

    # Didn't find it
    raise RuntimeError("Couldn't find %s in %s." % (param, logfile))
# end function


def parse_infile(infile, param):
    """"
    Parse input file for one of a body's initial conditions and return it.  This
    is useful for when a parameter is not outputted but one needs it later on
    for analysis.

    Parameters
    ----------
    infile : str
        path to the input file.  Something like body.in
    param : str
        body's parameter

    Returns
    -------
    param : float
        initial condition of interest
    """

    # Open log file and get lines
    with open(infile) as handle:
        lines = handle.readlines()

    # Loop over line, look for parameter
    for ii in range(0,len(lines)):

        # Remove whitespace
        line = lines[ii].strip(' \t\n\r')

        # Found it? Done!
        # Correct line, if it exists, will look something like this:
        # dParameter some_number # comments
        if line.startswith(param):
            words = line.split()
            return float(words[1])

    # Didn't find it, so why did you tell me to look for it?
    raise RuntimeError("Couldn't find %s in %s." % (param, infile))
# end function


def get_cols(datadir=".",infiles=None):
    """
    Given a directory where simulation data are stored and the name of the input files,
    return the name of each body's output columns

    Parameters
    ----------
    datadir : str
        Name of directory where simulation results are kept
    infiles : list
        list containing input file names for each body.
        Compute using get_infiles function

    Returns
    -------
    data_cols : dict
        dictionary containing output variable names for each body
        like the following: data_cols["body0"] = ["Time","Radius",...]
    """

    if infiles is None:
        raise IOError("infiles is None!  Must be list of input files.")

    # Dict to hold all columns for each body [like time, semi, ecc ...]
    data_cols = {}

    # Loop over files corresponding to each body
    for infile in infiles:
        with open(os.path.join(datadir,infile)) as f:
            lines = f.readlines()

            # Loop over all lines in the input file
            for ii in range(0,len(lines)):
                line = lines[ii]
                line = str(line).strip(' \t\n\r') # Remove all kinds of whitespace from sides

                # Is this the saOutputOrder line and it isn't commented out?
                if line.find("saOutputOrder") != -1 and line[0] != "#":
                    cols = line.split()[1:] # ignore saOutputOrder

                    # Add all lines below it that have a "$", the line continuation character
                    while("$" in str(lines[ii]).strip(' \t\n\r')):

                        # Move to next line
                        ii = ii + 1

                        cols = cols + str(lines[ii]).strip(' \t\n\r').split()

                    # Remove any - if there are any
                    # Also ignore commented out (#) stuff
                    good_cols = []
                    for jj in range(0,len(cols)):
                        if "#" in cols[jj]: # First time this goes, rest of stuff is ignored
                            break

                        # Get rid of - sign if it's there
                        cols[jj] = cols[jj].replace("-", "")

                        # Column name is good and processed, so add it
                        good_cols.append(cols[jj])

                        # Get rid of $ sign if it's there
                        if "$" in good_cols:
                            good_cols.remove("$")

                    # Save the columns, break out of this infile!
                    data_cols[infile] = good_cols
                    break

    return data_cols
# end function


def get_dirs(src,order="none"):
    """
    Given a path to where simulation directories live,
    create a list of directories from which simulation
    data will be extracted in a user-specified order.

    Parameters
    ----------

    src : str
        Path to simulation suite directory which contains all simulation sub directories
    order : str
        How user wants dataset ordered.  Defaults to "None" which means
        the code loads in the data in whatever order the simulation dirs
        are in.  Options: "none", "grid"

    Returns
    -------
    dirs : list
        List of simulation directories ordered by order
    """

    # No order, return via listdir which does NOT preserve order
    print("Finding simulation subdirectories in %s ordered by %s." % \
         (src, order))
    sys.stdout.flush()

    # No order
    if order.lower() == "none":
        dirs = list(filter(lambda x: os.path.isdir(os.path.join(src, x)), os.listdir(src)))
    # Grid order.  Preserves vspace-given order for a grid of simulations
    elif order.lower() == "grid":
        dirs = list(sorted(filter(lambda x: os.path.isdir(os.path.join(src, x)), os.listdir(src))))
    # Not a valid option!
    else:
        raise ValueError("Invalid order: %s." % order)

    return dirs
#end function


def get_infiles(datadir="."):
    """
    Given a directory where simulation data are stored, get the input files
    names (like body0.in, body1.in, ...)

    Parameters
    ----------
    datadir : str
        Name of directory where simulation results are kept

    Returns
    -------
    infiles : list
        list containing input file names for each body
    """

    infiles = []

    # Find all input files: end in .in but aren't vpl.in
    for f in os.listdir(datadir):
        if f.endswith(".in") and f != "vpl.in":
            infiles.append(f)

    return infiles
# end function


def halt_check(direct,TOL=1.0e-6):
    """
    Parse simulation's logfile to check if the simulation ran to completion.  If the simulation halted
    for some reason, find the halt code (not implemented in VPLANET yet) and return to user.

    Parameters
    ----------
    direct : str
        Path to simulation directory
    TOL : float (optional)
        Tolerance for float comparisons (for stop time to actual stop time comparison)

    Returns
    -------
    halt : int
        Indicates whether or not sim completed.  0 == sim completed, halt != 0 is the halt code
        and indicates the simulation ended before reaching the stop time.  Defaults to 1.

    Example
    -------
    >> direct = "/Users/dflemin3/Desktop/vplanet/examples/binary_test/"
    >> halt_check(direct)
       0 # Simulation ran to completion!
    """

    # Find the log file
    logcount = 0
    for f in os.listdir(direct):
        if f.endswith(".log"):
            logfile = os.path.join(direct,f)
            logcount = logcount + 1

    # Ensure there aren't two+ or 0 logfiles for whatever reason
    assert logcount == 1, "ERROR: There can only be one (log)! Number of logs: %d" % logcount

    # Parse logfile
    stoptime = -1.0
    lasttime = 0.0
    halt = 0
    with open(logfile) as f:
        content = f.readlines()

        for line in content:
            line = line.strip() # remove whitespace

            # Parse for stop time
            # Line looks like: Stop Time: 3.155760e+09
            if line.startswith("Stop Time:"):
                stoptime = float(line.split()[-1])
                continue

            # Parse for actual sim ending time
            # Line looks like: (Time) Simulation Time [sec] 3.155760e+09
            # Note: code finds this twice with 2nd being final system age
            if line.startswith("(Time) Simulation Time"):
                lasttime = float(line.split()[-1])
                continue

            # TODO: once implemented in VPLANET, search for halt code

    # Logfile parsed, see if it finished
    if np.fabs(lasttime - stoptime) < TOL and halt == 0:
        return halt
    # Didn't finish
    else:
        return 1
# end function


# Parallel stuff here


#############################################################################
#
# Below are data extraction functions that work with the hdf5 data format
#
# This is the preferred option!
#
#############################################################################

class Dataset(object):
    """
    Abstraction class for working with hdf5 datasets using the h5py
    python wrapper package for VPLANET data.

    TODO: better docs for this class

    """

    def __init__(self,data=None,size=0,order="none",number_to_sim=None,
                 count_map=None,input_files=None,data_cols=None):
        self.data = data # Path(s) to hdf5 dataset(s)
        self.size = size # Length of data (number of groups from root)
        self.order = order # How the simulations are ordered
                            # Defaults to None aka ordered as they were found
        self.input_files = input_files # List of names of input files
        self.data_cols = data_cols # Dict of parameters for each body

        # Map sim number to correct hdf5 file when many exist
        # Set to None when using only 1 hdf5 file
        self.__count_map = count_map

        # Translates # to sim name
        self.__number_to_sim = number_to_sim

        # Check to see if data was extracted in parallel, i.e. data is list
        # of hdf5 files.  Sets up the file names properly
        #if hasattr(self.data, "__len__") and (not isinstance(self.data, str)) and (not isinstance(self.data, unicode)):
        #    for ii in range(len(data)):
        #        self.data[ii] = self.data[ii] + ".hdf5"
        #else:
        #    self.data = data + ".hdf5"
    # end function


    def get(self, simulation, body, variables, dtype=np.float64):
        """
        Wrapper for get_data_hdf5 function (see its docs).  This is a workhorse
        data accessing function.

        Parameters
        ----------
        simulation : int
            Integer identifier
        body : string
            Name of the body
        variables : string or list of string
            Name(s) of body's parameters you wish to access
        dtype : numpy datatype (optional)
            Datatype of data to retrieve.  99.999% of the time, it's a float64
        parallel : bool (optional)
            whether or not there are multiple hdf5 files to look through.
            Defaults to False

        Returns
        -------
        res : array

        """

        # Enforce array bounds for less-cryptic error message
        if simulation >= 0 and simulation < self.size:
            return self.__get_data_hdf5(simulation, body, variables,
                                        dtype=dtype, count_map=self.__count_map)
        else:
            raise ValueError("Invalid simulation number: %d. Dataset size: %d" %
                  (simulation,self.size))
    # end function


    def __get_data_hdf5(self, simulation, body, variables, dtype=np.float64,
                        count_map=None):
        """
        Given the simulation number, body and the variable, access and return the data.  For example,
        get_data(1,"secondary","Eccentricity") will return a numpy array containing the secondary's
        eccentricity vector.  A call like get_data(1,"secondary",["Time","Eccentricity"]) will return
        a 2D numpy array whose columns are Time and Eccentricity.

        Parameters
        ----------
        simulation : int
            Number corresponding to a given simulation
        body : str
            Name of the body whose data you wish to access
        variables : str or iterable
            Name(s) of the dataset to return for given body
        dtype : datatype (optional)
            Type of data to return.  Defaults to np.float64
        count_map : list (optional)
            mapping between simulation number and hdf5 file when using many hdf5
            files.  Defaults to None.

        Returns
        -------
        data : numpy array
            Requested dataset as a numpy array
        """

        # If many dataset files exist, figure out which one data lives in
        if count_map is not None:
            num, file_num = self.__id_to_sim(simulation)
            data_file = self.data[file_num]
        # Only 1 hdf5 data file
        else:
            num = simulation
            data_file = self.data

        # Create dataset path (where data is located in hdf5 file)
        path = os.path.join(str(num),body)

        data = []
        # Open dataset, access data
        try:
            with h5py.File(data_file, "r") as hf:

                # Multiple variables (not just a string)
                if isinstance(variables, list):
                    for var in variables:
                        data.append(np.array(hf.get(path)[var]))
                # Only one variables (string!)
                else:
                    return np.array(hf.get(path)[variables]).squeeze()

            return np.asarray(data,dtype=dtype).squeeze()
        except RuntimeError:
            raise RuntimeError("Invalid get call.  Simulation, body, variable(s) : %s, %s, %s" %
                  (simulation,body,variables))
    # end function


    def sim_name(self,sim):
        """
        Given sim #, return the name.  Name is the name of the directory.

        Parameters
        ----------
        sim : int
            Integer identifier

        Returns
        -------
        name : string
            Name of the simulation directory
        """

        # Enforce array bounds for less-cryptic error message
        if sim >= 0 and sim < self.size and self.__number_to_sim is not None:
            return self.__number_to_sim[sim]
        else:
            raise ValueError("Invalid simulation number: %d. Dataset size: %d" %
                  (sim,self.size))
        # end function


    def __id_to_sim(self,num):
        """
        Map simulation number to the proper sim location in some hdf5 file.

        Parameters
        ----------
        num : int
            simulation number user wishes to access

        Returns
        -------
        file_num : int
            which hdf5 file to access
        count : int
            which sim in that hdf5 file to access
        """

        # If only 1 hdf5 file, no need to map
        if self.__count_map is None:
            return num, 0

        # Figure out which file sim is in, return it and the index for that
        # file

        # In 1st file
        if num < self.__count_map[0]:
            return num, 0

        # Which file does it live in?
        mapper = np.cumsum(self.__count_map)
        for ii in range(1,len(self.__count_map)):
            if num < self.__count_map[ii]:
                return num - self.__count_map[ii-1], ii
            else:
                continue
    # end function


    def __repr__(self):
        """
        For pretty prints.
        """
        return "Name: %s. Size: %d. Order: %s" % (self.data,self.size,self.order)
# end class


def data_from_dir_hdf5(f_set, grpname, datadir=".",data_cols=None,infiles=None,
                       compression="gzip",remove_halts=True, var_from_log=None,
                       var_from_infile=None):
    """
    Given a directory where simulation data are stored, the name of each body's output
    columns and the name of the input files, pull the data!

    Parameters
    ----------
    f_set : HDF5 group
        root dir-level hdf5 group
    grpname : string
        Name of the hdf5 dataset group that will contain directory's simulation
        data.  Typically named after sim #
    datadir : str
        Name of directory where simulation results are kept.  Defaults to .
    data_cols : dict
        dictionary contains each body's output variable names. Defaults to None.
    infiles : list
        list containing input file names for each body.  Defaults to None
    compression : str
        compression algorithm used to reduce dataset size.  Defaults to gzip.
        None (no quotes) turns off compression
    remove_halts : bool
        Whether or not to exclude simulations that did not run to completion
        Defaults to True
    var_from_log : dict
        Dict of bodies, parameters to extract from logfile.  Defaults to None.
        Something like dict = {"body1" : [var1, var2], "body2" : [var3, var4]}
    var_from_infile : dict
        Dict of input files, parameters to extract from input files.  Defaults
        to None. Looks like dict = {"body1" : [var1], "body2" : [var2]}

    Returns
    -------
    success : int (bool)
        1 if successful, 0 if not (i.e. halt, sim didn't finish, etc)
    """

    # data_cols and infiles must be given
    if data_cols is None or infiles is None:
        raise ValueError("data_cols and infiles must both be specified.")

    # If user wants to, look for halts/sims not finishing
    if remove_halts:
        halt = halt_check(datadir)
    else:
        halt = 0

    # Did it halt? ... assuming user cares
    if halt != 0:
        return 0

    # Find logfile for parsing later?
    if var_from_log is not None:
        logfile = find_logfile(datadir)

    # No halt, make the group!
    # Create group for each directory
    # Group's name == directory path
    grp = f_set.create_group(grpname)

    # Now loop over each output file to extract the data
    for infile in infiles:
        # Isolate the body name
        infile = infile.split(".")[0]

        # Loop over all files in dir, open the one that contains the body name
        for f in os.listdir(datadir):
            if f.endswith(".forward") and (f.find(infile) != -1):
                # For unexpected reasons, this *could* fail, so just chalk it up to
                # some crazy, unanticipated error.  Maybe vplanet output error?
                # This should never happen and if it does, who knows
                try:
                    # Read in data as pandas dataframe, use C engine for speed!
                    tmp = pd.read_table(os.path.join(datadir,f),
                                             header=None,
                                             delim_whitespace=True,
                                             engine="c",
                                             names=data_cols[infile + ".in"],
                                             dtype=np.float64,
                                             memory_map=True)

                    # Create corresponding hdf5 subgroup for body, store data as
                    # numpy array. Subgroup's name is body name
                    sub = grp.create_group(infile)
                    tmp_len = len(tmp[data_cols[infile + ".in"][0]]) # Store this for later

                    # Write columns to subgroup as datasets [allows for POSIX-like access]
                    for col in data_cols[infile + ".in"]:
                        data = pd.np.array(tmp[col])
                        tmp_dset = sub.create_dataset(col, data.shape,
                                                      dtype="f",
                                                      compression=compression)
                        tmp_dset.id.write(h5py.h5s.ALL, h5py.h5s.ALL, data)

                    # Pull variables from logfile?
                    if var_from_log is not None:
                        # Loop over body's, infile's, params if any are given
                        if infile in var_from_log.keys():
                            for param in var_from_log[infile]:
                                var = parse_log(logfile, infile, param)

                                # Create/write dataset
                                # Make var an array.  Wasteful, but whatever
                                var = np.ones(tmp_len) * var
                                tmp_dset = sub.create_dataset(param,
                                                              var.shape,
                                                              dtype="f",
                                                              compression=compression)
                                tmp_dset.id.write(h5py.h5s.ALL, h5py.h5s.ALL, var)

                    # Pull variables from input file?
                    if var_from_infile is not None:
                        # Loop over body's, infile's, params if any are given
                        if infile in var_from_infile.keys():
                            for param in var_from_infile[infile]:
                                # Give path for input file
                                infile_path = os.path.join(datadir,(infile+".in"))
                                var = parse_infile(infile_path, param)

                                # Create/write dataset
                                # Make var an array.  Wasteful, but whatever
                                var = np.ones(tmp_len) * var
                                tmp_dset = sub.create_dataset(param,
                                                              var.shape,
                                                              dtype="f",
                                                              compression=compression)
                                tmp_dset.id.write(h5py.h5s.ALL, h5py.h5s.ALL, var)


                except RuntimeError:
                    print("Did you format var_from_infile/log correctly?")
                    print("var_from_infile = {'body'} : [list, of, params], ...")
                    raise RuntimeError("Unknown pd.read_table error.")

    # Return 1 since 1 new sim was successfully processed/stored
    return 1
# End function


def batch_extraction(dirs, src, dset, counter=0, data_cols=None, infiles=None,
                     compression="gzip",remove_halts=True, var_from_log=None,
                     var_from_infile=None, cadence=None, order="none",core=None):
    """
    Extract the data from a batch of VPLANET simulation directories.
    Essentially, this calls data_from_dir_hdf5 len(dirs) times and stores the
    results in their own hdf5 files.

    Parameters
    ----------
    dirs : list
        list of paths to directories where VPLANET simulation results reside
    src : str
        Path to simulation suite directory which contains all simulation sub directories
    dset : string
        Name of the hdf5 dataset to create for this batch of simulations
    counter : int
        keeps track of how many directories this function parases.  Useful for
        determining total number of simulations
    data_cols : dict
        dictionary contains each body's output variable names. Defaults to None.
    infiles : list
        list containing input file names for each body.  Defaults to None
    compression : str
        compression algorithm used to reduce dataset size.  Defaults to gzip.
        None (no quotes) turns off compression
    remove_halts : bool
        Whether or not to exclude simulations that did not run to completion
        Defaults to True
    var_from_log : dict
        Dict of bodies, parameters to extract from logfile.  Defaults to None.
        Something like dict = {"body1" : [var1, var2], "body2" : [var3, var4]}
    var_from_infile : dict
        Dict of input files, parameters to extract from input files.  Defaults
        to None. Looks like dict = {"body1" : [var1], "body2" : [var2]}
    order : str
        How user wants dataset ordered.  Defaults to "none" which means
        the code loads in the data in whatever order the simulation dirs
        are in.  Currently, this does nothing for this function.
    core : int
        If processsing in parallel, keep track of core running this function
        for bookkeeping later on.  Defaults to None (aka serial process)

    Returns
    -------
    counter : int
        keeps track of how many directories this function parases.  Useful for
        determining total number of simulations
    number_to_sum : list
        dictionary to translate simulation number to simulation name
    """
    # Create new hdf5 file
    f_set = h5py.File(dset, "w")

    # List to store what dir sim # corresponds to
    number_to_sim = []

    # Loop over all directories and extract the simulation results
    for direct in dirs:

        # Store data from each simulation, increment counter if succesful
        res = data_from_dir_hdf5(f_set,str(counter),os.path.join(src,direct),
                                 data_cols,infiles,compression=compression,
                                 remove_halts=remove_halts,
                                 var_from_log=var_from_log,
                                 var_from_infile=var_from_infile)

        # Increment counter (keeps track of valid parsed simulation dirs)
        counter += res
        if res != 0:
            # store sim - counter mapping
            number_to_sim.append(direct)

        # Output progress to user?
        if cadence is not None and counter % int(cadence) == 0 and cadence > 0:
            print("Simulations processed so far: %d" % counter)
            sys.stdout.flush()

    # Done! Close file, return stuff
    f_set.close()

    if core is None:
        return counter, number_to_sim
    else:
        return core, counter, number_to_sim
# end function


def extract_data_hdf5(src=".", dataset="simulation", order="none",
                      compression="gzip", remove_halts=False, cadence=None,
                     skip_body=None, var_from_log=None, var_from_infile=None,
                     parallel=False):
    """
    Given the root directory path for a suite of simulations, pull all the data
    and store it in a hd5f database.  This allows for iteration/processing of data
    that is way too large to concurrently store in memory.

    Parameters
    ----------
    src : str
        Path to simulation suite directory which contains all simulation sub directories
    dataset : str
        Name (including path) of hdf5 dataset.  Don't include a file extension.
        If you do its ok, but I'll be disappointed in you. Note that src and
        dataset are in general different because the simulation results and the
        *.hdf5 files can live in different locations.
    order : str
        How user wants dataset ordered.  Defaults to "none" which means
        the code loads in the data in whatever order the simulation dirs
        are in
    compression : str
        compression algorithm used to reduce dataset size.  Defaults to gzip.
        None (no quotes) turns off compression. Options: "gzip", "lzf", None.
        If you're hurting for speed and have tons of storage space, set to None.
    remove_halts : bool
        Whether or not to exclude simulations that did not run to completion
    cadence : int (optional)
        Cadence at which function outputs what sim number it's on.
        Ex: cadence == 1,000 -> every 1,000 sims processed, function outputs sim number
    skip_body : list (optional)
        Bodies to not process.  Ex: Have a body, primary, who either has no output or
        the user doesn't care what its output is and hence does not want to save it.
        if skip_body = ["primary.in"], the body primary with input file primary.in will
        be ignored from all data processing
    var_from_log : dict
        Dict of bodies, parameters to extract from logfile.  Defaults to None.
        Something like dict = {"body1" : [var1, var2], "body2" : [var3, var4]}
    var_from_infile : dict
        Dict of input files, parameters to extract from input files.  Defaults
        to None. Looks like dict = {"body1" : [var1], "body2" : [var2]}
    parallel : bool
        whether or not to extract data in parallel,i.e. use all available cores.
        Defaults to False.

    Returns
    -------
    data : Dataset object
        See Dataset object docs
    """

    # Make sure dataset path is clean (no extensions)
    dataset = os.path.splitext(dataset)[0]

    # Find all *.hdf5 files in the directory, if they exist and if there are
    # more than one
    is_parallel, found_datasets = find_hdf5_files(dataset)

    # Only run this function if you can't find dataset(s) in the dataset dir
    if found_datasets is not None:

        # Dataset exists
        print("Hdf5 dataset already exists.  Reading from: %s" % dataset)
        print("Using metadata stored in dataset object.")
        sys.stdout.flush()

        # Load the metadata, return a dataset obj for data-wrangling
        return load_metadata(dataset)

    # No dataset exists: Make the dataset!

    # Create hdf5 dataset
    print("Creating hdf5 dataset %s:" % dataset)
    sys.stdout.flush()

    # Get list of all data directories in src to iterate over
    dirs = get_dirs(src,order=order)

    # If using parallel, need some additional setup
    if parallel:

        # Get number of cores
        N_CORES = mlt.cpu_count()

        # If not using more than one core, why are you using parallel?
        assert N_CORES > 1, "To run in parallel, must use at least 2 cores."

        # Partition dirs list into one list for each core preserving order
        dirs_gen = np.array_split(np.array(dirs),N_CORES)

    # Find out what the input files (like body.in) are based on the first
    # directory since they are assumed to be the same in all directories
    infiles = get_infiles(os.path.join(src,dirs[0]))

    # Skip any bodies (i.e. some bodies didn't have output you care about)?
    if skip_body is not None:
        for sbody in skip_body:
            if sbody in infiles:
                infiles.remove(sbody)
                print("Skipped %s." % sbody)
                sys.stdout.flush()

    print("Infiles:",infiles)
    sys.stdout.flush()

    # Get the names of the output variables for each body
    data_cols = get_cols(os.path.join(src,dirs[0]),infiles)
    print("Data Columns:",data_cols)
    sys.stdout.flush()

    # How should I process the data: In parallel or serially?
    if parallel:

        # Take the threads to the pool
        pool = mlt.Pool(processes=N_CORES)

        # Get base dataset name for naming smaller dataset chunks
        base_name = os.path.splitext(dataset)[0]

        # Make a list of dataset names for all hdf5 files
        dataset_names = []
        for ii in range(N_CORES):
            dataset_names.append(base_name+"_"+str(ii)+".hdf5")

        # Make list of counts to sync sim-number mapping
        #counts = [0,len(dirs_gen[0])]
        #for ii in range(2,N_CORES):
        #    counts.append(len(dirs_gen[ii-1])+counts[ii-1])

        # Map: Partition dirs out to corresponding cores, extract data
        results = [pool.apply_async(batch_extraction,
                                    (dirs_gen[ii], src, dataset_names[ii]),
                                    {"counter" : 0,
                                    "data_cols" : data_cols,
                                    "infiles" : infiles,
                                    "compression" : compression,
                                    "remove_halts" : remove_halts,
                                    "var_from_log" : var_from_log,
                                    "var_from_infile" : var_from_infile,
                                    "cadence" : cadence,
                                    "order" : order,
                                    "core" : ii})
                                    for ii in range(N_CORES)]

        # Reduce: Wrap all separate hdf5 files under one class

        # Get the results once they're done, sort by processor order
        results = [p.get() for p in results]
        results.sort()

        # Combine all counts, number_to_sims
        number_to_sim = []
        counts = []
        for res in results:
            counts.append(res[1])
            number_to_sim = number_to_sim + res[2]

        # Total number of parsed simulations
        counter = len(number_to_sim)

        # Make list to keep tracks of bounds of which sims live in which file
        count_map = np.cumsum(counts)

        dataset_obj = save_metadata(dataset, counter, order, number_to_sim,
                                    infiles, data_cols, var_from_log,
                                    var_from_infile, count_map)

        return dataset_obj

    # Use just one processor like some sort of peasant
    else:

        # Loop over directories (and hence simulations) and get the data
        # User counter for name of group/means to identify simulation
        counter = 0

        # Not parallel so no need to map
        count_map = None

        # Load data into hdf5 file, get how many there were (counter) and make
        # a dict to translate between sim number and name (number_to_sim)
        counter, number_to_sim = batch_extraction(dirs, src, (dataset+".hdf5"),
                                                  counter=counter,
                                                  data_cols=data_cols,
                                                  infiles=infiles,
                                                  compression=compression,
                                                  remove_halts=remove_halts,
                                                  var_from_log=var_from_log,
                                                  var_from_infile=var_from_infile,
                                                  cadence=cadence)

        # Store metadata into a handler object
        try:

            # Save metadata into hdf5 file, return dataset object
            dataset_obj = save_metadata(dataset, counter, order, number_to_sim,
                                        infiles, data_cols, var_from_log,
                                        var_from_infile, count_map)

        # This can fail sometimes... but it shouldn't
        except RuntimeError:
            raise RuntimeError("Unable to store metadata.  Shouldn't happen!")

        # Create Dataset class to return
        return dataset_obj
# End function


def save_metadata(dataset, counter, order, number_to_sim, infiles, data_cols,
                  var_from_log, var_from_infile, count_map):
    """
    Save simulation suite metadata into a hdf5 file.

    Parameters
    ----------
    dataset : str
        Name (including path) of hdf5 dataset.  Don't include a file extension.
    counter : int
        number of processed simulations in the suite
    order : str
        How user wants dataset ordered.  Defaults to "none" which means
        the code loads in the data in whatever order the simulation dirs
        are in
    number_to_sim : list
        list of mapping between number (index) and simuatin name
    infiles : list
        list of the names of simulation input files
    data_cols : dict
        dict of bodies and their respective parameters which are simulation
        outputs
    var_from_log : dict
        Dict of bodies, parameters to extract from logfile.
        Something like dict = {"body1" : [var1, var2], "body2" : [var3, var4]}
    var_from_infile : dict
        Dict of input files, parameters to extract from input files. Looks like
        dict = {"body1" : [var1], "body2" : [var2]}
    count_map : list
        mapping between simulation number and hdf5 file when using many hdf5
        files

    Returns
    -------
    data : Dataset object
        See Dataset object docs
    """

    # TODO
    # Handle when the input isn't provided
    # Probs should just yell at user

    is_parallel, datasets = find_hdf5_files(dataset)

    # Figure out name of primary hdf5 file.  This is the 1st if many or the
    # only if there's one
    if is_parallel:
        file_name = datasets[0]
    else:
        file_name = datasets

    # Read existing dataset (always first one in sim dir)
    # Note that this one already ends in .hdf5
    f_set = h5py.File(file_name, "r+")

    # Make variable-length string dtype so hdf5 could understand it
    string_dt = h5py.special_dtype(vlen=str)

    # Store size of dataset, i.e. number of simulations
    f_set.create_dataset("size", data=np.array([counter]), dtype=np.int64)

    # Store how the dataset is ordered
    data = np.asarray([order], dtype=object)
    f_set.create_dataset("order", data=data, dtype=string_dt)

    # Store mapping between number, simulation name
    data = np.asarray(number_to_sim, dtype=object)
    f_set.create_dataset("number_to_sim", data=data, dtype=string_dt)

    # Store the name of the input files for each simulation
    data = np.asarray(infiles, dtype=object)
    f_set.create_dataset("input_files", data=data, dtype=string_dt)

    # Store the names of the variables you're saving
    data = []
    for key in data_cols.keys():
        for var in data_cols[key]:
            # Make variable look like body_variable
            data.append(str(key).replace(".in","") + "_" + str(var))

    # Any variables from the log file?
    if var_from_log is not None:
        for key in var_from_log.keys():
            for var in var_from_log[key]:
                # Make variable look like body_variable
                data.append(str(key).replace(".in","") + "_" + str(var))

    # Any variables from input files?
    if var_from_infile is not None:
        for key in var_from_infile.keys():
            for var in var_from_infile[key]:
                # Make variable look like body_variable
                data.append(str(key).replace(".in","") + "_" + str(var))

    data_cols = np.asarray(data, dtype=object)
    f_set.create_dataset("data_cols", data=data_cols, dtype=string_dt)

    # Store count_map
    if count_map is not None:
        data = np.asarray(count_map, dtype=np.int64)
        f_set.create_dataset("count_map", data=data, dtype=np.int64)
    else:
        # Store number 0 to tell that no count_map was provided
        data = np.asarray([0])
        f_set.create_dataset("count_map", data=data, dtype=np.int64)

    # Close dataset
    f_set.close()

    # Create dataset object to wrangle all the data
    dataset_obj = Dataset(data=datasets,size=counter,order=order,
                          number_to_sim=number_to_sim,
                          input_files=infiles,
                          data_cols=data_cols,
                          count_map=count_map)

    return dataset_obj
# end function


def load_metadata(dataset):
    """
    Load simulation suite metadata from a hdf5 file.

    Parameters
    ----------
    dataset : str
        Name (including path) of hdf5 dataset.  Don't include a file extension.
    counter : int
        number of processed simulations in the suite
    order : str
        How user wants dataset ordered.  Defaults to "none" which means
        the code loads in the data in whatever order the simulation dirs
        are in
    number_to_sim : list
        list of mapping between number (index) and simuatin name
    infiles : list
        list of the names of simulation input files
    data_cols : dict
        dict of bodies and their respective parameters which are simulation
        outputs
    var_from_log : dict
        Dict of bodies, parameters to extract from logfile.
        Something like dict = {"body1" : [var1, var2], "body2" : [var3, var4]}
    var_from_infile : dict
        Dict of input files, parameters to extract from input files. Looks like
        dict = {"body1" : [var1], "body2" : [var2]}
    count_map : list
        mapping between simulation number and hdf5 file when using many hdf5
        files

    Returns
    -------
    data : Dataset object
        See Dataset object docs
    """

    is_parallel, datasets = find_hdf5_files(dataset)

    # Figure out path to where the hdf5 file(s) live
    #path = dataset.split("/")[:-1]
    #path = os.path.join('/', *path)

    # Find all .hdf5 files, sort them so 0th one shows up first
    # Metadata always in 0th file
    #datasets = []
    #for d_file in os.listdir(path):
    #    if d_file.endswith(".hdf5"):
    #        datasets.append(os.path.join(path,d_file))
    #datasets.sort()

    # Figure out name of primary hdf5 file.  This is the 1st if many or the
    # only if there's one
    if is_parallel:
        file_name = datasets[0]
    else:
        file_name = datasets

    # Read existing dataset (always first one in sim dir)
    # Note that this one already ends in .hdf5
    f_set = h5py.File(file_name, "r")

    # Try to load metadata
    try:
        # Load size
        length = f_set["size"][0]
    except RuntimeError:
        raise RuntimeError("Failed to load size.")

    try:
        # Load order
        order = f_set["order"][0]
    except RuntimeError:
        raise RuntimeError("Failed to load order.")

    try:
        # Get list to convert from # -> sim name
        number_to_sim = list(f_set["number_to_sim"])
    except RuntimeError:
        raise RuntimeError("Failed to load number_to_sim.")

    try:
        # Get list to convert from # -> sim name
        input_files = list(f_set["input_files"])
    except RuntimeError:
        raise RuntimeError("Failed to load input_files.")

    try:
        # Get list to convert from # -> sim name
        data_cols = list(f_set["data_cols"])
    except RuntimeError:
        raise RuntimeError("Failed to load data_cols.")

    try:
        # Get list to convert from # -> sim name
        count_map = f_set["count_map"]

        # If no count_map provided (0 flag), set it to None
        if count_map[0] == 0:
            count_map = None
        else:
            count_map = list(count_map)

    except RuntimeError:
        raise RuntimeError("Failed to load count_map.")

    # Close dataset, return object handler
    f_set.close()

    return Dataset(data=datasets,size=length,order=order,
                   number_to_sim=number_to_sim,
                   input_files=input_files,
                   data_cols=data_cols,
                   count_map=count_map)
# end function


#############################################################################
#
# Below are misc data processing functions for regularly gridded data
#
#############################################################################

def aggregate_data(data=None, bodies=None, funcs=None, new_cols=None,
                   cache="cache.pkl",fmt="hdf5",ind = 0,**kwargs):
    """
    Iterate through data and return the initial conditions for each simulation in addition
    to any other user-specified quantity.

    Parameters
    ----------
    data : DataSet object
        master data object produced by extract_data_hdf5
    bodies : dict
        body names are keys and values are list of body variables to extract
    funcs : dict of dict
        Dictionary of functions that operates on the listed body's variable.
        Example: Mean of cbp semimajor axis -> funcs = {"cbp" : {"SemimajorAxis" : np.mean}}
    new_cols : dict
        Similar to funcs, but caries function and name of data to be added to the dataframe
        Example: new_cols = {"cbp" : {"DampTime" : custom_function}}
        The custom_function takes the following args: data,i,body,**kwargs
        where i is the simulation number and body is the str name of the body.
    cache : str
        Name of the cache file
    fmt : str (optional)
        Name of data format.  If hdf5, will use those data accessing utilities.
        Options: "hdf5".  Note: if using "hdf5", data is the name of the
        dataset.
    ind : int
        Index of variable's array to select.  Defaults to initial condition
    kwargs : dict
        keyword arguments to be passed to new_cols functions

    Returns
    -------
    ret : dataframes
        Data frame containing simulation initial values for variables specified by user and
        any additional aggregate values.  1 row == 1 simulation.

    Example
    -------

        >>> bodies = {'cbp': ['Ecce'],'secondary': ['SemiMajorAxis','Eccentricity']}
        >>> new_cols = {"cbp" : {"DampTime" : de.find_damping_time}}
        >>> kw = {"key" : "Ecce", "last_time" : 7.0e9}

        >>> df = aggregate_data(data, bodies=bodies, funcs={"cbp" : {"Semim" : np.mean}},
                            new_cols=new_cols,cache="cache.pkl",fmt=fmt,**kw)

        >>> df["cbp_Ecce"]
            (some pandas Series)
    """

    # If cache exists, load that and return df
    if os.path.exists(cache):
        print("Reading data from cache:",cache)
        sys.stdout.flush()

        # Check pandas version to make sure I don't break stuff
        if float(pd.__version__.split(".")[1]) >= 0.2:
            return pd.read_pickle(cache)
        else:
            with open(cache, 'rb') as handle:
                return pickle.load(handle)

    # User forgot to specify data and cache
    if data is None:
        raise ValueError("No data or cache provided!")

    # No cache given, user must supply bodies
    if bodies is None:
        raise ValueError("No bodies dict provided!")

    # If funcs is None, make it empty dict
    if funcs is None:
        funcs = {}

    # Same with new_cols
    if new_cols is None:
        new_cols = {}

    res = {}

    # Define default function
    def default_func(arr, ind):
        return arr[ind]

    if fmt == "hdf5":
        # Init data
        for body in bodies.keys():
            # Loop over variables
            for var in bodies[body]:
                res[body + "_" + var] = np.zeros(data.size)
            # Loop over new columns, if there are any
            if body in new_cols.keys():
                for col in new_cols[body].keys():
                    res[body + "_" + col] = np.zeros(data.size)

        # Loop over simulations
        for i in range(data.size):
            # Loop over bodies
            for body in bodies.keys():
                # Loop over variables
                for var in bodies[body]:

                    # Apply function to data
                    # No function given or empty dict
                    if body not in funcs.keys() or var not in funcs[body].keys() or funcs[body] is None:
                        res[body + "_" + var][i] = default_func(data.get(i, body, var),ind)
                    else:
                        res[body + "_" + var][i] = funcs[body][var](data.get(i, body, var))
                # Loop over new cols, if there are any for this body
                if body in new_cols.keys():
                    for col in new_cols[body].keys():
                        res[body + "_" + col][i] = new_cols[body][col](data,i,body,fmt=fmt,**kwargs)

    else:
        raise ValueError("Invalid format: %s" % fmt)

    # Convert data dict -> pandas dataframe
    res = pd.DataFrame(res)

    # Cache the result?
    if cache is not None:
        print("Caching data at %s" % cache)
        sys.stdout.flush()
        with open(cache, 'wb') as handle:
            pickle.dump(res, handle)

    return res
# End function


def add_column(data, df=None, new_cols=None, cache=None, fmt="hdf5", **kwargs):
    """
    Add a new column computed as some user-defined function of simulation output
    to a dataframe made by aggregate_data.

    Parameters
    ----------
    data : DataSet object
        master data object produced by extract_data_hdf5
    df : pandas dataframe
        df produced/read in by aggregate_data
    new_cols : dict
        Nested dict which caries function and name of data to be added to the dataframe
        Example: new_cols = {"cbp" : {"DampTime" : custom_function}}
        The custom_function takes the following args: data,i,body,**kwargs
        where i is the simulation number and body is the str name of the body.
    cache : str
        Name of the cache file (a .pkl pickle file)
    fmt : str
        Name of data format.  If hdf5, will use those data accessing utilities.
        Options: "hdf5".  Note: if using "hdf5", data is the name of the
        dataset.
    kwargs : dict
        keyword arguments to be passed to new_cols functions

    Returns
    -------
    ret : dataframes
        Same dataframe as produced by aggregate_data but with additional columns specified
        by new_cols

    Example
    -------

    >>> def mean_func(data,i,body,fmt="hdf5",**kwargs):
    >>>    return np.mean(data.get(i,body,"Eccentricity"))

    >>> new_cols = {"cbp" : {"MeanEcc" : mean_func}}
    >>> df = add_column(df,new_cols=new_cols,cache=src+"trivial_cache.pkl")
    >>> df["cbp_MeanEcc"]
        (some pandas Series)
    """

    # User forgot to specify cache or df
    if cache is None and df is None:
        print("No cache or dataframe provided! Provide a cache path or dataframe.")
        return None

    if new_cols is None:
        raise ValueError("add_column called but no new_cols provided!")

    res = {}

    # Init data
    for body in new_cols.keys():
        # Loop over variables
        for var in new_cols[body]:
            # Make sure that column doesn't already exist, if so ignore
            next_col = body + "_" + var
            if next_col not in df.columns:
                res[next_col] = np.zeros(len(df))

    # Loop over simulations
    for i in range(len(df)):
        # Loop over bodies
        for body in new_cols.keys():
            # Loop over variables
            for col in new_cols[body]:
                next_col = body + "_" + col
                if next_col in res.keys():
                    res[next_col][i] = new_cols[body][col](data,i,body,fmt=fmt,**kwargs)

    # Convert data dict -> pandas dataframe
    res = pd.DataFrame(res)

    # Now merge the two
    df = pd.concat([df, res], axis=1)

    # Cache the result
    if cache is not None:
        print("Caching data at %s" % cache)
        with open(cache, 'wb') as handle:
            pickle.dump(df, handle)

    return df
# End function


def reduce_dimensions(x, y, z, shape, dims=(-1,), reduce_func = np.nanmean):
    """
    Reduce dimensionality of data for 2D plotting.  For example, if x is some variable
    tracked over 125 simulations and takes 5 values and similarly for for y, we want to
    compress over the 3rd dimension to produce a 5x5 grid for plotting

    Parameters
    ----------
    x : numpy array
    y : numpy array
    z : numpy array
        Color variable
    shape : tuple
        Dimensionality of data, e.g. (5, 4) for x,y,z of len 20
    dims : array-like
        Dimensions to compress, e.g. for a (5, 4, 3) dimensional data set that we
        wish to compress to (5, 4), dims == (-1)
    reduce_func : function
        Function used to compress color dimension, typically numpy mean

    Returns
    -------
    x, y, z : numpy arrays
        where x.shape == y.shape == z.shape == shape
    """

    # What is done here:
    # 1) Reshape x, y, z e.g. 20 -> (5, 4)
    # 2) Apply some function over axes to reduce dimension to 2 for plotting
    # 3) Apply squeeze to remove extra dimension, e.g. (5, 4, 1) -> (5, 4)
    # Call mean so dimensions compress properly for x, y
    x = np.squeeze(np.apply_over_axes(np.mean,x.values.reshape(shape),axes=dims))
    y = np.squeeze(np.apply_over_axes(np.mean,y.values.reshape(shape),axes=dims))
    z = np.squeeze(np.apply_over_axes(reduce_func,z.values.reshape(shape),axes=dims))

    return x, y, z
# End function
