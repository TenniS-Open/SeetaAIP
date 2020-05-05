#!python
# coding: utf-8

from ctypes import *
from ctypes.util import find_library
from . import libfinder
from typing import List

kernel32Name = "kernel32"
libTennis = find_library(kernel32Name)

kernel32 = None
if libTennis is not None:
    try:
        kernel32 = CDLL(libTennis, RTLD_GLOBAL)
    except:
        raise


msg = None
if kernel32 is None:
    from . import libfinder
    kernel32, msg = libfinder.load_library(kernel32Name)


if kernel32 is None:
    if msg is None:
        raise ImportError("Can find library: {}".format(kernel32Name))
    else:
        raise ImportError(msg)


def __KERNEL32_IMPORT(sym, restype=None, *argtypes):
    # type: (str, object, List[object]) -> callable
    if not hasattr(kernel32, sym):
        import sys
        restype_str = "void" if restype is None else restype.__name__
        argtypes_str = ["void" if t is None else t.__name__ for t in argtypes]
        argtypes_str = ", ".join(argtypes_str)
        message = "Can not open sym: {} {}({})\n".format(restype_str, sym, argtypes_str)
        sys.stderr.write("[ERROR]: {}\n".format(message))
        raise Exception(message)
    f = getattr(kernel32, sym)
    f.argtypes = argtypes
    f.restype = restype
    return f


LoadLibrary = __KERNEL32_IMPORT("LoadLibrary", POINTER(c_int32), c_char_p)
GetProcAddress = __KERNEL32_IMPORT("GetProcAddress", c_void_p, POINTER(c_int32), c_char_p)
FreeLibrary = __KERNEL32_IMPORT("FreeLibrary", c_int32, POINTER(c_int32))


BYTE = 0        # byte type
FLOAT32 = 1     # float type with 4-bytes
INT32 = 2       # signed integer type with 4-bytes
FLOAT64 = 3     # signed float type with 8-bytes


FORMAT_U8RAW = 0      # byte type
FORMAT_F32RAW = 1     # float type with 4-bytes
FORMAT_I32RAW = 2     # signed integer type with 4-bytes
FORMAT_U8RGB = 1001   # byte format for RGB888
FORMAT_U8BGR = 1002   # byte format for BGR888
FORMAT_U8RGBA = 1003  # byte format for RGBA8888
FORMAT_U8BGRA = 1004  # byte format for BGRA8888
FORMAT_U8Y = 1005     # byte format for gray image

'''
* Unknown shape
'''
SHAPE_UNKNOWN = 0

'''
* with forced rotate=0, scale=1, size>=1
* points represents points
'''
SHAPE_POINTS = 1

'''
* with forced rotate=0, scale=1, size>=2
* points represents multi lines:
*     points[0]->points[1], points[1]->points[2], ..., points[size-2]->points[size-1]
'''
SHAPE_LINES = 2

'''
* with forced scale=1, size=2
* rotate represents the angle of rotation around the center point.
* points[0] represents the left-top corner
* points[1] represents the right-bottom corner
'''
SHAPE_RECTANGLE = 3

'''
* with forced rotate=0, scale=1, size=3
* rotate represents the angle of rotation around the center point.
* points represents the first 3 points of parallelogram with anticlockwise
    '''
SHAPE_PARALLELOGRAM = 4

'''
* with forced rotate=0, scale=1, size>=2
* points represents multi lines with anticlockwise:
    *     points[0]->points[1], points[1]->points[2], ...,
*     points[size-2]->points[size-1], points[size-1]->points[0]
'''
SHAPE_POLYGON = 5

'''
* with forced rotate=0, size=1
* scale represents the radius
* points[0] represents the center
'''
SHAPE_CIRCLE = 6

'''
* with forced rotate=0, scale=1, size=3
* points[0] represents the left-top-front corner
* points[1] represents the right-bottom-front corner
* points[2] represents the right-top-back corner
'''
SHAPE_CUBE = 7

'''
* Means the shape is undefined
'''
SHAPE_NONE = 255


SHAPE_LOAD_SUCCEED = 0
SHAPE_LOAD_SIZE_NOT_ENOUGH = 0xf001    # once this error return the wanted version will be set.
SHAPE_LOAD_UNHANDLED_INTERNAL_ERROR = 0xf002   # for unknown load failed, no more information.
SHAPE_LOAD_AIP_VERSION_MISMATCH = 0xf003   # for AIP version mismatched.


