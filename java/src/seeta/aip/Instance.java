package seeta.aip;

/**
 * Instance of AI package.
 * Can build with seeta.aip.Pacakge or seeta.aip.Engine.
 * </br>
 * Example to create instance:
 * <pre>
 * Engine dll = new Engine("libname");
 * Instance instance = new Instance(dll, new Device("cpu"), new String[]{"model.json"}, null);
 * </pre>
 * </br>
 * Example to print properties:
 * <pre>
 * String[] properties = instance.property();
 * for (int i = 0; i < properties.length; i += 1) {
 *     System.out.printf("[Property] %s = %f\n", properties[i], instance.getd(properties[i]));
 * }
 * </pre>
 */
public class Instance {
    private long handle = 0;
    private Package aip;
    private Engine engine;

    /**
     * Construct instance with AI Package description and specific computing device.
     * Call {@code dispose} explicit if you want dispose immediately.
     * @param aip AI Pakcage description {@code Engine}.
     * @param device [optinal] computing device, use `cpu(0)` by default.
     * @param models [optinal] list of paths to models.
     * @param objects [optinal] list of config {@code Object}.
     * @throws seeta.aip.Exception when library loaded or construre faied.
     */
    public Instance(Package aip, Device device, String[] models, Object[] objects) throws seeta.aip.Exception {
        if (device == null) device = new Device();
        if (models == null) models = new String[0];
        if (objects == null) objects = new Object[0];
        this.aip = aip;
        this.handle = this.aip.create(device, models, objects);
    }

    /**
     * Construct instance with DLL's engine and specific computing device.
     * Call {@code dispose} explicit if you want dispose immediately.
     * @param engine DLL's engine {@code Engine}.
     * @param device [optinal] computing device, use `cpu(0)` by default.
     * @param models [optinal] list of paths to models.
     * @param objects [optinal] list of config {@code Object}.
     * @throws seeta.aip.Exception when library loaded or construre faied.
     */
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

    /**
     * Construct instance with library name and specific computing device.
     * Call {@code dispose} explicit if you want dispose immediately.
     * @param libname library name which will be parse to {@code Engine}.
     * @param device [optinal] computing device, use `cpu(0)` by default.
     * @param models [optinal] list of paths to models.
     * @param objects [optinal] list of config {@code Object}.
     * @throws seeta.aip.Exception when library loaded or construre faied.
     */
    public Instance(String libname, Device device, String[] models, Object[] objects) throws seeta.aip.Exception {
        this(new Engine(libname), device, models, objects, false);
    }

    /**
     * Construct instance with AI Package description and specific computing device.
     * Call {@code dispose} explicit if you want dispose immediately.
     * @param aip AI Pakcage description {@code Engine}.
     * @param device [optinal] computing device, use `cpu(0)` by default.
     * @param models [optinal] list of paths to models.
     * @throws seeta.aip.Exception when library loaded or construre faied.
     */
    public Instance(Package aip, Device device, String[] models) throws seeta.aip.Exception {
        this(aip, device, models, null);
    }

    /**
     * Construct instance with DLL's engine and specific computing device.
     * Call {@code dispose} explicit if you want dispose immediately.
     * @param engine DLL's engine {@code Engine}.
     * @param device [optinal] computing device, use `cpu(0)` by default.
     * @param models [optinal] list of paths to models.
     * @throws seeta.aip.Exception when library loaded or construre faied.
     */
    public Instance(Engine engine, Device device, String[] models) throws seeta.aip.Exception {
        this(engine, device, models, null);
    }

    /**
     * Construct instance with library name and specific computing device.
     * Call {@code dispose} explicit if you want dispose immediately.
     * @param libname library name which will be parse to {@code Engine}.
     * @param device [optinal] computing device, use `cpu(0)` by default.
     * @param models [optinal] list of paths to models.
     * @throws seeta.aip.Exception when library loaded or construre faied.
     */
    public Instance(String libname, Device device, String[] models) throws seeta.aip.Exception {
        this(libname, device, models, null);
    }

    /**
     * Dispose instance immediately, make sure instance has no longer to use.
     * You can just leave this method implicit by `finalize`.
     */
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
    
    /**
     * Get description message of give error number.
     * The error code shoud be returned by instance methods.
     * If `errorCode` is -1, get lastest error's description message.
     * @param errorCode error number.
     * @return description message.
     */
    public String error(int errorCode) {
        return this.aip.error(handle, errorCode);
    }

    /**
     * Get all properties can be get or set. 
     * @return list of properties.
     * @throws seeta.aip.Exception user mode call will not throw.
     */
    public String[] property() throws seeta.aip.Exception {
        return this.aip.property(handle);
    }

    /**
     * Set property with float value.
     * @param name property name.
     * @param value property value.
     * @throws seeta.aip.Exception when name is not exsits or value is not valid.
     */
    public void setd(String name, double value) throws seeta.aip.Exception {
        this.aip.setd(handle, name, value);
    }

    /**
     * Set property with float value.
     * @param name property name.
     * @param value property value.
     * @throws seeta.aip.Exception when name is not exsits or value is not valid.
     */
    public void set(String name, double value) throws seeta.aip.Exception {
        this.aip.setd(handle, name, value);
    }

    /**
     * Get float property.
     * @param name property name.
     * @return property value.
     * @throws seeta.aip.Exception when name is not exsits.
     */
    public double getd(String name) throws seeta.aip.Exception {
        return this.aip.getd(handle, name);
    }

    /**
     * Reset instance state. ONLY work in stream mode (frames are time or serial dependent).
     * @throws seeta.aip.Exception user mode call will not throw.
     */
    public void reset() throws seeta.aip.Exception {
        this.aip.reset(handle);
    }

    /**
     * Call instance method with images and objects, return images and objects in {@code Result}.
     * Check AI Pakcage documents to find out posible method id and required images and objects.
     * Different AIPS provide different methods and call requirements.
     * @param methodID method id, must greater than 0.
     * @param images method requires images.
     * @param objects method requires images.
     * @return result contains images and objects.
     * @throws seeta.aip.Exception when parameter not valid or other runtime exception.
     */
    public Result forward(int methodID, ImageData[] images, Object[] objects) throws seeta.aip.Exception {
        return this.aip.forward(handle, methodID, images, objects);
    }

    /**
     * Get result's objects tag description. {@code Object} for more detail.
     * @param methodID the result's called method id, leave 0 if no related to method id.
     * @param labelIndex the object' tag index, used when there are many tags in one object.
     * @param labelValue the object tag's label.
     * @return Description of tag. Get empty if no tag description found.
     * @throws seeta.aip.Exception user mode call will not throw.
     */
    public String tag(int methodID, int labelIndex, int labelValue) throws seeta.aip.Exception {
        return this.aip.tag(handle, methodID, labelIndex, labelValue);
    }

    /**
     * Set property with object value.
     * @param name property name.
     * @param value property value.
     * @throws seeta.aip.Exception when name is not exsits or value is not valid.
     */
    public void set(String name, Object value) throws seeta.aip.Exception {
        this.aip.set(handle, name, value);
    }

    /**
     * Get object property.
     * @param name property name.
     * @return property value.
     * @throws seeta.aip.Exception when name is not exsits.
     */
    public Object get(String name) throws seeta.aip.Exception {
        return this.aip.get(handle, name);
    }
}
