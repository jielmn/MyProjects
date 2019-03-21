<%@ page contentType="text/html; charset=utf-8" %>
<%@ page import="java.sql.*" %>
<%@ page import="javax.sql.*" %>
<%@ page import="javax.naming.*" %>
<%@ page import="java.text.SimpleDateFormat" %>

<%!
	public Connection getConnection() throws NamingException, SQLException  {
		Context ctx = new InitialContext();
		DataSource ds = (DataSource)ctx.lookup("java:comp/env/jdbc/myjdbc");
		Connection con = ds.getConnection();
		return con;
	}
%>

<% 
	// 必须登录
	String name = ""; 
	name = (String)session.getAttribute("username");
	if ( name == null ) 
		name = "";
	
	if ( name.equals("") ) {
		response.sendRedirect("login.jsp");
		return;
	}
%>

<!DOCTYPE HTML>
<html>
    <head>
		<META http-equiv="Content-Type" content="text/html; charset=utf-8">
        <title>测试</title>
		<link href="css/index.css" rel="stylesheet">
    </head>
    <body>
		<div class="banner" ></div>		
		<div class="divMain" >
			<div class="divAdd" align="right"><a class="softDown" href="item.jsp?type=add">添加</a></div>
			<div style="width:100%;height:1px;background-color:gray;"></div>
			
<%
	try {
		
		Connection con = null;
		con = getConnection();
		
		Statement stmt = con.createStatement();      
		ResultSet rs = stmt.executeQuery( "select * from items order by id desc" );
		
		SimpleDateFormat time=new SimpleDateFormat("yyyy年MM月dd日"); 
		// result
		while ( rs.next() ) {
			int        item_id      = rs.getInt(1);
			String     title        = rs.getString(2);
			String     brief        = rs.getString(3);
			String     content      = rs.getString(4);
			Timestamp  c_time       = rs.getTimestamp(5);
			String     softwarePath = rs.getString(6);
			String     coverPath    = rs.getString(7);
%>
			<div class="divItem">
				<div class="divImg"><img src="<%=coverPath%>" style="width:90px;height:90px;margin:5px;" /></div>
				<div class="divContent">
					<div style="height:35px;"><a class="title" style="font-size:20px;" href="item.jsp?type=modify&itemid=<%=item_id%>"><b><%=title%></b></a></div>
					<div class="overFlow" style="font-size:18px;height:35px;color:#BC1739;" ><%=brief%></div>
					<div style="font-size:10px;"><%=time.format(c_time)%></div>
				</div>
				<div class="divDownload"><a class="softDown" href="abc.text">删除</a></div>
			</div>
<%
		} 
		rs.close();
		stmt.close();
		con.close();
	} catch (Exception ex ) {
		out.print(ex.getMessage());
		return;
	}
%>			
			<!--
			<div class="divItem">
				<div class="divImg"><img src="images/1.png" style="width:90px;height:90px;margin:5px;" /></div>
				<div class="divContent">
					<div><a class="title" href="www.abc.com"><b>软件1 1.0</b></a></div>
					<div class="overFlow">迅雷7官方版是一款极具特色的高速下载工具，迅雷7.9官方正式版基于多资源超线程技术，不仅加快了启动速度，更为用户提供更快速地下载，同时新增智能换肤、一键下载等多样功能，为用户带来更快、更好、更简便的高速下载全新体验!迅雷官网已停止该旧版本（迅雷7）推广，要求不再提供下载（该页面所提供的软件包为迅雷9软件）！迅雷官网最新版...</div>
					<div>2019-03-18</div>
				</div>
				<div class="divDownload"><a class="softDown" href="abc.text">删除</a></div>
			</div>	
			-->
			
		</div>
    </body>
</html>