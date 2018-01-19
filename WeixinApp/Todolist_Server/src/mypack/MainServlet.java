package mypack;

import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;
import java.util.*;
import java.sql.*;

// 传递的参数有：
//     type:    操作类型
//              egister，注册用户
public class MainServlet extends HttpServlet {
    
    public void doGet(HttpServletRequest req, HttpServletResponse rsp) throws ServletException, IOException
    {       
        rsp.setContentType("application/json;charset=utf-8");
        rsp.setHeader("Access-Control-Allow-Origin", "*");
		
		String type = new String();
		if ( null != req.getParameter("type") ) {
			type = req.getParameter("type");
		}
		
		PrintWriter out = rsp.getWriter();		
		
		if ( type.length() == 0 ) {
			setContentError(out,1);
		} else {			
			// 如果是注册用户
			if ( type.equals("register") ) {				
				String  open_id   = new String();
				String  name      = new String();
				String  avatarUrl = new String();
				
				if ( null != req.getParameter("open_id") ) {
					open_id = req.getParameter("open_id");
				}
				
				if ( null != req.getParameter("name") ) {
					name = req.getParameter("name");
				}
				
				if ( null != req.getParameter("avatarUrl") ) {
					avatarUrl = req.getParameter("avatarUrl");
				}
				
				register(out, open_id, name, avatarUrl);
			}
					
		}
		out.close();
    }   
	
	public void register(PrintWriter out, String open_id, String name, String avatarUrl) {
		// 没有填写open_id参数
		if ( open_id.length() == 0 ) {
			setContentError(out,2);
			return;
		}
		
		String open_id_sql = open_id.replace("'","''");
		String name_sql    = name.replace("'","''");
		String avatarUrl_sql = avatarUrl.replace("'","''");
		
		try {
			Class.forName("com.mysql.jdbc.Driver");
			DriverManager.registerDriver( new com.mysql.jdbc.Driver() );
			
			String dbUrl = "jdbc:mysql://localhost:3306/todo_list?useUnicode=true&characterEncoding=UTF-8";
			String dbUser="root";
			String dbPwd="";
			Connection con = java.sql.DriverManager.getConnection(dbUrl,dbUser,dbPwd);
			Statement stmt = con.createStatement();      
			ResultSet rs = stmt.executeQuery("select * from users where open_id='" + open_id_sql + "';" );
			
			// 如果已经注册
			if ( rs.next() ) {
				String name_get = rs.getString(2);
				String avatarUrl_get = rs.getString(4);				
				
				if ( !(name.equals(name_get) && avatarUrl.equals(avatarUrl_get)) ) {
					stmt.executeUpdate("update users set name='" + name_sql + "', avatar_url='" + avatarUrl_sql + "' where open_id = '" + open_id_sql + "';" );
				} 
			} 
			// 没有注册
			else {
				stmt.executeUpdate("insert into users values('" + open_id_sql + "','" + name_sql +"','" + name_sql + "','" + avatarUrl_sql + "') ");
			}
			
			setContentError(out,0);
			
			rs.close();
			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
	}
	
	public void setContentError(PrintWriter out, int errCode) {
		out.print("{\"error\":"+errCode+"}");
	}
}
