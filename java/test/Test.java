import seeta.aip.Engine;
import seeta.aip.Exception;

public class Test {
    public static void main(String[] argv) throws seeta.aip.Exception {
        System.out.println("Hey, I'm running!");

        Engine a = new Engine("../../lib/copy");
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


        a.dispose();
    }
}