import com.seetatech.aip.Engine;

public class Test {
    public static void main(String[] argv) {
        Engine a = new Engine("../../lib/copy");
        com.seetatech.aip.Package aip = a.load();
        com.seetatech.aip.Package aip2 = a.load();
        com.seetatech.aip.Package aip3 = a.load();
        com.seetatech.aip.Package aip4 = a.load();
        com.seetatech.aip.Package aip5 = a.load();
        a.dispose();
        System.out.println("Hey, I'm running!");
    }
}