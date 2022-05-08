package seeta.aip;

/**
 * Tag of Object.
 * Use Instance's method `tag` to get descriton of Tag.
 */
public class Tag {
    /**
     * Label of this tag.
     */
    public int label = 0;

    /**
     * Score of this tag.
     */
    public float score = 0;

    /**
     * Construct Tag(0, 0).
     */
    public Tag() {}

    /**
     * Construct tag with given parameters.
     * @param label label of tag.
     * @param score score of tag.
     */
    public Tag(int label, float score) {
        this.label = label;
        this.score = score;
    }
}