package seeta.aip;

public class Shape {
    public ShapeType type = ShapeType.Unknown;
    public Point[] landmarks = new Point[0];
    public float rotate = 0;
    public float scale = 1;

    public Shape() {}

    public Shape(ShapeType type, Point[] landmarks, float rotate, float scale) {
        this.type = type;
        this.landmarks = landmarks;
        this.rotate = rotate;
        this.scale = scale;
    }

    public Shape(ShapeType type, Point[] landmarks) {
        this(type, landmarks, 0, 1);
    }
}
