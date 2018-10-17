package mypack;

import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;
import java.util.*;
import java.sql.*;
import javax.sql.*;
import javax.naming.*;
import org.json.*;


// http
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.net.HttpURLConnection;
import java.net.Socket;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.HashMap; 

import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.HttpClient;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;

import org.apache.commons.fileupload.FileItem;
import org.apache.commons.fileupload.FileUploadException;
import org.apache.commons.fileupload.disk.DiskFileItemFactory;
import org.apache.commons.fileupload.servlet.ServletFileUpload;

import java.util.Date;
import java.text.SimpleDateFormat;

public class ManageServlet extends HttpServlet {
	    
    public void doGet(HttpServletRequest req, HttpServletResponse rsp) throws ServletException, IOException
    {       
        rsp.setContentType("text/html;charset=utf-8");
        rsp.setHeader("Access-Control-Allow-Origin", "*");
		
		String type = new String();
		if ( null != req.getParameter("type") ) {
			type = req.getParameter("type");
		}
		
		PrintWriter out = rsp.getWriter();		
		
			
		if ( type.length() == 0 ) {
			setContentError(out,-1);
			out.print("no type");
		} else {			
			if ( type.equals("heartbeat") ) {				
				String  name   = new String();
				
				if ( null != req.getParameter("name") ) {
					name = req.getParameter("name");
				}
				
				heartbeat(out, name, req.getRemoteAddr() );
			} else {
				setContentError(out,-1);
				out.print("invalid type");
			}
		}		
		
		out.close();
    }   
	
	public Date transForDate(Integer ms){
		if(ms==null){
			ms=0;
		}
		long msl=(long)ms*1000;
		SimpleDateFormat sdf=new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		Date temp=null;
		if(ms!=null){
			try {
				String str=sdf.format(msl);
				temp=sdf.parse(str);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return temp;
	}
	
	public Integer transForSecond(Date date){
		if(date==null) return null;
		return (int)(date.getTime()/1000);
	}
	
	public static String transForDate1(Integer ms){
		String str = "";
		if(ms!=null){
			long msl=(long)ms*1000;
			SimpleDateFormat sdf=new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
			
			if(ms!=null){
				try {
					str=sdf.format(msl);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}		
		return str;
	}
	
	public Connection getConnection() throws NamingException, SQLException  {
		Context ctx = new InitialContext();
		DataSource ds = (DataSource)ctx.lookup("java:comp/env/jdbc/luhe_123");
		Connection con = ds.getConnection();
		return con;
	}	
	
	public void setContentError(PrintWriter out, int errCode) {
		out.print(errCode);
	}
	
	public void heartbeat( PrintWriter out, String name, String station_addr ) {				
		
		try {
			
			Connection con = null;
			try{
				con = getConnection();
			}
			catch(Exception e ) {
				setContentError(out,-1);
				out.print(e.getMessage());
				return;
			}			
			
			Statement stmt = con.createStatement(); 
			ResultSet rs = stmt.executeQuery("select * from station where ip='" + station_addr + "';" );
			// if exists
			if ( rs.next() ) {
				rs.close();
				stmt.executeUpdate( "update station set name='" + name + "', atime=now() where ip='" + station_addr + "';" );
			} else {
				rs.close();
				stmt.executeUpdate( "insert into station (ip, name) values( '" + station_addr + "','" + name + "' );" );
			}
			
			setContentError(out,0);			
			stmt.close();			
			con.close();
        } catch (Exception ex ) {
			setContentError(out,-1);
			out.print(ex.getMessage());
        }		
	}
}


