package seeta.aip;

public class Object {
    public class Tag {
        public int label = 0;
        public float score = 0;

        public Tag() {}
        
        public Tag(int label, float score) {
            this.label = label;
            this.score = score;
        }
    }
    
    public Shape shape = new Shape();       ///< must be set
    public Tag[] tags = new Tag[0];         ///< must be set
    public Tensor extra = new Tensor();     ///< can be null, or empty tensor
}
