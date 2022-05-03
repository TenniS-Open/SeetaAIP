package seeta.aip;

public class Object {
    private Shape shape = new Shape();       ///< must be set
    private Tag[] tags = new Tag[0];         ///< must be set
    private Tensor extra = new Tensor();     ///< can be null, or empty tensor

    public Object() {}

    public Object(Shape shape, Tag[] tags, Tensor extra) {
        if (shape == null) shape = new Shape();
        if (tags == null) tags = new Tag[0];
        if (extra == null) extra = new Tensor();
        this.shape = shape;
        this.tags = tags;
        this.extra = extra;
    }

    public Object(Shape shape) {
        this(shape, null, null);
    }

    public Object(Shape shape, Tag[] tags) {
        this(shape, tags, null);
    }

    public Object(Tensor extra) {
        this(null, null, extra);
    }

    public Object(Tag[] tags, Tensor extra) {
        this(null, tags, extra);
    }

    public Shape getShape() {
        return this.shape;
    }

    public void setShape(Shape shape) {
        this.shape = shape;
    }

    public Tag[] getTags() {
        return this.tags;
    }

    public void setTags(Tag[] tags) {
        this.tags = tags;
    }

    public Tensor getExtra() {
        return this.extra;
    }

    public void setExtra(Tensor extra) {
        this.extra = extra;
    }

    public Tag getTag(int index) {
        return this.tags[index];
    }

    public void setTag(int index, Tag tag) {
        this.tags[index] = tag;
    }

    public Tag tag(int index) {
        return this.tags[index];
    }
}
