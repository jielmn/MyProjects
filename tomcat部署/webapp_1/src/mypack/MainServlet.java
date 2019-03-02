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

// utility
import mypack.Utility;

public class MainServlet extends HttpServlet {
	    
    public void doGet(HttpServletRequest req, HttpServletResponse rsp) throws ServletException, IOException {       
        rsp.setContentType("application/json;charset=utf-8");
		String type = Utility.GetParameter( req, "type" );
		PrintWriter out = rsp.getWriter();		
		
		// get parameter type
		if ( type.length() > 0 ) {
			if ( type.equals("test") ) {				
				test( out, req );
			} else {
				Utility.setContentError(out,Utility.EnumErrCode.INVALID_PARAM,"type '"+type+"' is not supported");
			}
		} else {
			Utility.setContentError(out,Utility.EnumErrCode.INVALID_PARAM,"no 'type' parameter");
		}			
		
		out.close();
    }   
	
	public void doPost(HttpServletRequest req, HttpServletResponse rsp) throws ServletException, IOException {       
       doGet( req, rsp );
    } 
	
	private void test(PrintWriter out, HttpServletRequest req ) {		
		Utility.setContentError(out,Utility.EnumErrCode.OK,"OK");
	}
}


