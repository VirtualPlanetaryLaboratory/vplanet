from __future__ import division, print_function, absolute_import
from . import data_extraction as de
import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import itertools

"""

ROUTINES FOR REGULARLY GRIDDED DATA

The below functions only work/make sense for some N-dimensional
simulation grid.

Note: For regularly gridded data, no simulations can be thrown away, i.e.
when running data_extraction, you CANNOT have excluded halted simulations.
If you did that, then the data will have been created from a grid, but the grid
will be likely missing chunks and hence the dimensionality reduction algorithms
will fail.

"""

def plot_red_dim(x, y, z, shape, fig, ax, labels=None, dims = (-1),
                 reduce_func = np.nanmean, nan_value = 0.0, bReduce=True,
                interp="gaussian",cmap="viridis",colorbar=True,aspect="auto",
                 origin="lower",vmin=None,vmax=None,**kwargs):
    """
    Work horse plotting function.  Plots simple compression of multidimensional
    gridded data down to a 3D (2 dims + color dim) space for imshow plots.

    Parameters
    ----------
    x : numpy array
    y : numpy array
    z : numpy array
        Color variable
    shape : tuple
        Dimensionality of data, e.g. (5, 4) for x,y,z of len 20
    fig : matplotlib figure object
    ax : matplotlib axis object
    labels : list of str
        Axis labels
    dims : tuple
        Dimensions to compress, e.g. for a (5, 4, 3) dimensional data set that we
        wish to compress to (5, 4), dims == (-1)
    reduce_func : function
        Function used to compress color dimension, typically numpy mean
    nan_value : float
        Value to set nans to for coloring purposes
    bReduce : bool
        Whether or not to reduce the data
    interp : str
        imshow interpolation method
    cmap : str
        Matplotlib colormap
    colorbar : bool
        Whether or not to plot a colorbar
    aspect : str
        See matplotlib imshow docs
    origin : str
        See matplotlib imshow docs
    vmin, vmax : float
        min, max of colorbar range, respectively

    Returns
    -------
    None

    Example usage:

    # Init axes labels, data to plot
    labels = ["Binary Semimajor Axis [AU]", "Binary Eccentricity", "Initial CBP Eccentricty"]
    x = df["secondary"]["SemiMajorAxis"]
    y = df["secondary"]["Eccentricity"]
    z = df["cbp"]["Ecce"]

    # Shape of the data
    shape = (5, 4, 3)

    fig, ax = plt.subplots()

    plot_red_dim(x, y, z, shape, ax, labels=labels)

    """

    # Reduce the dimensionality of the data?
    if bReduce:
        x, y, z = de.reduce_dimensions(x, y, z, shape=shape, dims=dims,
                                    reduce_func = reduce_func)

    # Filter our nans
    z[np.isnan(z)] = nan_value

    # Set colorbar ranges if not given
    if vmin == None or vmax == None:
        vmin=np.min(z)
        vmax=np.max(z)

    # Plot
    cax = ax.imshow(z, cmap=cmap, vmin=vmin, vmax=vmax, origin=origin, aspect=aspect,
                    extent=[x.min(), x.max(), y.min(), y.max()],interpolation=interp,**kwargs)


    # Make it square and set limits
    ax.set_adjustable('box-forced')
    ax.set_xlim(x.min(),x.max())
    ax.set_ylim(y.min(),y.max())

    # Format plot
    if labels != None and labels != []:
        ax.set_xlabel(labels[0])
        ax.set_ylabel(labels[1])

        if colorbar:
            # Create colorbar
            cbar = fig.colorbar(cax)
            cbar.set_label(labels[2], rotation=270, labelpad=25)
            cax.set_clim(vmin,vmax)

    return None
# End function

