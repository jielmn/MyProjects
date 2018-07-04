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


import javax.crypto.Mac;
import javax.crypto.spec.SecretKeySpec;


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
			ResultSet rs = stmt.executeQuery("select * from config where id = 1;" );
			
			String old_addr = "";
			if ( rs.next() ) {
				old_addr = rs.getString(2);				
			} 
			
			String new_addr = request.getRemoteAddr();
			
			// 如果不相等，更新dns解析
			if ( !old_addr.equals( new_addr ) ) {
				updateDns(new_addr,out);
			}
			
			stmt.executeUpdate( "UPDATE config set cfg_0='" + new_addr + "',cfg_1 = now() where id = 1;" );
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
	
	public void updateDns(String new_addr,PrintWriter out) {
		
		try
		{
			Base64.Encoder  base64_encoder = Base64.getEncoder();
				
			int  nonce = (int)(Math.random()*10000);
			long time  = System.currentTimeMillis();
			String nowTimeStamp = String.valueOf(time / 1000);
			
			String action = "RecordModify";
			String secretId = "AKID63pPo2e68zgCYjhXAcs20KSO9i6IZefU";
			String signatureMethod = "HmacSHA256";
			String domain = "telemed-healthcare.cn";
			int    recordId = 365503848;
			String line = "默认";
			String recordType = "A";
			String subDomain = "buz";
			
			String s = "Action=" + action;			
			s += "&Nonce=" + nonce;
			s += "&SecretId=" + secretId;
			s += "&SignatureMethod=" + signatureMethod;
			s += "&Timestamp=" + nowTimeStamp;
			s += "&domain=" + domain;			
			s += "&recordId=" + recordId;
			s += "&recordLine=" + line;
			s += "&recordType=" + recordType;
			s += "&subDomain=" + subDomain;						
			s += "&value=" + new_addr;
		
			String serverAddr = "cns.api.qcloud.com/v2/index.php";
			String secretKey = "sZ5QaZMxMZaP2TmureIl3CG9rNisqdEj";
			
			byte[] textByte = sha256_HMAC_1("GET"+serverAddr+"?"+s, secretKey);
			String signature = base64_encoder.encodeToString(textByte);
				
			HashMap<String, Object> params = new HashMap<String, Object>();
			params.put("Action",            action);  
			params.put("Nonce",             nonce);  
			params.put("SecretId",          secretId);  
			params.put("SignatureMethod",   signatureMethod);  
			params.put("Timestamp",         nowTimeStamp);  
			
			params.put("domain",            domain);  
			params.put("recordId",          recordId);  
			params.put("recordLine",        line);  
			params.put("recordType",        recordType);  
			params.put("subDomain",         subDomain);  
			params.put("value",             new_addr);  
			
			params.put("Signature",         java.net.URLEncoder.encode(signature,"UTF-8") );  
		
			String ret = HttpHelper.sendGetByHttpUrlConnection("https://" + serverAddr, params, "utf-8");
			out.print(ret);
		}
		catch(Exception ex) {
			 out.print(ex.getMessage());
		}
	}
	
	public byte[] sha256_HMAC_1(String message, String secret) {
		byte[] a = null;
		try {
		   Mac sha256_HMAC = Mac.getInstance("HmacSHA256");
		   SecretKeySpec secret_key = new SecretKeySpec(secret.getBytes(), "HmacSHA256");
		   sha256_HMAC.init(secret_key);
		   byte[] bytes = sha256_HMAC.doFinal(message.getBytes());
		   return bytes;
		} 
		catch (Exception e) {
			//System.out.println("Error HmacSHA256 ===========" + e.getMessage());
		}
		return a;
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