#!python
# coding: utf-8

import sys
if sys.version > '3':
    pass
else:
    raise Exception("\"{}\" only support python3.".format(__file__))

import os
import numpy
import copy
from typing import Union, Iterable, Tuple, Dict, List, Any, Optional
from . import dtype as aip_dtype

VOID = 0     # for non value type
BYTE = 2        # byte type
INT32 = 5       # signed integer type with 4-bytes
FLOAT32 = 10     # float type with 4-bytes
FLOAT64 = 11     # signed float type with 8-bytes


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


class Device(object):
    """
    Compute device.
    Attr device: device name like 'cpu' or 'gpu'
    Attr id: device index, like 0, 1, 2
    """
    def __init__(self, dev: str = "cpu", i: int = 0):
        """
        :param dev: device name
        :param i: device id
        """
        self.__type = dev
        self.__id = i

    @property
    def device(self) -> str:
        return self.__type

    @property
    def id(self) -> int:
        return self.__id

    def __str__(self):
        return "Device{{type='{}', id={}}}".format(self.device, self.id)


class ImageData(object):
    """
    Image data
    Attr fmt: format of image data layout
        [FORMAT_U8RAW, FORMAT_F32RAW, FORMAT_I32RAW, FORMAT_U8RGB, FORMAT_U8BGR, FORMAT_U8RGBA, FORMAT_U8BGRA,
         FORMAT_U8Y, ]
    Attr number: number of image
    Attr height: height of image
    Attr width: width of image
    Attr channels: channels of image
    Attr dims: get [number, height, width, channels] of image
    Attr shape: get memory layout shape. It's different to dims when format is NCHW_*
    Attr data/number: get numpy.ndarray, which contains data of image.
    """
    def __init__(self, obj: Union[numpy.ndarray, Iterable, int, float] = None,
                 fmt: int = None, shape: Iterable[int] = None,
                 number: int = None, width: int = None, height: int = None, channels: int = None):
        """
        :param obj: data which can be parse to numpy.asarray()
        :param fmt: image format, must be FORMAT_*
        :param shape: shape of image, like [number, height, width, channels]
        :param number:
        :param width:
        :param height:
        :param channels:
        Usage:
            image = ImageData(data, fmt=FORMAT_U8BGR, shape=[1, 300, 400, 3])
            image = ImageData(data, fmt=FORMAT_U8BGR, number=1, height=300, width=400, channels=3)
            image = ImageData(fmt=FORMAT_U8BGR, number=1, height=300, width=400, channels=3)
        You can treat ImageData as numpy.ndarray. like image[:, :, 0] = 4.
        """
        # assert isinstance(fmt, (int, type(None)))
        if fmt is not None:
            assert fmt in {
                FORMAT_U8RAW,
                FORMAT_U8Y,
                FORMAT_U8RGB,
                FORMAT_U8BGR,
                FORMAT_U8RGBA,
                FORMAT_U8BGRA,
                FORMAT_I32RAW,
                FORMAT_F32RAW,
            }

        self.__type = BYTE
        self.__dims = [1, 0, 0, 0]
        self.__numpy = None  # numpy
        self.__fmt = FORMAT_U8RAW

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
                fmt = FORMAT_U8RAW
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

    def _fmt_to_dtype(self, fmt: int) -> int:
        fmt_map = {
            FORMAT_U8RAW: BYTE,
            FORMAT_U8Y: BYTE,
            FORMAT_U8RGB: BYTE,
            FORMAT_U8BGR: BYTE,
            FORMAT_U8RGBA: BYTE,
            FORMAT_U8BGRA: BYTE,
            FORMAT_I32RAW: INT32,
            FORMAT_F32RAW: FLOAT32,
        }
        return fmt_map[fmt]

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
            return INT32
        if dtype is float or dtype == float:
            return FLOAT32
        if isinstance(dtype, int):
            assert dtype in {BYTE, INT32, FLOAT32, FLOAT64}, \
                "Not supported converting aip dtype = {}".format(dtype)
            return dtype
        # assert isinstance(dtype, numpy.generic)
        assert dtype in {numpy.uint8, numpy.int32, numpy.float32, numpy.float64}, \
            "Not supported numpy.dtype={}".format(str(dtype.__name__))
        return aip_dtype.from_numpy(dtype)

    def _dtype_to_fmt(self, dtype: int) -> int:
        dtype_map = {
            BYTE: FORMAT_U8RAW,
            INT32: FORMAT_I32RAW,
            FLOAT32: FORMAT_F32RAW,
        }
        return dtype_map[dtype]

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
    def shape(self):
        return self.__numpy.shape

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


