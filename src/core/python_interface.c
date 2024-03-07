/**
  @file python_interface.c
  @brief The main entry point for the code. All the magic starts here.
  @author Rodrigo Luger ([rodluger](https://github.com/rodluger/))
  @date May 14 2021
*/

#ifdef VPLANET_PYTHON_INTERFACE

#include "vplanet.h"
#define PY_SSIZE_T_CLEAN
#include <Python.h>

// Get the code version, passed in as a macro
#ifndef VPLANET_VERSION
#define VPLANET_VERSION_STRING "0.0.0"
#else
#ifdef VPLANET_ON_WINDOWS
// Silly Windows needs some extra love
#define STRINGIFY(X) #X
#define VPLANET_VERSION_STRING STRINGIFY(VPLANET_VERSION)
#else
#define VPLANET_VERSION_STRING VPLANET_VERSION
#endif
#endif

int main_impl(int, const char *(*)[9]);

static PyObject *vplanet_core_version(PyObject *self, PyObject *args) {
  const char *version = VPLANET_VERSION_STRING;
  PyObject *pVersion  = PyBytes_FromString(version);
  Py_INCREF(pVersion);
  return pVersion;
}

static PyObject *vplanet_core_run(PyObject *self, PyObject *args) {

  // Get the options (built-in max of 9)
  int argc = PyTuple_GET_SIZE(args);
  const char *argv[9];
  if (!PyArg_ParseTuple(args, "|sssssssss", &argv[0], &argv[1], &argv[2],
                        &argv[3], &argv[4], &argv[5], &argv[6], &argv[7],
                        &argv[8])) {
    return NULL;
  }

  // Run vplanet
  main_impl(argc, &argv);

  // Return None
  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef VplanetCoreMethods[] = {
      {"run", vplanet_core_run, METH_VARARGS, NULL},
      {"version", vplanet_core_version, METH_VARARGS, NULL},
      {NULL, NULL, 0, NULL}};

static struct PyModuleDef vplanet_core_module = {
      PyModuleDef_HEAD_INIT, "vplanet_core", NULL, -1, VplanetCoreMethods};

PyMODINIT_FUNC PyInit_vplanet_core(void) {
  PyObject *m = PyModule_Create(&vplanet_core_module);
  if (m == NULL) {
    return NULL;
  }
  return m;
}

#endif
