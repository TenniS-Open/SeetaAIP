package seeta.aip;

public class Exception extends java.lang.Exception {

    /**
     * Auto generated.
     */
    private static final long serialVersionUID = -8242291872956083467L;

    private int errorCode = -1;
    private String message;

    public Exception(int errorCode, String message) {
        this.errorCode = errorCode;
        this.message = message;
    }

    public int getErrorCode() {
        return errorCode;
    }

    public String getMessage() {
        return message;
    }
}