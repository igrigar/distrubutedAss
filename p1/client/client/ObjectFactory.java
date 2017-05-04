
package client;

import javax.xml.bind.JAXBElement;
import javax.xml.bind.annotation.XmlElementDecl;
import javax.xml.bind.annotation.XmlRegistry;
import javax.xml.namespace.QName;


/**
 * This object contains factory methods for each 
 * Java content interface and Java element interface 
 * generated in the client package. 
 * <p>An ObjectFactory allows you to programatically 
 * construct new instances of the Java representation 
 * for XML content. The Java representation of XML 
 * content can consist of schema derived interfaces 
 * and classes representing the binding of schema 
 * type definitions, element declarations and model 
 * groups.  Factory methods for each of these are 
 * provided in this class.
 * 
 */
@XmlRegistry
public class ObjectFactory {

    private final static QName _Md5_QNAME = new QName("http://ws/", "md5");
    private final static QName _Md5Response_QNAME = new QName("http://ws/", "md5Response");
    private final static QName _Exception_QNAME = new QName("http://ws/", "Exception");

    /**
     * Create a new ObjectFactory that can be used to create new instances of schema derived classes for package: client
     * 
     */
    public ObjectFactory() {
    }

    /**
     * Create an instance of {@link Md5 }
     * 
     */
    public Md5 createMd5() {
        return new Md5();
    }

    /**
     * Create an instance of {@link Md5Response }
     * 
     */
    public Md5Response createMd5Response() {
        return new Md5Response();
    }

    /**
     * Create an instance of {@link Exception }
     * 
     */
    public Exception createException() {
        return new Exception();
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link Md5 }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://ws/", name = "md5")
    public JAXBElement<Md5> createMd5(Md5 value) {
        return new JAXBElement<Md5>(_Md5_QNAME, Md5 .class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link Md5Response }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://ws/", name = "md5Response")
    public JAXBElement<Md5Response> createMd5Response(Md5Response value) {
        return new JAXBElement<Md5Response>(_Md5Response_QNAME, Md5Response.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link Exception }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://ws/", name = "Exception")
    public JAXBElement<Exception> createException(Exception value) {
        return new JAXBElement<Exception>(_Exception_QNAME, Exception.class, null, value);
    }

}
