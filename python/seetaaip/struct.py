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
        return _C.pointer(self.__raw)

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
                self.__numpy = numpy.ascontiguousarray(obj, dtype=self._check_dtype_numpy(dtype))
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
        return _C.pointer(self.ref)

    @property
    def raw(self) -> _C.POINTER(_C.Tensor):
        return self._as_parameter_

    @property
    def ref(self) -> _C.Tensor:
        self.__raw = _C.Tensor(type=_C.VOID, data=None, dims=_C.Tensor.Dims(data=None, size=0))
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

    def __getitem__(self, item):
        return self.__numpy[item]

    def __setitem__(self, key, value):
        self.__numpy[key] = value


class ImageData(object):
    def __init__(self, obj: Union[_C.POINTER(_C.Tensor), _C.Tensor, numpy.ndarray, Iterable, int, float] = None,
                 fmt: int = None, shape: Iterable[int] = None,
                 number: int = None, width: int = None, height: int = None, channels: int = None):
        assert isinstance(fmt, (int, type(None)))
        if fmt is not None:
            assert fmt in {
                _C.FORMAT_U8RAW,
                _C.FORMAT_U8Y,
                _C.FORMAT_U8RGB,
                _C.FORMAT_U8BGR,
                _C.FORMAT_U8RGBA,
                _C.FORMAT_U8BGRA,
                _C.FORMAT_I32RAW,
                _C.FORMAT_F32RAW,
            }

        self.__type = _C.BYTE
        self.__dims = [1, 0, 0, 0]
        self.__numpy = None  # numpy
        self.__fmt = _C.FORMAT_U8RAW

        self.__raw = None   # _C.Tensor; use for import and export
        self.__raw_dims = None  # numpy.ndarray; usr for export

        # check shape
        if shape is not None:
            assert number is None and width is None and height is None and channels is None, \
                "Can not set both shape and number, width, height, channels in same time"
            shape = list(shape)
            assert 2 <= len(shape) <= 4
            if len(shape) == 2:
                shape = [1, shape[0], shape[1], 1]
            if len(shape) == 3:
                shape = [1, shape[0], shape[1], shape[2]]
        if number is not None or width is not None or height is not None or channels is not None:
            if width is None or height is None:
                raise Exception("number, width, height, channels must be set in same time")
            if number is None:
                number = 1
            if channels is None:
                channels = 3
            shape = [number, height, width, channels]

        if obj is None:
            if fmt is None and shape is None:
                return
            if shape is None:
                shape = []
            if fmt is None:
                fmt = _C.FORMAT_U8RAW
            dtype = self._fmt_to_dtype(fmt)
            self.__fmt = fmt
            self.__type = dtype
            self.__dims = tuple(shape)
            self.__numpy = numpy.zeros(self.__dims, self._check_dtype_numpy(dtype))
        elif isinstance(obj, ImageData):
            self.__fmt = obj.__fmt
            self.__type = obj.__type
            self.__dims = obj.__dims
            self.__numpy = obj.__numpy
            # self.__raw = obj.__raw
        elif isinstance(obj, _C.POINTER(_C.ImageData)):
            assert hasattr(obj, "contents")
            contents = obj.contents
            assert isinstance(contents, _C.ImageData)
            self.__raw = contents
            self._import_raw()
        elif isinstance(obj, _C.ImageData):
            self.__raw = obj
            self._import_raw()
        else:
            if fmt is None:
                self.__numpy = numpy.ascontiguousarray(obj)
                self.__numpy = numpy.ascontiguousarray(self.__numpy,
                                                       dtype=self._narraw_numpy_dtype(self.__numpy.dtype.type))
                dtype = self._check_dtype_aip(self.__numpy.dtype.type)
                fmt = self._dtype_to_fmt(dtype)
            else:
                dtype = self._fmt_to_dtype(fmt)
                self.__numpy = numpy.ascontiguousarray(obj, dtype=self._check_dtype_numpy(dtype))
            self.__type = self._check_dtype_aip(self.__numpy.dtype.type)
            self.__fmt = fmt
            if shape is not None:
                self.__numpy = numpy.reshape(self.__numpy, shape)
            else:
                shape = self.__numpy.shape
                assert 2 <= len(shape) <= 4
                if len(shape) == 2:
                    shape = [1, shape[0], shape[1], 1]
                if len(shape) == 3:
                    shape = [1, shape[0], shape[1], shape[2]]
                self.__numpy = numpy.reshape(self.__numpy, shape)
            self.__dims = self.__numpy.shape

    def _import_raw(self):
        c_data = self.__raw.data
        if not c_data:
            return
        c_fmt = self.__raw.format
        c_type = self._fmt_to_dtype(c_fmt)

        c_dims = [self.__raw.number, self.__raw.height, self.__raw.width, self.__raw.channels]
        dims = [int(c_dims[i]) for i in range(len(c_dims))]

        c_dtype_data = _C.cast(c_data, _C.POINTER(self._to_ctype(int(c_type))))
        self.__numpy = numpy.ctypeslib.as_array(c_dtype_data, shape=tuple(dims))
        self.__type = int(c_type)
        self.__fmt = int(c_fmt)
        self.__dims = tuple(dims)

    def _to_ctype(self, dtype: int) -> Any:
        dtype_map = {
            _C.BYTE: _C.c_uint8,
            _C.INT32: _C.c_int32,
            _C.FLOAT32: _C.c_float,
            _C.FLOAT64: _C.c_double,
        }
        return dtype_map[dtype]

    def _fmt_to_dtype(self, fmt: int) -> int:
        fmt_map = {
            _C.FORMAT_U8RAW: _C.BYTE,
            _C.FORMAT_U8Y: _C.BYTE,
            _C.FORMAT_U8RGB: _C.BYTE,
            _C.FORMAT_U8BGR: _C.BYTE,
            _C.FORMAT_U8RGBA: _C.BYTE,
            _C.FORMAT_U8BGRA: _C.BYTE,
            _C.FORMAT_I32RAW: _C.INT32,
            _C.FORMAT_F32RAW: _C.FLOAT32,
        }
        return fmt_map[fmt]

    def _dtype_to_fmt(self, dtype: int) -> int:
        dtype_map = {
            _C.BYTE: _C.FORMAT_U8RAW,
            _C.INT32: _C.FORMAT_I32RAW,
            _C.FLOAT32: _C.FORMAT_F32RAW,
        }
        return dtype_map[dtype]

    def _narraw_numpy_dtype(self, dtype: Any):
        dtype_map = {
            numpy.uint8: numpy.uint8,
            numpy.int32: numpy.int32,
            numpy.float32: numpy.float32,
            numpy.float64: numpy.float32,   # image not support float64
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
    def fmt(self) -> int:
        return self.__fmt

    @property
    def dims(self) -> Tuple[int]:
        return self.__dims

    @property
    def shape(self) -> Tuple[int]:
        return self.__dims

    @property
    def number(self) -> int:
        return self.__dims[0]

    @property
    def height(self) -> int:
        return self.__dims[1]

    @property
    def width(self) -> int:
        return self.__dims[2]

    @property
    def channels(self) -> int:
        return self.__dims[3]

    @property
    def data(self) -> Optional[numpy.ndarray]:
        return self.__numpy

    @property
    def _as_parameter_(self) -> _C.POINTER(_C.ImageData):
        return _C.pointer(self.ref)

    @property
    def raw(self) -> _C.POINTER(_C.ImageData):
        return self._as_parameter_

    @property
    def ref(self) -> _C.ImageData:
        self.__raw = _C.ImageData(format=_C.FORMAT_U8RAW, data=None,
                                  number=1, height=0, width=0, channels=0)
        if self.__numpy is not None:
            self.__raw_dims = numpy.ascontiguousarray(self.dims, dtype=numpy.uint32)
            c_dims_data = self.__raw_dims.ctypes.data_as(_C.POINTER(_C.c_uint32))
            c_dims_size = _C.c_uint32(len(self.dims))
            c_dims = _C.Tensor.Dims(data=c_dims_data, size=c_dims_size)
            c_data = self.data.ctypes.data_as(_C.c_void_p)
            self.__raw.format = _C.c_int32(self.fmt)
            self.__raw.data = c_data
            self.__raw.number = _C.c_uint32(self.number)
            self.__raw.height = _C.c_uint32(self.height)
            self.__raw.width = _C.c_uint32(self.width)
            self.__raw.channels = _C.c_uint32(self.channels)
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

    def __getitem__(self, item):
        return self.__numpy[item]

    def __setitem__(self, key, value):
        self.__numpy[key] = value


class Point(_C.Point):
    def __init__(self, x=0, y=0):
        super(Point, self).__init__(x=x, y=y)

    def __str__(self):
        return "({}, {})".format(self.x, self.y)

    def __repr__(self):
        return "({}, {})".format(self.x, self.y)

    @staticmethod
    def FromRaw(raw: Union[_C.POINTER(_C.Point), _C.Point]):
        if isinstance(raw, _C.POINTER(_C.Point)):
            raw = raw.contents
        return Point(raw.x, raw.y)


class Shape(object):
    def __init__(self, shape_type: int, landmarks: Iterable[Union[Point, Tuple]],
                 rotate: float = 0, scale: float = 1):
        self.__shape_type = shape_type
        self.__landmarks = []
        self.__rotate = rotate
        self.__scale = scale

        self.__raw = None   # _C.Shape

        # check tags
        for point in landmarks:
            if isinstance(point, Point):
                self.__landmarks.append(point)
                continue
            if not isinstance(point, (Tuple, List)):
                raise RuntimeError("param tags must be list of Point or tuple[2]")
            if len(point) != 2:
                raise RuntimeError("param tags must be list of Point or tuple[2]")
            self.__landmarks.append(Point(point[0], point[1]))

    @staticmethod
    def FromRaw(raw: Union[_C.POINTER(_C.Shape), _C.Shape]):
        if isinstance(raw, _C.POINTER(_C.Shape)):
            raw = raw.contents
        shape_type = raw.type
        rotate = raw.rotate
        scale = raw.scale
        c_landmarks = raw.landmarks
        landmarks = [Point.FromRaw(c_landmarks.data[i]) for i in range(c_landmarks.size)]
        return Shape(shape_type, landmarks, rotate=rotate, scale=scale)

    @property
    def type(self):
        return self.__shape_type

    @property
    def scale(self):
        return self.__scale

    @property
    def rotate(self):
        return self.__rotate

    @property
    def landmarks(self):
        return self.__landmarks

    @scale.setter
    def scale(self, value):
        self.__scale = value

    @rotate.setter
    def rotate(self, value):
        self.__rotate = value

    @property
    def _as_parameter_(self) -> _C.POINTER(_C.Shape):
        return _C.pointer(self.ref)

    @property
    def raw(self) -> _C.POINTER(_C.Shape):
        return self._as_parameter_

    @property
    def ref(self) -> _C.Shape:
        landmarks = list(self.__landmarks)
        c_landmarks_data = (_C.Point * len(landmarks))(*landmarks)
        c_landmarks_size = len(landmarks)
        c_landmarks = _C.Shape.Landmarks(data=c_landmarks_data, size=c_landmarks_size)
        self.__raw = _C.Shape(type=self.__shape_type, landmarks=c_landmarks,
                              rotate=self.__rotate, scale=self.__scale)
        return self.__raw

    def __str__(self):
        points = ",".join([str(p) for p in self.__landmarks])
        return "Shape{{type={}, landmarks=[{}], rotate={}, scale={}}}".format(
            self.__shape_type, points, self.__rotate, self.__scale)

    def __repr__(self):
        return self.__str__()


class Tag(_C.Tag):
    def __init__(self, label=0, value=0):
        super(Tag, self).__init__(label=label, value=value)

    def __str__(self):
        return "({}, {})".format(self.label, self.value)

    def __repr__(self):
        return "({}, {})".format(self.label, self.value)

    def __getitem__(self, item):
        assert item in {0, 1}
        if item == 0:
            return self.label
        else:
            return self.value

    def __setitem__(self, key, value):
        assert key in {0, 1}
        if key == 0:
            self.label = value
        else:
            self.value = value

    @staticmethod
    def FromRaw(raw: Union[_C.POINTER(_C.Tag), _C.Tag]):
        if isinstance(raw, _C.POINTER(_C.Tag)):
            raw = raw.contents
        return Tag(raw.label, raw.value)


class Object(object):
    def __init__(self, shape: Shape = None, tags: Iterable[Union[Tag, Tuple]] = None, extra: Union[Any, Tensor] = None):
        assert isinstance(shape, (type(None), Shape))
        if extra is not None and not isinstance(extra, Tensor):
            extra = Tensor(extra)
        if tags is None:
            tags = []

        self.__shape = shape
        self.__extra = extra
        self.__tags = []
        self.__raw = None   # _C.Object

        # check tags
        for tag in tags:
            if isinstance(tag, Tag):
                self.__tags.append(tag)
                continue
            if not isinstance(tag, (Tuple, List)):
                raise RuntimeError("param tags must be list of Tag or tuple[2]")
            if len(tag) != 2:
                raise RuntimeError("param tags must be list of Tag or tuple[2]")
            self.__tags.append(Tag(tag[0], tag[1]))

        self.__tmp_shape = None
        self.__tmp_extra = None

    @staticmethod
    def FromRaw(raw: Union[_C.POINTER(_C.Object), _C.Object]):
        if isinstance(raw, _C.POINTER(_C.Object)):
            raw = raw.contents
        shape = Shape.FromRaw(raw.shape)
        extra = Tensor(raw.extra)
        c_tags = raw.tags
        c_tags_data = c_tags.data
        c_tags_size = c_tags.size
        tags = [Tag.FromRaw(c_tags_data[i]) for i in range(c_tags_size)]
        return Object(shape=shape, tags=tags, extra=extra)

    @property
    def tags(self):
        return self.__tags

    @property
    def shape(self):
        return self.__shape

    @shape.setter
    def shape(self, value):
        assert isinstance(value, (type(None), Shape))
        self.__shape = value

    @property
    def extra(self):
        return self.__extra

    @extra.setter
    def extra(self, value):
        assert isinstance(value, (type(None), Tensor))
        self.__extra = value

    @property
    def _as_parameter_(self) -> _C.POINTER(_C.Object):
        return _C.pointer(self.ref)

    @property
    def raw(self) -> _C.POINTER(_C.Object):
        return self._as_parameter_

    @property
    def ref(self) -> _C.Object:
        tags = list(self.__tags)
        c_tags_data = (_C.Tag * len(tags))(*tags)
        c_tags_size = len(tags)
        c_tags = _C.Object.TagArray(data=c_tags_data, size=c_tags_size)

        self.__tmp_shape = self.__shape
        if self.__tmp_shape is None:
            self.__tmp_shape = Shape(_C.SHAPE_NONE, [])
        self.__tmp_extra = self.__extra
        if self.__tmp_extra is None:
            self.__tmp_extra = Tensor()

        c_shape = self.__tmp_shape.ref
        c_extra = self.__tmp_extra.ref

        self.__raw = _C.Object(shape=c_shape, tags=c_tags, extra=c_extra)
        return self.__raw

    def __str__(self):
        return "Object{{shape={}, tags=[{}], extra={}}}".format(
            self.__shape if self.__shape is not None else "null",
            ",".join([str(tag) for tag in self.__tags]),
            self.__extra if self.__extra is not None else "null",
        )

    def __repr__(self):
        return self.__str__()


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
        return _C.pointer(self.__raw)

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

        def __bool__(self):
            return bool(self.__handle)

        def __nonzero__(self) -> bool:
            return not self.__bool__()

    def error(self, handle: Optional[Handle], errcode: int) -> str:
        assert isinstance(handle, (type(None), Package.Handle))
        c_handle = handle.handle if handle else _C.c_void_p(0)
        message = self.__raw.error(c_handle, errcode)
        if not message:
            return ""
        return message.decode()

    def free(self, handle: Optional[Handle]):
        assert isinstance(handle, (type(None), Package.Handle))
        c_handle = handle.handle if handle else _C.c_void_p(0)
        self.__raw.free(c_handle)

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
            c_objects_tmp = [o.ref for o in objects]
            c_objects = (_C.Object * len(c_objects_tmp))(*c_objects_tmp)
            c_objects_size = len(c_objects_tmp)

        c_errcode = self.__raw.create(c_phandle, c_device, c_models, c_objects, c_objects_size)
        if c_errcode != 0:
            raise Exception("0x{}: {}".format(c_errcode, self.error(None, c_errcode)))

        return Package.Handle(c_handle)

    def reset(self, handle: Optional[Handle]):
        assert isinstance(handle, (type(None), Package.Handle))
        c_handle = handle.handle if handle else _C.c_void_p(0)
        c_errcode = self.__raw.reset(c_handle)
        if c_errcode != 0:
            raise Exception("0x{}: {}".format(c_errcode, self.error(None, c_errcode)))

    def forward(self, handle: Optional[Handle], method_id: int,
                images: Iterable[ImageData], objects: Iterable[Object]) -> Tuple[List[Object], List[ImageData]]:
        assert isinstance(handle, (type(None), Package.Handle))
        c_handle = handle.handle if handle else _C.c_void_p(0)

        c_method_id = _C.c_uint32(method_id)
        c_images_tmp = [i.ref for i in images]
        c_images = (_C.ImageData * len(c_images_tmp))(*c_images_tmp)
        c_images_size = len(c_images_tmp)
        c_objects_tmp = [o.ref for o in objects]
        c_objects = (_C.Object * len(c_objects_tmp))(*c_objects_tmp)
        c_objects_size = len(c_objects_tmp)
        c_out_objects = _C.POINTER(_C.Object)()
        c_out_objects_size = _C.c_uint32(0)
        c_out_images = _C.POINTER(_C.ImageData)()
        c_out_images_size = _C.c_uint32(0)

        c_errcode = self.__raw.forward(c_handle, c_method_id,
                                       c_images, c_images_size, c_objects, c_objects_size,
                                       _C.byref(c_out_objects), _C.byref(c_out_objects_size),
                                       _C.byref(c_out_images), _C.byref(c_out_images_size))
        if c_errcode != 0:
            raise Exception("0x{}: {}".format(c_errcode, self.error(None, c_errcode)))

        out_objects = [Object.FromRaw(c_out_objects[i]) for i in range(int(c_out_objects_size.value))]
        out_images = [ImageData(c_out_images[i]) for i in range(int(c_out_images_size.value))]

        return out_objects, out_images

    def property(self, handle: Optional[Handle]) -> List[str]:
        assert isinstance(handle, (type(None), Package.Handle))
        c_handle = handle.handle if handle else _C.c_void_p(0)

        c_properties = self.__raw.property(c_handle)

        if not c_properties:
            return []

        properties = []

        c_i = 0
        while True:
            c_prop = c_properties[c_i]
            if not c_prop:
                break
            c_i += 1

            properties.append(c_prop.decode())

        return properties

    def setd(self, handle: Optional[Handle], name: str, value: float):
        assert isinstance(handle, (type(None), Package.Handle))
        c_handle = handle.handle if handle else _C.c_void_p(0)

        c_name = name.encode()
        c_value = _C.c_double(value)

        c_errcode = self.__raw.setd(c_handle, c_name, c_value)
        if c_errcode != 0:
            raise Exception("0x{}: {}".format(c_errcode, self.error(None, c_errcode)))

    def getd(self, handle: Optional[Handle], name: str) -> float:
        assert isinstance(handle, (type(None), Package.Handle))
        c_handle = handle.handle if handle else _C.c_void_p(0)

        c_name = name.encode()
        c_value = _C.c_double()

        c_errcode = self.__raw.getd(c_handle, c_name, _C.byref(c_value))
        if c_errcode != 0:
            raise Exception("0x{}: {}".format(c_errcode, self.error(None, c_errcode)))

        return c_value.value

    def set(self, handle: Optional[Handle], name: str, value: Object):
        assert isinstance(handle, (type(None), Package.Handle))
        c_handle = handle.handle if handle else _C.c_void_p(0)

        c_name = name.encode()
        c_value = value.ref

        c_errcode = self.__raw.set(c_handle, c_name, _C.byref(c_value))
        if c_errcode != 0:
            raise Exception("0x{}: {}".format(c_errcode, self.error(None, c_errcode)))

    def get(self, handle: Optional[Handle], name: str) -> Object:
        assert isinstance(handle, (type(None), Package.Handle))
        c_handle = handle.handle if handle else _C.c_void_p(0)

        value = Object()

        c_name = name.encode()
        c_value = value.ref

        c_errcode = self.__raw.get(c_handle, c_name, _C.byref(c_value))
        if c_errcode != 0:
            raise Exception("0x{}: {}".format(c_errcode, self.error(None, c_errcode)))

        return Object.FromRaw(c_value)

    def tag(self, handle: Optional[Handle], method_id: int, label_index: int, label_value: int) -> str:
        assert isinstance(handle, (type(None), Package.Handle))
        c_handle = handle.handle if handle else _C.c_void_p(0)

        c_method_id = _C.c_uint32(method_id)
        c_label_index = _C.c_uint32(label_index)
        c_label_value = _C.c_int32(label_value)

        c_tag = self.__raw.tag(c_handle, c_method_id, c_label_index, c_label_value)

        if not c_tag:
            return ""

        return c_tag.decode()


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

    def reset(self):
        self.__package.reset(self.__handle)

    def forward(self, method_id: int,
                images: Iterable[Union[Any, ImageData]] = None, objects: Iterable[Object] = None) \
            -> Tuple[List[Object], List[ImageData]]:
        if images is None:
            images = []
        if objects is None:
            objects = []
        if isinstance(images, ImageData):
            images = [images]
        checked_images = []
        for image in images:
            if isinstance(image, ImageData):
                checked_images.append(image)
                continue
            try:
                checked_images.append(ImageData(image))
            except:
                raise RuntimeError("param images must be list of ImageData or Any can parse to ImageData")
        for obj in objects:
            assert isinstance(obj, Object), "param objects must be list of Object"
        return self.__package.forward(self.__handle, method_id, images=checked_images, objects=objects)

    def property(self) -> List[str]:
        return self.__package.property(self.__handle)

    def tag(self, method_id: int, label_index: int, label_value: int) -> str:
        return self.__package.tag(self.__handle, method_id, label_index, label_value)

    def setd(self, name: str, value: Union[bool, int, float]):
        assert isinstance(value, (bool, int, float))
        if isinstance(value, bool):
            self.__package.setd(self.__handle, name, 1 if value else 0)
            return
        self.__package.setd(self.__handle, name, value)

    def getd(self, name: str) -> float:
        return self.__package.getd(self.__handle, name)

    def set(self, name: str, value: Union[bool, int, float, Object]):
        assert isinstance(value, (bool, int, float, Object))
        if isinstance(value, bool):
            self.__package.setd(self.__handle, name, 1 if value else 0)
            return
        if isinstance(value, (int, float)):
            self.__package.setd(self.__handle, name, value)
            return
        assert isinstance(value, Object)
        self.__package.set(self.__handle, name, value)

    def get(self, name: str) -> Object:
        return self.__package.get(self.__handle, name)

