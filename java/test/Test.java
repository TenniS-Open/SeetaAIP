import seeta.aip.*;
import seeta.aip.Object;
import seeta.aip.Point;

import java.awt.image.*;// for BufferedImage
import javax.imageio.*;//for ImageIO.read
import java.awt.*;//for JFrame and JLabel etc
import java.io.*;//for catch (IOException e),File,InputStream, BufferedInputStream,and FileInputStream ect

public class Test {
    public static BufferedImage ReadImage(String path) throws IOException {
        File file = new File(path);
        BufferedImage image = ImageIO.read(file);
        return image;
    }

    public static void WriteImage(RenderedImage image, String path, String ext) throws IOException {
        File file = new File(path);
        ImageIO.write(image, ext, file);
    }

    public static ImageData ConvertImage(BufferedImage image) {
        int width = image.getWidth();
        int height = image.getHeight();

        byte []data = new byte[width * height * 3];

        // convert to BGR image data
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int rgb = image.getRGB(x, y);
                byte r = (byte)((rgb & 0x00ff0000) >> 16);
                byte g = (byte)((rgb & 0x0000ff00) >> 8);
                byte b = (byte)(rgb & 0x0000ff);

                int index = (y * width + x) * 3;
                data[index] = b;
                data[index + 1] = g;
                data[index + 2] = r;
            }
        }

        return new ImageData(data, 1, width, height, 3, ImageFormat.U8Bgr);
    }

    public static byte[] ReadBinary(String path) throws IOException {
        File file = new File(path);
        BufferedInputStream in = new BufferedInputStream(new FileInputStream(file));
        ByteArrayOutputStream out = new ByteArrayOutputStream(1024);
        byte[] temp = new byte[1024];
        int size = 0;
        while ((size = in.read(temp)) != -1) {
            out.write(temp, 0, size);
        }
        in.close();
        byte[] content = out.toByteArray();
        return content;
    }

    public static Object ReadObject(String path) throws IOException {
        byte[] content = ReadBinary(path);
        return new Object(new Tensor(content, new int[]{content.length}));
    }

    public static void main(String[] argv) throws seeta.aip.Exception, IOException {
        System.out.println("Hey, I'm running!");

        String dll = "libname";
        String cfg = "model.json";
        String bin = "model.tsm";
        String img = "test.jpg";

        String pack_cfg = "PlateDetectorA_pack.json";

        Object cfg_data = ReadObject(cfg);
        Object bin_data = ReadObject(bin);

        BufferedImage canvas = ReadImage(img);
        ImageData image = ConvertImage(canvas);
        System.out.printf("ReadImage : [%d, %d, %d]\n", image.getWidth(), image.getHeight(), image.getChannels());

        /// #1 Load SDK(dll)
        Engine engine = new Engine(dll);

        /// #2 Create instance to compute
        /// New instance method 1, using assets in memory
        Instance instance = new Instance(engine, new Device("cpu"), new String[0], new Object[]{cfg_data, bin_data});
        // New instance method 2, using json model file, which set backbone.tsm = "@file@..."
        // Instance instance = new Instance(engine, new Device("cpu"), new String[]{pack_cfg});

        String[] properties = instance.property();
        for (int i = 0; i < properties.length; i += 1) {
            System.out.printf("[Property] %s = %f\n", properties[i], instance.getd(properties[i]));
        }

        /// #3 [Optional] set property
        instance.set("threshold", 0.8);
        instance.set("thread_number", 4);

        /// #4 call method with image
        Result result = instance.forward(0, new ImageData[]{image}, new Object[0]);

        System.out.printf("Detected objects: %d\n", result.objects.length);

        Graphics2D g = (Graphics2D)canvas.getGraphics();
        BasicStroke stroke = new BasicStroke(3.0f);
        g.setStroke(stroke);

        for (int i = 0; i < result.objects.length; i += 1) {
            Object box = result.objects[i];
            Point[] p = box.getShape().landmarks;

            g.setColor(Color.GREEN);
            g.drawLine((int)p[0].x, (int)p[0].y, (int)p[1].x, (int)p[0].y);
            g.drawLine((int)p[1].x, (int)p[0].y, (int)p[1].x, (int)p[1].y);
            g.drawLine((int)p[1].x, (int)p[1].y, (int)p[0].x, (int)p[1].y);
            g.drawLine((int)p[0].x, (int)p[1].y, (int)p[0].x, (int)p[0].y);
            g.setColor(Color.RED);
            g.drawLine((int)p[2].x, (int)p[2].y, (int)p[3].x, (int)p[3].y);
            g.drawLine((int)p[3].x, (int)p[3].y, (int)p[4].x, (int)p[4].y);
            g.drawLine((int)p[4].x, (int)p[4].y, (int)p[5].x, (int)p[5].y);
            g.drawLine((int)p[5].x, (int)p[5].y, (int)p[2].x, (int)p[2].y);
        }

        /// Save result file
        WriteImage(canvas, "output.jpg", "jpg");

        /// #5 dispose instance manually
        instance.dispose();

        // used to free library, not recommended for MinGW's dll
        // engine.dispose();
    }
}