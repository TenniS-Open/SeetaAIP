import seeta.aip.Engine;
import seeta.aip.Exception;
import seeta.aip.Device;
import seeta.aip.Object;
import seeta.aip.ImageData;

public class Test {
    public static void main(String[] argv) throws seeta.aip.Exception {
        System.out.println("Hey, I'm running!");

        Engine a = new Engine("../../lib/log");
        seeta.aip.Package aip = a.getAIP();

        System.out.printf("AIP's description: %s\n", aip.description);
        System.out.printf("AIP's mID: %s\n", aip.mID);
        System.out.printf("AIP's sID: %s\n", aip.sID);
        System.out.printf("AIP's version: %s\n", aip.version);
        System.out.print("AIP's support: [");
        for (int i = 0; i < aip.support.length; ++i) {
            if (i > 0) {
                System.out.print(", ");
            }
            System.out.print(aip.support[i]);
        }
        System.out.println("]");

        long handle = aip.create(new Device("cpu"), new String[]{"A"}, new Object[0]);
        aip.forward(handle, 0, new ImageData[]{new ImageData(new byte[]{1, 2, 3}, 1, 1, 1, 3)}, new Object[]{new Object()});
        aip.free(handle);

        a.dispose();
    }
}