package seeta.aip;

public class Engine {
    private static LoadJNI _load_jni = new LoadJNI();

    private long handle = 0;
    private Package aip;

    private native void construct(String libname) throws seeta.aip.Exception;
    private native void destruct();

    public Engine(String libname) throws seeta.aip.Exception {
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

    public Package getAIP() { return aip; }
}