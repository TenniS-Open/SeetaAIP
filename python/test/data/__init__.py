from seetaaip.toc import *
import numpy
from pack import pack


class MyAIP(AIP):
    def module(self) -> str:
        return ""

    def description(self) -> str:
        return ""

    def mID(self) -> str:
        return ""

    def sID(self) -> str:
        return ""

    def version(self) -> str:
        return ""

    def support(self) -> List[str]:
        return []

    def init(self, device: Device, models: List[str], objects: List[Object]):
        print("call init({}, {}, {})".format(device, models, objects))

    def __del__(self):
        self.dispose()

    def dispose(self):
        print("call dispose()")

    def reset(self):
        print("call reset()")

    def forward(self, method_id: int,
                images: List[ImageData],
                objects: List[Object]) \
            -> Tuple[
                List[Union[numpy.ndarray, Shape, Object]],
                List[ImageData]
            ]:
        print("call forward({}, {}, {})".format(method_id, images, objects))
        return [Rectangle(xywh=[1, 2, 3, 4])], []

    def property(self) -> List[str]:
        print("call property()")
        return []

    def tag(self, method_id: int, label_index: int, label_value: int) -> str:
        print("call tag({}, {}, {})".format(method_id, label_index, label_value))
        return ""

    def setd(self, name: str, value: float):
        print("call setd({}, {})".format(name, value))

    def getd(self, name: str) -> Union[bool, int, float]:
        print("call getd({})".format(name,))
        return 0

    def set(self, name: str, value: Object):
        print("call set({}, {})".format(name, value))

    def get(self, name: str) -> Union[bool, int, float, numpy.ndarray, Shape, Object]:
        print("call get({})".format(name,))
        return Object()


"""
The must be a function create to return new AIP instance. 
"""


def create():
    """
    :return: instance of seetaaip.toc.AIP.
    """
    return MyAIP()
