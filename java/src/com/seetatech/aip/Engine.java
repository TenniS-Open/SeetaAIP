package com.seetatech.aip;

public class Engine {
    private static LoadJNI _load_jni = new LoadJNI();

    private long handle = 0;

    private native void construct(String libname);
    private native void destruct();

    public Engine(String libname) {
        this.construct(libname);
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

    public void dispose() {
        this.destruct();
    }

    public native Package load();
}