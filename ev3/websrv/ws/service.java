package ws;

import javax.jws.WebService;
import javax.jws.WebMethod;

@WebService
public class service {
    @WebMethod
    public void prnt_str(String str) {
        System.out.println(str);
    }
}
