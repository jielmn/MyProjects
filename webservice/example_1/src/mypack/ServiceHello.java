package mypack;

import javax.jws.WebService;
import javax.xml.ws.Endpoint;

@WebService
public class ServiceHello {

    public String getValue(String name){
        return "Hello:"+name;
    }
    public static void main(String[] args) {

        Endpoint.publish("http://localhost:9090/Service/ServiceHello", new ServiceHello());
        System.out.println("Service success!");
    }

}