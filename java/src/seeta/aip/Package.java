package seeta.aip;

/**
 * AI Package description. Method should call using {@code Instance}.
 * This is interface class with native achivements. 
 */
public class Package {
    private byte[] cdata;

    private native void construct(byte[] cdata);

    public Package(byte[] cdata) {
        this.construct(cdata);
    }

    /**
     * AIP api version.
     */
    public int aip_version;

    /**
     * String for module name, must be `[a-zA-Z_][a-zA-Z_0-9]*`, used to distinguish from other libraries.
     */
    public String module;

    /**
     * Description of this AIP.
     */
    public String description;

    /**
     * Not readable ID of AIP, only satisfied in system.
     */
    public String mID;

    /**
     * Self describable algorithm ID, like SSD, FRCNN etc.
     */
    public String sID;

    /**
     * This AIP's version of mID, comparable `Dotted String`, like 1.3, 6.4.0, or 1.2.3.rc1.
     */
    public String version;

    /**
     * Array of string, like {'cpu', 'gpu', NULL}, only for tips.
     */
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