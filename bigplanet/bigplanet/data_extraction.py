"""
David Fleming (dflemin3) Spring 2016

Functions for processing results of VPLANET simulations from a
vspace ensemble of simulations

"""

from __future__ import division, print_function, absolute_import
import numpy as np
import os
import pandas as pd
import h5py
import pickle

def get_cols(datadir=".",infiles=[]):
    """
    Given a directory where simulation data are stored and the name of the input files,
    return the name of each body's output columns

    Parameters
    ----------
    datadir : str
        Name of directory where simulation results are kept
    infiles : list
        list containing input file names for each body

    Returns
    -------
    data_cols : dict
        dictionary containing output variable names for each body
        like the following: data_cols["body0"] = ["Time","Radius",...]
    """

    # Dict to hold all columns for each body [like time, semi, ecc ...]
    data_cols = {}

    # Loop over files corresponding to each body
    for infile in infiles:
        with open(datadir+infile) as f:
            lines = f.readlines()

            # Loop over all line in the input file
            for line in lines:
                line = str(line).rstrip("\n")

                # Is this the saOutputOrder line and it isn't commented out?
                if line.find("saOutputOrder") != -1 and "#saOutputOrder" not in line:
                    cols = line.split()[1:] # ignore saOutputOrder

                    # Remove any - if there are any
                    # Also ignore commented out (#) stuff
                    good_cols = []
                    for i in range(0,len(cols)):
                        if "#" in cols[i]: # First time this goes, rest of stuff is ignored
                            break
                        else:
                            # Get rid of - sign
                            cols[i] = cols[i].replace("-", "")

                            # Column name is good and processed, so add it
                            good_cols.append(cols[i])

            # Save the columns!
            data_cols[infile] = good_cols

    return data_cols
# end function

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

def data_from_dir(datadir=".",data_cols={},infiles=[]):
    """
    Given a directory where simulation data are stored, the name of each body's output
    columns and the name of the input files, pull the data!

    Parameters
    ----------
    datadir : str
        Name of directory where simulation results are kept
    data_cols : dict
        dictionary contains each body's output variable names
    infiles : list
        list containing input file names for each body

    Returns
    -------
    data : dict
        dictionary with a pandas dataframe for each body containing the output
    """

    # Data dict will end up holding pandas df for each body
    data = {}

    # Now loop over each output file to extract the data
    for infile in infiles:
        # Isolate the body name
        infile = infile.replace(".in","")

        # Loop over all files in dir, open the one that contains the body name
        for f in os.listdir(datadir):
            if f.endswith(".forward") and (f.find(infile) != -1):
                # Read in data as pandas dataframe
                data[infile] = pd.read_table(datadir + f,
                                             header=None,
                                             delim_whitespace=True,
                                             engine="c",
                                             names=data_cols[infile + ".in"],
                                             dtype=np.float64)

    return data
# end function

def extract_data(src="."):
    """
    Given the root directory path for a suite of simulations, pull all the data
    and return it in a nice dictionary for easy processing

    Parameters
    ----------
    src : str
        Path to simulation suite directory which contains all simulation sub directories

    Returns
    -------
    data : dict
        Dictionary containing all simulation results
    """

    data = []
    dirs = filter(lambda x: os.path.isdir(os.path.join(src, x)), os.listdir(src))

    # Find out what the infiles are based on the first dir
    # Since they're the same in all dirs
    infiles = get_infiles(src + "/" + dirs[0] + "/")
    print("Infiles:",infiles)

    # Get the names of the output variables for each body
    data_cols = get_cols(src + "/" + dirs[0] + "/",infiles)
    print("Data Columns:",data_cols)

    # Loop over directories and get the data
    for direct in dirs:
        data.append(data_from_dir(src + "/" + direct + "/",data_cols,infiles))

    return data
#end function

#############################################################################
#
# Below are data extraction functions that work with the hdf5 data format
#
#############################################################################

class Dataset(object):
    """
    Abstraction class for working with hdf5 datasets using the h5py
    python wrapper package.
    """

    def __init__(self,data = None,size = 0,order="None"):
        self.data = data # Path to hdf5 dataset
        self.size = size # Length of data (number of groups from root)
        self.order = order # How the simulations are ordered
                            # Defaults to None or ordered "as they were found"

    def get(self, simulation, body, variables, dtype=np.float64):
        """
        Wrapper for get_data_hdf5 function (see its docs)
        """
        return get_data_hdf5(self.data, simulation, body, variables, dtype=dtype)

    def __repr__(self):
        """
        For pretty prints
        """
        return "Name: %s. Size: %d." % (self.data,self.size)
