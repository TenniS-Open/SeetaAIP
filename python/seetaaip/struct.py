#!python
# coding: utf-8

from . import _C
from typing import List, Optional, Union, Iterable, Any, Tuple, Sized
import numpy
from . import dtype as aip_dtype

import sys
if sys.version > '3':
    pass
else:
    raise Exception("\"{}\" only support python3.".format(__file__))


def _to_ctypes_array(array, dtype):
    ctypes_dtype = aip_dtype.to_ctypes(dtype)
    numpy_dtype = aip_dtype.to_numpy_dtype(dtype)
    np_array = numpy.ascontiguousarray(array, dtype=numpy_dtype)
    c_len = len(np_array)
    c_array = np_array.ctypes.data_as(_C.POINTER(ctypes_dtype))
    return c_array, c_len, np_array


class Device(object):
    def __init__(self, type: str = "cpu", id: int = 0):
        type = type.encode()
        self.__raw = _C.Device(type, id)

    @property
    def device(self) -> str:
        return self.__raw.type.decode()

    @property
    def id(self) -> int:
        return int(self.__raw.id)

    @property
    def _as_parameter_(self) -> _C.POINTER(_C.Device):
        return _C.byref(self.__raw)

    @property
    def raw(self) -> _C.POINTER(_C.Device):
        return self._as_parameter_

    @property
    def ref(self) -> _C.Device:
        return self.__raw

    def __str__(self):
        return "Device{{type='{}', id={}}}".format(self.device, self.id)


