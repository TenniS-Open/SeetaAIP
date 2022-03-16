#!/usr/bin/env python

"""
:author Kier
"""

import platform
import os
import sys
from ctypes.util import find_library
from ctypes import CDLL


def safeCDLL(libpath):
    try:
        lib = CDLL(libpath)
        return lib, ""
    except Exception as e:
        return None, str(e)


def load_library(libname):
    # type: (str) -> (CDLL, str)
    """
    :param libname:
    :return: lib, msg
    if lib is None and msg is None, No file found
    if lib is None and msg is str, there is exception when open library
    """
    # try load lib form file
    CWD = os.getcwd()
    CFD = os.path.abspath(os.path.dirname(__file__))
    PATH = os.environ["PATH"] if "PATH" in os.environ else ""
    LIBRARTY_PATH = os.environ["LIBRARTY_PATH"] if "LIBRARTY_PATH" in os.environ else ""
    LD_LIBRARY_PATH = os.environ["LD_LIBRARY_PATH"] if "LD_LIBRARY_PATH" in os.environ else ""
    DYLD_LIBRARY_PATH = os.environ["DYLD_LIBRARY_PATH"] if "DYLD_LIBRARY_PATH" in os.environ else ""
    SYS_PATH = type(sys.path)(sys.path)

    sep = ";" if platform.system() == "Windows" else ":"

    ALL_PATH = [CWD, CFD]
    ALL_PATH += LIBRARTY_PATH.split(sep)
    ALL_PATH += LD_LIBRARY_PATH.split(sep)
    ALL_PATH += DYLD_LIBRARY_PATH.split(sep)
    ALL_PATH += PATH.split(sep)
    ALL_PATH += SYS_PATH

    ALL_PATH = [path for path in ALL_PATH if os.path.isdir(path)]

    ALL_PATH_ENV = sep.join(ALL_PATH)

    os.environ["PATH"] = ALL_PATH_ENV
    os.environ["LIBRARTY_PATH"] = ALL_PATH_ENV
    os.environ["LD_LIBRARY_PATH"] = ALL_PATH_ENV
    os.environ["DYLD_LIBRARY_PATH"] = ALL_PATH_ENV
    # sys.path = ALL_PATH

    lib = None
    msg = None
    libpath = find_library(libname)
    if libpath is None:
        libpath = "%s.dll" % libname if platform.system() == "Windows" else "lib%s.so" % libname
    if libpath.find("/") >= 0 or libpath.find("\\") >= 0:
        sys.stderr.write("Find {} in: {}\n".format(libname, libpath))
        lib, msg = safeCDLL(libpath)
    else:
        for root in ALL_PATH:
            fullpath = os.path.join(root, libpath)
            if not os.path.isfile(fullpath): continue
            lib, msg = safeCDLL(fullpath)
            if lib is not None:
                sys.stderr.write("Find {} in: {}\n".format(libname, fullpath))
                msg = None
                break
            else:
                break
                # sys.stderr.write("{}\n".format(msg))

    os.environ["PATH"] = PATH
    os.environ["LIBRARTY_PATH"] = LIBRARTY_PATH
    os.environ["LD_LIBRARY_PATH"] = LD_LIBRARY_PATH
    os.environ["DYLD_LIBRARY_PATH"] = DYLD_LIBRARY_PATH
    # sys.path = SYS_PATH

    return lib, msg

class _cd_(object):
    def __init__(self, path):
        self.__cwd = os.getcwd()
        if os.path.isdir(path):
            os.chdir(path)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        os.chdir(self.__cwd)

class _fuse_env_(object):
    def __init__(self):
        CWD = os.getcwd()
        CFD = os.path.abspath(os.path.dirname(__file__))
        PATH = os.environ["PATH"] if "PATH" in os.environ else ""
        LIBRARTY_PATH = os.environ["LIBRARTY_PATH"] if "LIBRARTY_PATH" in os.environ else ""
        LD_LIBRARY_PATH = os.environ["LD_LIBRARY_PATH"] if "LD_LIBRARY_PATH" in os.environ else ""
        DYLD_LIBRARY_PATH = os.environ["DYLD_LIBRARY_PATH"] if "DYLD_LIBRARY_PATH" in os.environ else ""
        SYS_PATH = type(sys.path)(sys.path)

        sep = ";" if platform.system() == "Windows" else ":"

        ALL_PATH = [CWD, CFD]
        ALL_PATH += LIBRARTY_PATH.split(sep)
        ALL_PATH += LD_LIBRARY_PATH.split(sep)
        ALL_PATH += DYLD_LIBRARY_PATH.split(sep)
        ALL_PATH += PATH.split(sep)
        ALL_PATH += SYS_PATH

        ALL_PATH = [path for path in ALL_PATH if os.path.isdir(path)]

        ALL_PATH_ENV = sep.join(ALL_PATH)

        os.environ["PATH"] = ALL_PATH_ENV
        os.environ["LIBRARTY_PATH"] = ALL_PATH_ENV
        os.environ["LD_LIBRARY_PATH"] = ALL_PATH_ENV
        os.environ["DYLD_LIBRARY_PATH"] = ALL_PATH_ENV

        self.PATH = PATH
        self.LIBRARTY_PATH = LIBRARTY_PATH
        self.LD_LIBRARY_PATH = LD_LIBRARY_PATH
        self.DYLD_LIBRARY_PATH = DYLD_LIBRARY_PATH

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        os.environ["PATH"] = self.PATH
        os.environ["LIBRARTY_PATH"] = self.LIBRARTY_PATH
        os.environ["LD_LIBRARY_PATH"] = self.LD_LIBRARY_PATH
        os.environ["DYLD_LIBRARY_PATH"] = self.DYLD_LIBRARY_PATH


def load_native_library(libname, loader):
    # type: (str, Callable) -> (any)
    # check system
    mac_ver, _, _ = platform.mac_ver(None)
    if mac_ver is not None:
        prefix = 'lib'
        suffix = '.dylib'
    elif platform.system() == "Windows":
        prefix = 'lib'
        suffix = '.dll'
    else:
        prefix = 'lib'
        suffix = ".so"

    def loader_v2(r, n, p, s):
        path = p + n + s
        if len(r) > 0:
            path = os.path.join(r, path)
        return loader(path.encode())

    # start try
    head, tail = os.path.split(libname)

    with _fuse_env_():
        with _cd_(head):
            return loader(libname.encode()) or \
                   loader_v2(head, tail, prefix, suffix) or \
                   loader_v2(head, tail, "", suffix) or \
                   loader_v2(head, tail, prefix, "") or \
                   loader_v2("", tail, prefix, suffix) or \
                   loader_v2("", tail, "", suffix) or \
                   loader_v2("", tail, prefix, "") or \
                   loader_v2("", tail, "", "") or \
                   loader(libname.encode())
