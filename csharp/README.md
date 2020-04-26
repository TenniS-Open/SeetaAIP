# C# load interface.

Use `Seeta.AIP.Engine` and `Seeta.AIP.Instance` to load AIP.
Notice: Use `Dispose` method to control memory, as that GC not free memory in time.

## In Linux or MacOS
build this projcet, copy `libkernel32.so` generated in `<root>/lib` to project root, to fix that `kernel32` library not found error.



