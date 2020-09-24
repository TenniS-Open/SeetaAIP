package seeta.aip;

public class Tensor {
    public ValueType type = ValueType.Void;
    public float[] data_float;
    public byte[] data_byte;
    public int[] data_int;
    public double[] data_double;
    public int[] dims = new int[0];
}
