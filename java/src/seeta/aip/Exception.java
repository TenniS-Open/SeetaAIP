package seeta.aip;

/**
 * Warpper of built-in exception to tell which excpetion are AIP throwed.
 */
public class Exception extends java.lang.Exception {

    /**
     * Auto generated.
     */
    private static final long serialVersionUID = -8242291872956083467L;

    private int errorCode = -1;
    private String message;

    /**
     * Construct exception with error code and error message.
     * @param errorCode non-zero error number.
     * @param message description of this exception.
     */
    public Exception(int errorCode, String message) {
        this.errorCode = errorCode;
        this.message = message;
    }

    /**
     * Get error number.
     * @return error number.
     */
    public int getErrorCode() {
        return errorCode;
    }

    /**
     * Get error description.
     * @return error description.
     */
    public String getMessage() {
        return message;
    }
}