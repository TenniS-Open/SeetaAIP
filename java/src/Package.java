package com.seetatech.aip;

public class Package {
    private long handle = 0;

    private native void construct(String libname);
    private native void destruct();

    public Package(String libname) {
        this.construct(libname);
    }

    protected void finalize() {
        this.destruct();
    }

    public native String error(long handle);
    public native int free(long handle);
}