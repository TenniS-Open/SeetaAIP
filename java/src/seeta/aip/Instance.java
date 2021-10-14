package seeta.aip;

public class Instance {
    private long handle = 0;
    private Package aip;
    private Engine engine;

    public Instance(Package aip, Device device, String[] models, Object[] objects) throws seeta.aip.Exception {
        if (device == null) device = new Device();
        if (models == null) models = new String[0];
        if (objects == null) objects = new Object[0];
        this.aip = aip;
        this.handle = this.aip.create(device, models, objects);
    }

    public Instance(Engine engine, Device device, String[] models, Object[] objects) throws seeta.aip.Exception {
        this(engine.getAIP(), device, models, objects);
    }

    private Instance(Engine engine, Device device, String[] models, Object[] objects,
                     boolean borrow_engine) throws seeta.aip.Exception {
        this(engine, device, models, objects);
        if (!borrow_engine) {
            this.engine = engine;
        }
    }

    public Instance(String libname, Device device, String[] models, Object[] objects) throws seeta.aip.Exception {
        this(new Engine(libname), device, models, objects, false);
    }

    public Instance(Package aip, Device device, String[] models) throws seeta.aip.Exception {
        this(aip, device, models, null);
    }

    public Instance(Engine engine, Device device, String[] models) throws seeta.aip.Exception {
        this(engine, device, models, null);
    }

    public Instance(String libname, Device device, String[] models) throws seeta.aip.Exception {
        this(libname, device, models, null);
    }

    public void dispose() {
        if (handle != 0) {
            this.aip.free(handle);
            handle = 0;
        }
        if (engine != null) {
            engine.dispose();
            engine = null;
        }
    }

    @Deprecated
    protected void finalize() throws Throwable {
        try {
            this.dispose();
        } catch(Throwable t){
            throw t;
        } finally {
            super.finalize();
        }
    }
    
    public String error(int errorCode) {
        return this.aip.error(handle, errorCode);
    }

    public String[] property() throws seeta.aip.Exception {
        return this.aip.property(handle);
    }

    public void setd(String name, double value) throws seeta.aip.Exception {
        this.aip.setd(handle, name, value);
    }

    public void set(String name, double value) throws seeta.aip.Exception {
        this.aip.setd(handle, name, value);
    }

    public double getd(String name) throws seeta.aip.Exception {
        return this.aip.getd(handle, name);
    }

    public void reset() throws seeta.aip.Exception {
        this.aip.reset(handle);
    }

    public Result forward(int methodID, ImageData[] images, Object[] objects) throws seeta.aip.Exception {
        return this.aip.forward(handle, methodID, images, objects);
    }

    public String tag(int methodID, int labelIndex, int labelValue) throws seeta.aip.Exception {
        return this.aip.tag(handle, methodID, labelIndex, labelValue);
    }

    public void set(String name, Object value) throws seeta.aip.Exception {
        this.aip.set(handle, name, value);
    }

    public Object get(String name) throws seeta.aip.Exception {
        return this.aip.get(handle, name);
    }
}
