@rem check parameter
@echo off
set /a param_num=0

::for %%a in (%*) do echo %%a
for %%a in (%*) do set /a param_num+=1
::echo total param count = %param_num%


if %param_num% == 1 (
	goto param_1
) else (
	if %param_num% == 0 (
		goto param_0
	)
	goto end
)

:param_1
if %1 == clean (
	echo do something to clean...
)
goto end

:param_0
echo compileing...
javac -encoding UTF-8 -classpath ../../lib/servlet-api.jar;WEB-INF/lib/json.jar;WEB-INF/lib/httpclient-4.5.5.jar;WEB-INF/lib/httpcore-4.4.9.jar;WEB-INF/lib/commons-fileupload-1.3.3.jar  -sourcepath src -d WEB-INF/classes src/mypack/UserServlet.java
goto end

:end
echo end.
