"""
David Fleming (dflemin3) Spring 2016

Functions for processing results of VPLANET simulations from an ensemble of
simulations

"""

from __future__ import division, print_function, absolute_import
import numpy as np
import os
import pandas as pd
import h5py
import pickle

# Tell module what it's allowed to import
__all__ = ["reduce_dimensions",
           "aggregate_data",
           "Dataset",
           "extract_data_hdf5"]

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
        IOError("infiles is None!  Must be list of input files.")

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

    # No order
    if order.lower() == "none":
        dirs = filter(lambda x: os.path.isdir(os.path.join(src, x)), os.listdir(src))
    # Grid order.  Preserves vspace-given order for a grid of simulations
    elif order.lower() == "grid":
        dirs = sorted(filter(lambda x: os.path.isdir(os.path.join(src, x)), os.listdir(src)))
    # Not a valid option!
    else:
        ValueError("Invalid order: %s." % order)

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
            logcount = logcount +1

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
            # Line looks like: (Age) System Age [sec] 3.155760e+09
            # Note: code finds this twice with 2nd being final system age
            if line.startswith("(Age) System Age"):
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
    """

    def __init__(self,data=None,size = 0,order="none",number_to_sim=None):
        self.data = data # Path to hdf5 dataset
        self.size = size # Length of data (number of groups from root)
        self.order = order # How the simulations are ordered
                            # Defaults to None aka ordered as they were found
        # Private list
        self.__number_to_sim = number_to_sim # Translates # to sim name

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

        Returns
        -------
        res : array

        """

        # Enforce array bounds for less-cryptic error message
        if simulation >= 0 and simulation < self.size:
            return get_data_hdf5(self.data, simulation, body, variables,
                                     dtype=dtype)
        else:
            ValueError("Invalid simulation number: %d. Dataset size: %d" %
                  (simulation,self.size))


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
            ValueError("Invalid simulation number: %d. Dataset size: %d" %
                  (sim,self.size))


    def __repr__(self):
        """
        For pretty prints.
        """
        return "Name: %s. Size: %d. Order: %s" % (self.data,self.size,self.order)
# end class


def data_from_dir_hdf5(f_set,grpname, datadir=".",data_cols=None,infiles=None,
                       compression="gzip",remove_halts=True):
    """
    Given a directory where simulation data are stored, the name of each body's output
    columns and the name of the input files, pull the data!

    Parameters
    ----------
    f_set : HDF5 group
        root dir-level hdf5 group
    grpname : string
        Name of the hdf5 dataset group that will contain directory's simulation data
        Named after sim #
    datadir : str
        Name of directory where simulation results are kept
    data_cols : dict
        dictionary contains each body's output variable names
    infiles : list
        list containing input file names for each body
    compression : str
        compression algorithm used to reduce dataset size.  Defaults to gzip.
        None (no quotes) turns off compression
    remove_halts : bool
        Whether or not to exclude simulations that did not run to completion

    Returns
    -------
    success : int (bool)
        1 if successful, 0 if not (i.e. halt, sim didn't finish, etc)
    """

    # data_cols and infiles must be given
    if data_cols is None or infiles is None:
        ValueError("data_cols and infiles must both be specified.")

    # If user wants to, look for halts/sims not finishing
    if remove_halts:
        halt = halt_check(datadir)
    else:
        halt = 0

    # Did it halt? ... assuming user cares
    if halt != 0:
        return 0

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
                                             dtype=np.float64)

                    # Create corresponding hdf5 subgroup for body, store data as
                    # numpy array. Subgroup's name is body name
                    sub = grp.create_group(infile)

                    # Write columns to subgroup as datasets [allows for POSIX-like access]
                    for col in data_cols[infile + ".in"]:
                        sub.create_dataset(col, data = pd.np.array(tmp[col]), dtype="f",
                                          compression = compression)
                except RuntimeError:
                    raise RuntimeError("Unknown pd.read_table error.")

    # Return 1 since 1 new sim was successfully processed/stored
    return 1
# End function


