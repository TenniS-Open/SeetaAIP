from seetaaip.struct import *
import numpy
from seetaaip import _C

if __name__ == '__main__':
    # a = Tensor(shape=[1, 2, 3], dtype=numpy.float32)
    # b = Tensor(a.ref)
    # print(a.type)
    # print(a)
    # print(b.type)
    # print(b)

    device = Device()

    engine = Engine("../../lib/log")
    package = engine.package
    print(package.aip_version)

    fake_image = numpy.zeros([300, 400, 3], dtype=numpy.uint8)

    data = ImageData(fake_image, fmt=_C.FORMAT_U8BGR)
    data2 = ImageData(data.ref)

    print(data2.shape)


    obj = Object(shape=Shape(_C.SHAPE_POINTS, [(1, 2), (3, 4)]))
    obj2 = Object.FromRaw(obj.ref)

    instance = Instance(engine, device, ["test1"], [obj2])

    objects, images = instance.forward(0, [data2], [obj2])

    print(objects, images)

    instance.set("A", obj)
    print(instance.get("A"))

    print(instance.property())

    instance.dispose()

    engine.dispose()

    p = Point(x=0, y=0)

    print(p)

    pass