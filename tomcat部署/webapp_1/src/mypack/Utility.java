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

public class Utility {
	
	public enum EnumErrCode {
		OK,
		UNKNOWN,
		INVALID_PARAM,
		IO,
		DATABASE,
	}
	
	static private String JDBC_NAME="jdbc_name";
	static private String ERROR_CODE="errCode";
	static private String ERROR_MSG="errMsg";
    
    public static Connection getConnection() throws NamingException, SQLException  {
		Context ctx = new InitialContext();
		DataSource ds = (DataSource)ctx.lookup("java:comp/env/jdbc/"+JDBC_NAME);
		Connection con = ds.getConnection();
		return con;
	}
	
	public static void setContentError( PrintWriter out, EnumErrCode errCode, String errMsg ) {
		JSONObject rsp_obj = new JSONObject();
		rsp_obj.put(ERROR_CODE, errCode.ordinal());
		rsp_obj.put(ERROR_MSG,  errMsg);
		out.print(rsp_obj.toString());
	}
	
	public static void setContentError( PrintWriter out, EnumErrCode errCode ) {
		JSONObject rsp_obj = new JSONObject();
		rsp_obj.put(ERROR_CODE, errCode.ordinal());
		out.print(rsp_obj.toString());
	}
	
	// ignore null paramter
	public static String GetParameter(HttpServletRequest req, String paraName) {
		String ret = req.getParameter(paraName);
		if ( null == ret ) {
			return new String();
		}
		return ret;
	}
}


