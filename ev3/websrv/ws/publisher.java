package ws;

import javax.xml.ws.Endpoint;

public class publisher {
    public static void main(String[] args) {
        final String url = "http://localhost:8888/rs";
        System.out.println("Service at: " + url);
        Endpoint.publish(url, new service());
    }
}
