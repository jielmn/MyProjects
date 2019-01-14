#!/bin/bash
if [ $# -eq 1 ];then
   if [ $1 == "clean" ];then
     cd tmp
     rm -fr *.png
     rm -fr *.jpg
   fi
elif [ $# -eq 0 ];then
   javac -classpath ../../lib/servlet-api.jar:mysql-connector-java-8.0.13.jar:WEB-INF/lib/json.jar:WEB-INF/lib/httpclient-4.5.5.jar:WEB-INF/lib/httpcore-4.4.9.jar:WEB-INF/lib/commons-fileupload-1.3.3.jar  -sourcepath src -d WEB-INF/classes src/mypack/MainServlet.java
fi
