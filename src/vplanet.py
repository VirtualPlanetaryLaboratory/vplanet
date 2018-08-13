import ctypes
import os

OPTLEN = 24
OPTDESCR = 128
OPTLONDESCR = 2048
MAXFILES = 24

MODULEOPTEND = 2400

class OPTIONS(ctypes.Structure):
    """Input options struct."""

    _fields_ = [("cName", ctypes.c_char * OPTLEN),
                ("cDescr", ctypes.c_char * OPTDESCR),
                ("cLongDescr", ctypes.c_char * OPTLONDESCR),
                ("cValues", ctypes.c_char * OPTDESCR),
                ("iType", ctypes.c_int),
                ("cDefault", ctypes.c_char * OPTDESCR),
                ("dDefault", ctypes.c_double),
                ("iModuleBit", ctypes.c_int),
                ("iMultiFile", ctypes.c_int),
                ("iMultiIn", ctypes.c_int),
                ("iLine", ctypes.POINTER(ctypes.c_double)),
                ("iFile", ctypes.POINTER(ctypes.c_int)),
                ("cFile", (ctypes.c_char * MAXFILES) * OPTLEN),
                ("bNeg", ctypes.c_int),
                ("cNeg", ctypes.c_char * OPTDESCR),
                ("iFileType", ctypes.c_int),
                ("dNeg", ctypes.c_double)]

    def __init__(self, **kwargs):
        pass


lib = ctypes.CDLL(os.path.join(os.path.dirname(
                  os.path.dirname(os.path.abspath(__file__))),
    'vplanetlib.so'))


_HelpOptions = lib.HelpOptions
_HelpOptions.argtypes = [ctypes.POINTER(OPTIONS) * MODULEOPTEND, ctypes.c_int]

options = (ctypes.POINTER(OPTIONS) * MODULEOPTEND)()


_InitializeOptions = lib.InitializeOptions
_InitializeOptions.argtypes = [ctypes.POINTER(OPTIONS) * MODULEOPTEND,
                               fnReadOption]

fnRead = (ctypes.POINTER(fnReadOption) * MODULEOPTEND)()


_InitializeOptions(options, fnRead)

_HelpOptions(options, 0)