def extract_data_hdf5(src=".", dataset="simulation.hdf5", order="none",
                      compression="gzip",remove_halts=False,cadence=None,
                     skip_body=None):
    """
    Given the root directory path for a suite of simulations, pull all the data
    and store it in a hd5f database.  This allows for iteration/processing of data
    that is way too large to concurrently store in memory.

    Parameters
    ----------
    src : str
        Path to simulation suite directory which contains all simulation sub directories
    dataset : str
        Name (including path) of hdf5 database
    order : str
        How user wants dataset ordered.  Defaults to "None" which means
        the code loads in the data in whatever order the simulation dirs
        are in
    compression : str
        compression algorithm used to reduce dataset size.  Defaults to gzip.
        None (no quotes) turns off compression. Options: "gzip", "lzf"
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

    Returns
    -------
    data : Dataset object
        See Dataset object docs
    """

    # Only run this function if the dataset doesn't exist in the src dir
    if (not os.path.exists(dataset)):

        # Create hdf5 dataset
        print("Creating hdf5 dataset:",dataset)
        f_set = h5py.File(dataset, "w")

        # Dataset exists
    else:
        print("Hdf5 dataset already exists.  Reading from: %s." % dataset)
        print("Using size, order stored in dataset.")

        # Read existing dataset
        f_set = h5py.File(dataset, "r")

        try:
            # Load size
            length = f_set["meta"][0]
        except RuntimeError:
            raise RuntimeError("Failed to load length.  Defaulting to 1000.")

        try:
            # Load order
            order = f_set["order"][0]
        except RuntimeError:
            raise RuntimeError("Failed to load order.  Defaulting to None.")

        try:
            # Get list to convert from # -> sim name
            number_to_sim = list(f_set["number_to_sim"])
        except RuntimeError:
            raise RuntimeError("Failed to load number_to_sim.  Defaulting to [].")

        # Close dataset, return object handler
        f_set.close()
        return Dataset(dataset,length,order,number_to_sim)

    # Make the dataset!

    # Get list of all data directories in src to iterate over
    dirs = get_dirs(src,order=order)

    # Find out what the infiles are based on the first dir
    # Since they're the same in all dirs
    infiles = get_infiles(os.path.join(src,dirs[0]))

    # Skip any bodies?
    if skip_body is not None:
        for sbody in skip_body:
            if sbody in infiles:
                infiles.remove(sbody)
                print("Skipped %s." % sbody)

    print("Infiles:",infiles)

    # Get the names of the output variables for each body
    data_cols = get_cols(os.path.join(src,dirs[0]),infiles)
    print("Data Columns:",data_cols)

    # Loop over directories (and hence simulation) and get the data
    # User counter for name of group/means to identify simulation
    counter = 0

    # List to store what dir sim # corresponds to
    number_to_sim = []
    for direct in dirs:

        # Store data from each simulation, increment counter if succesful
        res = data_from_dir_hdf5(f_set,str(counter),os.path.join(src,direct),
                                 data_cols,infiles,compression=compression,
                                 remove_halts=remove_halts)

        # Increment counter (keeps track of valid parsed simulation dirs)
        counter += res
        if res != 0:
            # store sim - counter mapping
            number_to_sim.append(direct)

        # Output progress to user?
        if cadence != None and counter % int(cadence) == 0:
            print("Simulations processed so far: %d" % counter)

    # Try to store metadata
    # Add top level dataset with information about dataset
    # Meta holds the length (number of root level groups aka
    # number of simulations)
    # number_to_sim is a python dict that maps #->sim name,

    try:
        # Make variable-length string dtype so hdf5 could understand it
        string_dt = h5py.special_dtype(vlen=str)

        f_set.create_dataset("meta", data=np.array([counter]), dtype=np.int64)
        # Note: for python 3, str replaced by bytes

        f_set.create_dataset("order", data=np.array([order]), dtype=string_dt)

        data = np.asarray(number_to_sim, dtype=object)
        f_set.create_dataset("number_to_sim", data=data, dtype=string_dt)

    except RuntimeError:
        raise RuntimeError("Unable to store number_to_sim.  Shouldn't happen!")

    # Close dataset
    f_set.close()

    # Create Dataset class to return
    return Dataset(dataset,counter,order,number_to_sim)
# End function


def get_data_hdf5(dataset, simulation, body, variables, dtype=np.float64):
    """
    Given the simulation number, body and the variable, access and return the data.  For example,
    get_data(1,"secondary","Eccentricity") will return a numpy array containing the secondary's
    eccentricity vector.  A call like get_data(1,"secondary",["Time","Eccentricity"]) will return
    a 2D numpy array whose columns are Time and Eccentricity.

    Parameters
    ----------
    dataset : hdf5 dataset
        hdf5 dataset to open
    simulation : int
        Number corresponding to a given simulation
    body : str
        Name of the body whose data you wish to access
    variables : str or iterable
        Name(s) of the dataset to return for given body
    dtype : datatype (optional)
        Type of data to return.  Defaults to np.float64

    Returns
    -------
    data : numpy array
        Requested dataset as a numpy array
    """

    # Create dataset path
    path = os.path.join(str(simulation),body)

    data = []
    # Open dataset, access data
    try:
        with h5py.File(dataset, "r") as hf:

            # Only one variable
            if type(variables) == str:
                return np.array(hf.get(path)[variables])
            # Multiple variables
            else:
                for var in variables:
                    data.append(np.array(hf.get(path)[var]))

        return np.asarray(data,dtype=dtype)
    except RuntimeError:
        raise RuntimeError("Invalid get call.  Simulation, body, variable(s) : %s, %s, %s" %
              (simulation,body,variables))
        return None
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
        with open(cache, 'rb') as handle:
            return pickle.load(handle)

    # User forgot to specify data and cache
    if data is None:
        ValueError("No data or cache provided!")

    # No cache given, user must supply bodies
    if bodies is None:
        ValueError("No bodies dict provided!")

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
        ValueError("Invalid format: %s" % fmt)

    # Convert data dict -> pandas dataframe
    res = pd.DataFrame(res)

    # Cache the result?
    if cache is not None:
        print("Caching data at %s" % cache)
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
        ValueError("add_column called but no new_cols provided!")

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
    x = np.squeeze(np.apply_over_axes(np.mean,x.reshape(shape),axes=dims)) # Call mean so dimensions compress properly
    y = np.squeeze(np.apply_over_axes(np.mean,y.reshape(shape),axes=dims))
    z = np.squeeze(np.apply_over_axes(reduce_func,z.reshape(shape),axes=dims))

    return x, y, z
# End function


"""

Misc

"""