package seeta.aip;

public class Device {
    public String device = "cpu";
    public int id = 0;

    public Device() {}

    public Device(String device, int id) {
        this.device = device;
        this.id = id;
    }

    public Device(String device) {
        this.device = device;
    }
}
