package ws;

import javax.jws.WebService;
import javax.jws.WebMethod;
import java.security.MessageDigest;

@WebService
public class md5Service {
    @WebMethod
    public String md5(String msg) throws Exception {
        MessageDigest md = MessageDigest.getInstance("MD5");
        byte[] dataBytes = msg.getBytes();
        md.update(dataBytes, 0, msg.length());
        byte[] mdbytes = md.digest();

        StringBuffer sb = new StringBuffer();
        for (int i = 0; i < mdbytes.length; i++) {
          sb.append(Integer.toString((mdbytes[i] & 0xff) + 0x100, 16).substring(1));
        }

        return sb.toString();
    }
}
