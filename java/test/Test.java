import com.seetatech.aip.Engine;

public class Test {
    public static void main(String[] argv) {
        System.out.println("Hey, I'm running!");

        Engine a = new Engine("../../lib/copy");
        com.seetatech.aip.Package aip = a.getAIP();

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