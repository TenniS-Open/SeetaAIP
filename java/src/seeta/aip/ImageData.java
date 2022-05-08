package seeta.aip;
import java.lang.RuntimeException;

/**
 * AI Package ImageData.
 * Support RGB, BGR, RGBA and BGRA image input.
 * BGR is always recommended.
 * </br>
 * Load image example:
 * <pre>
 * import java.awt.image.*;// for BufferedImage
 * import javax.imageio.*;//for ImageIO.read
 * import java.awt.*;//for JFrame and JLabel etc
 * import java.io.*;//for catch (IOException e),File,InputStream, BufferedInputStream,and FileInputStream ect
 * 
 * public class Test {
 *     public static BufferedImage ReadImage(String path) throws IOException {
 *         File file = new File(path);
 *         BufferedImage image = ImageIO.read(file);
 *         return image;
 *     }
 *     public static ImageData ConvertImage(BufferedImage image) {
 *         int width = image.getWidth();
 *         int height = image.getHeight();
 * 
 *         byte []data = new byte[width * height * 3];
 * 
 *         // convert to BGR image data
 *         for (int y = 0; y < height; ++y) {
 *             for (int x = 0; x < width; ++x) {
 *                 int rgb = image.getRGB(x, y);
 *                 byte r = (byte)((rgb & 0x00ff0000) >> 16);
 *                 byte g = (byte)((rgb & 0x0000ff00) >> 8);
 *                 byte b = (byte)(rgb & 0x0000ff);
 * 
 *                 int index = (y * width + x) * 3;
 *                 data[index] = b;
 *                 data[index + 1] = g;
 *                 data[index + 2] = r;
 *             }
 *         }
 * 
 *        return new ImageData(data, 1, width, height, 3, ImageFormat.U8Bgr);
 *     }
 * }
 * </pre>
 * </br>
 * Save image example:
 * <pre>
 * public static void WriteImage(RenderedImage image, String path, String ext) throws IOException {
 *     File file = new File(path);
 *     ImageIO.write(image, ext, file);
 * }
 * </pre>
 */
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

   /**
    * Construct empty image[0, 0, 0].
    */
   public ImageData() {}

   /**
    * Construct image with given image format.
    * Be careful of the order of width and height.
    * @param format image format, BGR is always recommended.
    * @param number number of image, always be 1.
    * @param width width of image.
    * @param height height of image.
    * @param channels channels of image, always be 3 in color.
    */
   public ImageData(ImageFormat format,
                    int number,
                    int width,
                    int height,
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
         default:
            break;
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

   /**
    * Construct image with given float data. Format will be ImageFormat.F32Raw.
    * Be careful of the order of width and height.
    * Length of data must be `number * width * height * channels`.
    * @param data float data.
    * @param number number of image, always be 1.
    * @param width width of image.
    * @param height height of image.
    * @param channels channels of image, always be 3 in color.
    */
   public ImageData(float[] data,
                    int number,
                    int width,
                    int height,
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

   /**
    * Construct image with given int data. Format will be ImageFormat.I32Raw.
    * Be careful of the order of width and height.
    * Length of data must be `number * width * height * channels`.
    * @param data integer data.
    * @param number number of image, always be 1.
    * @param width width of image.
    * @param height height of image.
    * @param channels channels of image, always be 3 in color.
    */
   public ImageData(int[] data,
                    int number,
                    int width,
                    int height,
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
      this.format = ImageFormat.I32Raw;
      this.type = ValueType.Int;
   }

   /**
    * Construct image with given byte data in format. Format must be U8xxx.
    * Be careful of the order of width and height.
    * Length of data must be `number * width * height * channels`.
    * @param data byte data.
    * @param number number of image, always be 1.
    * @param width width of image.
    * @param height height of image.
    * @param channels channels of image, always be 3 in color.
    * @param format image format, must be U8xxx.
    */
   public ImageData(byte[] data,
                    int number,
                    int width,
                    int height,
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

   /**
    * Construct image with given byte data. Format will be ImageFormat.U8Raw.
    * Be careful of the order of width and height.
    * Length of data must be `number * width * height * channels`.
    * @param data integer data.
    * @param number number of image, always be 1.
    * @param width width of image.
    * @param height height of image.
    * @param channels channels of image, always be 3 in color.
    */
   public ImageData(byte[] data,
                    int number,
                    int width,
                    int height,
                    int channels) {
      this(data, number, width, height, channels, ImageFormat.U8Raw);
   }

   /**
    * Get value type of image format.
    * @param format image format.
    * @return value type.
    */
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

   /**
    * Get image format.
    * @return image format.
    */
   public ImageFormat getFormat() {
      return format;
   }

   /**
    * Get value type.
    * @return value type.
    */
   public ValueType getType() {
      return type;
   }

   /**
    * Get number of image(s).
    * @return number of image(s).
    */
   public int getNumber() {
      return number;
   }

   /**
    * Get height of image(s).
    * @return height of image(s).
    */
   public int getHeight() {
      return height;
   }

   /**
    * Get width of image(s).
    * @return width of image(s).
    */
   public int getWidth() {
      return width;
   }

   /**
    * Get channels of image(s).
    * @return channels of image(s).
    */
   public int getChannels() {
      return channels;
   }

   /**
    * Get byte data of image, which means image format must be U8xxx.
    * @return byte data of image.
    */
   public byte[] getByteData() {
      return data_byte;
   }

   /**
    * Get float data of image, which means image format must be F32xxx.
    * @return float data of image.
    */
   public float[] getFloatData() {
      return data_float;
   }

   /**
    * Get integer data of image, which means image format must be I32xxx.
    * @return integer data of image.
    */
   public int[] getIntData() {
      return data_int;
   }
}