package seeta.aip;

/**
 * Shape of Object
 */
public class Shape {
    /**
     * Object shape, describe by landmarks, rotate and scale. @see ShapeType for more details.
     */
    public ShapeType type = ShapeType.Unknown;

    /**
     * Landmarks of shape. @see ShapeType for more details.
     */
    public Point[] landmarks = new Point[0];

    /**
     * Rotate of shape. Using degree measure, positive value means anticlockwise.
     */
    public float rotate = 0;

    /**
     * Normally means scale of point's coordinate, specially means radius of a circle.
     */
    public float scale = 1;

    /**
     * Construct an `Unknown` shape. with no landmarks, rotate = 0 and scale = 1.
     */
    public Shape() {}

    /**
     * Construct shape with given parameters. @see ShapeType for more detials.
     * @param type shape type.
     * @param landmarks list of points.
     * @param rotate rotate of shape.
     * @param scale scale of shape.
     */
    public Shape(ShapeType type, Point[] landmarks, float rotate, float scale) {
        this.type = type;
        this.landmarks = landmarks;
        this.rotate = rotate;
        this.scale = scale;
    }

    /**
     * Construct shape with given parameters. @see ShapeType for more detials.
     * @param type shape type.
     * @param landmarks list of points.
     */
    public Shape(ShapeType type, Point[] landmarks) {
        this(type, landmarks, 0, 1);
    }
}
