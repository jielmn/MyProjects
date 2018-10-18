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
			
		} else {			
			if ( type.equals("upload") ) {				
				String  temp   = new String();
				String  time   = new String();
				String  bind   = new String();
				
				if ( null != req.getParameter("temp") ) {
					temp = req.getParameter("temp");
				}
				
				if ( null != req.getParameter("time") ) {
					time = req.getParameter("time");
				}

				if ( null != req.getParameter("bind") ) {
					bind = req.getParameter("bind");
				}				
				
				addtemp(out, Integer.parseInt(temp), Integer.parseInt(time), Integer.parseInt(bind)  );
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
	
	public void addtemp(PrintWriter out, int temp, int d, int bind ) {		
		
		if ( temp == 0 ) {
			setContentError(out,1);
			return;
		}
		
		if ( d == 0 ) {
			setContentError(out,2);
			return;
		}
		
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
			stmt.executeUpdate( "insert into tempermonitor values( null, 'reader', 'tag', " + temp + ", '" + transForDate1(d) + "', 'nurse', 0, 1, " + bind + " );" );
			setContentError(out,0);
			
			stmt.close();
			con.close();
        } catch (Exception ex ) {
			setContentError(out,-1);
			out.print(ex.getMessage());
        }		
	}
}


