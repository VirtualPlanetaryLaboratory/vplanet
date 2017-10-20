#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
:py:mod:`pool.py` - Multiprocessing
-----------------------------------

An implementation of three different types of pools:
    
    - An MPI pool borrowed from :py:mod:`emcee`. This pool passes Python objects
      back and forth to the workers and communicates once per task.
        
    - A multiprocessing for local parallelization, borrowed from :py:mod:`emcee`
    
    - A serial pool, which uses the built-in :py:obj:`map` function
    

'''

from __future__ import division, print_function, absolute_import, unicode_literals
import numpy as np
import sys
try:
    from mpi4py import MPI
    MPI = MPI
except ImportError:
    MPI = None
import signal
import functools
import multiprocessing
import multiprocessing.pool
import logging
log = logging.getLogger(__name__)

__all__ = ['MPIPool', 'MultiPool', 'SerialPool', 'Pool']

class _close_pool_message(object):
    def __repr__(self):
        return "<Close pool message>"

class _function_wrapper(object):
    def __init__(self, function):
        self.function = function

def _error_function(*args):
    '''
    The default worker function. Should be replaced
    with the desired mapping function on the first
    call.
    
    '''
  
    raise Exception("Pool was sent tasks before being told what "
                    "function to apply.")

def _test_function(x):
    '''
    Wastes a random amount of time, then
    returns the average of :py:obj:`x`.
    
    '''
    
    for i in range(np.random.randint(99999)):
        j = i ** 2
    
    return np.sum(x) / float(len(x))

def _initializer_wrapper(actual_initializer, *rest):
    """
    We ignore SIGINT. It's up to our parent to kill us in the typical
    condition of this arising from `^C` on a terminal. If someone is
    manually killing us with that signal, well... nothing will happen.

    """
    signal.signal(signal.SIGINT, signal.SIG_IGN)
    if actual_initializer is not None:
        actual_initializer(*rest)

class GenericPool(object):
    '''
    A generic multiprocessing pool object with a :py:obj:`map` method.
    
    '''
    
    def __init__(self, **kwargs):
        '''
        
        '''
        
        self.rank = 0
    
    @staticmethod
    def enabled():
        '''
        
        '''
        
        return False
    
    def is_master(self):
        '''
        
        '''
        
        return self.rank == 0

    def is_worker(self):
        '''
        
        '''
        
        return self.rank != 0
    
    def wait(self):
        '''
        
        '''
        
        return NotImplementedError('Method ``wait`` must be called from subclasses.')
    
    def map(self, *args, **kwargs):
        '''
        
        '''
        
        return NotImplementedError('Method ``map`` must be called from subclasses.')
    
    def close(self):
        '''
        
        '''
        
        pass
    
    def __enter__(self):
        '''
        
        '''
        
        return self

    def __exit__(self, *args):
        '''
        
        '''
        
        self.close()

class MPIPool(GenericPool):
    """
    A pool that distributes tasks over a set of MPI processes. MPI is an
    API for distributed memory parallelism.  This pool will let you run
    emcee without shared memory, letting you use much larger machines
    with emcee.

    The pool only support the :py:func:`map` method at the moment because
    this is the only functionality that emcee needs. That being said,
    this pool is fairly general and it could be used for other purposes.

    Contributed by `Joe Zuntz <https://github.com/joezuntz>`_.

    :param comm: (optional)
        The :py:mod:`mpi4py` communicator.
        
    :param loadbalance: (optional)
        if :py:obj:`True` and :py:obj:`ntask` > :py:obj:`Ncpus`, tries to loadbalance by sending
        out one task to each cpu first and then sending out the rest
        as the cpus get done.
    """
    def __init__(self, comm=None, loadbalance=False, debug=False,
                 wait_on_start = True, exit_on_end = True, 
                 cores_per_task = 1, **kwargs):
        if MPI is None:
            raise ImportError("Please install mpi4py")

        self.comm = MPI.COMM_WORLD if comm is None else comm
        self.rank = self.comm.Get_rank()
        if cores_per_task > 1:
          self.size  = max(1, self.comm.Get_size() // cores_per_task)
        else:
          self.size = self.comm.Get_size() - 1
        self.function = _error_function
        self.loadbalance = loadbalance
        self.debug = debug
        if self.size == 0:
            raise ValueError("Tried to create an MPI pool, but there "
                             "was only one MPI process available. "
                             "Need at least two.")
        self.exit_on_end = exit_on_end
        
        # Enter main loop for workers?
        if wait_on_start:
            if self.is_worker():
                self.wait()
                
    @staticmethod
    def enabled():
        '''
        
        '''
        
        if MPI is not None:
            if MPI.COMM_WORLD.size > 1:
                return True
        return False
        
    def wait(self):
        """
        If this isn't the master process, wait for instructions.

        """
        if self.is_master():
            raise RuntimeError("Master node told to await jobs.")

        status = MPI.Status()

        while True:
            # Event loop.
            # Sit here and await instructions.
            if self.debug:
              print("Worker {0} waiting for task.".format(self.rank))

            # Blocking receive to wait for instructions.
            task = self.comm.recv(source=0, tag=MPI.ANY_TAG, status=status)
              
            if self.debug:
              print("Worker {0} got task {1} with tag {2}."
                        .format(self.rank, type(task), status.tag))

            # Check if message is special sentinel signaling end.
            # If so, stop.
            if isinstance(task, _close_pool_message):
                if self.debug:
                  print("Worker {0} told to quit.".format(self.rank))
                break

            # Check if message is special type containing new function
            # to be applied
            if isinstance(task, _function_wrapper):
                self.function = task.function
                if self.debug:
                  print("Worker {0} replaced its task function: {1}."
                            .format(self.rank, self.function))
                continue

            # If not a special message, just run the known function on
            # the input and return it asynchronously.
            result = self.function(task)
            if self.debug:
              print("Worker {0} sending answer {1} with tag {2}."
                        .format(self.rank, type(result), status.tag))
            self.comm.isend(result, dest=0, tag=status.tag)
    
        # Kill the process?
        if self.exit_on_end:
            sys.exit()
    
    def map(self, function, tasks):
        """
        Like the built-in :py:func:`map` function, apply a function to all
        of the values in a list and return the list of results.

        :param function:
            The function to apply to the list.

        :param tasks:
            The list of elements.

        """
        ntask = len(tasks)

        # If not the master just wait for instructions.
        if not self.is_master():
            self.wait()
            return

        if function is not self.function:
            if self.debug: 
              print("Master replacing pool function with {0}."
                        .format(function))

            self.function = function
            F = _function_wrapper(function)

            # Tell all the workers what function to use.
            requests = []
            for i in range(self.size):
                r = self.comm.isend(F, dest=i + 1)
                requests.append(r)

            # Wait until all of the workers have responded. See:
            #       https://gist.github.com/4176241
            MPI.Request.waitall(requests)

        if (not self.loadbalance) or (ntask <= self.size):
            # Do not perform load-balancing - the default load-balancing
            # scheme emcee uses.

            # Send all the tasks off and wait for them to be received.
            # Again, see the bug in the above gist.
            requests = []
            for i, task in enumerate(tasks):
                worker = i % self.size + 1
                if self.debug:
                  print("Sent task {0} to worker {1} with tag {2}."
                            .format(type(task), worker, i))
                r = self.comm.isend(task, dest=worker, tag=i)
                requests.append(r)

            MPI.Request.waitall(requests)

            # Now wait for the answers.
            results = []
            for i in range(ntask):
                worker = i % self.size + 1
                if self.debug:
                  print("Master waiting for worker {0} with tag {1}"
                            .format(worker, i))
                result = self.comm.recv(source=worker, tag=i)
                
                results.append(result)

            return results

        else:
            # Perform load-balancing. The order of the results are likely to
            # be different from the previous case.
            for i, task in enumerate(tasks[0:self.size]):
                worker = i+1
                if self.debug:
                  print("Sent task {0} to worker {1} with tag {2}."
                            .format(type(task), worker, i))
                # Send out the tasks asynchronously.
                self.comm.isend(task, dest=worker, tag=i)

            ntasks_dispatched = self.size
            results = [None]*ntask
            for itask in range(ntask):
                status = MPI.Status()
                # Receive input from workers.
                try:
                  result = self.comm.recv(source=MPI.ANY_SOURCE,
                                          tag=MPI.ANY_TAG, status=status)
                except Exception as e:
                  self.close()
                  raise e
                
                worker = status.source
                i = status.tag
                results[i] = result
                if self.debug:
                  print("Master received from worker {0} with tag {1}"
                            .format(worker, i))

                # Now send the next task to this idle worker (if there are any
                # left).
                if ntasks_dispatched < ntask:
                    task = tasks[ntasks_dispatched]
                    i = ntasks_dispatched
                    if self.debug:
                      print("Sent task {0} to worker {1} with tag {2}."
                                .format(type(task), worker, i))
                    # Send out the tasks asynchronously.
                    self.comm.isend(task, dest=worker, tag=i)
                    ntasks_dispatched += 1

            return results

    def bcast(self, *args, **kwargs):
        """
        Equivalent to mpi4py :func:`bcast` collective operation.
        """
        return self.comm.bcast(*args, **kwargs)

    def close(self):
        """
        Just send a message off to all the pool members which contains
        the special :class:`_close_pool_message` sentinel.

        """
        if self.is_master():
            for i in range(self.size):
                self.comm.isend(_close_pool_message(), dest=i + 1)
    
class SerialPool(GenericPool):
    '''
    
    '''
    
    def __init__(self, **kwargs):
        '''
        
        '''
        
        self.size = 0
        self.rank = 0
    
    @staticmethod
    def enabled():
        '''
        
        '''
        
        return True
        
    def wait(self):
        '''
        
        '''
        
        raise Exception('``SerialPool`` told to wait!')
    
    def map(self, function, iterable):
        '''
        
        '''
        
        return list(map(function, iterable))

class MultiPool(multiprocessing.pool.Pool):
    """
    This is simply :py:mod:`emcee`'s :py:class:`InterruptiblePool`.
    
    A modified version of :py:class:`multiprocessing.pool.Pool` that has better
    behavior with regard to :py:obj:`KeyboardInterrupts` in the :py:func:`map` method.
    
    Contributed by Peter K. G. Williams <peter@newton.cx>.
    
    :param processes: (optional)
        The number of worker processes to use; defaults to the number of CPUs.

    :param initializer: (optional)
        Either `None`, or a callable that will be invoked by each worker
        process when it starts.

    :param initargs: (optional)
        Arguments for *initializer*; it will be called as
        `initializer(*initargs)`.

    :param kwargs: (optional)
        Extra arguments. Python 2.7 supports a `maxtasksperchild` parameter.

    """
    wait_timeout = 3600

    def __init__(self, processes=None, initializer=None, initargs=(),
                 **kwargs):
        new_initializer = functools.partial(_initializer_wrapper, initializer)
        super(MultiPool, self).__init__(processes, new_initializer,
                                                initargs, **kwargs)
        self.size = 0
        
    @staticmethod
    def enabled():
        '''
        
        '''
        
        return True
    
    def map(self, func, iterable, chunksize=None):
        """
        Equivalent of `map()` built-in, without swallowing
        `KeyboardInterrupt`.

        :param func:
            The function to apply to the items.

        :param iterable:
            An iterable of items that will have `func` applied to them.

        """
        # The key magic is that we must call r.get() with a timeout, because
        # a Condition.wait() without a timeout swallows KeyboardInterrupts.
        r = self.map_async(func, iterable, chunksize)

        while True:
            try:
                return r.get(self.wait_timeout)
            except multiprocessing.TimeoutError:
                pass
            except KeyboardInterrupt:
                self.terminate()
                self.join()
                raise

def Pool(pool = 'AnyPool', **kwargs):
    '''
    Chooses between the different pools.
    If ``pool == 'AnyPool'``, chooses based on availability.
    
    '''
    
    if pool == 'MPIPool':
        return MPIPool(**kwargs)  
    elif pool == 'MultiPool':
        return MultiPool(**kwargs)
    elif pool == 'SerialPool':
        return SerialPool(**kwargs)
    elif pool == 'AnyPool':
        if MPIPool.enabled():
            return MPIPool(**kwargs)  
        elif MultiPool.enabled():
            return MultiPool(**kwargs)
        else:
            return SerialPool(**kwargs)
    else:
        raise ValueError('Invalid pool ``%s``.' % pool)
        
if __name__ == '__main__':
    
    # Instantiate the pool
    with Pool() as pool:
        
        # The iterable we'll apply ``_test_function`` to
        walkers = np.array([[i, i] for i in range(100)], dtype = 'float64')
    
        # Use the pool to map ``walkers`` onto the function
        res = pool.map(_test_function, walkers)

        # Check if the parallelization worked
        assert np.allclose(res, [_test_function(w) for w in walkers])
    
        print("%s: success!" % type(pool).__name__)