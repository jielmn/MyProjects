package mypack;

import mypack1.ServiceHello;
import mypack1.ServiceHelloService;

public class ServiceTest {

    public static void main(String[] args) {
        ServiceHello hello=new ServiceHelloService().getServiceHelloPort();
        String name=hello.getValue("world!!!");
        System.out.println(name);
    }

}