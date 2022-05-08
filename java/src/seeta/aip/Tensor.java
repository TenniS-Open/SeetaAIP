package seeta.aip;

/**
 * Tensor: extra field of Object.
 */
public class Tensor {
    private ValueType type = ValueType.Void;
    private float[] data_float;
    private byte[] data_byte;
    private int[] data_int;
    private double[] data_double;
    private int[] dims = new int[0];
    private String data_string;

    /**
     * Construct empty tensor.
     */
    public Tensor() {}

    /**
     * Construct tensor with valeu type and shape.
     * @param type tensor's value type.
     * @param dims tensor's dims.
     */
    public Tensor(ValueType type, int[] dims) {
        if (type == null) type = ValueType.Void;
        if (dims == null) dims = new int[0];
        int N = 1;
        for (int i = 0; i < dims.length; ++i) {
            N *= dims[i];
        }
        this.type = type;
        this.dims = dims;
        switch (type) {
            default:
            case Void:
                break;
            case Byte:
                this.data_byte = new byte[N];
                break;
            case Int:
                this.data_int = new int[N];
                break;
            case Float:
                this.data_float = new float[N];
                break;
            case Double:
                this.data_double = new double[N];
                break;
        }
    }

    /**
     * Construct with byte data and its dims.
     * The length of data must be production of dims.
     * @param data tensor's byte value data.
     * @param dims shape of tensor.
     */
    public Tensor(byte[] data, int[] dims) {
        if (dims == null) dims = new int[0];
        int N = 1;
        for (int i = 0; i < dims.length; ++i) {
            N *= dims[i];
        }
        if (N != data.length) {
           throw new RuntimeException(String.format("data.length must be %d = prod(dims), got %d",
              N, data.length
           ));
        }
        this.type = ValueType.Byte;
        this.data_byte = data;
        this.dims = dims;
    }

    /**
     * Construct with integer data and its dims.
     * The length of data must be production of dims.
     * @param data tensor's integer value data.
     * @param dims shape of tensor.
     */
    public Tensor(int[] data, int[] dims) {
        if (dims == null) dims = new int[0];
        int N = 1;
        for (int i = 0; i < dims.length; ++i) {
            N *= dims[i];
        }
        if (N != data.length) {
           throw new RuntimeException(String.format("data.length must be %d = prod(dims), got %d",
              N, data.length
           ));
        }
        this.type = ValueType.Int;
        this.data_int = data;
        this.dims = dims;
    }

    /**
     * Construct with float data and its dims.
     * The length of data must be production of dims.
     * @param data tensor's float value data.
     * @param dims shape of tensor.
     */
    public Tensor(float[] data, int[] dims) {
        if (dims == null) dims = new int[0];
        int N = 1;
        for (int i = 0; i < dims.length; ++i) {
            N *= dims[i];
        }
        if (N != data.length) {
           throw new RuntimeException(String.format("data.length must be %d = prod(dims), got %d",
              N, data.length
           ));
        }
        this.type = ValueType.Float;
        this.data_float = data;
        this.dims = dims;
    }

    /**
     * Construct with double data and its dims.
     * The length of data must be production of dims.
     * @param data tensor's double value data.
     * @param dims shape of tensor.
     */
    public Tensor(double[] data, int[] dims) {
        if (dims == null) dims = new int[0];
        int N = 1;
        for (int i = 0; i < dims.length; ++i) {
            N *= dims[i];
        }
        if (N != data.length) {
           throw new RuntimeException(String.format("data.length must be %d = prod(dims), got %d",
              N, data.length
           ));
        }
        this.type = ValueType.Double;
        this.data_double = data;
        this.dims = dims;
    }

    /**
     * Construct string tensor.
     * @param data string value.
     */
    public Tensor(String data) {
        this.type = ValueType.Char;
        this.data_string = data;
        this.dims = new int[data.length()];
    }

    /**
     * Get value type.
     * @return value type.
     */
    public ValueType getType() {
        return this.type;
    }

    /**
     * Get dims.
     * @return dims.
     */
    public int[] getDims() {
        return this.dims;
    }

    /**
     * Get byte data of tensor. Assume that value type is ValueType.Byte.
     * @return byte data.
     */
    public byte[] getByteData() {
        return this.data_byte;
    }

    /**
     * Get integer data of tensor. Assume that value type is ValueType.Int.
     * @return integer data.
     */
    public int[] getIntData() {
        return this.data_int;
    }

    /**
     * Get float data of tensor. Assume that value type is ValueType.Float.
     * @return float data.
     */
    public float[] getFloatData() {
        return this.data_float;
    }

    /**
     * Get double data of tensor. Assume that value type is ValueType.Double.
     * @return double data.
     */
    public double[] getDoubleData() {
        return this.data_double;
    }

    /**
     * Get string data of tensor. Assume that value type is ValueType.Char.
     * @return string data.
     */
    public String getString() {
        return this.data_string;
    }
}
