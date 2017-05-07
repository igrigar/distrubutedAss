
package webservice;

import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.jws.WebResult;
import javax.jws.WebService;
import javax.xml.bind.annotation.XmlSeeAlso;
import javax.xml.ws.Action;
import javax.xml.ws.FaultAction;
import javax.xml.ws.RequestWrapper;
import javax.xml.ws.ResponseWrapper;


/**
 * This class was generated by the JAX-WS RI.
 * JAX-WS RI 2.2.4-b01
 * Generated source version: 2.2
 * 
 */
@WebService(name = "md5Service", targetNamespace = "http://ws/")
@XmlSeeAlso({
    ObjectFactory.class
})
public interface Md5Service {


    /**
     * 
     * @param arg0
     * @return
     *     returns java.lang.String
     * @throws Exception_Exception
     */
    @WebMethod
    @WebResult(targetNamespace = "")
    @RequestWrapper(localName = "md5", targetNamespace = "http://ws/", className = "webservice.Md5")
    @ResponseWrapper(localName = "md5Response", targetNamespace = "http://ws/", className = "webservice.Md5Response")
    @Action(input = "http://ws/md5Service/md5Request", output = "http://ws/md5Service/md5Response", fault = {
        @FaultAction(className = Exception_Exception.class, value = "http://ws/md5Service/md5/Fault/Exception")
    })
    public String md5(
        @WebParam(name = "arg0", targetNamespace = "")
        String arg0)
        throws Exception_Exception
    ;

}
