
package service;

import javax.xml.bind.JAXBElement;
import javax.xml.bind.annotation.XmlElementDecl;
import javax.xml.bind.annotation.XmlRegistry;
import javax.xml.namespace.QName;


/**
 * This object contains factory methods for each 
 * Java content interface and Java element interface 
 * generated in the service package. 
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

    private final static QName _PrntStr_QNAME = new QName("http://ws/", "prnt_str");
    private final static QName _PrntStrResponse_QNAME = new QName("http://ws/", "prnt_strResponse");

    /**
     * Create a new ObjectFactory that can be used to create new instances of schema derived classes for package: service
     * 
     */
    public ObjectFactory() {
    }

    /**
     * Create an instance of {@link PrntStrResponse }
     * 
     */
    public PrntStrResponse createPrntStrResponse() {
        return new PrntStrResponse();
    }

    /**
     * Create an instance of {@link PrntStr }
     * 
     */
    public PrntStr createPrntStr() {
        return new PrntStr();
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link PrntStr }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://ws/", name = "prnt_str")
    public JAXBElement<PrntStr> createPrntStr(PrntStr value) {
        return new JAXBElement<PrntStr>(_PrntStr_QNAME, PrntStr.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link PrntStrResponse }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://ws/", name = "prnt_strResponse")
    public JAXBElement<PrntStrResponse> createPrntStrResponse(PrntStrResponse value) {
        return new JAXBElement<PrntStrResponse>(_PrntStrResponse_QNAME, PrntStrResponse.class, null, value);
    }

}