def plot_red_dim_contour(x, y, z, shape, fig, ax, labels=None, dims = (-1),
                         reduce_func = np.nanmean, nan_value = 0.0, bReduce=True,
                         interp="gaussian",cmap="viridis",levels=20,clines=False,
                         colorbar=False,origin="lower",**kwargs):
    """
    Work horse plotting function.  Plots simple compression of multidimensional
    gridded data down to a 3D (2 dims + color dim) space for imshow plots.

    Parameters
    ----------
    x : numpy array
    y : numpy array
    z : numpy array
        Color variable
    shape : tuple
        Dimensionality of data, e.g. (5, 4) for x,y,z of len 20
    fig : matplotlib figure object
    ax : matplotlib axis object
    labels : list of str
        Axis labels
    dims : tuple
        Dimensions to compress, e.g. for a (5, 4, 3) dimensional data set that we
        wish to compress to (5, 4), dims == (-1)
    reduce_func : function
        Function used to compress color dimension, typically numpy mean
    nan_value : float
        Value to set nans to for coloring purposes
    bReduce : bool
        Whether or not to reduce the data
    interp : str
        imshow interpolation method
    cmap : str
        Matplotlib colormap
    levels : int
        Number of contour levels to plot
    clines : bool
        Whether or not to plot contour lines
    colorbar : bool
        Whether or not to plot a colorbar
    origin : str
        See matplotlib imshow docs

    Returns
    -------
    None

    Example usage:
    # Init axes labels, data to plot
    labels = ["Binary Semimajor Axis [AU]", "Binary Eccentricity", "Initial CBP Eccentricty"]
    x = df["secondary"]["SemiMajorAxis"]
    y = df["secondary"]["Eccentricity"]
    z = df["cbp"]["Ecce"]

    # Shape of the data
    shape = (5, 4, 3)
    levels = 15

    fig, ax = plt.subplots()

    plot_red_dim_contour(x, y, z, shape, ax, labels=labels, levels=levels)

    """

    # Reduce the dimensionality of the data?
    if bReduce:
        x, y, z = de.reduce_dimensions(x, y, z, shape=shape, dims=dims,
                                    reduce_func = reduce_func)

    # Filter our nans
    z[np.isnan(z)] = nan_value

    # Set colorbar ranges
    vmin=np.min(z)
    vmax=np.max(z)

    # Plot
    cax = ax.contourf(z, levels, cmap=cmap,vmin=vmin,vmax=vmax,origin=origin,
                      extent=[x.min(), x.max(), y.min(), y.max()],**kwargs)

    # Plot/label contour lines?
    if clines:
        # Plot contour lines
        ax.contour(cax, levels=cax.levels[1::2],
                      colors="white",
                      origin="lower",
                      hold='on')

        # Label contours
        ax.clabel(cax, cax.levels[::2],  # label every second level
           inline=0,
           fmt='%1.1f',
           fontsize=15,
           colors="white")


    # Format plot
    if labels != None and labels != []:
        ax.set_xlabel(labels[0])
        ax.set_ylabel(labels[1])

        if colorbar:
            # Create colorbar
            cbar = fig.colorbar(cax)
            cbar.set_label(labels[2], rotation=270, labelpad=25)
            cax.set_clim(vmin,vmax)

    return None
# End function