class Tensor(object):
    def __init__(self, obj: Union[_C.POINTER(_C.Tensor), _C.Tensor, numpy.ndarray, Iterable, int, float] = None,
                 dtype: Union[int, Any, type(int), type(float)]=None, shape: Iterable[int] = None):
        self.__type = _C.VOID
        self.__dims = []
        self.__numpy = None  # numpy

        self.__raw = None   # _C.Tensor; use for import and export
        self.__raw_dims = None  # numpy.ndarray; usr for export

        if obj is None:
            if dtype is None and shape is None:
                return
            if shape is None:
                shape = []
            if dtype is None:
                dtype = _C.FLOAT32
            dtype = self._check_dtype_aip(dtype)
            self.__type = dtype
            self.__dims = tuple(shape)
            self.__numpy = numpy.zeros(self.__dims, self._check_dtype_numpy(dtype))
        elif isinstance(obj, Tensor):
            self.__type = obj.__type
            self.__dims = obj.__dims
            self.__numpy = obj.__numpy
            # self.__raw = obj.__raw
        elif isinstance(obj, _C.POINTER(_C.Tensor)):
            assert hasattr(obj, "contents")
            contents = obj.contents
            assert isinstance(contents, _C.Tensor)
            self.__raw = contents
            self._import_raw()
        elif isinstance(obj, _C.Tensor):
            self.__raw = obj
            self._import_raw()
        else:
            if dtype is None:
                self.__numpy = numpy.ascontiguousarray(obj)
                self.__numpy = numpy.ascontiguousarray(self.__numpy,
                                                       dtype=self._narraw_numpy_dtype(self.__numpy.dtype.type))
            else:
                self.__numpy = numpy.ascontiguousarray(obj, dtype=dtype)
            self.__type = self._check_dtype_aip(self.__numpy.dtype.type)
            if shape is not None:
                self.__numpy = numpy.reshape(self.__numpy, shape)
            self.__dims = self.__numpy.shape

    def _import_raw(self):
        c_data = self.__raw.data
        if not c_data:
            return
        c_type = self.__raw.type
        if int(c_type) == _C.VOID:
            return
        c_dims = self.__raw.dims
        dims = [int(c_dims.data[i]) for i in range(int(c_dims.size))]

        c_dtype_data = _C.cast(c_data, _C.POINTER(self._to_ctype(int(c_type))))
        self.__numpy = numpy.ctypeslib.as_array(c_dtype_data, shape=tuple(dims))
        self.__type = int(c_type)
        self.__dims = tuple(dims)

    def _to_ctype(self, dtype: int) -> Any:
        dtype_map = {
            _C.BYTE: _C.c_uint8,
            _C.INT32: _C.c_int32,
            _C.FLOAT32: _C.c_float,
            _C.FLOAT64: _C.c_double,
        }
        return dtype_map[dtype]

    def _narraw_numpy_dtype(self, dtype: Any):
        dtype_map = {
            numpy.uint8: numpy.uint8,
            numpy.int32: numpy.int32,
            numpy.float32: numpy.float32,
            numpy.float64: numpy.float64,
            numpy.int8: numpy.int32,
            numpy.uint32: numpy.int32,
            numpy.int16: numpy.int32,
            numpy.uint16: numpy.int32,
            numpy.int64: numpy.int32,
            numpy.uint64: numpy.int32,
        }
        if dtype not in dtype_map:
            raise Exception("Not supported numpy.dtype={}".format(str(dtype.__name__)))

        return dtype_map[dtype]

    def _check_dtype_aip(self, dtype: Union[int, Any]) -> int:
        if dtype is int or dtype == int:
            return _C.INT32
        if dtype is float or dtype == float:
            return _C.FLOAT32
        if isinstance(dtype, int):
            assert dtype in {_C.BYTE, _C.INT32, _C.FLOAT32, _C.FLOAT64}, \
                "Not supported converting aip dtype = {}".format(dtype)
            return dtype
        # assert isinstance(dtype, numpy.generic)
        assert dtype in {numpy.uint8, numpy.int32, numpy.float32, numpy.float64}, \
            "Not supported numpy.dtype={}".format(str(dtype.__name__))
        return aip_dtype.from_numpy(dtype)

    def _check_dtype_numpy(self, dtype: Union[int, Any]) -> Any:
        if dtype is int or dtype == int:
            return numpy.int32
        if dtype is float or dtype == float:
            return numpy.float32
        if isinstance(dtype, int):
            return aip_dtype.to_numpy(dtype)
        # assert isinstance(dtype, numpy.generic)
        assert dtype in {numpy.uint8, numpy.int32, numpy.float32, numpy.float64}, \
            "Not supported numpy.dtype={}".format(str(dtype.__name__))
        return dtype

    def empty(self) -> bool:
        return self.__numpy is not None

    def __bool__(self):
        return not self.empty()

    @property
    def type(self) -> int:
        return self.__type

    @property
    def dims(self) -> Tuple[int]:
        return self.__dims

    @property
    def shape(self) -> Tuple[int]:
        return self.__dims

    @property
    def data(self) -> Optional[numpy.ndarray]:
        return self.__numpy

    @property
    def _as_parameter_(self) -> _C.POINTER(_C.Tensor):
        return _C.byref(self.ref)

    @property
    def raw(self) -> _C.POINTER(_C.Tensor):
        return _C.byref(self.ref)

    @property
    def ref(self) -> _C.Tensor:
        self.__raw = _C.Tensor(type=_C.BYTE)
        if self.__numpy is not None:
            self.__raw_dims = numpy.ascontiguousarray(self.dims, dtype=numpy.uint32)
            c_dims_data = self.__raw_dims.ctypes.data_as(_C.POINTER(_C.c_uint32))
            c_dims_size = _C.c_uint32(len(self.dims))
            c_dims = _C.Tensor.Dims(data=c_dims_data, size=c_dims_size)
            c_data = self.data.ctypes.data_as(_C.c_void_p)
            self.__raw.type = _C.c_int32(self.type)
            self.__raw.data = c_data
            self.__raw.dims = c_dims
        return self.__raw

    def __repr__(self):
        return self.__numpy.__repr__()

    def __str__(self):
        return self.__numpy.__str__()

    def __nonzero__(self) -> bool:
        return not self.empty()

    def __array__(self) -> numpy.ndarray:
        return self.__numpy

    @property
    def numpy(self) -> numpy.ndarray:
        return self.__numpy


class Object(object):
    # TOOD: finish Object
    pass


