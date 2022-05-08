package seeta.aip;

/**
 * AI Package's shape type.
 * Each type has requirements for points, rotate and scale.
 */
public enum ShapeType {
    /**
     * <pre>
     * Unknown shape
     * </pre>
     */
    Unknown,

    /**
     * <pre>
     * With forced rotate=0, scale=1, size>=1.
     * points represents points.
     * </pre>
     */
    Points,

     /**
     * <pre>
     * With forced rotate=0, scale=1, size>=2.
     * points represents multi lines:
     *     points[0]->points[1], points[1]->points[2], ..., points[size-2]->points[size-1].
     * </pre>
     */
    Lines,

    /**
     * <pre>
     * With forced scale=1, size=2.
     * rotate represents the angle of rotation around the center point.
     * points[0] represents the left-top corner.
     * points[1] represents the right-bottom corner.
     * </pre>
     */
    Rectangle,

    /**
     * <pre>
     * With forced rotate=0, scale=1, size=3.
     * rotate represents the angle of rotation around the center point.
     * points represents the first 3 points of parallelogram with anticlockwise.
     * </pre>
     */
    Parallelogram,

    /**
     * <pre>
     * With forced rotate=0, scale=1, size>=2.
     * points represents multi lines with anticlockwise:
     *     points[0]->points[1], points[1]->points[2], ...,
     *     points[size-2]->points[size-1], points[size-1]->points[0].
     * </pre>
     */
    Polygon,

    /**
     * <pre>
     * With forced rotate=0, size=1.
     * scale represents the radius.
	 * points[0] represents the center.
     * </pre>
     */
    Circle,

    /**
     * <pre>
     * With forced rotate=0, scale=1, size=3.
     * points[0] represents the left-top-front corner.
     * points[1] represents the right-bottom-front corner.
     * points[2] represents the right-top-back corner.
     * </pre>
     */
    Cube,

    /**
     * <pre>
     * Means the shape is undefined.
     * </pre>
     */
    NoShape,
}
