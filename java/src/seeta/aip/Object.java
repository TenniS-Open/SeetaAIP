package seeta.aip;

public class Object {
    public class Tag {
        public int label;
        public float score;
    }
    
    public Shape shape;     ///< must be set
    public Tag[] tags;      ///< must be set
    public Tensor extra;    ///< can be null, or empty tensor
}
