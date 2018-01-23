package mypack;

import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;
import java.util.*;
import java.sql.*;
import org.json.*;

// 传递的参数有：
//     type:    操作类型
//              egister，注册用户
public class MainServlet extends HttpServlet {
	
	String dbUrl = "jdbc:mysql://localhost:3306/todo_list?useUnicode=true&characterEncoding=UTF-8";
	String dbUser="root";
	String dbPwd="";
	int    INCREASE_QUERY = 5;
    
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
					
		}
		
		//test(out);
		
		out.close();
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
			Class.forName("com.mysql.jdbc.Driver");
			DriverManager.registerDriver( new com.mysql.jdbc.Driver() );
						
			Connection con = java.sql.DriverManager.getConnection(dbUrl,dbUser,dbPwd);
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
			Class.forName("com.mysql.jdbc.Driver");
			DriverManager.registerDriver( new com.mysql.jdbc.Driver() );
			
			Connection con = java.sql.DriverManager.getConnection(dbUrl,dbUser,dbPwd);
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
			Class.forName("com.mysql.jdbc.Driver");
			DriverManager.registerDriver( new com.mysql.jdbc.Driver() );
						
			Connection con = java.sql.DriverManager.getConnection(dbUrl,dbUser,dbPwd);
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
			Class.forName("com.mysql.jdbc.Driver");
			DriverManager.registerDriver( new com.mysql.jdbc.Driver() );
						
			Connection con = java.sql.DriverManager.getConnection(dbUrl,dbUser,dbPwd);
			Statement stmt = con.createStatement();      
			ResultSet rs = stmt.executeQuery("select * from todolist_items where owner_id='" + open_id_sql + "' AND is_complete = 1 order by end_time limit " + start_index + "," + INCREASE_QUERY + " ;" );
			
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
			Class.forName("com.mysql.jdbc.Driver");
			DriverManager.registerDriver( new com.mysql.jdbc.Driver() );
						
			Connection con = java.sql.DriverManager.getConnection(dbUrl,dbUser,dbPwd);
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
			Class.forName("com.mysql.jdbc.Driver");
			DriverManager.registerDriver( new com.mysql.jdbc.Driver() );
						
			Connection con = java.sql.DriverManager.getConnection(dbUrl,dbUser,dbPwd);
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
