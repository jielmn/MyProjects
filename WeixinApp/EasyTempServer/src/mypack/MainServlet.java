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
	
	//String appid = "wx692cfffa15fdf5d9";
	//String secret = "fabdcb7f72cf10cf0bde84e04fb28814";
	
	String appid = "wx23bed15f4ab06c8b";
	String secret = "fdee94a77e0a4eaf41759dd87b7a0cfd";
	float feverThreshold = 37.3f;
	float uploadThreshold = 35.0f;
	int   maxFeverSpan = 7;        
	
	private class FeverPerson {
		String   openId   = "";
		int      memberId = 0;
		
		public FeverPerson( String id, int mid ) {
			openId = id;
			memberId = mid;
		}
		
		@Override
		public boolean equals(Object o) {
			if (this == o) return true;
			if (o == null || getClass() != o.getClass()) return false;
			FeverPerson person = (FeverPerson)o;
			if ( !openId.equals(person.openId)) return false;
			return true;
		}
		
		@Override
		public int hashCode() {
			return openId.hashCode() + memberId;
		}
		
		@Override
		public String toString(){
			return "{open id: " + openId + ",member id: " + memberId + "}";
		}

	}
	
	private class FeverPersonData {
		double     lat  = 0.0;
		double     lng  = 0.0;
		int      adCode = 0;
		public FeverPersonData( double a, double b) {
			lat = a;
			lng = b;
		}
		
		@Override
		public String toString(){
			return "{lat: " + lat + ",lng: " + lng + ",adcode: " + adCode + "}";
		}
	}
	
	private  class  AreaData {
		String  name = "";
		int     num  = 0;
		int     num1 = 0;
		
		public AreaData(String n) {
			name = n;
		}
	}
	
	
	
	
    
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
			else if ( type.equals("updatemember") ) {
				String openid = new String();
				String newMemberName = new String();
				int memberId = 0;
				if ( null != req.getParameter("openid") ) {
					openid = req.getParameter("openid");
				}
				if ( null != req.getParameter("membername") ) {
					newMemberName = req.getParameter("membername");
				}
				if ( null != req.getParameter("memberid") ) {
					memberId = Integer.parseInt(req.getParameter("memberid"));
				}
				updateMember(out,openid, newMemberName, memberId);
			}
			else if ( type.equals("delmember") ) {
				String openid = new String();
				int memberId = 0;
				if ( null != req.getParameter("openid") ) {
					openid = req.getParameter("openid");
				}
				if ( null != req.getParameter("memberid") ) {
					memberId = Integer.parseInt(req.getParameter("memberid"));
				}
				deleteMember(out, openid, memberId);
			}
			else if ( type.equals("uploadtemperature") ) {
				String openid = new String();
				String tagid = new String();
				double lat = 0;
				double lng = 0;
				float temperature = 0;
				
				if ( null != req.getParameter("openid") ) {
					openid = req.getParameter("openid");
				}				
				if ( null != req.getParameter("tagid") ) {
					tagid = req.getParameter("tagid");
				}
				if ( null != req.getParameter("temperature") ) {
					temperature = Float.parseFloat(req.getParameter("temperature"));
				}
				if ( null != req.getParameter("lat") ) {
					lat = Double.parseDouble(req.getParameter("lat"));
				}
				if ( null != req.getParameter("lng") ) {
					lng = Double.parseDouble(req.getParameter("lng"));
				}			
				
				uploadTemperature(out, openid, tagid, temperature, lat, lng );
			}
			else if ( type.equals("binding") ) {
				String openid = new String();
				String tagid = new String();
				int memberId = 0;
				
				if ( null != req.getParameter("openid") ) {
					openid = req.getParameter("openid");
				}				
				if ( null != req.getParameter("tagid") ) {
					tagid = req.getParameter("tagid");
				}
				if ( null != req.getParameter("memberid") ) {
					memberId = Integer.parseInt(req.getParameter("memberid"));
				}		
				
				binding(out, openid, tagid, memberId );
			}
			else if ( type.equals("datavtest0") ) {
				datavtest0(out);
			}
			else if ( type.equals("userscnt") ) {
				userscnt(out);
			}
			else if ( type.equals("tagscnt") ) {
				tagscnt(out);
			}
			else if ( type.equals("feverpeople") ) {
				feverpeople(out);
			}
			else if ( type.equals("rotation") ) {
				rotation(out);
			}
			else if ( type.equals("updaterotation") ) {
				updaterotation(out);
			}
			else if ( type.equals("test") ) {
				test(out);
			}
		}
		
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
			JSONArray tagsbinding = new JSONArray();
			JSONArray lasttemperature = new JSONArray();
			object.put("logined", true);
			object.put("members", members);
			object.put("tagsbinding", tagsbinding);
			object.put("lasttemperature", lasttemperature);
			
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
				
				rs.close();
				rs = stmt.executeQuery("select a.tagid, a.memberid from binding a inner join family b on a.memberid = b.memberid where b.open_id='" + open_id_sql + "'" );
				while ( rs.next() ) {
					JSONObject binding = new JSONObject();					
					binding.put("tagid",     rs.getString(1));
					binding.put("memberid",  rs.getInt(2));
					tagsbinding.put(binding);
				}
				
				rs.close();
				rs = stmt.executeQuery("select a.temp, a.ctime, b.memberid from temperature a left join binding b on a.tagid = b.tagid where a.open_id='" + open_id_sql + "' order by b.memberid, a.ctime desc;");
				int lastMemberId = -1;
				while ( rs.next() ) {
					int memberId = rs.getInt(3);
					if ( memberId != lastMemberId ) {
						JSONObject item = new JSONObject();					
						item.put("temperature",     rs.getFloat(1));
						item.put("time",            rs.getTimestamp(2).getTime());
						item.put("memberid",        memberId);
						lasttemperature.put(item);
						lastMemberId = memberId;
					}
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
	
	public void updateMember(PrintWriter out, String openid, String newMemberName, int memberId) {
		// String openid_sql        = openid.replace("'","''");
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
			stmt.executeUpdate("update family set membername='" + newMemberName + "' where memberid=" + memberId);
			rsp_obj.put("error", 0);
			out.print(rsp_obj.toString());
			
			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
	}
	
	public void deleteMember(PrintWriter out, String openid, int memberId) {
		String open_id_sql        = openid.replace("'","''");
		
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
			JSONArray lasttemperature = new JSONArray();
			rsp_obj.put("lasttemperature", lasttemperature);
			
			Statement stmt = con.createStatement();      
			stmt.executeUpdate("delete from family where memberid=" + memberId);
			stmt.executeUpdate("delete from binding where memberid=" + memberId );
			
			ResultSet rs = stmt.executeQuery("select a.temp, a.ctime, b.memberid from temperature a left join binding b on a.tagid = b.tagid where a.open_id='" + open_id_sql + "' order by b.memberid, a.ctime desc;");
			int lastMemberId = -1;
			while ( rs.next() ) {
				int memid = rs.getInt(3);
				if ( memid != lastMemberId ) {
					JSONObject item = new JSONObject();					
					item.put("temperature",     rs.getFloat(1));
					item.put("time",            rs.getTimestamp(2).getTime());
					item.put("memberid",        memid);
					lasttemperature.put(item);
					lastMemberId = memid;
				}
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
	
	public void uploadTemperature(PrintWriter out, String openid, String tagid,  float temperature, double lat, double lng) {
		String openid_sql        = openid.replace("'","''");
		
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
			// 如果上传温度小于阈值
			if ( temperature < uploadThreshold ) {
				rsp_obj.put("error", 0);
				Date now = new Date();
				rsp_obj.put("time", now.getTime());
				out.print(rsp_obj.toString());
				return;
			}
			
			Statement stmt = con.createStatement();      
			stmt.executeUpdate("insert into temperature values(null, '" + openid_sql + "', '" + tagid + "', " + temperature + ",NOW()," + lat + "," + lng + ")");
			rsp_obj.put("error", 0);
			Date now = new Date();
			rsp_obj.put("time", now.getTime());
			out.print(rsp_obj.toString());			
			
			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
	}
	
	public void binding(PrintWriter out, String openid, String tagid,  int memberId) {
		String open_id_sql        = openid.replace("'","''");
		
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
			JSONArray lasttemperature = new JSONArray();
			rsp_obj.put("lasttemperature", lasttemperature);
			
			Statement stmt = con.createStatement();      
			ResultSet rs = stmt.executeQuery("select * from binding where tagid='" + tagid + "'" );
			
			// 如果已经有绑定关系
			if ( rs.next() ) {
				if ( memberId <= 0 ) {
					stmt.executeUpdate("delete from binding where tagid='" + tagid + "'");
				} else {
					stmt.executeUpdate("update binding set memberid=" + memberId + " where tagid='" + tagid + "'");
				}
			}
			// 如果没有绑定关系
			else {
				if ( memberId > 0 ) {
					stmt.executeUpdate("insert into binding values('" + tagid + "', " + memberId + ")");
				}
			}
			
			rs.close();
			rs = stmt.executeQuery("select a.temp, a.ctime, b.memberid from temperature a left join binding b on a.tagid = b.tagid where a.open_id='" + open_id_sql + "' order by b.memberid, a.ctime desc;");
			int lastMemberId = -1;
			while ( rs.next() ) {
				int memid = rs.getInt(3);
				if ( memid != lastMemberId ) {
					JSONObject item = new JSONObject();					
					item.put("temperature",     rs.getFloat(1));
					item.put("time",            rs.getTimestamp(2).getTime());
					item.put("memberid",        memid);
					lasttemperature.put(item);
					lastMemberId = memid;
				}
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
	
	public void datavtest0(PrintWriter out) {
		
		try {			
			Connection con = null;
			try{
				con = getConnection();
			}
			catch(Exception e ) {
				out.print(e.getMessage());
				return;
			}
			
			JSONArray rsp_obj = new JSONArray();			
			Statement stmt = con.createStatement();      
			ResultSet rs = stmt.executeQuery("select open_id, lat, lng  from temperature order by open_id, ctime desc" );
			
			String lastopenid = "";
			while ( rs.next() ) {
				String itemid = rs.getString(1);
				if ( !itemid.equals(lastopenid) ) {
					JSONObject item = new JSONObject();					
					item.put("lat",     rs.getDouble(2));
					item.put("lng",     rs.getDouble(3));
					item.put("value",   1);
					rsp_obj.put(item);
					lastopenid = itemid;
				}
			}
			
			out.print(rsp_obj.toString());			
			
			rs.close();
			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
	}
	
	public void userscnt(PrintWriter out) {
		try {			
			Connection con = null;
			try{
				con = getConnection();
			}
			catch(Exception e ) {
				out.print(e.getMessage());
				return;
			}
			
			JSONArray rsp_obj = new JSONArray();			
			Statement stmt = con.createStatement();      
			ResultSet rs = stmt.executeQuery("select count(*) from users" );
			if ( rs.next() ) {
				JSONObject item = new JSONObject();
				item.put("value",     rs.getInt(1));
				rsp_obj.put(item);
			}	
			out.print(rsp_obj.toString());			
			
			rs.close();
			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
	}
	
	public void tagscnt(PrintWriter out) {
		try {			
			Connection con = null;
			try{
				con = getConnection();
			}
			catch(Exception e ) {
				out.print(e.getMessage());
				return;
			}
			
			JSONArray rsp_obj = new JSONArray();			
			Statement stmt = con.createStatement();      
			ResultSet rs = stmt.executeQuery("select count(distinct tagid) from temperature" );
			if ( rs.next() ) {
				JSONObject item = new JSONObject();
				item.put("value",     rs.getInt(1));
				rsp_obj.put(item);
			}	
			out.print(rsp_obj.toString());			
			
			rs.close();
			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
	}
	
	public void feverpeople(PrintWriter out) {
		try {			
			Connection con = null;
			try{
				con = getConnection();
			}
			catch(Exception e ) {
				out.print(e.getMessage());
				return;
			}
			
			JSONArray rsp_obj = new JSONArray();			
			Statement stmt = con.createStatement();      
			ResultSet rs = stmt.executeQuery("select a.open_id, a.temp, a.ctime, a.lat, a.lng, b.memberid from temperature a left join binding b on a.tagid = b.tagid where datediff(now(),a.ctime) < " + maxFeverSpan + " order by a.open_id, b.memberid, a.ctime desc;" );
			
			String lastOpenId = new String();
			int lastMemberId = 0;
			
			while ( rs.next() ) {
				String openid = rs.getString(1);
				int memberId = rs.getInt(6);
				// 如果不是同一个人
				if ( !openid.equals(lastOpenId) || memberId != lastMemberId ) {
					float  temperature = rs.getFloat(2);
					// 如果是发热
					if ( temperature >= feverThreshold ) {
						JSONObject item = new JSONObject();					
						item.put("lat",     rs.getDouble(4));
						item.put("lng",     rs.getDouble(5));
						item.put("value",   1);
						rsp_obj.put(item);
					}
					lastOpenId = openid;
					lastMemberId = memberId;
				}
			}
						
			out.print(rsp_obj.toString());			
			
			rs.close();
			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
	}
	
	public void rotation(PrintWriter out) {
		try {			
			Connection con = null;
			try{
				con = getConnection();
			}
			catch(Exception e ) {
				out.print(e.getMessage());
				return;
			}
			
			JSONArray rsp_obj = new JSONArray();			
			Statement stmt = con.createStatement();      
			ResultSet rs = stmt.executeQuery("select * from areaupload" );
			while ( rs.next() ) {
				JSONObject item = new JSONObject();					
				item.put("area",     rs.getString(2));
				item.put("num",      rs.getInt(3));
				item.put("num1",     rs.getInt(4));
				rsp_obj.put(item);
			}
						
			out.print(rsp_obj.toString());			
			
			rs.close();
			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
	}
	
	// 更新昨日发热总数，新增数
	public void updaterotation(PrintWriter out) {
		try {			
			Connection con = null;
			try{
				con = getConnection();
			}
			catch(Exception e ) {
				out.print(e.getMessage());
				return;
			}
			
			JSONArray rsp_obj = new JSONArray();
			Map<FeverPerson,FeverPersonData>  yesterday = new HashMap<FeverPerson,FeverPersonData>();
			Statement stmt = con.createStatement();      
			// 昨天的总发热人数
			ResultSet rs = stmt.executeQuery("select a.open_id, a.temp, a.ctime, a.lat, a.lng, b.memberid from temperature a left join binding b on a.tagid = b.tagid where datediff(now(),a.ctime) < " + (maxFeverSpan +1) + " and datediff(now(),a.ctime) > 0  order by a.open_id, b.memberid, a.ctime desc;" );
			
			String lastOpenId = new String();
			int lastMemberId = 0;
			
			while ( rs.next() ) {
				String openid = rs.getString(1);
				int memberId = rs.getInt(6);
				// 如果不是同一个人
				if ( !openid.equals(lastOpenId) || memberId != lastMemberId ) {
					float  temperature = rs.getFloat(2);
					// 如果是发热
					if ( temperature >= feverThreshold ) {
						FeverPerson item = new FeverPerson(openid, memberId);
						yesterday.put(item, new FeverPersonData(rs.getDouble(4), rs.getDouble(5)) );
					}
					lastOpenId = openid;
					lastMemberId = memberId;
				}
			}
			
			// 获取区域号码adcode
			Iterator<Entry<FeverPerson, FeverPersonData>> entries = yesterday.entrySet().iterator();
			while(entries.hasNext()){
				Entry<FeverPerson, FeverPersonData> entry = entries.next();
				FeverPerson key = entry.getKey();
				FeverPersonData value = entry.getValue();
				getAdCode(value);
				/*
				JSONObject item = new JSONObject();					
				item.put("lat",     value.lat);
				item.put("lng",     value.lng);
				item.put("adcode",  value.adCode );
				item.put("test", value.lat + "," + value.lng );
				rsp_obj.put(item);
				*/
			}	
			
			// 前天的总发热人数
			Map<FeverPerson, FeverPersonData>  added = new HashMap<FeverPerson,FeverPersonData>();
			added.putAll(yesterday);
			
			rs.close();
			rs = stmt.executeQuery("select a.open_id, a.temp, a.ctime, a.lat, a.lng, b.memberid from temperature a left join binding b on a.tagid = b.tagid where datediff(now(),a.ctime) < " + (maxFeverSpan +2) + " and datediff(now(),a.ctime) > 1  order by a.open_id, b.memberid, a.ctime desc;" );
			
			lastOpenId = "";
			lastMemberId = 0;
			
			while ( rs.next() ) {
				String openid = rs.getString(1);
				int memberId = rs.getInt(6);
				// 如果不是同一个人
				if ( !openid.equals(lastOpenId) || memberId != lastMemberId ) {
					float  temperature = rs.getFloat(2);
					// 如果是发热
					if ( temperature >= feverThreshold ) {
						FeverPerson p = new FeverPerson(openid, memberId);
						if ( added.containsKey(p) ) {
							added.remove(p);
						}
					}					
				
					lastOpenId = openid;
					lastMemberId = memberId;
				}
			}
			
			Map<Integer,AreaData>  areas = new HashMap<Integer,AreaData>();
			areas.put(110101, new AreaData("东城区"));
			areas.put(110102, new AreaData("西城区"));
			areas.put(110105, new AreaData("朝阳区"));
			areas.put(110106, new AreaData("丰台区"));
			areas.put(110107, new AreaData("石景山区"));
			areas.put(110108, new AreaData("海淀区"));
			areas.put(110109, new AreaData("门头沟区"));
			areas.put(110111, new AreaData("房山区"));
			areas.put(110112, new AreaData("通州区"));
			areas.put(110113, new AreaData("顺义区"));
			areas.put(110114, new AreaData("昌平区"));
			areas.put(110115, new AreaData("大兴区"));
			areas.put(110116, new AreaData("怀柔区"));
			areas.put(110117, new AreaData("平谷区"));
			areas.put(110118, new AreaData("密云区"));
			areas.put(110119, new AreaData("延庆区"));
			
			entries = yesterday.entrySet().iterator();
			while(entries.hasNext()){
				Entry<FeverPerson, FeverPersonData> entry = entries.next();
				FeverPerson key = entry.getKey();
				FeverPersonData value = entry.getValue();		
				if ( areas.containsKey(value.adCode) ) {
					AreaData data = (AreaData)areas.get(value.adCode);
					data.num += 1;
				}
			}
			
			entries = added.entrySet().iterator();
			while(entries.hasNext()){
				Entry<FeverPerson, FeverPersonData> entry = entries.next();
				FeverPerson key = entry.getKey();
				FeverPersonData value = entry.getValue();		
				if ( areas.containsKey(value.adCode) ) {
					AreaData data = (AreaData)areas.get(value.adCode);
					data.num1 += 1;
				}
			}
			
			// 更新昨日发热总数，新增数
			Iterator<Entry<Integer,AreaData>> entries_1 = areas.entrySet().iterator();
			while(entries_1.hasNext()){
				Entry<Integer,AreaData> entry = entries_1.next();
				int key = entry.getKey();
				AreaData value = entry.getValue();						
				stmt.executeUpdate("update areaupload set yesterdayupload=" + value.num + ", yesterdayadded=" + value.num1 + " where adcode="+key);
			}
			
						
			out.print(rsp_obj.toString());						
			
			rs.close();
			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
	}
	
	private void getAdCode(FeverPersonData value) {
		HashMap<String, Object> params = new HashMap<String, Object>();
		params.put("ak",          "wWYw0yCb8ntXmSgTxTx40vKR");  
		params.put("location",    value.lat + "," + value.lng);  
		params.put("output",      "json");  

		String ret = HttpHelper.sendGetByHttpUrlConnection("http://api.map.baidu.com/geocoder/v2/", params, "utf-8");
		JSONObject object = new JSONObject(ret);		
		JSONObject subObject = object.getJSONObject("result");
		if ( subObject != null ) {
			subObject = subObject.getJSONObject("addressComponent");
			if ( subObject != null ) {
				value.adCode = subObject.getInt("adcode");
			}
		}
	}
	
	
	
	
	

	public void test( PrintWriter out) {
		/*
		Map<FeverPerson,FeverPersonData>  yesterday = new HashMap<FeverPerson,FeverPersonData>();
		FeverPerson item = new FeverPerson("abc", 100);
		yesterday.put(item, new FeverPersonData(1.0, 2.0 ));
		
		Iterator<Entry<FeverPerson, FeverPersonData>> entries = yesterday.entrySet().iterator();
		while(entries.hasNext()){
			Entry<FeverPerson, FeverPersonData> entry = entries.next();
			FeverPerson key = entry.getKey();
			FeverPersonData value = entry.getValue();
			value.adCode += 10;
		}
		
		entries = yesterday.entrySet().iterator();
		while(entries.hasNext()){
			Entry<FeverPerson, FeverPersonData> entry = entries.next();
			FeverPerson key = entry.getKey();
			FeverPersonData value = entry.getValue();
			out.print(key+":"+value);
		}
		*/
		
		HashMap<String, Object> params = new HashMap<String, Object>();
		params.put("ak",          "wWYw0yCb8ntXmSgTxTx40vKR");  
		params.put("location",    "39.915156,116.401394");  
		params.put("output",      "json");  

		String ret = HttpHelper.sendGetByHttpUrlConnection("http://api.map.baidu.com/geocoder/v2/", params, "utf-8");
		JSONObject object = new JSONObject(ret);		
		out.print(object.getJSONObject("result").getJSONObject("addressComponent").getString("adcode"));
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
