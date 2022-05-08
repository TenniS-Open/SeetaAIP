package seeta.aip;

/**
 * AI Pacakge computing device.
 */
public class Device {
    public String device = "cpu";
    public int id = 0;

    /**
     * Same to Devcie("cpu", 0).
     */
    public Device() {}

    /**
     * Build computing device object.
     * @param device compute device name, should be lower case, like cpu, gpu.
     * @param id compute device id, always start with 0.
     */
    public Device(String device, int id) {
        this.device = device;
        this.id = id;
    }

    /**
     * Same to Device(`device`, 0).
     * @param device compute device name, should be lower case, like cpu, gpu.
     */
    public Device(String device) {
        this.device = device;
    }
}