def red_dim_grid(df, shape, dims, color_by="cbp_DampTime", left_color_func = np.nanmean, nan_value = 0.0,
                 bReduce=True,interp="nearest",lcmap="viridis_r",rcmap="magma_r",levels=None,clines=False,
                 origin="lower",save=False,right_color_func = np.std,**kwargs):
    """
    Produce a grid of plots where each subplot shows 2 variables versus each other colored by a 3rd variable.
    The lower left triangle plots the 3rd variable marginalized (averaged) over all other dimensions while
    the upper right triangle plots some measure of the variance of the marginalized 3rd variable.

    Parameters
    ----------
    df: dateframe produced by the "aggrategate_data" fn
        Data frame containing simulation initial values for variables specified by user and
        any additional aggregate values.  1 row == 1 simulation.
    shape : dict of dict
        Dictionary containing body and variable and dimensional length of that variable, e.g.
        shape = {"secondary" : {"Eccentricity" : 5}} indicates that this simulation suite was
        run for 5 values of the secondary eccentricity.
    color_by : str
        Name of body_var to color output by
    left_color_func : function
        function used to marginalize.  typically numpy.nanmean or some similar variant
    right_color_func : function
        function used to plot something variance-like.  typically numpy.nanstd or some similar variant
    nan_value: float
        set NaNs to this value
    bReduce : bool
        whether or not to marginalize over higher dimensions.  Should be true and mainly is here
        for backwards compatibility.
    interp : str
        imshow interpolation method.  see matplotlib imshow docs for more info
    lcmap : str
        matplotlib colormap for lefthand triangle
    rcmap : str
        matplotlib colormap for righthand triangle
    levels : int
        number of contours if making a contour plot.  Currently not implemented.
    clines : int
        number of contour lines if making a contour plot.  Currently not implemented.
    origin : str
        matplotlib imshow parameter
    save : bool
        whether or not to save the plot as a image

    Returns
    -------
    fig : matplotlib figure object
    axes : array
        array of fig's axes


    Example
    -------
    >>> # Define the shape
    >>> shape = {"secondary" : {"Eccentricity" : 5, "SemimajorAxis" : 5}, "cbp" : {"Eccentricity" : 5,
    >>>         "SemimajorAxis" : 5}}
    >>> # Define dims
    >>> dims = {"secondary" : {"Eccentricity" : 0, "SemimajorAxis" : 1}, "cbp": {"Eccentricity" : 2,
    >>>                                                                        "SemimajorAxis" : 3}}

    >>> fig, axes = red_dim_grid(df, shape, color_by="cbp_DampTime", interp="nearest",
    >>>                        lcmap="viridis_r",rcmap="plasma_r",save=True, right_color_func = np.std)
        (plot saved to confusogram.pdf)
    """

    # Using df, shape, color_name, make a list containing all the axes
    # combinations
    combos = create_axes_combos(df,color_by=color_by)

    # Length of size of plot square == number of variables varied in simulation suite
    size = int(np.sqrt(len(combos)))

    # If size isn't at least 3, this function isn't for you
    assert size >= 3,"Too few dimensions. Use imshow or contourf instead!"

    # Make a figure/axes to plot on
    fig, axes = plt.subplots(size, size, figsize=(size*9,size*8))

    # Get colorbar ranges
    # Is there something smarter I can do here?
    lvmin = np.min(df[color_by])
    lvmax = np.max(df[color_by])

    rvmin = 0.0
    rvmax = np.std(df[color_by])

    # Iterate over combos going left->right->down->repeat
    # along the axes
    # Loop over a rows
    for i in range(0,size):
        # Loop over cols
        for j in range(0,size):

            # Along the diagonal? skip
            if i == j:
                fig.delaxes(axes[i,j])
                continue

            # Make subplot axis labels
            label = [combos[i*size + j][1].split("_")[0] + " " + combos[i*size + j][1].split("_")[1],
                    combos[i*size + j][0].split("_")[0] + " " + combos[i*size + j][0].split("_")[1],
                    ""]

            # LEFT of the diagonal? Marginalize (via mean)
            if j < i:
                combo = combos[i*size + j]
                xcombo = combo[1]
                ycombo = combo[0]

                x = df[xcombo].values
                y = df[ycombo].values
                z = df[color_by].values

                # Get shape of data
                tmp_shape = axes_to_shape(combo, shape)

                # Get dimensions to marginalize over
                xkey, xvar = xcombo.split("_")
                ykey, yvar = ycombo.split("_")

                tmp_dims = get_dims(dims, xkey, xvar, ykey, yvar)

                plot_red_dim(x, y, z, tmp_shape, fig, axes[i,j], labels=label, dims=tmp_dims,
                         reduce_func = left_color_func, nan_value = nan_value, bReduce=bReduce,
                        interp=interp,cmap=lcmap,colorbar=False,origin=origin,vmin=lvmin,
                                vmax=lvmax)

            # RIGHT of the diagonal? Some measure of the STD/Variance
            elif j > i:

                # Note: here x, y are switched for readbility
                combo = combos[i*size + j]
                ycombo = combo[1]
                xcombo = combo[0]

                # Exchange labels
                tmp_lab = label[0]
                label[0] = label[1]
                label[1] = tmp_lab

                x = df[ycombo].values
                y = df[xcombo].values
                z = df[color_by].values

                # Get shape of data
                tmp_shape = axes_to_shape(combo, shape)

                # Get dimensions to marginalize over
                xkey, xvar = xcombo.split("_")
                ykey, yvar = ycombo.split("_")

                tmp_dims = get_dims(dims, xkey, xvar, ykey, yvar)

                plot_red_dim(x, y, z, tmp_shape, fig, axes[i,j], labels=label, dims=tmp_dims,
                         reduce_func = right_color_func, nan_value = nan_value, bReduce=bReduce,
                        interp=interp,cmap=rcmap,colorbar=False,origin=origin,vmin=rvmin,
                                vmax=rvmax)


        # Add colorbars for both triangles

        # Left side...STRONG SIDE
        lcax = fig.add_axes([0.05, 0.1, 0.02, 0.6])
        lnorm = mpl.colors.Normalize(vmin=lvmin, vmax=lvmax)
        lcb = mpl.colorbar.ColorbarBase(lcax, cmap=lcmap,spacing='proportional',
                                       norm=lnorm)
        lcb.set_label('DampTime')
        lcb.ax.yaxis.set_ticks_position('left')
        lcb.ax.yaxis.set_label_position('left')

        # Right Side
        rcax = fig.add_axes([0.93, 0.3, 0.02, 0.6])
        rnorm = mpl.colors.Normalize(vmin=rvmin, vmax=rvmax)
        rcb = mpl.colorbar.ColorbarBase(rcax, cmap=rcmap,spacing='proportional',
                                       norm=rnorm)
        rcb.set_label('DampTime std',rotation=270,labelpad=30)

        # Save the figure?
        if save:
            fig.savefig("confusogram.pdf",bbox_extra_artists=(rcax,lcax),bbox_inches='tight')

    return fig, axes
