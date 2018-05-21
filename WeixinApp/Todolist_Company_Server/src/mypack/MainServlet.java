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
//     type:    操作类型
//              egister，注册用户
public class MainServlet extends HttpServlet {
	
	//String dbUrl = "jdbc:mysql://localhost:3306/todo_list?useUnicode=true&characterEncoding=UTF-8";
	//String dbUser="root";
	//String dbPwd="";
	int    INCREASE_QUERY = 10;
	int    INCREASE_QUERY_WEIBO = 6;
	String appid = "wx7a3e83a343c2347e";
	String secret = "384078a850d0bd5bdc842e89be5c88d8";
    
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
			setContentError(out,1);
		} else {			
			// 如果是注册用户
			if ( type.equals("register") ) {				
				String  open_id   = new String();
				String  name      = new String();
				String  avatarUrl = new String();
				
				if ( null != req.getParameter("open_id") ) {
					open_id = req.getParameter("open_id");
				}
				
				if ( null != req.getParameter("name") ) {
					name = req.getParameter("name");
				}
				
				if ( null != req.getParameter("avatarUrl") ) {
					avatarUrl = req.getParameter("avatarUrl");
				}
				
				register(out, open_id, name, avatarUrl);
			}
			else if ( type.equals("additem") ) {
				String  open_id   = new String();
				String  item      = new String();
				
				if ( null != req.getParameter("open_id") ) {
					open_id = req.getParameter("open_id");
				}
				
				if ( null != req.getParameter("item") ) {
					item = req.getParameter("item");
				}
				
				additem(out, open_id, item);
			}
			else if ( type.equals("todolist") ) {
				String  open_id   = new String();
				
				if ( null != req.getParameter("open_id") ) {
					open_id = req.getParameter("open_id");
				}
				
				getTodoList(out, open_id);
			}
			else if ( type.equals("delete") ) {
				String  item_id   = new String();
				
				if ( null != req.getParameter("id") ) {
					item_id = req.getParameter("id");
				}
				
				deleteItem(out, item_id);
			}
			else if ( type.equals("finish") ) {
				String  item_id   = new String();
				
				if ( null != req.getParameter("id") ) {
					item_id = req.getParameter("id");
				}
				
				finishItem(out, item_id);
			}
			else if ( type.equals("history") ) {
				String  open_id     = new String();
				int     start_index = 0;
				
				if ( null != req.getParameter("open_id") ) {
					open_id = req.getParameter("open_id");
				}
				
				if ( null != req.getParameter("start_index") ) {
					start_index = Integer.valueOf(req.getParameter("start_index"));
				}
				
				getHistoryTodoList(out, open_id, start_index);
			}
			else if ( type.equals("get_openid") ) {
				String  code     = new String();
				
				if ( null != req.getParameter("code") ) {
					code = req.getParameter("code");
				}
				
				getOpenId(out, code);
			}
			else if ( type.equals("users") ) {
				
				getUsers(out);
			}
			else if ( type.equals("any_todolist") ) {
				String  user_id     = new String();			
				int     is_complete = 0;
				int     start_index = 0;
				
				if ( null != req.getParameter("user_id") ) {
					user_id = req.getParameter("user_id");
				}
				
				if ( null != req.getParameter("is_complete") ) {
					is_complete = Integer.valueOf( req.getParameter("is_complete") );
				}
				
				if ( null != req.getParameter("start_index") ) {
					start_index = Integer.valueOf( req.getParameter("start_index") );
				}
				
				getAnyTodolist(out, user_id, is_complete, start_index);
			}
			// web客户端获取用户信息
			else if ( type.equals("userinfo") ) {				
				String  user_id     = new String();			
				
				if ( null != req.getParameter("user_id") ) {
					user_id = req.getParameter("user_id");
				}
				
				getUserInfo( out, user_id );
			} 
			else if ( type.equals("bk") ) {				
				
				getBackground( out );
			} 
			else if ( type.equals("addhotpoint") ) {
				String  open_id   = new String();
				String  item      = new String();
				
				if ( null != req.getParameter("open_id") ) {
					open_id = req.getParameter("open_id");
				}
				
				if ( null != req.getParameter("item") ) {
					item = req.getParameter("item");
				}
				
				String[] image_urls = new String[9];
				int i;
				for ( i = 0; i <  9; i++ ) {
					String img_url_name = "img" + i;
					if ( null != req.getParameter( img_url_name ) ) {
						image_urls[i] = req.getParameter(img_url_name);
					} else {
						break;
					}
				}
				
				addhotpoint( out, open_id, item, image_urls );
			}
			else if ( type.equals("weibo") ) {
				
				int  start_index = 0;
				String  open_id   = new String();
				
				if ( null != req.getParameter("start_index") ) {
					start_index = Integer.valueOf(req.getParameter("start_index"));
				}
				
				if ( null != req.getParameter("open_id") ) {
					open_id = req.getParameter("open_id");
				}
				
				getHotPoint(out, start_index, open_id);
			}
					
		}
		
		//test(out);
		
		out.close();
    }   
	
	public Connection getConnection() throws NamingException, SQLException  {
		Context ctx = new InitialContext();
		DataSource ds = (DataSource)ctx.lookup("java:comp/env/jdbc/todolist_company");
		Connection con = ds.getConnection();
		return con;
	}
	
	
	public void register(PrintWriter out, String open_id, String name, String avatarUrl) {
		// 没有填写open_id参数
		if ( open_id.length() == 0 ) {
			setContentError(out,2);
			return;
		}
		
		String open_id_sql = open_id.replace("'","''");
		String name_sql    = name.replace("'","''");
		String avatarUrl_sql = avatarUrl.replace("'","''");
		
		try {
			//Class.forName("com.mysql.jdbc.Driver");
			//DriverManager.registerDriver( new com.mysql.jdbc.Driver() );						
			//Connection con = java.sql.DriverManager.getConnection(dbUrl,dbUser,dbPwd);
			
			//Context ctx = new InitialContext();
			//DataSource ds = (DataSource)ctx.lookup("java:comp/env/jdbc/todolist");
			//Connection con = ds.getConnection();
			
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
			
			// 如果已经注册
			if ( rs.next() ) {
				String name_get = rs.getString(2);
				String avatarUrl_get = rs.getString(4);				
				
				if ( !(name.equals(name_get) && avatarUrl.equals(avatarUrl_get)) ) {
					stmt.executeUpdate("update users set name='" + name_sql + "', avatar_url='" + avatarUrl_sql + "' where open_id = '" + open_id_sql + "';" );
				} 
			} 
			// 没有注册
			else {
				rs.close();
				rs = stmt.executeQuery("select * from config;" );
				int  register_enable = 0;
				if ( rs.next() )
				{
					register_enable = rs.getInt(2);
				}
				
				// 如果不允许注册
				if ( register_enable == 0 ) {
					setContentError(out,99);
					
					rs.close();
					stmt.close();
					con.close();
					return;
				}
				stmt.executeUpdate("insert into users values('" + open_id_sql + "','" + name_sql +"','" + name_sql + "','" + avatarUrl_sql + "') ");
			}
			
			setContentError(out,0);
			
			rs.close();
			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
	}
	
	public void additem(PrintWriter out, String open_id, String item) {
		// 没有填写open_id参数
		if ( open_id.length() == 0 ) {
			setContentError(out,2);
			return;
		}
		
		// 没有填写item参数
		if ( item.length() == 0 ) {
			setContentError(out,3);
			return;
		}
		
		String open_id_sql = open_id.replace("'","''");
		String item_sql    = item.replace("'","''");
		
		try {
			//Class.forName("com.mysql.jdbc.Driver");
			//DriverManager.registerDriver( new com.mysql.jdbc.Driver() );
			
			//Connection con = java.sql.DriverManager.getConnection(dbUrl,dbUser,dbPwd);
			Connection con = null;
			try{
				con = getConnection();
			}
			catch(Exception e ) {
				out.print(e.getMessage());
				return;
			}
			
			Statement stmt = con.createStatement();      
			stmt.executeUpdate( "insert into todolist_items values( null, '" + item_sql + "', NOW(), null, 0, '" + open_id_sql + "' );" );
			
			// 获取插入值id			
			ResultSet rs = stmt.executeQuery("select max(item_id) from todolist_items where owner_id='" + open_id_sql + "'" );
			JSONObject rsp_obj = new JSONObject();
			
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
	
	public void getTodoList(PrintWriter out, String open_id) {
		// 没有填写open_id参数
		if ( open_id.length() == 0 ) {
			setContentError(out,2);
			return;
		}
		
		String open_id_sql = open_id.replace("'","''");
		
		try {
			//Class.forName("com.mysql.jdbc.Driver");
			//DriverManager.registerDriver( new com.mysql.jdbc.Driver() );
						
			//Connection con = java.sql.DriverManager.getConnection(dbUrl,dbUser,dbPwd);
			Connection con = null;
			try{
				con = getConnection();
			}
			catch(Exception e ) {
				out.print(e.getMessage());
				return;
			}
			
			Statement stmt = con.createStatement();      
			ResultSet rs = stmt.executeQuery("select * from todolist_items where owner_id='" + open_id_sql + "' AND is_complete = 0 order by start_time ;" );
			
			// 获取清单
			JSONArray item_arr = new JSONArray();
			while ( rs.next() ) {
				int        item_id     = rs.getInt(1);
				String     content     = rs.getString(2);
				Timestamp  start_time  = rs.getTimestamp(3);
				//int        is_complete = rs.getInt(5);
				//String     owner_id    = rs.getString(6);
				
				JSONObject item_obj = new JSONObject();
				item_obj.put("id",            item_id);
				item_obj.put("value",         content);
				item_obj.put("start_time",    start_time.getTime());
				//item_obj.put("is_complete",   is_complete);
				//item_obj.put("owner_id",      owner_id);
				
				item_arr.put(item_obj);
			} 

			JSONObject rsp_obj = new JSONObject();
			rsp_obj.put("todolist", item_arr);
			rsp_obj.put("error", 0);
			
			out.print(rsp_obj.toString());
			
			rs.close();
			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
	}
	
	public void getHistoryTodoList(PrintWriter out, String open_id, int start_index) {
		// 没有填写open_id参数
		if ( open_id.length() == 0 ) {
			setContentError(out,2);
			return;
		}
		
		String open_id_sql = open_id.replace("'","''");
		
		try {
			//Class.forName("com.mysql.jdbc.Driver");
			//DriverManager.registerDriver( new com.mysql.jdbc.Driver() );
						
			//Connection con = java.sql.DriverManager.getConnection(dbUrl,dbUser,dbPwd);
			Connection con = null;
			try{
				con = getConnection();
			}
			catch(Exception e ) {
				out.print(e.getMessage());
				return;
			}
			
			Statement stmt = con.createStatement();      
			ResultSet rs = stmt.executeQuery("select * from todolist_items where owner_id='" + open_id_sql + "' AND is_complete = 1 order by end_time desc limit " + start_index + "," + INCREASE_QUERY + " ;" );
			
			// 获取清单
			JSONArray item_arr = new JSONArray();
			while ( rs.next() ) {
				int        item_id     = rs.getInt(1);
				String     content     = rs.getString(2);
				Timestamp  start_time  = rs.getTimestamp(3);
				Timestamp  end_time    = rs.getTimestamp(4);
				//int        is_complete = rs.getInt(5);
				//String     owner_id    = rs.getString(6);
				
				JSONObject item_obj = new JSONObject();
				item_obj.put("id",            item_id);
				item_obj.put("value",         content);
				item_obj.put("start_time",    start_time.getTime());
				item_obj.put("end_time",      end_time.getTime());
				//item_obj.put("is_complete",   is_complete);
				//item_obj.put("owner_id",      owner_id);				
				item_arr.put(item_obj);
			} 

			JSONObject rsp_obj = new JSONObject();
			rsp_obj.put("todolist", item_arr);
			rsp_obj.put("error", 0);
			
			out.print(rsp_obj.toString());
			
			rs.close();
			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
	}
	
	public void deleteItem(PrintWriter out, String item_id) {
		// 没有填写id参数
		if ( item_id.length() == 0 ) {
			setContentError(out,10);
			return;
		}
		
		try {
			//Class.forName("com.mysql.jdbc.Driver");
			//DriverManager.registerDriver( new com.mysql.jdbc.Driver() );
						
			//Connection con = java.sql.DriverManager.getConnection(dbUrl,dbUser,dbPwd);
			Connection con = null;
			try{
				con = getConnection();
			}
			catch(Exception e ) {
				out.print(e.getMessage());
				return;
			}
			
			Statement stmt = con.createStatement();      
			stmt.executeUpdate("delete from todolist_items where item_id = " + item_id + ";" );
			
			setContentError( out, 0 );

			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
	}
	
	public void finishItem(PrintWriter out, String item_id) {
		// 没有填写id参数
		if ( item_id.length() == 0 ) {
			setContentError(out,10);
			return;
		}
		
		try {
			//Class.forName("com.mysql.jdbc.Driver");
			//DriverManager.registerDriver( new com.mysql.jdbc.Driver() );
						
			//Connection con = java.sql.DriverManager.getConnection(dbUrl,dbUser,dbPwd);
			Connection con = null;
			try{
				con = getConnection();
			}
			catch(Exception e ) {
				out.print(e.getMessage());
				return;
			}
			
			Statement stmt = con.createStatement();      
			stmt.executeUpdate("UPDATE todolist_items set is_complete = not is_complete, end_time=NOW() where item_id=" + item_id );
			
			setContentError( out, 0 );

			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
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
			out.print(ret);
		}
		catch(Exception ex) {
			 out.print(ex.getMessage());
		}
				
	}
	
	public void getUsers(PrintWriter out ) {
						
		try {
			//Class.forName("com.mysql.jdbc.Driver");
			//DriverManager.registerDriver( new com.mysql.jdbc.Driver() );
						
			//Connection con = java.sql.DriverManager.getConnection(dbUrl,dbUser,dbPwd);
			Connection con = null;
			try{
				con = getConnection();
			}
			catch(Exception e ) {
				out.print(e.getMessage());
				return;
			}
			
			Statement stmt = con.createStatement();      
			ResultSet rs = stmt.executeQuery("select * from users;" );
			
			// 获取清单
			JSONArray item_arr = new JSONArray();
			while ( rs.next() ) {
				String     user_id       = rs.getString(1);
				String     nick_name     = rs.getString(3);
				String     avatarUrl     = rs.getString(4);
				
				JSONObject item_obj = new JSONObject();
				item_obj.put("user_id",       user_id);
				item_obj.put("nick_name",     nick_name);
				item_obj.put("avatarUrl",     avatarUrl);
					
				item_arr.put(item_obj);
			} 

			JSONObject rsp_obj = new JSONObject();
			rsp_obj.put("users", item_arr);
			rsp_obj.put("error", 0);
			
			out.print(rsp_obj.toString());
			
			rs.close();
			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
				
	}
	
	public void getAnyTodolist(PrintWriter out, String user_id, int is_complete, int start_index ) {
		
		String user_id_sql = user_id.replace("'","''");
						
		try {
			//Class.forName("com.mysql.jdbc.Driver");
			//DriverManager.registerDriver( new com.mysql.jdbc.Driver() );
						
			//Connection con = java.sql.DriverManager.getConnection(dbUrl,dbUser,dbPwd);
			Connection con = null;
			try{
				con = getConnection();
			}
			catch(Exception e ) {
				out.print(e.getMessage());
				return;
			}
			
			Statement stmt = con.createStatement();      
			ResultSet rs = null;
			
			if ( user_id.length() == 0 ) {
				if ( is_complete == 0 ) {
					rs = stmt.executeQuery("select a.item_id, a.content, a.start_time, a.end_time, b.nickname from todolist_items a inner join users b on a.owner_id = b.open_id where a.is_complete =0;" );
				} else {
					rs = stmt.executeQuery("select a.item_id, a.content, a.start_time, a.end_time, b.nickname from todolist_items a inner join users b on a.owner_id = b.open_id where a.is_complete = 1 order by a.end_time desc limit " + start_index + "," + INCREASE_QUERY + ";" );
				}
				
			} else {
				if ( is_complete == 0 )
				{
					rs = stmt.executeQuery("select a.item_id, a.content, a.start_time, a.end_time, b.nickname from todolist_items a inner join users b on a.owner_id = b.open_id where a.is_complete =0 AND a.owner_id='" + user_id_sql + "';" );
				} else {
					rs = stmt.executeQuery("select a.item_id, a.content, a.start_time, a.end_time, b.nickname from todolist_items a inner join users b on a.owner_id = b.open_id where a.is_complete =1 AND a.owner_id='" 
					                       + user_id_sql + "' order by a.end_time desc limit " +  start_index + "," + INCREASE_QUERY + " ;" );
				}				
			}
			
			
			// 获取清单
			JSONArray item_arr = new JSONArray();
			while ( rs.next() ) {
				int        item_id     = rs.getInt(1);
				String     content     = rs.getString(2);
				Timestamp  start_time  = rs.getTimestamp(3);
				Timestamp  end_time    = rs.getTimestamp(4);
				String     nickname    = rs.getString(5);
				
				JSONObject item_obj = new JSONObject();
				item_obj.put("id",            item_id);
				item_obj.put("value",         content);
				item_obj.put("start_time",    start_time.getTime());
				if ( is_complete != 0 ) {
					item_obj.put("end_time",      end_time.getTime());
				}					
				item_obj.put("nickname",      nickname);
							
				item_arr.put(item_obj);
			} 

			JSONObject rsp_obj = new JSONObject();
			rsp_obj.put("todolist", item_arr);
			rsp_obj.put("error", 0);
			
			out.print(rsp_obj.toString());
			
			rs.close();
			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
				
	}
	
	public void getUserInfo(PrintWriter out, String user_id ) {
		
		// 没有填写参数
		if ( user_id.length() == 0 ) {
			setContentError(out,30);
			return;
		}
		
		String user_id_sql = user_id.replace("'","''");
						
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
			ResultSet rs = stmt.executeQuery("select * from users where open_id = '" + user_id_sql + "';" );
						
			if ( rs.next() ) {
				JSONObject item_obj = new JSONObject();
				
				String     nick_name     = rs.getString(3);
				String     avatarUrl     = rs.getString(4);
				
				item_obj.put("nick_name",     nick_name);
				item_obj.put("avatarUrl",     avatarUrl);
				
				JSONObject rsp_obj = new JSONObject();
				rsp_obj.put("userinfo", item_obj);
				rsp_obj.put("error", 0);
				
				out.print(rsp_obj.toString());
			} else {
				setContentError(out,31);
			}
			
			rs.close();
			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
				
	}
	
	
	public void getBackground(PrintWriter out ) {
		
		Calendar now = Calendar.getInstance();

		int year  = now.get(Calendar.YEAR);
        int month = now.get(Calendar.MONTH) + 1;
        int day   = now.get(Calendar.DAY_OF_MONTH);
		
		boolean find = false;
		String  bkImage = "common.jpg";
		
		// 公历节日：元旦，妇女节，愚人节，劳动节，建军节、建党节、国庆节、圣诞节
		
		// 元旦
		if ( month == 1 && day == 1 ) {
			find = true;
		} else if ( month == 3 && 8 == day ) {
			find = true;
		} else if ( month == 4 && 1 == day ) {
			find = true;
		} else if ( month == 5 && 1 == day ) {
			find = true;
		} else if ( month == 7 && 1 == day ) {
			find = true;
		} else if ( month == 8 && 1 == day ) {
			find = true;
		} else if ( month == 10 && 1 == day ) {
			find = true;
		} else if ( month == 12 && 25 == day ) {
			find = true;
		}
		
		if ( find ) {
			bkImage = "" + month + "_" + day + ".jpg";
			
			JSONObject rsp_obj = new JSONObject();
			rsp_obj.put("bkimage", bkImage);
			rsp_obj.put("error", 0);
			
			out.print(rsp_obj.toString());
			return;
 		}
		
		// 农历节日查表
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
			ResultSet rs = stmt.executeQuery("select bkimage from festivals where year = " + year + " and month = " + month + " and day = " + day );
						
			if ( rs.next() ) {
				bkImage = rs.getString(1);
			}
			
			JSONObject rsp_obj = new JSONObject();
			rsp_obj.put("bkimage", bkImage);
			rsp_obj.put("error", 0);			
			out.print(rsp_obj.toString());
			
			rs.close();
			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
	}
	
	
	/*
	public void doPost(HttpServletRequest req, HttpServletResponse rsp) throws ServletException, IOException {
		
	}
	*/
	
	
	public void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {

		
		response.setContentType("application/json;charset=utf-8");
        response.setHeader("Access-Control-Allow-Origin", "*");
		
		DiskFileItemFactory factory = new DiskFileItemFactory();
		ServletFileUpload upload = new ServletFileUpload(factory);
		
		PrintWriter out = response.getWriter();		
		JSONArray item_arr = new JSONArray();		
		int nError = 0;
		
		try {
			List items = upload.parseRequest(request);
			Iterator itr = items.iterator();
			while (itr.hasNext()) {				
				FileItem item = (FileItem) itr.next();
				if (item.isFormField()) {
					//out.print("<p>from param name:" + item.getFieldName() + "，value:" + item.getString("UTF-8") + "</p>");
				} else {
					if (item.getName() != null && !item.getName().equals("")) {
						//out.print("<p>file size:" + item.getSize() + "</p>");
						//out.print("<p>file type:" + item.getContentType() + "</p>");
						// item.getName()返回上传文件在客户端的完整路径名称
						//out.print("<p>file name:" + item.getName() +"</p>");
						
						int pos = item.getName().lastIndexOf(".");  
						String post_fix = new String();
						if ( pos > -1 ) {
							post_fix = item.getName().substring( pos );
						}
						
						Date d = new Date();
						SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMddHHmmssSSS");						
						String newRelativeName = "tmp/" + sdf.format(d) + post_fix;
						String newFileName = this.getServletContext().getRealPath("/") + newRelativeName;
						
						//out.print("<p>new file:" + newFileName + "</p>" );										
						File file = new File(newFileName);
						item.write(file);
						//out.print("<p>upload success!</p>");
						
						JSONObject item_obj = new JSONObject();
						item_obj.put("filename",  newRelativeName);
						item_arr.put(item_obj);
						
					} else {
						//out.print("<p>no file!</p>");
					}
				}
			}
		} catch (FileUploadException e) {
			e.printStackTrace();
			nError = -1;
		} catch (Exception e) {
			e.printStackTrace();
			//out.print("<p>failed to upload file!</p>");
			nError = -1;
		}
		
		if ( nError == 0 ) {			
			JSONObject rsp_obj = new JSONObject();
			rsp_obj.put("upload_files", item_arr);
			rsp_obj.put("error", 0);
			out.print(rsp_obj.toString());
			//setContentError(out,0);
		} else {
			JSONObject rsp_obj = new JSONObject();
			rsp_obj.put("error", -1);
			out.print(rsp_obj.toString());
		}
		
		out.close();
	}
	
	public void addhotpoint( PrintWriter out, String open_id, String item, String[] img_urls ) {
		// 没有填写open_id参数
		if ( open_id.length() == 0 ) {
			setContentError(out,2);
			return;
		}
		
		// 没有填写item参数
		if ( item.length() == 0 ) {
			setContentError(out,3);
			return;
		}
		
		String open_id_sql = open_id.replace("'","''");
		String item_sql    = item.replace("'","''");
		
		for ( int i = 0; i <  img_urls.length; i++ ) {
			if ( img_urls[i] == null ) {
				img_urls[i] = "null";
			} else {
				img_urls[i] = "'" + img_urls[i] + "'";
			}			
		}
		
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
			stmt.executeUpdate( "insert into weibo_items values( null, '" + open_id_sql + "', NOW(), '" + item_sql + "'," + img_urls[0] + "," + img_urls[1] + "," 
			     + img_urls[2] +"," + img_urls[3] + "," + img_urls[4] + "," + img_urls[5] + "," + img_urls[6] + "," + img_urls[7] + "," + img_urls[8] + ");"  );
						
			// 获取插入值id			
			ResultSet rs = stmt.executeQuery("select max(weibo_id) from weibo_items where user_id='" + open_id_sql + "'" );			
			int id = 0;
			if ( rs.next() ) {
				id = rs.getInt(1);
			}						
			rs.close();
			
			stmt.executeUpdate( "insert into weibo_reader values( " + id + ", '" + open_id_sql + "' );" );
						
			JSONObject rsp_obj = new JSONObject();			
			rsp_obj.put("error", 0);
			out.print(rsp_obj.toString());
			
			stmt.close();
			con.close();
        } catch (Exception ex ) {
           out.print(ex.getMessage());
        }
	}
	
	public void getHotPoint(PrintWriter out, int start_index, String  open_id) {
		// 没有填写open_id参数
		if ( open_id.length() == 0 ) {
			setContentError(out,2);
			return;
		}
		
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
			ResultSet rs = stmt.executeQuery("select a.weibo_id,b.nickname,b.avatar_url,a.pub_time,a.content,a.img0,a.img1,a.img2,a.img3," 
			    + "a.img4,a.img5,a.img6,a.img7,a.img8 from weibo_items a inner join users b on a.user_id = b.open_id order by a.pub_time desc limit " + start_index + "," + INCREASE_QUERY_WEIBO + " ;" );
			
			Statement sub_stmt = con.createStatement();
			// 获取清单
			JSONArray item_arr = new JSONArray();
			while ( rs.next() ) {
				int        item_id     = rs.getInt(1);
				String     nickname    = rs.getString(2);
				String     avatar_url  = rs.getString(3);
				Timestamp  pub_time    = rs.getTimestamp(4);
				String     content     = rs.getString(5);
				
				String[] imgs = new String[9];
				for ( int i = 0; i < 9; i++ ) {
					imgs[i] = rs.getString(6+i);
				}
								
				
				JSONObject item_obj = new JSONObject();
				item_obj.put("id",            item_id);
				item_obj.put("name",          nickname);
				item_obj.put("avatarUrl",     avatar_url);
				item_obj.put("content",       content);
				item_obj.put("pubtime",       pub_time.getTime());
				
				JSONArray img_arr = new JSONArray();
				for ( int i = 0; i < 9; i++ ) {
					if ( imgs[i] != null ) {
						//JSONObject img_obj = new JSONObject();
						//img_obj.put("src", imgs[i]);
						//img_arr.put( img_obj );
						img_arr.put( imgs[i] );
					}					
				}
				item_obj.put("images", img_arr);
				
				JSONArray reader_arr = new JSONArray();
				ResultSet sub_rs = sub_stmt.executeQuery( "select b.nickname from weibo_reader a inner join users b on a.user_id = b.open_id and a.weibo_id = " + item_id );
				while ( sub_rs.next() ) {
					String reader = sub_rs.getString(1);
					reader_arr.put( reader );
				}
				item_obj.put("readers", reader_arr);
				sub_rs.close();		
				try{
					sub_stmt.executeUpdate("insert into weibo_reader values ( " + item_id + ", '" + open_id_sql + "' );");
				}
				catch(Exception ex){
					
				}
					
				item_arr.put(item_obj);
			}
			sub_stmt.close();			

			JSONObject rsp_obj = new JSONObject();
			rsp_obj.put("weibo_items", item_arr);
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
