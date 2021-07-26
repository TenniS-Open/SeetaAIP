from seetaaip.toc import *
import cv2

if __name__ == '__main__':
    # create instance by python script or
    filepath = "data/data.zip"
    instance = Instance(Engine(filepath), Device("cpu"), "A")
    """
    Use Engine to load zip files:
    `Engine("data/aip.zip", replace=False)`, the file will extracted in "data/aip" on first time.
    `Engine("data/aip.zip", replace=True)`, the file will extracted in "data/aip" on every time.
    `Engine("data/aip.zip", tmpdir="/tmp/aip")`, the file will extracted in "/tmp/aip" on every time.
    """

    # set property
    instance.setd("threshold", 0.3)

    # load and build input image
    image = cv2.imread("a.jpg")
    image = ImageData(image, fmt=FORMAT_U8BGR)

    # call method 0
    result_objects, result_images = instance.forward(0, [image], [])

    print(result_objects)
    print(result_images)
