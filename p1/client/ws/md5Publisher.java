package ws;

import javax.xml.ws.Endpoint;

public class md5Publisher {
    public static void main(String[] args) {
        final String url = "http://localhost:8888/rs";
        System.out.println("MD5 Service at: " + url);
        Endpoint.publish(url, new md5Service());
    }
}
