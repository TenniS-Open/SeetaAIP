 package seeta.aip;

 public class ImageData {
    public ImageFormat format = ImageFormat.U8Raw;
    public ValueType type = ValueType.Byte;
    public float[] data_float;
    public byte[] data_byte = new byte[0];
    public int[] data_int;
    public int number = 0;
    public int height = 0;
    public int width = 0;
    public int channels = 0;
 }