class Point(object):
    """
    Point in image.
    Origin is the left upper corner.
    Attr x: means coordinates from left to right.
    Attr y: means coordinates from top to bottom.
    """
    def __init__(self, x=0, y=0):
        self.x = x
        self.y = y

    def __str__(self):
        return "({}, {})".format(self.x, self.y)

    def __repr__(self):
        return "({}, {})".format(self.x, self.y)


class Shape(object):
    """
    Object shape, See SHAPE_* variable for more details
    Attr type: type of shape, in
        [SHAPE_POINTS, SHAPE_LINES, SHAPE_RECTANGLE, SHAPE_PARALLELOGRAM, SHAPE_POLYGON, SHAPE_CIRCLE, SHAPE_CUBE]
    Attr landmarks: points of shape
    Attr rotate: using degree measure, positive value means anticlockwise
    Attr scale: normally means scale of point's coordinate, specially means radius of a circle
    """
    def __init__(self, shape_type: int,
                 landmarks: Iterable[Union[Point, Tuple]],
                 rotate: float = 0, scale: float = 1):
        """
        :param shape_type: type of shape, must be SHAPE_*
        :param landmarks: points of this shape
        :param rotate: using degree measure, positive value means anticlockwise
        :param scale: normally means scale of point's coordinate, specially means radius of a circle
        """
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

    def __str__(self):
        points = ",".join([str(p) for p in self.__landmarks])
        return "Shape{{type={}, landmarks=[{}], rotate={}, scale={}}}".format(
            self.__shape_type, points, self.__rotate, self.__scale)

    def __repr__(self):
        return self.__str__()


class Tag(object):
    """
    Object tag.
    """
    def __init__(self, label=0, value=0):
        """
        :param label: label index
        :param value: score of each label
        """
        self.label = label
        self.value = value

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


class Object(object):
    """
    Detectable object.
    Attr shape: Shape
    Attr tags: list of Tag
    Attr extra: numpy.ndarray
    """

    Tag = Tag

    def __init__(self, shape: Shape = None, tags: Iterable[Union[Tag, Tuple]] = None,
                 extra: Union[Any, numpy.ndarray] = None):
        """
        :param shape: instance of Shape
        :param tags: list of Tag
        :param extra: instance of numpy.ndarray, means the extra data of object.
            must be uint8, int32, float32 or float64.
        """
        assert isinstance(shape, (type(None), Shape))
        if extra is not None and not isinstance(extra, numpy.ndarray):
            extra = numpy.asarray(extra)
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
        if value is None:
            self.__extra = None
            return
        self.__extra = numpy.asarray(value)

    def __str__(self):
        return "Object{{shape={}, tags=[{}], extra={}}}".format(
            self.__shape if self.__shape is not None else "null",
            ",".join([str(tag) for tag in self.__tags]),
            self.__extra if self.__extra is not None else "null",
        )

    def __repr__(self):
        return self.__str__()


