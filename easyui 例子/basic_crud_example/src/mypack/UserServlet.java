package mypack;

import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;
import java.util.*;
import java.sql.*;
import javax.sql.*;
import javax.naming.*;
import org.json.*;

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

// HTTP
import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.HttpClient;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;

// file upload
import org.apache.commons.fileupload.FileItem;
import org.apache.commons.fileupload.FileUploadException;
import org.apache.commons.fileupload.disk.DiskFileItemFactory;
import org.apache.commons.fileupload.servlet.ServletFileUpload;

import java.util.Date;
import java.text.SimpleDateFormat;


// 传递的参数有：
// type: 操作类型
//       add
//       modify
//       delete
//       list

// error: 
//      1,  invalidate  url params
//      2,  failed to get a connection
public class UserServlet extends HttpServlet {
	
	static private String POOL_NAME  = "test";
	static private String TABLE_NAME = "users";
	
    public void doGet( HttpServletRequest req, HttpServletResponse rsp ) throws ServletException, IOException
    {       
        rsp.setContentType("application/json;charset=utf-8");
		
		String type = getParameter(req, "type");		
		PrintWriter out = rsp.getWriter();		
		
		if ( type.length() == 0 ) {
			setContentError( out, 1, "no type parameter" );
			out.close();
			return;
		}
		
		// 如果是获取某一页的列表
		if ( type.equals("list") ) {							
			list(req, out);
		}		
		
		out.close();
    }   
	
	public void doPost( HttpServletRequest req, HttpServletResponse rsp ) throws ServletException, IOException
    {       
        this.doGet(req, rsp);
    } 
	
	public Connection getConnection() throws NamingException, SQLException  {
		Context ctx = new InitialContext();
		DataSource ds = (DataSource)ctx.lookup("java:comp/env/jdbc/" + POOL_NAME );
		Connection con = ds.getConnection();
		return con;
	}

	// 注意：UIEASY page的页码是从1开始的
	public void list(HttpServletRequest req, PrintWriter out ) {
		String page = getParameter( req, "page" );		
		String rows = getParameter( req, "rows" );
		
		Connection con = null;
		try{
			con = getConnection();
		}
		catch(Exception e ) {
			setContentError(out, 2, e.getMessage());
			return;
		}
		
		try{
			JSONObject result=new JSONObject();
			JSONArray jsonArray=userList(con, parseInt(page,1)-1, parseInt(rows,10) );			
			int total=userCount(con);
			result.put("rows", jsonArray);
			result.put("total", total);
			result.put("error code", 0);
			out.print(result.toString());			
			con.close();
		}
		catch(Exception e ) {
			setContentError(out, 3, e.getMessage());
		}
	}
	
	
	private static void setContentError(PrintWriter out, int errCode, String errMsg) {
		JSONObject rsp_obj = new JSONObject();
		rsp_obj.put("error code", errCode);
		rsp_obj.put("error message", errMsg);
		out.print(rsp_obj.toString());
	}
	
	private static String getParameter( HttpServletRequest req, String strName ) {
		if ( null != req.getParameter(strName) ) {
			return req.getParameter(strName);
		}		
		return new String();
	}
	
	private static JSONArray formatRsToJsonArray(ResultSet rs) throws Exception {
		ResultSetMetaData md = rs.getMetaData();
		int num = md.getColumnCount();
		JSONArray array=new JSONArray();
		while( rs.next() ){
			JSONObject mapOfColValues=new JSONObject();
			for(int i=1;i<=num;i++){
				mapOfColValues.put(md.getColumnName(i), rs.getObject(i));
			}
			array.put(mapOfColValues);
		}
		return array;
	}
	
	private static JSONArray userList(Connection con,int pageIndex, int rowsPerPage )throws Exception{
		String sql="select * from " + TABLE_NAME + " limit ?,?";
		
		PreparedStatement pstmt=con.prepareStatement(sql);
		pstmt.setInt(1, pageIndex*rowsPerPage);
		pstmt.setInt(2, rowsPerPage);
		ResultSet rs = pstmt.executeQuery();		
		JSONArray array = formatRsToJsonArray(rs);
		
		rs.close();
		pstmt.close();
		return array;
	}
	
	private static int userCount(Connection con)throws Exception{
		String sql="select count(*) as total from " + TABLE_NAME;
		PreparedStatement pstmt=con.prepareStatement(sql);
		ResultSet rs=pstmt.executeQuery();
		int cnt = 0;
		if(rs.next()){
			cnt = rs.getInt("total");
		}
		rs.close();
		pstmt.close();
		return cnt;
	}
	
	private static int parseInt(String s) {
		return parseInt(s,0);
	}

	private static int parseInt(String s, int d) {
		if ( s == null ) {
			return d;
		}
		
		try {
			return Integer.parseInt(s);
		}catch(Exception e){
			return d;
		}
	}
}