class Point(Structure):
    _fields_ = [
        ("x", c_float),
        ("y", c_float),
    ]


class Shape(Structure):
    class Landmarks(Structure):
        _fields_ = [
            ("data", POINTER(Point)),
            ("size", c_uint32),
        ]

    _fields_ = [
        ("type", c_int32),
        ("landmarks", Landmarks),
        ("rotate", c_float),
        ("scale", c_float),
    ]


class Tag(Structure):
    _fields_ = [
        ("label", c_int32),
        ("score", c_float),
    ]


class Tensor(Structure):
    class Dims(Structure):
        _fields_ = [
            ("data", POINTER(c_uint32)),
            ("size", c_uint32),
        ]

    _fields_ = [
        ("type", c_int32),
        ("data", c_void_p),
        ("dims", Dims),
    ]


class Object(Structure):
    class TagArray(Structure):
        _fields_ = [
            ("data", POINTER(Tag)),
            ("size", c_uint32),
        ]

    _fields_ = [
        ("shape", Shape),
        ("tags", TagArray),
        ("extra", Tensor),
    ]


class Device(Structure):
    _fields_ = [
        ("type", c_char_p),
        ("id", c_int32),
    ]


class ImageData(Structure):
    _fields_ = [
        ("format", c_int32),
        ("data", c_void_p),
        ("number", c_uint32),
        ("height", c_uint32),
        ("width", c_uint32),
        ("channels", c_uint32),
    ]


seeta_aip_error = CFUNCTYPE(c_char_p, c_void_p, c_int32)
seeta_aip_create = CFUNCTYPE(c_int32, POINTER(c_void_p), POINTER(Device), POINTER(c_char_p), POINTER(Object), c_uint32)
seeta_aip_free = CFUNCTYPE(c_int32, c_void_p)
seeta_aip_reset = CFUNCTYPE(c_int32, c_void_p)
seeta_aip_forward = CFUNCTYPE(c_int32, c_void_p, c_uint32,
                              POINTER(ImageData), c_uint32,
                              POINTER(Object), c_uint32,
                              POINTER(POINTER(Object)), POINTER(c_uint32),
                              POINTER(POINTER(ImageData)), POINTER(c_uint32))
seeta_aip_property = CFUNCTYPE(POINTER(c_char_p), c_void_p)
seeta_aip_setd = CFUNCTYPE(c_int32, c_void_p, c_char_p, c_double)
seeta_aip_getd = CFUNCTYPE(c_int32, c_void_p, c_char_p, POINTER(c_double))
seeta_aip_tag = CFUNCTYPE(c_char_p, c_void_p, c_uint32, c_uint32, c_int32)
seeta_aip_set = CFUNCTYPE(c_int32, c_void_p, c_char_p, POINTER(Object))
seeta_aip_get = CFUNCTYPE(c_int32, c_void_p, c_char_p, POINTER(Object))


class Package(Structure):
    _fields_ = [
        ("aip_version", c_int32),
        ("module", c_char_p),
        ("description", c_char_p),
        ("mID", c_char_p),
        ("sID", c_char_p),
        ("version", c_char_p),
        ("support", POINTER(c_char_p)),
        ("error", seeta_aip_error),
        ("create", seeta_aip_create),
        ("free", seeta_aip_free),
        ("property", seeta_aip_property),
        ("getd", seeta_aip_getd),
        ("setd", seeta_aip_setd),
        ("reset", seeta_aip_reset),
        ("forward", seeta_aip_forward),
        ("tag", seeta_aip_tag),
        ("get", seeta_aip_get),
        ("set", seeta_aip_set),
    ]


seeta_aip_load_entry = CFUNCTYPE(c_int32, POINTER(Package), c_uint32)


class DynamicLibrary(object):
    def __init__(self, libname):
        assert isinstance(libname, str)
        self.lib = LoadLibrary(libname.encode())
        if not self.lib:
            raise ImportError("Can find library: {}".format(libname))

    def __del__(self):
        if self.lib is not None:
            FreeLibrary(self.lib)
            self.lib = None

    def dispose(self):
        self.__del__()

    def symbol(self, func, name):
        assert isinstance(name, str)
        assert isinstance(func, type(seeta_aip_load_entry))
        sym = GetProcAddress(self.lib, name.encode())
        if not sym:
            return None
        return func(int(sym))