class AIP(object):
    """
    Base class of AIP(AI Package)
    """
    def module(self) -> str:
        """
        String for module name, must be `[a-zA-Z_][a-zA-Z_0-9]*`, used to distinguish from other libraries.
            For example, SeetaFaceDetector610
        :return: module
        """
        return ""

    def description(self) -> str:
        """
        json string for more information, the format will post later
        :return: description
        """
        return ""

    def mID(self) -> str:
        """
        not readable ID of AIP, only satisfied in system
        :return: main ID
        """
        return ""

    def sID(self) -> str:
        """
        self describable algorithm ID, like SSD, FRCNN etc.
        :return: sub ID
        """
        return ""

    def version(self) -> str:
        """
        this AIP's version of mID, comparable `Dotted String`, like 1.3, 6.4.0, or 1.2.3.rc1
        :return: version
        """
        return ""

    def support(self) -> List[str]:
        """
        array of string, like ['cpu', 'gpu'], only for tips
        :return: array of supported compute device
        """
        return []

    def init(self, device: Device,
             models: Union[str, Iterable[str]],
             objects: Union[Object, Iterable[Object]] = None):
        """
        Init created AIP instance.
        :param device: compute device
        :param models: used model files. Example: ["file1.dat", "file2.json"]
        :param objects: init objects, suggest first object use tag to set property, some AIP will give some property must be set
        :return:
        """
        raise NotImplementedError

    def __del__(self):
        self.dispose()

    def dispose(self):
        """
        Dispose this instance. This method will be called when object destructing.
        :return:
        """
        pass

    def reset(self):
        """
        Reset instance statement. Used in stream mode.
        :return:
        """
        pass

    def forward(self, method_id: int,
                images: Iterable[Union[Any, ImageData]] = None, objects: Iterable[Object] = None) \
            -> Tuple[List[Object], List[ImageData]]:
        """
        Do forward by method and image/object inputs.
        :param method_id: method id, there can be many method in single AIP.
        :param images: input images
        :param objects: input objects.
        :return: List of Object and List of ImageData
        """
        raise NotImplementedError

    def property(self) -> List[str]:
        """
        Get all readable and settable properties. example ["number_threads", "min_face_size"]
        :return: list of properties.
        """
        return []

    def tag(self, method_id: int, label_index: int, label_value: int) -> str:
        """
        Get readable label string, for debug. and result auto plot.
        :param method_id: method id
        :param label_index: label's index
        :param label_value: label's value
        :return:
        label index tell the label index. if an object get label [(3, 0.2), (4, 0.4)] from method 5.
            Followings will be called to get all labels: tag(5, 0, 3) and tag(5, 1, 4).
        """
        raise NotImplementedError

    def setd(self, name: str, value: Union[bool, int, float]):
        """
        set double-value property.
        :param name: property name
        :param value: property value
        :return:
        """
        raise Exception("Can not set property: {}".format(name))

    def getd(self, name: str) -> float:
        """
        get object-value property.
        :param name: property name
        :return: property value
        """
        raise Exception("Can not get property: {}".format(name))

    def set(self, name: str, value: Union[bool, int, float, Object]):
        """
        set object-value property.
        :param name: property name
        :param value: property value
        :return:
        """
        raise Exception("Can not set property: {}".format(name))

    def get(self, name: str) -> Object:
        """
        get object-value property.
        :param name: property name
        :return: property value
        """
        raise Exception("Can not get property: {}".format(name))


class Engine(object):
    """
    Engine to load python script.
    """

    ENTRY = "create"
    MAIN = "__init__.py"

    def __init__(self, filepath, tmpdir=None, replace=True):
        """
        Load python extension like normal AIP.
        :param filepath: directly python script or zip file contains __init__.py
        :param tmpdir: tmpdir to extract zip files.
        :param replace: if cover exist files when extract files.
        """
        root, name_ext = os.path.split(filepath)
        name, ext = os.path.splitext(name_ext)
        ext = ext.lower()
        root = os.path.abspath(root)

        if ext not in {".py", ".zip"}:
            raise Exception("Python AIP module must be *.py or *.zip")

        pyfile = filepath
        if ext == ".py":
            pass
        else:
            # get temp path
            if tmpdir is None:
                tmpdir = os.path.join(root, name)
            if not os.path.isdir(tmpdir):
                os.makedirs(tmpdir)
            try_main = os.path.join(tmpdir, self.MAIN)
            if replace or not os.path.isfile(try_main):
                sys.stdout.write("[INFO] Extracting {} into {}.\n".format(
                    filepath, tmpdir))
                # extract files
                import zipfile
                with zipfile.ZipFile(filepath) as zf:
                    zf.extractall(tmpdir)
            if not os.path.isfile(try_main):
                raise Exception("Can not found {} in {}.".format(self.MAIN, filepath))
            pyfile = try_main
            root = tmpdir

        g = copy.copy(globals())
        g.update({
            "__file__": pyfile,
            "__name__": "__main__",
        })
        # l = locals()
        if root not in sys.path:
            sys.path.append(root)

        with open(pyfile, 'rb') as file:
            exec(compile(file.read(), pyfile, 'exec'), g, None)

        if self.ENTRY not in g:
            raise Exception("Can not found method create in {}."
                            " which must return instance of 'seetaaip.toc.AIP',"
                            " and define like 'def create(): ...'".format(pyfile))

        self.__create = g["create"]

    def create(self,
               device: Device,
               models: Union[str, Iterable[str]],
               objects: Union[Object, Iterable[Object]] = None):
        instance = self.__create()
        assert isinstance(instance, AIP)
        instance.init(device, models, objects)
        return instance


