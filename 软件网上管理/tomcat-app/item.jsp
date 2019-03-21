<%@ page contentType="text/html; charset=utf-8" %>
<%@ page import="org.apache.commons.fileupload.FileItem" %>
<%@ page import="org.apache.commons.fileupload.FileUploadException" %>
<%@ page import="org.apache.commons.fileupload.disk.DiskFileItemFactory" %>
<%@ page import="org.apache.commons.fileupload.servlet.ServletFileUpload" %>
<%@ page import="java.util.*" %>
<%@ page import="java.io.*" %>
<%@ page import="java.sql.*" %>
<%@ page import="javax.sql.*" %>
<%@ page import="javax.naming.*" %>

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


<%
	String type = request.getParameter("type");
	if ( type == null ) {
		type = "";
	}
	
	String strItemId = request.getParameter("itemid");
	if ( strItemId == null ) {
		strItemId = "-1";
	}
	
	int itemId = Integer.parseInt(strItemId);
	
	// 如果是本页面提交
	if ( type.equals("submit")  ) {
		DiskFileItemFactory factory = new DiskFileItemFactory();
		ServletFileUpload upload = new ServletFileUpload(factory);
		
		try {
			List items = upload.parseRequest(request);
			Iterator itr = items.iterator();
			String    itemName = null;
			FileItem  itemFile = null;
			
			String title   = null;
			String brief   = null;
			String content = null;
			
			while (itr.hasNext()) {				
				FileItem item = (FileItem) itr.next();
				if (item.isFormField()) {
					if ( item.getFieldName().equals("filename") ) {
						itemName = item.getString("UTF-8");
					} else if ( item.getFieldName().equals("title") ) {
						title = item.getString("UTF-8");
					} else if ( item.getFieldName().equals("brief") ) {
						brief = item.getString("UTF-8");
					} else if ( item.getFieldName().equals("editorValue") ) {
						content = item.getString("UTF-8");
					} else if ( item.getFieldName().equals("itemid") ) {
						itemId = Integer.parseInt( item.getString("UTF-8") );
					}
				} else {
					if (item.getName() != null && !item.getName().equals("")) {
						itemFile = item;
					}
				}
			}
			
			//out.print("itemName="+itemName);
			//out.print("itemFile="+itemFile);
			
			if ( itemName != null && itemFile != null ) {
				int pos = itemFile.getName().lastIndexOf(".");  
				String post_fix = new String();
				if ( pos > -1 ) {
					post_fix = itemFile.getName().substring( pos );
				}
									
				String newRelativeName = "software/" + itemName + post_fix;
				String newFileName = this.getServletContext().getRealPath("/") + newRelativeName;
														
				File file = new File(newFileName);
				itemFile.write(file);
				
				// 得到各个参数				
				// out.print("title="+title+"<br/>");
				// out.print("brief="+brief+"<br/>");
				// out.print("content="+content+"<br/>");
				// out.print("id="+itemId+"<br/>");
				
				String newTitle   = title.replace("'", "''");
				String newBrief   = brief.replace("'", "''");
				String newContent = content.replace("'", "''");
				String newName = newRelativeName.replace("'", "''");
				
				// 写sql
				Connection con = null;
				con = getConnection();				
				Statement stmt = con.createStatement();      
				stmt.executeUpdate( "insert into items values( null, '" + title + "', '" + brief + "', '" + content + "', now(), '" + newName + "' ) " );
				stmt.close();
				con.close();
			}
			
			response.sendRedirect("manage.jsp");
		} catch (Exception e) {
			out.print(e.getMessage());
		}
		return;
	}
	
	boolean bAdd = true;
	if ( type.equals("modify") ) {
		bAdd = false;
	}
%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
        "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
    <title>软件后台管理</title>
    <meta http-equiv="Content-Type" content="text/html;charset=utf-8"/>
    <script type="text/javascript" charset="utf-8" src="ueditor.config.js"></script>
    <script type="text/javascript" charset="utf-8" src="ueditor.all.min.js"> </script>
    <!--建议手动加在语言，避免在ie下有时因为加载语言失败导致编辑器加载失败-->
    <!--这里加载的语言文件会覆盖你在配置项目里添加的语言类型，比如你在配置项目里配置的是英文，这里加载的中文，那最后就是中文-->
    <script type="text/javascript" charset="utf-8" src="lang/zh-cn/zh-cn.js"></script>

    <script type="text/javascript">
		function check( form ) {
			
			if ( form.title.value.length == 0 ) {
				alert("标题不能为空");
				return false;
			}
			
			if ( form.brief.value.length == 0 ) {
				alert("简略描述不能为空");
				return false;
			}
			
<%
	if ( bAdd ) {
%>
			if ( form.filename.value.length == 0 ) {
				alert("软件上传名不能为空");
				return false;
			}
			
			if ( form.file.value.length == 0 ) {
				alert("待上传软件不能为空");
				return false;
			}
			
			// 文件不能大于20M
			if ( form.file.files[0].size > 1024 * 1024 * 20 ) {
				alert("待上传软件不能大于20M");
				return false;
			}
			
<%	} else { %>
			if ( form.file.value.length > 0 ) {
				if ( form.filename.value.length == 0 ) {
					alert("如果要上传文件，软件上传名不能为空");
					return false;
				}
				
				// 文件不能大于20M
				if ( form.file.files[0].size > 1024 * 1024 * 20 ) {
					alert("待上传软件不能大于20M");
					return false;
				}
			}
<%
	}
%>
			var content = UE.getEditor('editor').getContent();
			if ( content.length == 0 ) {
				alert("内容不能为空");
				return false;
			}
			
			return true;
		}
	</script>
</head>
<body>
<div>
	<form style="width:1024px;margin:0 auto;" action="item.jsp?type=submit" onsubmit="return check(this);" method="post" enctype="multipart/form-data" >
		<table border="0">		  
		  <tr>
			<td width="100">标题：</td>
			<td><input type="text" style="width:300px;" name="title" /></td>
		  </tr>
		  
		  <tr>
			<td>简略描述：</td>
			<td><input type="text" style="width: 100%; " name="brief" /></td>
		  </tr>
		  
<%
	if ( !bAdd ) {
%>
		  <tr>
			<td>软件：</td>
			<td><img src="dialogs/attachment/fileTypeImages/icon_default.png" />&nbsp;test.rar</td>
		  </tr>
<%
	}
%>

		  <tr>
			<td>软件上传名：</td>
			<td><input type="text" style="width:300px;" name="filename" /></td>
		  </tr>
		  
		  <tr>
			<td>待上传软件：</td>
			<td><input type="file" name="file" /></td>
		  </tr>

		  <tr>
			<td valign="top">详细描述：</td>
			<td><script id="editor" type="text/plain" style="width:100%; height:500px;" ></script></td>
		  </tr>	
		  
		  <tr>
			<td colspan="2" align="center"><input type="submit"  /></td>
		  </tr>
		  
		  <tr>
			<td colspan="2" align="center"><input type="hidden" name="itemid" value="<%=itemId%>" /></td>
		  </tr>
		  
		</table>
		
	</form>
</div>

<script type="text/javascript">
    //实例化编辑器
    //建议使用工厂方法getEditor创建和引用编辑器实例，如果在某个闭包下引用该编辑器，直接调用UE.getEditor('editor')就能拿到相关的实例
    var ue = UE.getEditor('editor');
</script>

</body>
</html>