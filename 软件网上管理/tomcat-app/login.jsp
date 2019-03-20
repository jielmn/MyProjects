<%@ page contentType="text/html; charset=utf-8" %>
<% 	
	String username = request.getParameter("username");
	String password = request.getParameter("password");
	String errMsg = "";
	
	// 请求登录
	if ( username != null && password != null ) {
		// 登录成功
		if (username.equals("telemed") && password.equals("87374108") ) {
			session.setAttribute("username", username);
			response.sendRedirect("manage.jsp");
		} else {
			errMsg = "用户名或密码错误";
		}		
	}
%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
        "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
    <title>软件后台管理-登录</title>
    <meta http-equiv="Content-Type" content="text/html;charset=utf-8"/>
	<link href="css/index.css" rel="stylesheet">
</head>
<body>
	<div class="banner" ></div>
	<div class="divMain" >
		<form style="margin:0 auto;margin:10px 200px;" action="login.jsp" method="post" >
			<table border="0">		  
			  <tr>
				<td width="100">用户名：</td>
				<td><input type="text" name="username" /></td>
			  </tr>
			  
			  <tr>
				<td>密码：</td>
				<td><input type="password" name="password" /></td>
			  </tr>	
			  
			  <tr>
				<td colspan="2" align="center" >&nbsp;</td>
			  </tr>
			  
			  <tr>
				<td colspan="2" align="center" ><input type="submit" value="登录" /></td>
			  </tr>
			  
			  <tr>
				<td colspan="2" align="center" >&nbsp;</td>
			  </tr>
			  
			  <tr>
				<td colspan="2" align="center" style="font-size:10px;color:red;" ><%=errMsg%>&nbsp;</td>
			  </tr>
			  
			</table>
			
		</form>
	</div>

</body>
</html>