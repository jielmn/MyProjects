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

import java.io.File;
import java.io.IOException;


import jxl.Cell;
import jxl.Sheet;
import jxl.Workbook;
import jxl.read.biff.BiffException;

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
			} 
			else if (type.equals("station_list")) {
				rsp.setContentType("application/json;charset=utf-8");				
				station_list(out);
			}
			else if (type.equals("patient_list")) {
				rsp.setContentType("application/json;charset=utf-8");				
				patient_list(out);
			}
			else if (type.equals("clear_patient")) {
				rsp.setContentType("application/json;charset=utf-8");				
				clear_patient(out);
			}
			else if (type.equals("test")) {
				rsp.setContentType("application/json;charset=utf-8");				
				test(out);
			}
			else {
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
	
	public void setContentError_1(PrintWriter out, int errCode) {
		out.print("{\"error\":"+errCode+"}");
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
	
	public void station_list ( PrintWriter out ) {
		
		try {
			Connection con = null;
			try{
				con = getConnection();
			}
			catch(Exception e ) {
				out.print(e.getMessage());
				return;
			}
			
			Statement stmt = con.createStatement();      
			ResultSet rs = stmt.executeQuery("select * from station where unix_timestamp(now()) - unix_timestamp(atime) <= 60 ;" );			
			
			// 获取清单
			JSONArray item_arr = new JSONArray();
			while ( rs.next() ) {
				String     ip          = rs.getString(1);
				String     name        = rs.getString(2);
				Timestamp  time        = rs.getTimestamp(3);
				
				JSONObject item_obj = new JSONObject();
				item_obj.put("ip",           ip);
				item_obj.put("name",         name);
				item_obj.put("time",         time.getTime());
				
				item_arr.put(item_obj);
			} 

			JSONObject rsp_obj = new JSONObject();
			rsp_obj.put("stationlist", item_arr);
			rsp_obj.put("error", 0);			
			out.print(rsp_obj.toString());
			
			rs.close();
			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
	}
	
	public void patient_list ( PrintWriter out ) {
		
		try {
			Connection con = null;
			try{
				con = getConnection();
			}
			catch(Exception e ) {
				out.print(e.getMessage());
				return;
			}
			
			Statement stmt = con.createStatement();      
			ResultSet rs = stmt.executeQuery("select * from patientinfo;" );
			
			// 获取清单
			JSONArray item_arr = new JSONArray();
			while ( rs.next() ) {
				String     id          = rs.getString(2);
				String     name        = rs.getString(3);
				String     sex         = rs.getString(4);
				String     bed         = rs.getString(5);
				int        age         = rs.getInt(6);
				
				JSONObject item_obj = new JSONObject();
				item_obj.put("id",           id);
				item_obj.put("name",         name);
				item_obj.put("sex",          sex);
				item_obj.put("bed",          bed);
				item_obj.put("age",          age);
				
				item_arr.put(item_obj);
			} 

			JSONObject rsp_obj = new JSONObject();
			rsp_obj.put("patientlist", item_arr);
			rsp_obj.put("error", 0);			
			out.print(rsp_obj.toString());
			
			rs.close();
			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
	}
	
	public void clear_patient ( PrintWriter out ) {
		
		try {
			Connection con = null;
			try{
				con = getConnection();
			}
			catch(Exception e ) {
				out.print(e.getMessage());
				return;
			}
			
			Statement stmt = con.createStatement();      
			stmt.executeUpdate( "delete from patientreltag;" );
			stmt.executeUpdate( "delete from patientinfo;" );
			setContentError_1(out,0);
			
			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
	}
	
	
	
	public void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {

		
		response.setContentType("text/html;charset=utf-8");
        response.setHeader("Access-Control-Allow-Origin", "*");
		
		DiskFileItemFactory factory = new DiskFileItemFactory();
		ServletFileUpload upload = new ServletFileUpload(factory);
		
		PrintWriter out = response.getWriter();		
		JSONArray item_arr = new JSONArray();		
		int nError = 0;
		String description = new String();
		
		try {
			List items = upload.parseRequest(request);
			Iterator itr = items.iterator();
			while (itr.hasNext()) {				
				FileItem item = (FileItem) itr.next();
				if (item.isFormField()) {
					//out.print("<p>from param name:" + item.getFieldName() + "，value:" + item.getString("UTF-8") + "</p>");
				} else {
					if (item.getName() != null && !item.getName().equals("")) {		
						//description =  new String(item.getName().getBytes("gbk"),"UTF-8");
						//description = item.getName();
						
						String post_fix = new String();
						String s = item.getName();
						String tmp  = s.substring( s.length() - 3 );
						if ( tmp.equals("xls") ) {
							post_fix = ".xls";
						} else {
							post_fix = "";
						}
						
						/*
						int pos = item.getName().lastIndexOf(".");  
						String post_fix = new String();
						if ( pos > -1 ) {
							post_fix = item.getName().substring( pos );
						}
						*/
						
						Date d = new Date();
						SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMddHHmmssSSS");						
						String newRelativeName = "tmp/" + sdf.format(d) + post_fix;
						String newFileName = this.getServletContext().getRealPath("/") + newRelativeName;
						
						//out.print("<p>new file:" + newFileName + "</p>" );										
						File file = new File(newFileName);
						item.write(file);
						//out.print("<p>upload success!</p>");
						
						Connection con = getConnection();
						Statement stmt = con.createStatement();      
						
						File xlsFile = new File(newFileName);
						Workbook workbook = Workbook.getWorkbook(xlsFile);
						Sheet[] sheets = workbook.getSheets();
						if (sheets != null)
						{
							Sheet sheet = sheets[0];
							int rows = sheet.getRows();
							int cols = sheet.getColumns();
							if ( cols != 5 ) {
								throw new Exception("excel columns not equal 5");
							}
							
							for (int row = 1; row < rows; row++)
							{
								stmt.executeUpdate( "insert into patientinfo (patientid, patientname, gender, bedid, age ) values('" + sheet.getCell(0, row).getContents() + "', '" + sheet.getCell(1, row).getContents() + "','" 
								                    + (sheet.getCell(2, row).getContents().equals("Ů") ? 0 : 1) + "', '" + sheet.getCell(3, row).getContents() + "', " + sheet.getCell(4, row).getContents() + "); " );							   
							}
						}
						workbook.close();
						
						JSONObject item_obj = new JSONObject();
						item_obj.put("filename",  newRelativeName);
						item_arr.put(item_obj);
						
						break;
						
					} else {
						//out.print("<p>no file!</p>");
					}
				}
			}
		} catch (FileUploadException e) {
			e.printStackTrace();
			nError = -1;
			description = e.getMessage();
		} catch (Exception e) {
			e.printStackTrace();
			//out.print("<p>failed to upload file!</p>");
			nError = -2;
			description = e.getMessage();
		}
		
		if ( nError == 0 ) {			
			JSONObject rsp_obj = new JSONObject();
			rsp_obj.put("upload_files", item_arr);
			rsp_obj.put("error", 0);
			//rsp_obj.put("description", description);
			out.print(rsp_obj.toString());
			//setContentError(out,0);
		} else {
			JSONObject rsp_obj = new JSONObject();
			rsp_obj.put("error", nError);
			rsp_obj.put("description", description);
			out.print(rsp_obj.toString());
		}
		
		out.close();
	}
	
	
   public void test(PrintWriter out)
   {
	  try{
		  String newFileName = this.getServletContext().getRealPath("/") + "tmp/a.xls";
		  File xlsFile = new File(newFileName);
		  Workbook workbook = Workbook.getWorkbook(xlsFile);
		  Sheet[] sheets = workbook.getSheets();
		  if (sheets != null)
		  {
			Sheet sheet = sheets[0];
			int rows = sheet.getRows();
			int cols = sheet.getColumns();
			for (int row = 0; row < rows; row++)
			{
			   for (int col = 0; col < cols; col++)
			   {
				  out.print( "<div>" + sheet.getCell(col, row).getContents() + "</div>" );
			   }
			}
		  }
		  workbook.close();
	  }
	  catch (Exception e ) {
		  out.print(e.getMessage());
	  }
      
   }

}