# end class

def data_from_dir_hdf5(grp, datadir=".",data_cols={},infiles=[],order="None"):
    """
    Given a directory where simulation data are stored, the name of each body's output
    columns and the name of the input files, pull the data!

    Parameters
    ----------
    grp : HDF5 group
        Name of the hdf5 dataset group that will contain directory's simulation data
    datadir : str
        Name of directory where simulation results are kept
    data_cols : dict
        dictionary contains each body's output variable names
    infiles : list
        list containing input file names for each body
    order : str
        How user wants dataset ordered.  Defaults to "None" which means
        the code loads in the data in whatever order the simulation dirs
        are in

    Returns
    -------
    success : int (bool)
        1 if successful, 0 if not (i.e. halt, sim didn't finish, etc)
    """

    # Now loop over each output file to extract the data
    for infile in infiles:
        # Isolate the body name
        infile = infile.replace(".in","")

        # TODO Here's where I would look for halts/sims not finishing
        # And continue based on whether or not user wants them
        # if halt or sim didnt finish:
        # return 0

        # Loop over all files in dir, open the one that contains the body name
        for f in os.listdir(datadir):
            if f.endswith(".forward") and (f.find(infile) != -1):
                # Read in data as pandas dataframe
                tmp = pd.read_table(datadir + f,
                                             header=None,
                                             delim_whitespace=True,
                                             engine="c",
                                             names=data_cols[infile + ".in"],
                                             dtype=np.float64)

                # Create corresponding hdf5 subgroup for body, store data as
                # numpy array. Subgroup's name is body name
                sub = grp.create_group(infile)

                # Write columns to subgroup as datasets [allows for dict-like access]
                for col in data_cols[infile + ".in"]:
                    sub.create_dataset(col, data = pd.np.array(tmp[col]), dtype="f",
                                      compression = "gzip")

    return 1
# End function

def extract_data_hdf5(src=".", dataset="simulation.hdf5", order="None"):
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
        print("Hdf5 dataset already exists.  Reading from:", dataset)
        f_set = h5py.File(dataset, "r")
        length = f_set["meta"][0]
        f_set.close()
        return Dataset(dataset,length,order)

    # Get list of all data directories in src to iterate over
    dirs = filter(lambda x: os.path.isdir(os.path.join(src, x)), os.listdir(src))

    # Find out what the infiles are based on the first dir
    # Since they're the same in all dirs
    infiles = get_infiles(src + "/" + dirs[0] + "/")
    print("Infiles:",infiles)

    # Get the names of the output variables for each body
    data_cols = get_cols(src + "/" + dirs[0] + "/",infiles)
    print("Data Columns:",data_cols)

    # Loop over directories (and hence simulation) and get the data
    # User counter for name of group/means to identify simulation
    counter = 0
    for direct in dirs:

        # Create group for each directory, pass to load with data
        # Group's name == directory
        grp = f_set.create_group(str(counter))

        # Store data from each simulation, increment counter if succesful
        counter += data_from_dir_hdf5(grp,src + "/" + direct + "/",data_cols,infiles,
                                      order)

    # Add top level dataset with information about dataset
    # Currently "meta" only holds the length (number of root level groups aka
    # number of simulations)
    print("Length:",counter)
    f_set.create_dataset("meta", data=np.array([counter]), dtype="i")

    # Close dataset
    f_set.close()

    # Create Dataset class to return
    return Dataset(dataset,counter,order)
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
    path = str(simulation) + "/" + body

    data = []
    # Open dataset, access data
    with h5py.File(dataset, "r") as hf:

        # Only one variable
        if type(variables) == str:
            return np.array(hf.get(path)[variables])
        # Multiple variables
        else:
            for var in variables:
                data.append(np.array(hf.get(path)[var]))

    return np.asarray(data,dtype=dtype)
# end function

#############################################################################
#
# Below are misc data processing functions
#
#############################################################################

