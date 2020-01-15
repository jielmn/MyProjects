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


// 传递的参数有：
// type:    操作类型

public class MainServlet extends HttpServlet {
	
	String appid = "wx692cfffa15fdf5d9";
	String secret = "fabdcb7f72cf10cf0bde84e04fb28814";
    
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
			setContentError(out,1,"no parameter");
		} else {			
			if ( type.equals("get_openid") ) {
				String  code     = new String();				
				if ( null != req.getParameter("code") ) {
					code = req.getParameter("code");
				}				
				getOpenId(out, code);
			}
			else if ( type.equals("addmember") ) {
				String openid = new String();
				String newMemberName = new String();
				if ( null != req.getParameter("openid") ) {
					openid = req.getParameter("openid");
				}
				if ( null != req.getParameter("membername") ) {
					newMemberName = req.getParameter("membername");
				}
				addNewMember(out,openid, newMemberName);
			}
		}
		
		//test(out);
		
		out.close();
    }   
	
	public Connection getConnection() throws NamingException, SQLException  {
		Context ctx = new InitialContext();
		DataSource ds = (DataSource)ctx.lookup("java:comp/env/jdbc/easytemp");
		Connection con = ds.getConnection();
		return con;
	}
	
	public void getOpenId(PrintWriter out, String code) {
		// 没有填写参数
		if ( code.length() == 0 ) {
			setContentError(out,20);
			return;
		}
		
		HashMap<String, Object> params = new HashMap<String, Object>();
		params.put("appid",       appid);  
        params.put("secret",      secret);  
        params.put("js_code",     code);  
		params.put("grant_type",  "authorization_code");  
		
		try
		{
			String ret = HttpHelper.sendGetByHttpUrlConnection("https://api.weixin.qq.com/sns/jscode2session", params, "utf-8");
			// out.print(ret);
			
			JSONObject object = new JSONObject(ret);
			login( out, object );			
		}
		catch(Exception ex) {
			 out.print(ex.getMessage());
		}
				
	}
	
	
	public void login(PrintWriter out, JSONObject object) {		
	
		String open_id = object.get("openid").toString();
		String open_id_sql = open_id.replace("'","''");
		
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
			ResultSet rs = stmt.executeQuery("select * from users where open_id='" + open_id_sql + "';" );
			JSONArray members = new JSONArray();
			object.put("logined", true);
			object.put("members", members);
			
			// 如果已经注册
			if ( rs.next() ) {
				rs.close();
				rs = stmt.executeQuery("select * from family where open_id = '" + open_id_sql + "'" );
				while ( rs.next() ) {
					JSONObject member = new JSONObject();
					member.put("id",   rs.getInt(1));
					member.put("name", rs.getString(3));
					members.put(member);
				}
			} 
			// 没有注册
			else {
				rs.close();
				rs = stmt.executeQuery("select * from config where id = 1;" );
				int  register_enable = 0;
				if ( rs.next() ) {
					register_enable = rs.getInt(2);
				}
				
				// 如果不允许注册
				if ( register_enable == 0 ) {					
					object.put("logined", false);
					
					rs.close();
					stmt.close();
					con.close();
				} 
				// 允许注册
				else {
					stmt.executeUpdate("insert into users values('" + open_id_sql + "') ");
				}
			}
			
			out.print(object.toString());
			
			rs.close();
			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
	}
	
	public void addNewMember(PrintWriter out, String openid, String newMemberName ) {
		String openid_sql        = openid.replace("'","''");
		String newMemberName_sql = newMemberName.replace("'","''");
		
		try {			
			Connection con = null;
			try{
				con = getConnection();
			}
			catch(Exception e ) {
				out.print(e.getMessage());
				return;
			}
			
			JSONObject rsp_obj = new JSONObject();
			Statement stmt = con.createStatement();      
			stmt.executeUpdate("insert into family values(null, '" + openid_sql + "', '" + newMemberName_sql + "')" );
						
			// 获取插入值id			
			ResultSet rs = stmt.executeQuery("select max(memberid) from family where open_id='" + openid_sql + "'" );			
			if ( rs.next() ) {
				rsp_obj.put("id", rs.getInt(1));
			}			
			rsp_obj.put("error", 0);
			out.print(rsp_obj.toString());
			
			rs.close();
			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
	}
	

	public void test( PrintWriter out) {
		JSONObject object = new JSONObject();
		Object nullObj = null;
		try {
			object.put("name", "王小二");
			object.put("age", 25.2);
			object.put("birthday", "1990-01-01");
			object.put("school", "蓝翔");
			object.put("major", new String[] {"理发", "挖掘机"});
			object.put("has_girlfriend", false);
			object.put("car", nullObj);
			object.put("house", nullObj);
			object.put("comment", "这是一个注释");

			out.print(object.toString());

		} catch (JSONException e) {
			e.printStackTrace();
		}
	}
	
	public void setContentError(PrintWriter out, int errCode) {
		out.print("{\"error\":"+errCode+"}");
	}
	
	public void setContentError(PrintWriter out, int errCode, String errMsg) {
		out.print("{\"error\":"+errCode+", \"error message\":" + errMsg + "}");
	}
}





class HttpHelper {
    
    public static String sendGetByHttpUrlConnection(String urlStr, Map<String, Object> params, String charset) {
        StringBuffer resultBuffer = null;
        // 构建请求参数
        String sbParams= JoiningTogetherParams(params);
        HttpURLConnection con = null;
        BufferedReader br = null;
        try {
            URL url = null;
            if (sbParams != null && sbParams.length() > 0) {
                url = new URL(urlStr + "?" + sbParams);
            } else {
                url = new URL(urlStr);
            }
            con = (HttpURLConnection) url.openConnection();
            con.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");
            con.connect();
            resultBuffer = new StringBuffer();
            br = new BufferedReader(new InputStreamReader(con.getInputStream(), charset));
            String temp;
            while ((temp = br.readLine()) != null) {
                resultBuffer.append(temp);
            }
        } catch (Exception e) {
            throw new RuntimeException(e);
        } finally {
            if (br != null) {
                try {
                    br.close();
                } catch (IOException e) {
                    br = null;
                    throw new RuntimeException(e);
                } finally {
                    if (con != null) {
                        con.disconnect();
                        con = null;
                    }
                }
            }
        }
        return resultBuffer.toString();
    }

	private static String JoiningTogetherParams(Map<String, Object> params){
        StringBuffer sbParams = new StringBuffer();
        if (params != null && params.size() > 0) {
            for (Entry<String, Object> e : params.entrySet()) {
                sbParams.append(e.getKey());
                sbParams.append("=");
                sbParams.append(e.getValue());
                sbParams.append("&");
            }
            return sbParams.substring(0, sbParams.length() - 1);
        }
        return "";
    }
}
