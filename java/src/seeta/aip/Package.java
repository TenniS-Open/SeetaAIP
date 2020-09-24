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

    public native String error(long handle, int errorCode);
    public native long create(Device device, String[] models, Object[] objects) throws seeta.aip.Exception;
    public native void free(long handle);
    public native String[] property(long handle) throws seeta.aip.Exception;
    public native void setd(long handle, String name, double value) throws seeta.aip.Exception;
    public native double getd(long handle, String name) throws seeta.aip.Exception;
    public native void reset(long handle) throws seeta.aip.Exception;
    public native Result forward(long handle, int methodID, ImageData[] images, Object[] objects) throws seeta.aip.Exception;
    public native String tag(long handle, int methodID, int labelIndex, int labelValue) throws seeta.aip.Exception;
    public native void set(long handle, String name, Object value) throws seeta.aip.Exception;
    public native Object get(long handle, String name) throws seeta.aip.Exception;
}