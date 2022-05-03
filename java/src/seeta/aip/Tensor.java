package seeta.aip;

public class Tensor {
    private ValueType type = ValueType.Void;
    private float[] data_float;
    private byte[] data_byte;
    private int[] data_int;
    private double[] data_double;
    private int[] dims = new int[0];
    private String data_string;

    public Tensor() {}

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

    public Tensor(String data) {
        this.type = ValueType.Char;
        this.data_string = data;
        this.dims = new int[data.length()];
    }

    public ValueType getType() {
        return this.type;
    }

    public int[] getDims() {
        return this.dims;
    }

    public byte[] getByteData() {
        return this.data_byte;
    }

    public int[] getIntData() {
        return this.data_int;
    }

    public float[] getFloatData() {
        return this.data_float;
    }

    public double[] getDoubleData() {
        return this.data_double;
    }

    public String getString() {
        return this.data_string;
    }
}