# end function

###################################################################
#
# Regular Grid Plotting utilities functions.
#
###################################################################

def create_axes_combos(df,color_by="cbp_DampTime"):
    """
    Parse a shape nested dictionary to derive a shape tuple where the user defined
    parameters are the first two values of the tuple.  This function is used to create
    shape tuples needed for plotting functions that plot a 2D figure with a color that
    must marginalize over all higher dimensions.  The first 2 values in the tuple are the
    plotted variables while the others are the ones to be marginalized over.

    Parameters
    ----------
    df: dict of dateframes produced by the "aggrategate_data" fn
        1 dataframe for each body
        Data frame containing simulation initial values for variables specified by user and
        any additional aggregate values.  1 row == 1 simulation.
    color_by : str
        column in one of df's dataframes that will be the variable that describes (gives a color
        to) each simulation.  It is ignored from the combinations since we wouldn't color by a
        variable that also is plotted along an axis.

    Returns
    -------
    list : list
        list of every combination of length 2 of simulation parameters
        stored in the dataframe

    Example
    -------

    >>> create_axes_combos(df)
        [('cbp_Ecce', 'secondary_Eccentricity'),
        ('cbp_Ecce', 'secondary_SemiMajorAxis'),
        ('secondary_Eccentricity', 'secondary_SemiMajorAxis')]
    """

    # Get columns
    cols = list(df.columns)

    # Remove color_by
    cols.remove(color_by)

    # Return permutations
    return list(itertools.product(cols,cols))

    # Old code
    '''
    variables = []

    for key in df.keys():

        # Get variables associated with said body
        tmp = list(df[key].columns.values)

        # If color variable in list, remove it
        if color_by in tmp:
            tmp.remove(color_by)

        # Add body's name to variable for clarity
        for i in range(0,len(tmp)):
            tmp[i] = key + "_" + tmp[i]

        variables = variables + tmp

    # Get only unique combinations
    return list(itertools.product(variables,variables))
    '''
# end function

