package seeta.aip;

/**
 * AI Pakcage Engine to load DLL.
 * This is interface class with native achivements. 
 * </br>
 * Example:
 * <pre>
 * Engine dll = new Engine("libname");
 * </pre>
 */
public class Engine {
    private static LoadJNI _load_jni = new LoadJNI();

    private long handle = 0;
    private Package aip;

    private native void construct(String libname) throws seeta.aip.Exception;
    private native void destruct();

    /**
     * Construct engine using library name or path to DLL.
     * Call {@code dispose} explicit if you want close library immediately.
     * @param libname library name or path to DLL.
     * @throws seeta.aip.Exception when library not found or loadable.
     */
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

    /**
     * Dispose library immediately, make sure all packages instance disposed.
     * You can just leave this method implicit by `finalize`.
     */
    public void dispose() {
        this.destruct();
    }

    /**
     * Get description of AI package. 
     * AI Package: provides methods to create instance, forword method, property setters and so on.
     * @return AI Package.
     */
    public Package getAIP() { return aip; }
}