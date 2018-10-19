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

public class UpgradeServlet extends HttpServlet {
	    
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
			setContentError(out,-1);
			out.print("invalid type");
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
					
					if ( item.getFieldName().equals("version") ) {
						String newRelativeName = "upgrade/version.txt";
						String file = this.getServletContext().getRealPath("/") + newRelativeName;						
						String charSet="UTF-8";
						
						FileOutputStream fileWriter=new FileOutputStream(file);
						OutputStreamWriter writer=new OutputStreamWriter(fileWriter, charSet);
						writer.write(item.getString("UTF-8"));
						writer.close();
												
					}					

				} else {
					if (item.getName() != null && !item.getName().equals("")) {		
						//description =  new String(item.getName().getBytes("gbk"),"UTF-8");
						//description = item.getName();
						
						/*
						int pos = item.getName().lastIndexOf(".");  
						String post_fix = new String();
						if ( pos > -1 ) {
							post_fix = item.getName().substring( pos );
						}
						*/
						
						Date d = new Date();
						SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMddHHmmssSSS");						
						String newRelativeName = "upgrade/TelemedStation.exe";
						String newFileName = this.getServletContext().getRealPath("/") + newRelativeName;
						
						//out.print("<p>new file:" + newFileName + "</p>" );										
						File file = new File(newFileName);
						item.write(file);
						//out.print("<p>upload success!</p>");							
						
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
	

}


