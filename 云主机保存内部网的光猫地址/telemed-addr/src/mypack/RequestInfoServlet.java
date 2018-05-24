package mypack;

import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;
import java.util.*;
import java.sql.*;
import javax.sql.*;
import javax.naming.*;
import org.json.*;

public class RequestInfoServlet extends HttpServlet {
	public void doGet( HttpServletRequest request, HttpServletResponse response) 
	throws ServletException, IOException {
		
		response.setContentType("application/json;charset=utf-8");
        response.setHeader("Access-Control-Allow-Origin", "*");
		PrintWriter out = response.getWriter();
		
		try {
			Connection con = null;
			try{
				con = getConnection();
			}
			catch(Exception e ) {
				out.print(e.getMessage());
				out.close();
				return;
			}
			
			Statement stmt = con.createStatement();      
			stmt.executeUpdate( "UPDATE config set cfg_0='" + request.getRemoteAddr() + "',cfg_1 = now() where id = 1;" );
			stmt.close();
			con.close();
			
			JSONObject rsp_obj = new JSONObject();			
			rsp_obj.put("remoteAddr", request.getRemoteAddr());
			rsp_obj.put("error", 0);
			out.print(rsp_obj.toString());
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
		
		out.close();
	}
	
	public Connection getConnection() throws NamingException, SQLException  {
		Context ctx = new InitialContext();
		DataSource ds = (DataSource)ctx.lookup("java:comp/env/jdbc/global_db");
		Connection con = ds.getConnection();
		return con;
	}
}