<%@ page language="java" import="java.util.*" pageEncoding="UTF-8"%>
<%@ page import="javax.servlet.*,javax.servlet.http.*,java.sql.*,javax.sql.*,javax.naming.*"%>

<%
	Context ctx = new InitialContext();
	DataSource ds = (DataSource)ctx.lookup("java:comp/env/jdbc/test");
	Connection con = ds.getConnection();
	Statement stmt = con.createStatement();      
	ResultSet rs = stmt.executeQuery("select * from users where open_id='" + open_id_sql + "';" );
%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
  <head>    
    <title>温度阻值测试结果</title>
    
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	<meta http-equiv="keywords" content="keyword1,keyword2,keyword3">
	<meta http-equiv="description" content="This is my page">
	<!--
	<link rel="stylesheet" type="text/css" href="styles.css">
	-->
	<style type="text/css">
	*{
		margin: 0px;padding: 0px;font-family: "微软雅黑";
	}
	</style>
<link href="oa/css/public.css" rel="stylesheet" type="text/css">
	<script type="text/javascript" src="js/jquery.min.js"></script>
	<script type="text/javascript" src="js/global.js"></script>
  </head>
  
  <body>
  	<table id="tempresult" class="tableBasic">
  		<tr>
  			<td style="text-align: center;">序号</td>
  			<td style="width: 100px;text-align: center;">当前温度</td>
  			<td style="width: 100px;text-align: center;">当前阻值(k&Omega;)</td>
  			<td style="width: 100px;text-align: center;">测试结果</td>
  			<td style="width: 80px;text-align: center;">测试人</td>
  			<td style="width: 160px;text-align: center;">当前时间</td>
  		</tr>
  		<s:iterator var="temp" value="#session.tempList" status="sindex">
  		<tr>
  			<td style="text-align: center;"><s:property value="#sindex.index+1"></s:property></td>
  			<td style="text-align: center;"><s:property value="#temp.temp"></s:property></td>
  			<td style="text-align: center;"><s:property value="#temp.om"></s:property></td>
  			<td style="text-align: center;"><s:property value="#temp.correct"></s:property></td>
  			<td style="text-align: center;"><s:property value="#temp.testman"></s:property></td>
  			<td style="text-align: center;"><s:property value="#temp.timenow"></s:property></td>
  		</tr>
  		</s:iterator>
  	</table>
  </body>
</html>
