package seeta.aip;

public class Package {
    private byte[] cdata;

    private native void construct(byte[] cdata);

    public Package(byte[] cdata) {
        this.construct(cdata);
    }

    public int aip_version;
    public String module;
    public String description;
    public String mID;
    public String sID;
    public String version;
    public String[] support;

    public native String error(long handle);
    public native long create(String[] models, Object[] objects) throws seeta.aip.Exception;
    public native int free(long handle);
    public native String[] property(long handle) throws seeta.aip.Exception;
}