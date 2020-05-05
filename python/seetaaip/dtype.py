#!/usr/bin/env python

import numpy

VOID = 0
UINT8 = 1
FLOAT32 = 2
INT32 = 3
FLOAT64 = 4

__dtype_bytes_map = {
    VOID: 0,
    UINT8: 1,
    INT32: 4,
    FLOAT32: 4,
    FLOAT64: 8,
}

__dtype_str_map = {
    VOID: 'void',
    UINT8: 'byte',
    INT32: 'int32',
    FLOAT32: 'float32',
    FLOAT64: 'float64',
}

convert_pairs = [
    (numpy.int32, INT32),
    (numpy.uint8, UINT8),
    (numpy.float32, FLOAT32),
    (numpy.float64, FLOAT64),
]

__numpy_dtype_to_dtype = { pair[0]: pair[1] for pair in convert_pairs }
__dtype_to_numpy_dtype = { pair[1]: pair[0] for pair in convert_pairs }


def is_dtype(numpy_dtype, ts_dtype):
    # type: (numpy.dtype, int) -> bool
    """
    return if numpy's dtype is ts's dtype
    :param numpy_dtype: numpy.dtye
    :param ts_dtype: int value of ts.dtype
    :return: bool
    """
    if ts_dtype not in __dtype_to_numpy_dtype:
        return False
    return __dtype_to_numpy_dtype[ts_dtype] == numpy_dtype


def from_numpy(dtype):
    # type: (numpy.generic) -> int
    """
    convert numpy.dtype to ts.dtype
    :param dtype: numpy.dtype
    :return: int value of ts.dtype
    """
    '''
    # Test in py2.7
    print(isinstance(dtype, numpy.float32)) --> False
    print(dtype is numpy.float32) --> False
    print(dtype == numpy.float32) --> True
    print(isinstance(type(dtype), numpy.float32)) --> False
    print(type(dtype) is numpy.float32) --> False
    print(type(dtype) == numpy.float32) --> False
    '''
    for pair in convert_pairs:
        if dtype == pair[0]:
            return pair[1]
    raise Exception("Not supported numpy.dtype={}".format(dtype))


def to_numpy(dtype):
    # type: (int) -> numpy.generic
    """
    convert ts.dtype to numpy.dtype
    :param dtype: int value of ts.dtype
    :return: numpy.dtype
    """
    if dtype not in __dtype_to_numpy_dtype:
        raise Exception("Not supported converting aip dtype = {}".format(dtype))
    return __dtype_to_numpy_dtype[dtype]


def dtype_bytes(dtype):
    # type: (Union[int, numpy.dtype]) -> int
    if not isinstance(dtype, int):
        dtype = from_numpy(dtype=dtype)
    if dtype not in __dtype_bytes_map:
        raise Exception("Do not support dtype={}".format(dtype))
    return __dtype_bytes_map[dtype]


def dtype_str(dtype):
    # type: (Union[int, numpy.dtype]) -> str
    if not isinstance(dtype, int):
        dtype = from_numpy(dtype=dtype)
    if dtype not in __dtype_str_map:
        raise Exception("Do not support dtype={}".format(dtype))
    return __dtype_str_map[dtype]