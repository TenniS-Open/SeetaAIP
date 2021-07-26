# AIP python 接口

## python 调用 C++ AIP

调用示例见：[testaip.py](test/testaip.py)

采用了 `ctypes` 封装的动态加载 `AIP` 的动态库来进行调用。

## python 标准的 AIP 封装

可以将 `python` 代码按照 `AIP` 的逻辑封装成标准接口。
在 `python` 中的调用逻辑和 `C++` 版本的相同。

封装的示例见：[__init__.py](test/data/__init__.py)。
调用示例见：[testtoc.py](test/testtoc.py)。

只要按照 `seetaaip.toc.AIP` 的方式进行封装，就可以采用标准的接口进行动态加载了。