<%@ page language="java" import="java.util.*" pageEncoding="UTF-8"%>
<%@ page import="javax.servlet.*,javax.servlet.http.*,java.sql.*,javax.sql.*,javax.naming.*"%>

<%!
	private Connection getConnection() throws NamingException, SQLException  {
		Context ctx = new InitialContext();
		DataSource ds = (DataSource)ctx.lookup("java:comp/env/jdbc/test");
		Connection con = ds.getConnection();
		return con;
	}	
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
    <<link href="css/public.css" rel="stylesheet" type="text/css">
	<script type="text/javascript" src="js/jquery.min.js"></script>
	<script type="text/javascript" src="js/global.js"></script>
  </head>
  
  <body>
	<%
		Connection con = getConnection();
		Statement stmt = con.createStatement(); 
		ResultSet rs   = stmt.executeQuery("select id, temp, om, iscorrect, testman, TO_CHAR(timenow,'YYYY-MM-DD hh24:mi:ss')  from temptest_1 order by id" );		

		int nId = 0;
		float temp = 0.0f;
		float om = 0.0f;
		int nIsCorrect=0;
		String testman="";
		String testdate="";
	%>
	
  	<table id="tempresult" class="tableBasic">
  		<tr>
  			<td style="text-align: center;">序号</td>
  			<td style="width: 100px;text-align: center;">当前温度</td>
  			<td style="width: 100px;text-align: center;">当前阻值(k&Omega;)</td>
  			<td style="width: 100px;text-align: center;">测试结果</td>
  			<td style="width: 80px;text-align: center;">测试人</td>
  			<td style="width: 160px;text-align: center;">当前时间</td>
  		</tr>
		
		<%
			while ( rs.next() ) {
				nId = rs.getInt(1);
				temp = rs.getFloat(2);
				om   = rs.getFloat(3);
				nIsCorrect = rs.getInt(4);
				testman = rs.getString(5);
				testdate = rs.getString(6);
				%>
				
			<tr>
				<td style="text-align: center;"><%= nId %></td>
				<td style="text-align: center;"><%= temp %></td>
				<td style="text-align: center;"><%= om %></td>
				<td style="text-align: center;"><%= nIsCorrect %></td>
				<td style="text-align: center;"><%= testman %></td>
				<td style="text-align: center;"><%= testdate %></td>
			</tr>
				
		<%		
			} 
		%>
		
  		
  		
  	</table>
	
	<% rs.close(); %>
	<% stmt.close(); %>
	<% con.close(); %>
  </body>
</html>
