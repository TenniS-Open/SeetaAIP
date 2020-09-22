package com.seetatech.aip;

public class Package {
    private byte[] cdata;

    private native void construct(byte[] cdata);

    public Package(byte[] cdata) {
        this.construct(cdata);
    }

    public String description;
    public String mID;
    public String sID;
    public String version;
    public String[] support;

    public native String error(long handle);
    public native int free(long handle);
    public native long create(String[] models, Object[] objects);
}