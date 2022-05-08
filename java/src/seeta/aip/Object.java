package seeta.aip;

/**
 * AI Package Object, contains Shape, Tags and extra Tensor.
 * </br>
 * Example read binary to Object.
 * <pre>
 * public static byte[] ReadBinary(String path) throws IOException {
 *     File file = new File(path);
 *     BufferedInputStream in = new BufferedInputStream(new FileInputStream(file));
 *     ByteArrayOutputStream out = new ByteArrayOutputStream(1024);
 *     byte[] temp = new byte[1024];
 *     int size = 0;
 *     while ((size = in.read(temp)) != -1) {
 *         out.write(temp, 0, size);
 *     }
 *     in.close();
 *     byte[] content = out.toByteArray();
 *     return content;
 * }
 * 
 * public static Object ReadObject(String path) throws IOException {
 *     byte[] content = ReadBinary(path);
 *     return new Object(new Tensor(content, new int[]{content.length}));
 * }
 * </pre>
 * </br>
 * Example draw rectangle.
 * <pre>
 * BufferedImage canvas = ReadImage(img);
 * ImageData image = ConvertImage(canvas);
 * Result result = instance.forward(0, new ImageData[]{image}, new Object[0]);
 * 
 * Graphics2D g = (Graphics2D)canvas.getGraphics();
 * BasicStroke stroke = new BasicStroke(3.0f);
 * g.setStroke(stroke);
 * 
 * for (int i = 0; i < result.objects.length; i += 1) {
 *     Object box = result.objects[i];
 *     Point[] p = box.getShape().landmarks;
 * 
 *     g.setColor(Color.GREEN);
 *     g.drawLine((int)p[0].x, (int)p[0].y, (int)p[1].x, (int)p[0].y);
 *     g.drawLine((int)p[1].x, (int)p[0].y, (int)p[1].x, (int)p[1].y);
 *     g.drawLine((int)p[1].x, (int)p[1].y, (int)p[0].x, (int)p[1].y);
 *     g.drawLine((int)p[0].x, (int)p[1].y, (int)p[0].x, (int)p[0].y);
 * }
 * </pre>
 */
public class Object {
    private Shape shape = new Shape();       ///< must be set
    private Tag[] tags = new Tag[0];         ///< must be set
    private Tensor extra = new Tensor();     ///< can be null, or empty tensor

    /**
     * Construct empty object.
     */
    public Object() {}

    /**
     * Construct with given parameters.
     * @param shape shape of object.
     * @param tags tags of object.
     * @param extra extra tensor data of object.
     */
    public Object(Shape shape, Tag[] tags, Tensor extra) {
        if (shape == null) shape = new Shape();
        if (tags == null) tags = new Tag[0];
        if (extra == null) extra = new Tensor();
        this.shape = shape;
        this.tags = tags;
        this.extra = extra;
    }

    /**
     * Construct with only shape.
     * @param shape shape of object.
     */
    public Object(Shape shape) {
        this(shape, null, null);
    }

    /**
     * Construct with shape and tags.
     * @param shape shape of object.
     * @param tags tags of object.
     */
    public Object(Shape shape, Tag[] tags) {
        this(shape, tags, null);
    }

    /**
     * Construct with extra tensor data.
     * @param extra extra tensor data of object.
     */
    public Object(Tensor extra) {
        this(null, null, extra);
    }

    /**
     * Construct with tags and extra tensor data.
     * @param tags tags of object.
     * @param extra extra tensor data of object.
     */
    public Object(Tag[] tags, Tensor extra) {
        this(null, tags, extra);
    }

    /**
     * Get shape of object.
     * @return shape of object.
     */
    public Shape getShape() {
        return this.shape;
    }

    /**
     * Set shape of object.
     * @param shape shape of object.
     */
    public void setShape(Shape shape) {
        this.shape = shape;
    }

    /**
     * Get tags of object.
     * @return tags of object.
     */
    public Tag[] getTags() {
        return this.tags;
    }

    /**
     * Set tags of object.
     * @param tags tags of object.
     */
    public void setTags(Tag[] tags) {
        this.tags = tags;
    }

    /**
     * Get extra tensor data.
     * @return extra tensor data.
     */
    public Tensor getExtra() {
        return this.extra;
    }

    /**
     * Set extra tensor data.
     * @return extra tensor data.
     */
    public void setExtra(Tensor extra) {
        this.extra = extra;
    }

    /**
     * Get Tag of tags with index.
     * @param index tags index.
     * @return specific Tag.
     */
    public Tag getTag(int index) {
        return this.tags[index];
    }

    /**
     * Set Tag of tags.
     * @param index tags index.
     * @param tag tag value.
     */
    public void setTag(int index, Tag tag) {
        this.tags[index] = tag;
    }

    /**
     * Same like getTag.
     * @param index tags index.
     * @return specific Tag.
     */
    public Tag tag(int index) {
        return this.tags[index];
    }
}
