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
<%@ page import="java.util.Date" %>
<%@ page import="java.text.SimpleDateFormat" %>

<%!
	private Connection getConnection() throws NamingException, SQLException  {
		Context ctx = new InitialContext();
		DataSource ds = (DataSource)ctx.lookup("java:comp/env/jdbc/myjdbc");
		Connection con = ds.getConnection();
		return con;
	}
%>

<%!
	private String getFileTypeImg(String postFix)  {
		if ( postFix.equals(".rar") ) {
			return "icon_rar.gif";
		} else if (postFix.equals(".exe")) {
			return "icon_exe.gif";
		} else if (postFix.equals(".jpg")) {
			return "icon_jpg.gif";
		} else if (postFix.equals(".mp3")) {
			return "icon_mp3.gif";
		} else if (postFix.equals(".chm")) {
			return "icon_chm.gif";
		} else if ( postFix.equals(".doc") || postFix.equals(".docx") ) {
			return "icon_doc.gif";
		} else if ( postFix.equals(".mv") ) {
			return "icon_mv.gif";
		} else if ( postFix.equals(".pdf") ) {
			return "icon_pdf.gif";
		} else if ( postFix.equals(".ppt") ) {
			return "icon_ppt.gif";
		} else if ( postFix.equals(".txt") ) {
			return "icon_txt.gif";
		} else if ( postFix.equals(".xls") ||  postFix.equals(".xlsx") ) {
			return "icon_xls.gif";
		}
		else {
			return "icon_default.png";
		}
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
	
	int itemId = -1;
	String title   = "";
	String brief   = "";
	String content = "";
	
	String filePath = "";
	String coverPath = "";
	
	String filePostFix = "";
	String coverPostFix = "";
	
	String fileName = "";
	
	String newTitle   = "";
	String newBrief   = "";
	String newContent = "";
	String newName = "";
	
	// 如果是本页面提交
	if ( type.equals("submit")  ) {
		DiskFileItemFactory factory = new DiskFileItemFactory();
		ServletFileUpload upload = new ServletFileUpload(factory);
		
		try {
			List items = upload.parseRequest(request);
			Iterator itr = items.iterator();
			
			String    itemName  = null;
			FileItem  itemFile  = null;
			FileItem  coverFile = null;
			
			// iterate 所有参数
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
						if ( item.getFieldName().equals("file") ) {
							itemFile = item;
						} else if ( item.getFieldName().equals("cover") ) {
							coverFile = item;
						}
					}
				}
			}
			
			// 先保存文件
			if ( itemName != null && itemFile != null ) {
				int pos = itemFile.getName().lastIndexOf(".");  
				String post_fix = new String();
				if ( pos > -1 ) {
					post_fix = itemFile.getName().substring( pos );
				}
									
				filePath = "software/" + itemName + post_fix;
				String newFileName = this.getServletContext().getRealPath("/") + filePath;
														
				File file = new File(newFileName);
				itemFile.write(file);
			}
			
			// 如果有图片，保存图片
			if ( coverFile != null ) {
				int pos = coverFile.getName().lastIndexOf(".");  
				String post_fix = new String();
				if ( pos > -1 ) {
					post_fix = coverFile.getName().substring( pos );
				}
				
				Date d = new Date();
				SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMddHHmmssSSS");						
				coverPath = "covers/" + sdf.format(d) + post_fix;
				
				String newFileName = this.getServletContext().getRealPath("/") + coverPath;
				File file = new File(newFileName);
				coverFile.write(file);
			}
			
			// 新增请求
			if ( itemId == -1 ) {
				
				newTitle   = title.replace("'", "''");
				newBrief   = brief.replace("'", "''");
				newContent = content.replace("'", "''");
				newName    = filePath.replace("'", "''");
				String newCover   = null;
				if ( coverPath.length() > 0 ) {
					newCover   = coverPath.replace("'", "''");
				} else {
					newCover   = "images/default_cover.png";
				}
				
				Connection con = null;
				con = getConnection();				
				Statement stmt = con.createStatement();      
				stmt.executeUpdate( "insert into items values( null, '" + title + "', '" + brief + "', '" + content + "', now(), '" + newName + "', '" + newCover + "' ) " );
				stmt.close();
				con.close();		
				
			} else {
				
				newTitle   = title.replace("'", "''");
				newBrief   = brief.replace("'", "''");
				newContent = content.replace("'", "''");
				
				Connection con = null;
				con = getConnection();				
				Statement stmt = con.createStatement();      
				stmt.executeUpdate( "update items set title = '" + newTitle + "', abstract = '" + newBrief + "', content = '" + newContent + "' where id = " + itemId );
				
				if ( filePath.length() > 0 )  {
					stmt.executeUpdate( "update items set software_path = '" + filePath + "' where id = " + itemId );
				}
				
				if ( coverPath.length() > 0 )  {
					stmt.executeUpdate( "update items set cover = '" + coverPath + "' where id = " + itemId );
				}
				
				stmt.close();
				con.close();
			}
			
			response.sendRedirect("manage.jsp");
			
		} catch (Exception e) {
			out.print(e.getMessage());
		}
		return;
	} 
	// 非 submit type
	else {
		String strItemId = request.getParameter("itemid");
		if ( strItemId == null ) {
			strItemId = "-1";
		}
		itemId = Integer.parseInt(strItemId);
		
		if ( type.equals("delete") ) {
			if ( itemId > 0 ) {
				try {
					Connection con = null;
					con = getConnection();				
					Statement stmt = con.createStatement();      
					stmt.executeUpdate( "delete from items where id = " + itemId );					
					stmt.close();
					con.close();
					response.sendRedirect("manage.jsp");
				} catch (Exception e) {
					out.print(e.getMessage());
				}
			}			
			return;
		}
		
		if ( itemId > 0 ) {
			// 查询item
			try {
				Connection con = null;
				con = getConnection();
				
				Statement stmt = con.createStatement();      
				ResultSet rs = stmt.executeQuery( "select * from items where id = " + itemId );
				if ( rs.next() ) {
					title        = rs.getString(2);
					brief        = rs.getString(3);
					content      = rs.getString(4);
					filePath     = rs.getString(6);
					coverPath    = rs.getString(7);
				} 
				rs.close();
				stmt.close();
				con.close();
			} catch (Exception ex ) {
				out.print(ex.getMessage());
				return;
			}
		}
		
		int pos = filePath.lastIndexOf(".");  
		if ( pos > -1 ) {
			filePostFix = filePath.substring( pos );
		}
		
		pos = coverPath.lastIndexOf(".");  
		if ( pos > -1 ) {
			coverPostFix = coverPath.substring( pos );
		}
		
		pos = filePath.lastIndexOf("/");  
		if ( pos > -1 ) {
			fileName = filePath.substring( pos + 1 );
		} else {
			fileName = filePath;
		}
		
		newTitle   = title.replace("\"", "\"\"");
		newBrief   = brief.replace("\"", "\"\"");
		newName    = fileName.replace("\"", "\"\"");
		newContent = content.replace("'", "''");
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
			<td><input type="text" style="width:300px;" name="title" maxlength="100" value="<%=newTitle%>" /></td>
		  </tr>
		  
		  <tr>
			<td>简略描述：</td>
			<td><input type="text" style="width: 100%; " name="brief" maxlength="200" value="<%=newBrief%>" /></td>
		  </tr>
		  
<%
	if ( !bAdd ) {
%>
		  <tr>
			<td>软件：</td>
			<td><img src="dialogs/attachment/fileTypeImages/<%=getFileTypeImg(filePostFix)%>" />&nbsp;<%=newName%></td>
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
		  
<%
	if ( !bAdd ) {
%>
		  <tr>
			<td>封面：</td>
			<td><img src="<%=coverPath%>" style="width:90px;height:90px;margin:5px;" /></td>
		  </tr>
<%
	}
%>
		  
		  <tr>
			<td>软件封面：</td>
			<td><input type="file" name="cover" accept="image/*" /></td>
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
	ue.addListener("ready", function () { 
		ue.setContent('<%=newContent%>', false); 
	});
</script>

</body>
</html>