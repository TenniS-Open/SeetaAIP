package seeta.aip;
import java.lang.RuntimeException;

public class ImageData {
   private ImageFormat format = ImageFormat.U8Raw;
   private ValueType type = ValueType.Byte;
   private float[] data_float;
   private byte[] data_byte = new byte[0];
   private int[] data_int;
   private int number = 0;
   private int height = 0;
   private int width = 0;
   private int channels = 0;

   public ImageData() {}

   public ImageData(ImageFormat format,
                    int number,
                    int height,
                    int width,
                    int channels) {
      this.format = format;
      this.number = number;
      this.height = height;
      this.width = width;
      this.channels = channels;
      this.type = GetType(format);
      int N = number * height * width * channels;
      data_byte = null;
      switch (this.type) {
         case Byte:
            data_byte = new byte[N];
            break;
         case Int:
            data_int = new int[N];
            break;
         case Float:
            data_float = new float[N];
            break;
      }
   }

   public ImageData(float[] data,
                    int number,
                    int height,
                    int width,
                    int channels) {
      int N = number * height * width * channels;
      if (N != data.length) {
         throw new RuntimeException(String.format("data.length must be %d = %d x %d x %d x %d, got %d",
            N, number, height, width, channels, data.length
         ));
      }
      this.data_byte = null;
      this.data_float = data;
      this.number = number;
      this.height = height;
      this.width = width;
      this.channels = channels;
      this.format = ImageFormat.F32Raw;
      this.type = ValueType.Float;
   }

   public ImageData(int[] data,
                    int number,
                    int height,
                    int width,
                    int channels) {
      int N = number * height * width * channels;
      if (N != data.length) {
         throw new RuntimeException(String.format("data.length must be %d = %d x %d x %d x %d, got %d",
            N, number, height, width, channels, data.length
         ));
      }
      this.data_byte = null;
      this.data_int = data;
      this.number = number;
      this.height = height;
      this.width = width;
      this.channels = channels;
      this.format = ImageFormat.F32Raw;
      this.type = ValueType.Float;
   }

   public ImageData(byte[] data,
                    int number,
                    int height,
                    int width,
                    int channels,
                    ImageFormat format) {
      int N = number * height * width * channels;
      if (N != data.length) {
         throw new RuntimeException(String.format("data.length must be %d = %d x %d x %d x %d, got %d",
            N, number, height, width, channels, data.length
         ));
      }
      if (GetType(format) != ValueType.Byte) {
         throw new RuntimeException(String.format("Wanted byte type's format must be U8xxx"));
      }
      this.data_byte = data;
      this.number = number;
      this.height = height;
      this.width = width;
      this.channels = channels;
      this.format = format;
      this.type = ValueType.Byte;
   }

   public ImageData(byte[] data,
                    int number,
                    int height,
                    int width,
                    int channels) {
      this(data, number, height, width, channels, ImageFormat.U8Raw);
   }

   public static ValueType GetType(ImageFormat format) {
      switch (format) {
         default:
         case U8Raw:
         case U8Rgb:
         case U8Bgr:
         case U8Rgba:
         case U8Bgra:
         case U8Y:
             return ValueType.Byte;
         case F32Raw:
             return ValueType.Float;
         case I32Raw:
             return ValueType.Int;
      }
   }

   public ImageFormat getFormat() {
      return format;
   }

   public ValueType getType() {
      return type;
   }

   public int getNumber() {
      return number;
   }

   public int getHeight() {
      return height;
   }

   public int getWidth() {
      return width;
   }

   public int getChannels() {
      return channels;
   }

   public byte[] getByteData() {
      return data_byte;
   }

   public float[] getFloatData() {
      return data_float;
   }

   public int[] getIntData() {
      return data_int;
   }
}