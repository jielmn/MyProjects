:: jdk6的java命令 
C:\jdk6\jdk1.6.0_43\bin\wsimport -s src -p mypack1 -d classes -keep http://localhost:9090/Service/ServiceHello?wsdl
C:\jdk6\jdk1.6.0_43\bin\javac -sourcepath src -d classes src/mypack/ServiceTest.java