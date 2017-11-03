# -*- coding: utf-8 -*-
"""
Created on Tue Oct 18 13:51:42 2016

@author: dflemin3 [David P. Fleming, University of Washington, Seattle]
@email: dflemin3 (at) uw (dot) edu

This file contains routines to use bootstrapping to estimate the error of a given
fitting procedure.

Running this script shows a simple case of boot strapping a linear model to noisy data
to get a handle on the error of the fit.

Running std is calculated following Welford algorithm:
https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
"""
from __future__ import print_function, division, unicode_literals

import numpy as np

# Tell module what it's allowed to import
__all__ = ["bootstrap_error_estimate",
           "find_outliers"]


###################################################################
#
# Function declarations
#
###################################################################

def bootstrap_error_estimate(est, X, y, nboots=100, seed=None):
    """
    Clearly documented function.

    Parameters
    ----------
    est : sklearn estimator
        Scikit learn estimator.  Example: LinearRegression()
    X : array (n x d)
        Data array (n samples, d features)
    y : array (n x 1)
        Target vector
    nboots : int (optional)
        Number of bootstraps.  Defaults to 10
    seed : int (optional)
        RNG seed.  Defaults to None

    Returns
    -------
    fit_mean : array (n x 1)
        Mean prediction averaged over bootstraps
    fit_std : array (n x 1)
        Standard deviation of bootstraps computed via Welford algorithm
    cv_score : float
        Cross validation score
    """

    # Validate input
    assert nboots > 2, "Must use more than two bootstraps!"

    # Init arrays
    fit_mean = np.zeros_like(y)

    # Params required for numerically stable running std estimate
    n = 0
    mean = 0.0
    M2 = 0.0

    # Loop over bootsraps
    for ii in range(nboots):

        # Resample data with replacement
        inds = np.random.choice(range(0, len(y)), len(y), replace=True)
        X_train, y_train = X[inds], y[inds]

        # Perform fit using sklearn estimator object
        est.fit(X_train, y_train)

        # Predict on all data
        y_hat = est.predict(X)

        # Estimate fit mean, std
        fit_mean += y_hat/nboots

        # Running std estimate
        n += 1
        delta = y_hat - mean
        mean += delta/n
        M2 += delta*(y_hat - mean)

    # Wrap up std calculation
    fit_std = np.sqrt(M2/(n - 1))

    return fit_mean.reshape(y.shape), fit_std.reshape(y.shape)
# end function


def bootstrap_error_estimate_test(est, X_train, y_train, X_test, nboots=100, seed=None):
    """
    Clearly documented function.

    Parameters
    ----------
    est : sklearn estimator
        Scikit learn estimator.  Example: LinearRegression()
    X_train : array (n_train x d)
        Data array (n samples, d features)
    y_train : array (n x 1)
        Target vector
    X_test : array (n_test x d)
        testing data for predictions
    nboots : int (optional)
        Number of bootstraps.  Defaults to 10
    seed : int (optional)
        RNG seed.  Defaults to None

    Returns
    -------
    fit_mean : array (n x 1)
        Mean prediction averaged over bootstraps
    fit_std : array (n x 1)
        Standard deviation of bootstraps computed via Welford algorithm
    cv_score : float
        Cross validation score
    """

    # Validate input
    assert nboots > 2, "Must use more than two bootstraps!"

    # Init arrays
    fit_mean = np.zeros(X_test.shape[0])

    # Params required for numerically stable running std estimate
    n = 0
    mean = 0.0
    M2 = 0.0

    # Loop over bootsraps
    for ii in range(nboots):

        # Resample data with replacement
        inds = np.random.choice(range(0, len(y_train)), len(y_train), replace=True)
        X, y = X_train[inds], y_train[inds]

        # Perform fit using sklearn estimator object
        est.fit(X, y)

        # Predict on testing data
        y_hat = est.predict(X_test)

        # Estimate fit mean, std
        fit_mean += y_hat/nboots

        # Running std estimate
        n += 1
        delta = y_hat - mean
        mean += delta/n
        M2 += delta*(y_hat - mean)

    # Wrap up std calculation
    fit_std = np.sqrt(M2/(n - 1))

    return fit_mean.reshape(X_test.shape[0]), fit_std.reshape(X_test.shape[0])
# end function


def find_outliers(x, fn=np.median, n_stds=1.0):
    """
    Find elements of x that are n standard deviations away from the center.  By default,
    the center is taken to be the median but in general can be a user-specified function.

    Parameters
    ----------
    x : array
    fn : function (optional)
        function to compute center of x.  Defaults to numpy median
    n_stds: float (optional)
        number of standard deviations for clipping.  Defaults to 1.0

    Returns
    -------
    mask : array
        boolean array for numpy masking to find outliers
    center : float
        central value
    std : float
        standard deviations
    """

    center = fn(x)
    std = np.std(x)

    # Only care about fits that are worse, not better, than center
    mask = (x > center + n_stds*std)

    return mask, center, std
# end function


def loss_01(y, y_hat):
    """
    Compute the 0/1 loss.

    Parameters
    ----------
    y : array
        truth
    y_hat : array
        prediction

    Returns
    -------
    loss : float
        0/1 loss
    """

    return np.sum(y != y_hat)/len(y)
# end function


###################################################################
#
# Run example
#
###################################################################

if __name__ == "__main__":
    import matplotlib.pyplot as plt
    from sklearn.linear_model import LinearRegression

    # Define random seed
    seed = 1
    n_bootstraps = 100 # Number of bootstraps
    num  = 50 # Number of points
    sigma = 5 # Error amplitude

    # Set seed for reproducibility
    np.random.seed(seed)

    # Generate toy data: y = x + Gaussian noise
    X = np.linspace(0, 10, num)
    X = X.reshape(num, 1)
    eps = np.random.randn(len(X)) * sigma
    y = X + eps.reshape(num,1)
    y.reshape(num,1)

    # Init arrays to store mean fit, var of fit
    fit_mean = np.zeros_like(y)
    fit_var = np.zeros_like(y)

    # Loop over all resamples
    ii = 0
    lr = LinearRegression()
    fig, ax = plt.subplots(figsize=(8,8))

    # Params required for numerically stable running std estimate
    n = 0
    mean = 0.0
    M2 = 0.0
    for ii in range(n_bootstraps):

        # Resample data with replacement
        inds = np.random.choice(range(0, len(y)), len(y), replace=True)
        X_train, y_train = X[inds], y[inds]

        # Perform simple linear regression fit
        lr.fit(X_train, y_train)

        # Predict on all data
        y_hat = lr.predict(X)

        # Estimate fit mean, std
        fit_mean += y_hat/n_bootstraps

        # Running std estimate
        n += 1
        delta = y_hat - mean
        mean += delta/n
        M2 += delta*(y_hat - mean)

        # Plot fit prediction on all data to show scatter
        ax.plot(X, y_hat, color="grey", alpha=0.2, zorder=1)

    # Loop over, wrap up std calculation
    fit_std = np.sqrt(M2/(n - 1))

    # Plot true, mean fit, mean error
    ax.scatter(X,y,marker="o",color="orange",zorder=5,label="True")
    ax.plot(X,fit_mean,color="red",lw=2,zorder=6,label="Mean Fit")

    err_m = fit_mean - fit_std
    err_p = fit_mean + fit_std

    ax.plot(X,err_m,ls="--",color="red")
    ax.plot(X,err_p,ls="--",color="red")

    # Format, display
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_xlim(X.min(),X.max())
    ax.legend(loc="upper left")
    fig.tight_layout()
    plt.show()