def get_shape(shape, key1, var1, key2, var2):
    """
    Parse a shape nested dictionary to derive a shape tuple where the user defined
    parameters are the first two values of the tuple.  This function is used to create
    shape tuples needed for plotting functions that plot a 2D figure with a color that
    must marginalize over all higher dimensions.  The first 2 values in the tuple are the
    plotted variables while the others are the ones to be marginalized over.

    Parameters
    ----------
    shape : dict of dict
        Dictionary containing body and variable and dimensional length of that variable, e.g.
        shape = {"secondary" : {"Eccentricity" : 5}} indicates that this simulation suite was
        run for 5 values of the secondary eccentricity.
    key1, key2 : str
        Names of bodies
    var1, var2 : str
        Names of variables associated with given body

    Returns
    -------
    shape : tuple
        Shape of your data, e.g. (5,4,3)

    Usage:

    >>> holder = get_shape(shape, "secondary","Eccentricity", "cbp", "Ecce")
    >>> print(holder)
    >>> (5, 3, 4) # for a 60 simulation suite
    """

    holder = []

    # Loop over bodies
    for key in shape.keys():
        # Loop over variables for each body
        for var in shape[key].keys():
            # If key-var pair not a user-defined one, append it
            # These are the dimensions to marginalize over!
            if (var != var1 or key != key1) and (var != var2 or key != key2):
                holder.append(shape[key][var])

    # Insert front 2 dimensions
    holder.insert(0,shape[key2][var2])
    holder.insert(0,shape[key1][var1])

    return list(holder)
# end function

def axes_to_shape(combo, shape):
    """
    Create a shape tuple from

    Parameters
    ----------
    combo : tuple
        combination of length 2 of simulation parameters
        stored in the dataframe
    shape : dict of dict
        Dictionary containing body and variable and dimensional length of that variable, e.g.
        shape = {"secondary" : {"Eccentricity" : 5}} indicates that this simulation suite was
        run for 5 values of the secondary eccentricity.

    Returns
    -------
    shape : tuple
        Shape of your data, e.g. (5,4,3)

    Example
    -------

    >>> combo = create_axes_combos(df)[0] # Pick 1st combo to test
    >>> print(axes_to_shape(combo, shape))
        (5, 3, 4)
    """

    # Split combo into body_var
    word1 = combo[0]
    word2 = combo[1]

    # Isolate body name, variable name
    key1, var1 = word1.split("_")
    key2, var2 = word2.split("_")

    # Return shape
    return get_shape(shape, key1, var1, key2, var2)
# end function

def get_dims(dims, key1, var1, key2, var2):
    """
    Parse a shape nested dictionary to derive a dimension tuple for dimensions to
    marginalize over.  This function is used to create
    dimension tuples needed for plotting functions that plot a 2D figure with a color that
    must marginalize over all higher dimensions.  The first 2 values in the tuple are the
    plotted variables while the others are the ones to be marginalized over.

    Parameters
    ----------
    dims : dict of dict
        Dictionary containing body and variable and dimensional position of that variable, e.g.
        shape = {"secondary" : {"Eccentricity" : 0}} indicates that this simulation suite was
        run where secondary eccentricity was the first varied variable (1st line in the
        vspace input file).
    key1, key2 : str
        Names of bodies
    var1, var2 : str
        Names of variables associated with given body

    Returns
    -------
    dims : tuple
        dims of your data, e.g. (1,3)

    Example
    -------

    >>> dims = {"secondary" : {"Eccentricity" : 0, "SemimajorAxis" : 1}, "cbp": {"Eccentricity" : 2,
                                                                        "SemimajorAxis" : 3}}
    >>> get_dims(dims, "cbp", "Eccentricity", "secondary", "SemimajorAxis")
        (3, 0)
    """

    holder = []

    # Loop over bodies
    for key in dims.keys():
        # Loop over variables for each body
        for var in dims[key].keys():
            # If key-var pair not a user-defined one, append it
            # These are the dimensions to marginalize over!
            if (var != var1 or key != key1) and (var != var2 or key != key2):
                holder.append(dims[key][var])

    return list(holder)
# end function

