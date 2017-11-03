"""

Created sometime in Winter, 2017

@author: dflemin3 [David P. Fleming, University of Washington, Seattle]
@email: dflemin3 (at) uw (dot) edu

Approximate Bayesian Computation (ABC) routines for likelihood-free inference
of parameter posterior distributions

"""

from __future__ import (print_function, division, absolute_import, unicode_literals)
import numpy as np
import scipy.stats as ss


# Tell module what it's allowed to import
__all__ = ["binary",
           "euclidean",
           "Uniform",
           "Gaussian",
           "ForwardModel",
           "ABC"]


# Define some distance metrics for ABC calculations
def binary(x,y):
    """
    Binary distance, aka is x the same-ish as y?
    """
    return np.fabs(x-y) # fabs because I'm suspicious of floating point operations
# end function


def euclidean(x,y):
    """
    Euclidean distance given by

    d = sqrt( (x1 - y1)^2 + ... + (xn - yn)^2 )

    """
    return np.linalg.norm(x-y, ord=2)
# end function


# Define some prior objects
class Uniform(object):
    """
    Uniform prior

    Parameters
    ----------
    low : float
        lower bound
    high : float
        upper bound
    hard_bound : bool (optional)
        whether or not to enforce a hard bound.  If a value supplied is
        x < low or x > high, prob is -infinity.  Defaults to True.
    log : bool (optional)
        whether or not to use log probabilities.  Defaults to False.
    """

    def __init__(self,low=0.0,high=1.0,hard_bound=True,log=False):
        self.low = low
        self.high = high
        self.hard_bound = hard_bound
        self.dist = ss.uniform(self.low,(self.high-self.low))
        self.log = log
    # end function


    def __call__(self, x=None, size=1):
        """
        Either sample from the distribution or evaluate the pdf at a given point
        """
        # Sample from PDF
        if x is None:
            return self.dist.rvs(size=size)
        # x given, evaluate pdf
        else:
            if (x < self.low or x >= self.high) and self.hard_bound:
                return -np.inf # out of bounds!
            else:
                prob = self.dist.pdf(x)
                # Are we using log probabilities?
                if self.log:
                    return np.log(prob)
                else:
                    return prob
    # end function
# end class


class Gaussian(object):
    """
    Gaussian prior

    Parameters
    ----------
    mean : float
        mean of gaussian
    std : float
        standard deviation of gaussian
    log : bool (optional)
        whether or not to use log probabilities.  Defaults to False.
    """

    def __init__(self,mean=0.0,std=1.0,log=False):
        self.mean = mean
        self.std = std
        self.dist = ss.norm(mean,std)
        self.log = log
    # end function


    def __call__(self, x=None, size=1):
        """
        Either sample from the distribution or evaluate the pdf at a given point
        """
        # Sample from PDF
        if x is None:
            return self.dist.rvs(size=size)
        # x given: evaluate pdf
        else:
            prob = self.dist.pdf(x)
            # Are we using log probabilities?
            if self.log:
                return np.log(prob)
            else:
                return prob
    # end function
# end class


class ForwardModel(object):
    """
    Wrapper class to generalize running a model with initial conditions sampled
    from user-specified priors
    """

    def __init__(self, model, params=None, priors=None, scaler=None):
        """
        Initialize forward model object

        Parameters
        ----------
        model : object
            object with sklearn-like predict(x) method which returns result of
            model (or simulation) for input vector x
        params : list
            list of string names for parameters like params ["ecc","semi",...] mostly for
            convience and bookkeeping, not strictly required
        priors : list
            list of prior objects (like Uniform()) for each param
        scaler : sklearn preprocessing scaler
            if provided, scaler scales the input data using the same scale used
            when the model was learned
        """
        self.model = model
        self.params = params
        self.priors = priors
        self.scaler = scaler

        # If not supplied, put in dummy variable
        if self.params is None:
            self.params = ["x"]

        # Same with priors
        if self.priors is None:
            self.priors = [Uniform()]
    # end function


    def run(self):
        """
        Evaluate forward model using initial conditions sampled from parameter
        priors.  Whatever forward model you pass, it needs to have a predict method
        for this to work.

        Parameters
        ----------
        None

        Returns
        -------
        params : float (or array)
            initial conditions for that run
        res : float (or array)
            simulation results
        """

        # Get initial conditions sampled from given initial condition's prior
        init_cond = []
        for ii in range(len(self.priors)):
            init_cond.append(self.priors[ii]())

        # Reshape in case I need to rescale it
        init_cond = np.asarray(init_cond).reshape(1,len(init_cond))

        # Run sim, return stuff in "physical" frame
        if self.scaler is None:
            return init_cond, self.model.predict(init_cond)
        else:
            return init_cond, self.model.predict(self.scaler.transform(init_cond))
    # end function
# end object


class ABC(object):
    """
    Simple Approximate Bayesian Computation using rejection sampling.

    TODO: generalize to allow for other ABC methods (mcmc, for example)
    """

    def __init__(self, fmodel, truth=None):
        """
        Init ABC object

        Parameters
        ----------
        fmodel : object
            forward model class object (see above)
        truth : float, array
            truth observations (like a spectrum, a number, etc) that is known or observed.
            This is the thing your forward model can hopefully come close to reproducing
            with the correct parameters which ABC will hopefully derive
        """
        self.fmodel = fmodel
        self.truth = truth

        # Default to binary classification
        if self.truth is None:
            self.truth = 1.0

    def reject_sampling_abc(self, n_min=10000, distance="binary", eps=1.0e-3):
        """
        Perform Approximate Bayesian Computation using rejection sampling.
        A sample is accepted if distance(truth,sample) <= eps.  In the case of binary
        classification, this reduces to accepted if predicted_class == 1.

        Parameters
        ----------
        n_min : int
            minimum number of accepted samples required for convergence
        distance : str (optional)
            which distance function to use.  Defaults to "binary".  Other options include
            "euclidean"
        eps : float (optional)
            minimum distance threshold.  Defaults to 1.0e-3.  In general, this should be
            tuned

        Returns
        -------
        chain : array (n_accepted x n_parameters)
            all accepted parameter values
        """

        # Set up distance function
        if distance.lower() == "binary":
            dist = binary
        elif distance.lower() == "euclidean":
            dist = euclidean
        else:
            raise RuntimeError("Unknown distance %s. Check docs for valid options" % distance)

        chain = []
        ntot = 0
        while len(chain) < n_min:
            # Run model and return parameters (samp) and sim result
            samp, sim = self.fmodel.run()

            # Is the distance small enough? If so, accept the initial conditions
            if dist(self.truth, sim) <= eps:
                chain.append(samp)
            ntot += 1

        # Store latest run, output interesting info
        print("Acceptance ratio: %lf" % (n_min/ntot))
        self.chain = np.asarray(chain).squeeze()

        return self.chain
# end class
