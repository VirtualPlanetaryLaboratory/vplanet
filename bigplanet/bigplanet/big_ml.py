"""
@author David P. Fleming (dflemin3) Nov 2, 2016
@email dflemin3 (at) uw (dot) edu
University of Washington, Seattle

Functions for machine learning vplanet simulation results.  Functions include feature
generation, data manipulation and methods for supervised learning from the data using
sklearn functionality.

The idea behind these routines is to be able to learn a model to VPLANET initial
conditions to predict some output so you do not have to run more simulations.  Many of
these functions make standard data manipulation much more simple by wrapping sklearn
functions.

"""

from __future__ import division, print_function, absolute_import
import numpy as np
from sklearn import preprocessing

# Tell module what it's allowed to import
__all__ = ["poly_features",
           "fourier_features",
           "scale_data",
           "extract_features"]

def extract_features(df, features, target):
    """
    Extract features, target vectors from dataframe and return them as numpy arrays.  Also
    returns a mapping between matrix index and column.

    Parameters
    ----------
    df : pandas Dataframe
        input dataframe
    features : list
        list of string of column names of desired features (for X matrix)
    target : str
        name of target variable for supervised learning (for y vector)

    Returns
    -------
    X : array
    y : vector
    names : dict
    """

    X = df[features].values
    y = df[target].values
    names = dict(zip(features, [x for x in range(0,len(features))]))

    # Filter out NaNs just in case (sometimes halts throw NaNs)
    mask = np.isnan(y)
    X = X[~mask]
    y = y[~mask]

    return X, y, names
# end function


def poly_features(X, degree=2, interaction_only=False, include_bias=True):
    """
    Wrapper for sklearn PolynomialFeatures which performs the following:

    Generate a new feature matrix consisting of all polynomial combinations of the
    features with degree less than or equal to the specified degree. For example, if an
    input sample is two dimensional and of the form [a, b], the degree-2 polynomial
    features are [1, a, b, a^2, ab, b^2].

    See http://scikit-learn.org/stable/modules/generated/sklearn.preprocessing.
    PolynomialFeatures.html#sklearn.preprocessing.PolynomialFeatures for more details.

    Parameters
    ----------
    X : array
        input data.  Something like df[feature_columns].values
    degree : int (optional)
        Max degree of polynomial features.  Defaults to 2.  Fit is slow with degree > 2
    interaction_only : bool (optional)
        If true, only interaction features are produced: features that are products of
        at most degree distinct input features (so not x[1] ** 2, x[0] * x[2] ** 3, etc.).
        Defaults to False
    include_bias : bool (optional)
        If True (default), then include a bias column, the feature in which all
        polynomial powers are zero (i.e. a column of ones - acts as an intercept term in
        a linear model).

    Returns
    -------
    X : array
        transformed input data

    """
    # Make polynomial transformation, perform transformation
    poly = preprocessing.PolynomialFeatures(degree=degree,
                                            interaction_only=interaction_only,
                                            include_bias=include_bias)
    return poly.fit_transform(X)
# end function


def fourier_features(X, k=1000, v = None, b = None, verbose = False):
    """
    Generate random Fourier bases sin(Xv + b) where v in R^d and b in R are random
    variables drawn from N(0,1) and a uniform distribution on [0, 2pi],
    respectively.  Maps current features into this new fourier space.

    Parameters
    ----------
    X : array (n x d)
        input data.  Something like df[feature_cols].values for n samples, d features
    k : int (optional)
        number of new transformed features
    v : Random vector transformation matrix (optional)
        (features x new_features) matrix. Defaults None
    b : Random phase transformation vector (optional)
        (samples x 1) vector.  Defaults to None
    verbose : bool (optional)
        whether or not to return w, b if you're generating them.  Defaults to False

    Returns
    -------
    X : array (n x k)
        transformed input data for n samples, k fourier features
    Also w, b not supplied
    """

    # If not supplied, generate transformation!
    if v is None and b is None:

        # Generate synthetic data
        v = np.random.normal(size=(X.shape[-1],k))
        b = np.random.uniform(low=0.0, high=(2.0*np.pi), size=(X.shape[0],1))

        if verbose:
            return np.sin(X.dot(v) + b), v, b
        else:
            return np.sin(X.dot(v) + b)
    # Have v and b!
    elif v is not None and b is not None:
        return np.sin(X.dot(v) + b)
    else:
        raise ValueError("Either supply both v and b or neither!")
# end function


def naive_nn_layer(X, k = 5000, v = None, verbose = False):
	"""
	Perform a naive approximation to the first layer of a neural network to transform
	a d x 1 dimensional feature vector for a given sample to k x 1 via a linear
	combination of the original d features.  The mapping is given by the following

	h_i(x) = v_i dot x

	where v is a d x k matrix where each column is a d x 1 vector whose entries are
	sampled from the standard normal distribution.

	Parameters
	----------
	X : array (n x d)
		input data
	k : int (optional)
		number of features for of new sample.  Defaults to 5000
	v : array (d x k) (optional)
		weight mapping matrix.  generated from standard normal if not supplied
	verbose : bool (optional)
	    whether or not to return transformation matrix.  Defaults to False

	Returns
	-------
	X : array (n x k)
		transformed data
	"""

	# Generate feature mapping as random variates from standard normal distribution
	if v is None:
		v = np.random.normal(size=(X.shape[-1],k))

		if verbose:
		    return X.dot(v), v
		else:
		    return X.dot(v)
	else:
		return X.dot(v)
# end function


def scale_data(X):
    """
    Transform the data to center it by removing the mean value of each feature, then
    scale it by dividing non-constant features by their standard deviation using
    sklearn's preprocessing.scale

    Parameters
    ----------
    X : array
        input data (something like df.values)

    Returns
    -------
    X : array
        scaled input data
    """

    return preprocessing.StandardScaler().fit_transform(X)
# end function
