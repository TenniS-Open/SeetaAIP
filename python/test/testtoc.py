from seetaaip.toc import *
import cv2

if __name__ == '__main__':
    # create instance by python script or
    filepath = "data/__init__.py"
    instance = Instance(filepath, Device("cpu"), "A")

    # set property
    instance.setd("threshold", 0.3)

    # load and build input image
    image = cv2.imread("a.jpg")
    image = ImageData(image, fmt=FORMAT_U8BGR)

    # call method 0
    result_objects, result_images = instance.forward(0, [image], [])

    print(result_objects)
    print(result_images)