"""

FOR IRREGULAR/RANDOM DATA (or also gridded!)

"""

def multiscatter(df,z_var="cbp_DampTime",size_var=None,color_var=None,cmap="magma_r",
              alpha=0.5):
    """
    Plot a series of scatter plots of 1 dataframe column (z_var) vs every other column
    with the option to have one parameter size points and another parameter color the points.

    Parameters
    ----------
    df : Pandas dataframe
    z_var : string
        column name of dependent variable
    size_var : string (optional)
        column name that determines size of points
    color_var : string (optional)
        column name that determines color of points
    cmap : string (optional)
        matplotlib colormap name
    alpha : float (optional)
        alpha (shade) parameter which ranges from [0,1]

    Returns
    -------
    fig : matplotlib figure object
    axes : array of matplotlib axis objects

    Example
    -------
    >>> fig, axes = multiscatter(df,z_var="cbp_DampTime",size_var="secondary_Eccentricity",
                               color_var="secondary_SemimajorAxis")
        (plot)

    """

    # Can't color/size points by the dependent variable!
    assert(z_var != size_var and z_var != color_var), "Can't color/size points by the dependent variable!"

    # Set default color if none given
    if color_var != None:
        color = df[color_var]
    else:
        color = "black"

    # Set default size if none given
    if size_var != None:
        # Compute point sizes by normalizing data
        s = 10. + 240.*(df[size_var] - df[size_var].min())/np.ptp(df[size_var])
    else:
        s = 50

    # Get size for square grid of plots, leave space blank
    size = len(df.columns)
    grid_size = int(np.sqrt(size))

    # Make subplots
    fig, axes = plt.subplots(grid_size, grid_size, figsize=(grid_size*9,grid_size*8))
    axes = axes.flatten()

    # dependent var (y coor for all scatter plots)
    z = df[z_var]

    # Iterate over all things, scatter plot "z" value vs sim variables
    ii = 0
    # Loop over columns == simulation variables
    for col in df.columns.values:
        if col != z_var:
            im = axes[ii].scatter(df[col],z,c=color,s=s,alpha=alpha,cmap=cmap)

            # Format axes with labels, limits
            axes[ii].set_ylabel(z_var.split("_")[0] + " " + z_var.split("_")[1])
            axes[ii].set_xlabel(col.split("_")[0] + " " + col.split("_")[1])
            axes[ii].set_ylim(z.min()-0.05*z.min(),z.max()+0.05*z.max())

            # Increment ii
            ii = ii + 1

        else:
            pass

    # Add colorbar?
    if color_var != None:

        cbar = fig.colorbar(im, ax=axes.ravel().tolist())
        cbar.ax.set_ylabel((color_var.split("_")[0] + " " + color_var.split("_")[1]), rotation=270,
                          labelpad = 25)

    # Add legend that displays typical point sizes?
    if size_var != None:

        # Dummy plots for range of points
        sizes = s.max()
        s_inv = df[size_var].max()
        l1 = plt.scatter([],[], s=sizes/4., color="black")
        l2 = plt.scatter([],[], s=2.*sizes/4., color="black")
        l3 = plt.scatter([],[], s=3.*sizes/4., color="black")
        l4 = plt.scatter([],[], s=sizes, color="black")

        # Labels for points
        labels = ["%.2f" % (s_inv/4.), "%.2f" % (s_inv/2.),
                  "%.2f" % (3.*s_inv/4.), "%.2f" % s_inv]

        # Fancy legend relative to last axis
        leg = axes[-1].legend([l1, l2, l3, l4], labels, ncol=4, frameon=False, fontsize=15, loc = 8, borderpad = 1.0,
                              handletextpad=1, scatterpoints = 1, bbox_to_anchor=(-.1, -0.3),
                              title = size_var.split("_")[0] + " " + size_var.split("_")[1])


    return fig, axes
# End Function

"""

Misc

"""

# Tell module what it's allowed to import
__all__ = [plot_red_dim,
           plot_red_dim_contour,
           red_dim_grid,
           multiscatter]
