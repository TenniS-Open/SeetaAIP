package seeta.aip;

/**
 * AI Pakcage Point in image.
 * Origin is the left upper corner.
 */
public class Point {
    /**
     * X means coordinates from left to right.
     */
    public float x = 0;
    
    /**
     * Y means coordinates from top to bottom.
     */
    public float y = 0;

    /**
     * Construct Point(0, 0).
     */
    public Point() {}

    /**
     * Construct point with given coordinate.
     * @param x x coordinate.
     * @param y x coordinate.
     */
    public Point(float x, float y) {
        this.x = x;
        this.y = y;
    }
}
