package com.seetatech.aip;

public class Engine {
    static {
        System.loadLibrary("seeta_aip_java");
    }

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