class Package(object):
    def __init__(self, obj: _C.Package):
        self.__raw = obj

    @property
    def aip_version(self) -> int:
        return int(self.__raw.aip_version) if self.__raw.aip_version else ""

    @property
    def module(self) -> str:
        return self.__raw.module.decode() if self.__raw.module else ""

    @property
    def description(self) -> str:
        return self.__raw.description.decode() if self.__raw.description else ""

    @property
    def mID(self) -> str:
        return self.__raw.mID.decode() if self.__raw.mID else ""

    @property
    def sID(self) -> str:
        return self.__raw.sID.decode() if self.__raw.sID else ""

    @property
    def version(self) -> str:
        return self.__raw.version.decode() if self.__raw.version else ""

    @property
    def support(self) -> List[str]:
        raw_support = self.__raw.support
        result = []
        i = 0
        while True:
            p = raw_support[i]
            if not p:
                break
            result.append(p.decode())
            i += 1
        return result

    @property
    def _as_parameter_(self) -> _C.POINTER(_C.Package):
        return _C.byref(self.__raw)

    @property
    def raw(self) -> _C.POINTER(_C.Package):
        return self._as_parameter_

    @property
    def ref(self) -> _C.Package:
        return self.__raw

    class Handle(object):
        def __init__(self, handle: _C.c_void_p = None):
            if handle is None:
                handle = _C.c_void_p(0)
            self.__handle = handle

        def _as_parameter_(self) -> _C.c_void_p:
            return self.handle

        @property
        def handle(self) -> _C.c_void_p:
            return self.__handle

    def error(self, handle: Optional[Handle], errcode: int) -> str:
        assert isinstance(handle, (type(None), Package.Handle))
        ptr = handle.handle if handle else _C.c_void_p(0)
        message = self.__raw.error(ptr, errcode)
        if not message:
            return ""
        return message.decode()

    def free(self, handle: Optional[Handle]):
        assert isinstance(handle, (type(None), Package.Handle))
        ptr = handle.handle if handle else _C.c_void_p(0)
        self.__raw.free(ptr)

    def create(self, device: Device,
               models: Union[Sized, Iterable[str]], objects: Optional[Union[Sized, Iterable[Object]]]) -> Handle:
        assert isinstance(device, Device)

        c_handle = _C.c_void_p(0)
        c_phandle = _C.byref(c_handle)
        c_device = device.raw
        c_models_tmp = [_C.c_char_p(m.encode()) for m in models]
        c_models_tmp.append(None)
        c_models = (_C.c_char_p * len(c_models_tmp))(*c_models_tmp)
        if objects is None:
            c_objects = None
            c_objects_size = 0
        else:
            c_objects_tmp = [o.raw for o in objects]
            c_objects = (_C.POINTER(_C.Object) * len(c_objects_tmp))(*c_objects_tmp)
            c_objects_size = len(c_objects_tmp)

        c_errcode = self.__raw.create(c_phandle, c_device, c_models, c_objects, c_objects_size)
        if c_errcode != 0:
            raise Exception("0x{}: {}".format(c_errcode, self.error(None, c_errcode)))

        return Package.Handle(c_handle)


class Engine(object):
    AIP_VERSION = 2

    def __init__(self, libname: str):
        self.__lib = _C.DynamicLibrary(libname=libname)
        seeta_aip_load = self.__lib.symbol(_C.seeta_aip_load_entry, "seeta_aip_load")
        c_package = _C.Package()
        c_package_size = _C.sizeof(_C.Package)
        errcode = seeta_aip_load(_C.byref(c_package), c_package_size)
        load_error_map = {
            _C.SHAPE_LOAD_SUCCEED: "Succeed.",
            _C.SHAPE_LOAD_SIZE_NOT_ENOUGH: "Size not enough.",
            _C.SHAPE_LOAD_UNHANDLED_INTERNAL_ERROR: "Unhandled internal error.",
            _C.SHAPE_LOAD_AIP_VERSION_MISMATCH: "AIP version mismatch.",
        }
        if errcode != 0:
            message = load_error_map[errcode] if errcode in load_error_map else "Unknown error."
            raise Exception("Load {} failed with errorcode({}): {}".format(libname, hex(errcode), message))
        if c_package.aip_version != self.AIP_VERSION:
            raise Exception("AIP version not supported, {} wanted, got {}".format(
                self.AIP_VERSION, c_package.aip_version))
        self.__package = Package(c_package)

    def __del__(self):
        if self.__lib is not None:
            self.__package = None
            self.__lib.dispose()
            self.__lib = None

    def dispose(self):
        self.__del__()

    @property
    def package(self) -> Package:
        return self.__package


class Instance(object):
    def __init__(self, lib: Union[str, Engine, Package],
                 device: Device,
                 models: Union[str, Iterable[str]],
                 objects: Union[Object, Iterable[Object]] = None):
        self.__libname = None
        self.__engine = None
        self.__package = None
        self.__handle = None

        assert isinstance(lib, (str, Engine, Package))
        if isinstance(lib, str):
            self.__libname = lib
            self.__engine = Engine(self.__libname)
            self.__package = self.__engine.package
        elif isinstance(lib, Engine):
            self.__package = lib.package
        elif isinstance(lib, Package):
            self.__package = lib

        if isinstance(models, str):
            models = [models]
        if isinstance(objects, Object):
            objects = [objects]

        self.__handle = self.__package.create(device=device, models=models, objects=objects)

    def __del__(self):
        self.dispose()

    def dispose(self):
        if self.__handle is not None:
            self.__package.free(self.__handle)
            self.__handle = None
            self.__package = None
        if self.__engine is not None:
            self.__engine.dispose()
            self.__engine = None

