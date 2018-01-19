#!/bin/bash
javac -classpath ../../lib/servlet-api.jar:$JAVA_HOME/lib/mysql-connector-java-5.1.45-bin.jar  -sourcepath src -d WEB-INF/classes src/mypack/MainServlet.java