def aggregate_data(data, bodies={"cbp" : ["Eccentricity"]}, funcs={}, new_cols={},
                   cache="cache.pkl",fmt="hdf5",ind = 0,**kwargs):
    """
    Iterate through data and return the initial conditions for each simulation in addition
    to any other user-specified quantity.

    Parameters
    ----------
    data : iterable
        master data array produced by extract_data
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
    fmt : str
        Name of data format.  If hdf5, will use those data accessing utilities.
        Options: "dict", "hdf5".  Note: if using "hdf5", data is the name of the
        dataset.
    ind : int
        Index of variable's array to select.  Defaults to initial condition
    kwargs : dict
        keyword arguments to be passed to new_cols functions

    Returns
    -------
    ret : dict of dataframes
        1 dataframe for each body
        Data frame containing simulation initial values for variables specified by user and
        any additional aggregate values.  1 row == 1 simulation.

    Example
    -------

        >>> bodies = {'cbp': ['Ecce'],'secondary': ['SemiMajorAxis','Eccentricity']}
        >>> new_cols = {"cbp" : {"DampTime" : de.find_damping_time}}
        >>> kw = {"key" : "Ecce", "last_time" : 7.0e9}

        >>> df = aggregate_data(data, bodies=bodies, funcs={"cbp" : {"Semim" : np.mean}},
                            new_cols=new_cols,cache="cache.pkl",fmt=fmt,**kw)

        >>> df["cbp"]["Ecce"]
            (some pandas Series)
    """

    # If cache exists, load that and return df
    if os.path.exists(cache):
        print("Reading data from cache:",cache)
        with open(cache, 'rb') as handle:
            return pickle.load(handle)

    res = {}

    # Define default function
    def default_func(arr, ind):
        return arr[ind]

    if fmt == "dict":
        # Init data
        for body in bodies.keys():
            res[body] = {}
            # Loop over variables
            for var in bodies[body]:
                res[body][var] = np.zeros(len(data))
            # Loop over new columns, if there are any
            if body in new_cols.keys():
                for col in new_cols[body].keys():
                    res[body][col] = np.zeros(len(data))

        # Loop over simulations
        for i in range(0,data.size):
            # Loop over bodies
            for body in bodies.keys():
                # Loop over variables
                for var in bodies[body]:

                    # Apply function to data
                    # No function given or empty dict
                    if body not in funcs.keys() or var not in funcs[body].keys() or funcs[body] == None:
                        res[body][var][i] = default_func(data[i][body][var],ind)
                    else:
                        res[body][var][i] = funcs[body][var](data[i][body][var])
                # Loop over new cols, if there are any for this body
                if body in new_cols.keys():
                    for col in new_cols[body].keys():
                        res[body][col][i] = new_cols[body][col](data,i,body,fmt=fmt,**kwargs)

    elif fmt == "hdf5":
        # Init data
        for body in bodies.keys():
            res[body] = {}
            # Loop over variables
            for var in bodies[body]:
                res[body][var] = np.zeros(data.size)
            # Loop over new columns, if there are any
            if body in new_cols.keys():
                for col in new_cols[body].keys():
                    res[body][col] = np.zeros(data.size)

        # Loop over simulations
        for i in range(0,data.size):
            # Loop over bodies
            for body in bodies.keys():
                # Loop over variables
                for var in bodies[body]:

                    # Apply function to data
                    # No function given or empty dict
                    if body not in funcs.keys() or var not in funcs[body].keys() or funcs[body] == None:
                        res[body][var][i] = default_func(data.get(i, body, var),ind)
                    else:
                        res[body][var][i] = funcs[body][var](data.get(i, body, var))
                # Loop over new cols, if there are any for this body
                if body in new_cols.keys():
                    for col in new_cols[body].keys():
                        res[body][col][i] = new_cols[body][col](data,i,body,fmt=fmt,**kwargs)

    else:
        print("Invalid format:",fmt)
        return None

    # Convert data dict -> pandas dataframe
    for key in res.keys():
        res[key] = pd.DataFrame(res[key])

    # Cache the result?
    if cache != None:
        print("Caching data at:",cache)
        with open(cache, 'wb') as handle:
            pickle.dump(res, handle)

    return res
# End function

def reduce_dimensions(x, y, z, shape, dims=(-1), reduce_func = np.nanmean):
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
