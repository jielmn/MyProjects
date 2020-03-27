<%@ page language="java" import="java.util.*" pageEncoding="UTF-8"%>

<% String testman = "何海军"; %>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
  <head>    
    <title>温度阻值测试</title>
    
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	<meta http-equiv="keywords" content="keyword1,keyword2,keyword3">
	<meta http-equiv="description" content="This is my page">
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	<!--
	<link rel="stylesheet" type="text/css" href="styles.css">
	-->
	<style type="text/css">
	*{
		margin: 0px;padding: 0px;font-family: "微软雅黑";
	}
	</style>
	<script type="text/javascript" src="js/jquery.min.js"></script>
	<script type="text/javascript" src="js/global.js"></script>
<script type="text/javascript">
	function ttchange(){
		var b=parseFloat($("#b").val());
		var r1min=parseFloat($("#r1min").val());
		var r1max=parseFloat($("#r1max").val());
		var t1=parseFloat($("#t1").val())+273.15;
		var tt=parseFloat($("#tt").val())+273.15;
		$("#rtmin").val(Math.round(r1min*Math.exp(b*(1/tt-1/t1))*100000)/100000);
		$("#rtmax").val(Math.round(r1max*Math.exp(b*(1/tt-1/t1))*100000)/100000);
	}
	function ttkeydown(){
		if(event.keyCode==13){
			$("#om").focus();
		}else if(event.keyCode==38){
			$("#tt").val((100*parseFloat($("#tt").val())+1)/100);
		}else if(event.keyCode==40){
			$("#tt").val((100*parseFloat($("#tt").val())-1)/100);
		}
	}
	function omchange(){
		var rtmin=parseFloat($("#rtmin").val())-0.05;
		var rtmax=parseFloat($("#rtmax").val())+0.05;
		var om=parseFloat($("#om").val());
		$("#result").val((om>rtmin&&om<rtmax)?"合格":"不合格");
		$("#result").css("color",(om>rtmin&&om<rtmax)?"green":"red");
	}

	function omkeydown(){
		if(event.keyCode==13){
			$("#result").focus();
		}
	}
	
	function subclick(){
		
	}
	
	function resultkeydown(){
		if(event.keyCode==13){
			uptest();
		}
	}
	function uptest(){
		var testman='<%=testman%>';
		var ttval=$("#tt").val();
		var omval=$("#om").val();
		$.ajax({    
		     type:'post',    
		     url:'index.jsp',
		     async:false,
		     data:{
		    	 temp:ttval,
		    	 correct:$("#result").val(),
		    	 om:omval,
		    	 testman:testman
		    	 },    
		     contentType: "application/x-www-form-urlencoded; charset=utf-8", 
		     success:function(data){  
		          if( data =="true" ){    
		        	  	$("#tt").val(ttval);
						$("#om").val(omval);
						$("#result").val("");
						$('#ttiframe').attr('src', $('#ttiframe').attr('src'));
						$("#tt").focus(); 
		           }
		      },    
		      error:function(){
		    	  alert("error");
		      }    
		}); 
	}
</script>
  </head>
  
  <body>
  <div style="margin: 50px auto;width: 1100px;">
  <input type="text" style="width: 50px;display: none;" disabled="disabled" id="b" value="3950" >
  <input type="text" style="width: 50px;display: none;" disabled="disabled" id="r1min" value="29.8" >
  <input type="text" style="width: 50px;display: none;" disabled="disabled" id="r1max" value="29.83" >
  <input type="text" style="width: 50px;display: none;" disabled="disabled" id="t1" value="37" >
  当前温度下阻值范围（最小：<input type="text" style="width: 100px;" disabled="disabled" id="rtmin" value="29.8">k&Omega;
 最大：<input type="text" style="width: 100px;" disabled="disabled" id="rtmax" value="29.83">k&Omega;）
  测试人=<%=testman%>
  <table>
  	<tr>
  		<td>
		  	<table>
		  		<tr>
		  			<td style="font-weight: bold;">当前温度</td>
		  			<td><input type="text" value="" style="font-size: 100px;width: 350px;text-align: left;" id="tt" onchange="ttchange();" onkeydown="ttkeydown();" ></td>
		  			<td style="font-size: 80px;">&deg;C</td>
		  		</tr>
		  		<tr>
		  			<td style="font-weight: bold;">当前阻值</td>
		  			<td><input type="text" value="" style="font-size: 100px;width: 350px;text-align: left;" onfocus="ttchange();" onchange="omchange();" onkeydown="omkeydown();" id="om" ></td>
		  			<td style="font-size: 80px;">k&Omega;</td>
		  		</tr>
		  		<tr>
		  			<td style="font-weight: bold;">测试结果</td>
		  			<td><input type="text" id="result" style="font-size: 100px;width: 350px;" value="" onfocus="omchange();" onkeydown="resultkeydown();"></td>
		  			<td><!--<input type="button" onclick="uptest();" value="提交" style="width: 350px;height: 110px;font-size: 100px;" >--></td>
		  		</tr>
		  	</table>
  		</td>
  		<td>
  			<iframe id="ttiframe" src="result.jsp" style="width: 610px;height: 420px;border: 0px;"></iframe>
  		</td>
  	</tr>
  </table>
  </div>
  </body>
</html>
