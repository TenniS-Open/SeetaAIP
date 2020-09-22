package com.seetatech.aip;

public class Package {
    private long handle = 0;

    private native void construct(String libname);
    private native void construct(long handle);
    private native void destruct();

    public Package(String libname) {
        this.construct(libname);
    }

    public Package(long handle) {
        this.construct(handle);
    }

    @Deprecated
    protected void finalize() throws Throwable {
        try {
            this.destruct();
        } catch(Throwable t){
            throw t;
        } finally {
            super.finalize();
        }
    }

    public void dispose() { this.destruct(); }

    public native String error(long handle);
    public native int free(long handle);
    public native long create(String[] models, Object[] objects);
}