class Instance(object):
    def __init__(self, filename: Union[str, Engine],
                 device: Device,
                 models: Union[str, Iterable[str]],
                 objects: Union[Object, Iterable[Object]] = None):
        """
        Init created AIP instance.
        :param filename: instance Engine or filename list "entry.py" or "package.zip"
        :param device: compute device
        :param models: used model files. Example: ["file1.dat", "file2.json"]
        :param objects: init objects, suggest first object use tag to set property, some AIP will give some property must be set
        :return:
        """
        self.__filename = None
        self.__engine = None
        self.__instance = None

        if isinstance(models, str):
            models = [models]
        if isinstance(objects, Object):
            objects = [objects]

        assert isinstance(filename, (str, Engine))
        if isinstance(filename, str):
            self.__filename = filename
            self.__engine = Engine(self.__filename)
            self.__instance = self.__engine.create(device, models, objects)
        elif isinstance(filename, Engine):
            self.__engine = filename
            self.__instance = self.__engine.create(device, models, objects)
        else:
            pass

    @property
    def module(self) -> str:
        """
        String for module name, must be `[a-zA-Z_][a-zA-Z_0-9]*`, used to distinguish from other libraries.
            For example, SeetaFaceDetector610
        :return: module
        """
        return self.__instance.module()

    @property
    def description(self) -> str:
        """
        json string for more information, the format will post later
        :return: description
        """
        return self.__instance.description()

    @property
    def mID(self) -> str:
        """
        not readable ID of AIP, only satisfied in system
        :return: main ID
        """
        return self.__instance.mID()

    @property
    def sID(self) -> str:
        """
        self describable algorithm ID, like SSD, FRCNN etc.
        :return: sub ID
        """
        return self.__instance.sID()

    @property
    def version(self) -> str:
        """
        this AIP's version of mID, comparable `Dotted String`, like 1.3, 6.4.0, or 1.2.3.rc1
        :return: version
        """
        return self.__instance.property()

    @property
    def support(self) -> List[str]:
        """
        array of string, like {'cpu', 'gpu'}, only for tips
        :return: array of supported compute device
        """
        return self.__instance.support()

    def dispose(self):
        """
        Dispose this instance. This method will be called when object destructing.
        :return:
        """
        if self.__instance is not None:
            self.__instance.dispose()

    def reset(self):
        """
        Reset instance statement. Used in stream mode.
        :return:
        """
        self.__instance.reset()

    def forward(self, method_id: int,
                images: Iterable[Union[Any, ImageData]] = None, objects: Iterable[Object] = None) \
            -> Tuple[List[Object], List[ImageData]]:
        """
        Do forward by method and image/object inputs.
        :param method_id: method id, there can be many method in single AIP.
        :param images: input images
        :param objects: input objects.
        :return: List of Object and List of ImageData
        """
        return self.__instance.forward(method_id, images, objects)

    def property(self) -> List[str]:
        """
        Get all readable and settable properties. example ["number_threads", "min_face_size"]
        :return: list of properties.
        """
        return self.__instance.property()

    def tag(self, method_id: int, label_index: int, label_value: int) -> str:
        """
        Get readable label string, for debug. and result auto plot.
        :param method_id: method id
        :param label_index: label's index
        :param label_value: label's value
        :return:
        label index tell the label index. if an object get label [(3, 0.2), (4, 0.4)] from method 5.
            Followings will be called to get all labels: tag(5, 0, 3) and tag(5, 1, 4).
        """
        return self.__instance.tag(method_id, label_index, label_value)

    def setd(self, name: str, value: Union[bool, int, float]):
        """
        set double-value property.
        :param name: property name
        :param value: property value
        :return:
        """
        self.__instance.setd(name, value)

    def getd(self, name: str) -> float:
        """
        get double-value property.
        :param name: property name
        :return: property value
        """
        return self.__instance.getd(name)

    def set(self, name: str, value: Union[bool, int, float, Object]):
        """
        set object-value property.
        :param name: property name
        :param value: property value
        :return:
        """
        self.__instance.set(name, value)

    def get(self, name: str) -> Object:
        """
        get object-value property.
        :param name: property name
        :return: property value
        """
        return self.__instance.get(name)


if __name__ == '__main__':
    pass
