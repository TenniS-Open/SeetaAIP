package com.seetatech.aip;

public class Engine {
    private long handle = 0;

    private native void construct(String libname);
    private native void destruct();

    public Engine(String libname) {
        this.construct(libname);
    }

    protected void finalize() {
        this.destruct();
    }

    public native Package load();
}