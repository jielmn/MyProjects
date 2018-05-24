#!/bin/bash

javac -classpath ../../lib/servlet-api.jar:$JAVA_HOME/lib/mysql-connector-java-5.1.45-bin.jar:$JAVA_HOME/lib/json.jar:$JAVA_HOME/lib/httpclient-4.5.5.jar:$JAVA_HOME/lib/httpcore-4.4.9.jar:$JAVA_HOME/lib/commons-fileupload-1.3.3.jar  -sourcepath src -d WEB-INF/classes src/mypack/RequestInfoServlet.java
