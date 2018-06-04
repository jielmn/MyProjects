package mypack;

import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;
import java.util.*;
import java.sql.*;
import javax.sql.*;
import javax.naming.*;
import org.json.*;

public class GetAddrServlet extends HttpServlet {
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
			ResultSet rs = stmt.executeQuery( "SELECT * FROM config where id = 1;" );
			
			String addr = new String();
			if ( rs.next() ) {
				addr = rs.getString(2);
			} 
			
			rs.close();
			stmt.close();
			con.close();
			
			JSONObject rsp_obj = new JSONObject();			
			rsp_obj.put( "Addr", addr );
			rsp_obj.put( "error", 0 );
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