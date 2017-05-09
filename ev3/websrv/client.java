import java.net.*;
import java.util.Arrays;
import service.ServiceService;
import service.Service;

public class client {
    public static void main(String argv[]) {

        long t1, t2;
        int iter = 100;
        char[] str;
        try {
            ServiceService strService = new ServiceService(
                new URL("http://" + argv[0] + ":8888/rs?wsdl"));
            Service port = strService.getServicePort();

            for (int length:new int[] {10, 100, 200, 500, 1000, 2000, 5000, 
                    10000, 15000, 20000, 100000}) {
                str = new char[length];
                Arrays.fill(str, 'a');

                t1 = System.currentTimeMillis();
                for (int i = 0; i < iter; ++i) port.prntStr(new String(str));
                t2 = System.currentTimeMillis();
                System.out.println(String.valueOf(length) + ": " + String.valueOf(t2-t1));
            }
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }
    }
}
