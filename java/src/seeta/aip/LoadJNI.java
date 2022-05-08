package seeta.aip;

/**
 * Load JNI dynamic library.
 */
public class LoadJNI {
    static {
        System.loadLibrary("seeta_aip_java");
    }
}