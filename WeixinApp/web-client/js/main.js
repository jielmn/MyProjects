var uid = null;
var start_index = 0;

function onTabChange(index){
	if ( 0 == index ) {
		$("#imgMine").attr('src','images/mine.png'); 
		$("#imgGroup").attr('src','images/group_blur.png'); 
		
		$("#iframeMine").css('display','block'); 
		$("#iframeGroup").css('display','none'); 
	} else {
		$("#imgMine").attr('src','images/mine_blur.png'); 
		$("#imgGroup").attr('src','images/group.png'); 
		
		$("#iframeMine").css('display','none'); 
		$("#iframeGroup").css('display','block'); 
	}
}

function getUrlParam(name) {
 var reg = new RegExp("(^|&)" + name + "=([^&]*)(&|$)"); //构造一个含有目标参数的正则表达式对象
 var r = window.location.search.substr(1).match(reg);    //匹配目标参数
 if (r != null) return unescape(r[2]); return null;      //返回参数值
}

function onMainLoad() {
	uid = getUrlParam('uid');
	$("#iframeMine").attr('src','mine.html?uid=' + uid); 
	
	$("#divMine").click(function(){
	  onTabChange(0);
	});
	
	$("#divGroup").click(function(){
	  onTabChange(1);
	});
}

// “我的”页面加载
function onMineLoad() {
	uid = getUrlParam('uid');
	//alert("content:"+uid);
	$.get("https://www.telemed-healthcare.cn/todolist/main?type=userinfo&user_id="+uid,function(data,status){
		if ( "success" == status && 0 == data.error ) {
			console.log("get user info success");
			$("#imgAvatar").attr("src",data.userinfo.avatarUrl);
			$("#divNickname").text(data.userinfo.nick_name);
			getMyTodoList();
		}		
	});
	
	$("#divMoreHistory").click(function(){
		getMyHistoryTodoList();
	});
}

function getMyTodoList() {
	$.get("https://telemed-healthcare.cn/todolist/main?type=todolist&open_id="+uid,function(data,status){
		if ( "success" == status && 0 == data.error ) {
			console.log("get todolist success");
			//console.log(data);
			
			$("#divTodoList").css('display','block');
			if ( data.todolist.length == 0 ) {
				$("#divNone").css('display','block');
			} else {
				$("#divNone").css('display','none');
				
				for (let i = 0;i < data.todolist.length;i++ ){
					
					$("#divNone").before('<div id="divTodoItem' + data.todolist[i].id + '"  value_index="' + data.todolist[i].id + '"  ></div>');					
					$("#divNone").prev().append('<input type="checkbox" id="check_todo" class="listitem my-check" value="' + data.todolist[i].id + '" />');
					$("#divNone").prev().append('<div class="listitem my-content" >' + data.todolist[i].value  + '</div>');
					$("#divNone").prev().append('<div class="listitem my-time" >' + data.todolist[i].start_time  + '</div>');
					$("#divNone").prev().append('<div class="split-line" ></div>');
					
				}
				
				$("input#check_todo").click( function(){
					onTodoCheck(0,this.value);
				} );
				
			}
			
			// 获取历史todo
			getMyHistoryTodoList();
		}		
	});
	
}

function  onTodoCheck(item_type,id) {
	
	console.log("click id=" + id);	
	
	$.get("https://telemed-healthcare.cn/todolist/main?type=finish&id="+id,function(data,status){
		if ( "success" == status && 0 == data.error ) {
			console.log("finish todo item ok");
			
			/*
			var arr = $("#divTodoList").find("div#divTodoItem").filter( function( index ) {
				return $( this ).attr( "value_index" ) == id;
			}).remove();		
			*/
			
			var item = $("#divTodoItem"+id );
			item.remove();
			
			if ( 0 == item_type ) {
				$("#divNone1").css('display','none');
				
				item.click(function(){
					onTodoCheck(1,id);
				})
				
				$("#divHistoryTopLine").after(item);
				
				if ( $("#divTodoList").find("input").length == 0  ) {
					$("#divNone").css('display','block');
				}
			} else {
				$("#divNone").css('display','none');
				
				item.click(function(){
					onTodoCheck(0,id);
				})
				
				$("#divTopLine").after(item);
				
				if ( $("#divTodoListHistory").find("input").length == 0  ) {
					$("#divNone1").css('display','block');
				}
			}
			
		}
	});
	
	
}


function getMyHistoryTodoList() {
	var history_count = $("#divTodoListHistory").find("input").length;
		
	$.get("https://telemed-healthcare.cn/todolist/main?type=history&open_id="+uid+"&start_index="+history_count,function(data,status){
		if ( "success" == status && 0 == data.error ) {
			console.log("get history todolist success");
			//console.log(data);
			
			$("#divTodoListHistory").css('display','block');
			if ( data.todolist.length == 0 ) {
				if ( 0 == history_count ) {
					$("#divNone1").css('display','block');
				}
			} else {
				$("#divNone1").css('display','none');
				
				for (let i = 0;i < data.todolist.length;i++ ){
					
					$("#divNone1").before('<div id="divTodoItem' + data.todolist[i].id + '"  value_index="' + data.todolist[i].id + '"  ></div>');
					
					$("#divNone1").prev().append('<input type="checkbox" id="check_todo_history" checked="true" class="listitem my-check" value="' + data.todolist[i].id + '" />');
					$("#divNone1").prev().append('<div class="listitem my-content" >' + data.todolist[i].value  + '</div>');
					$("#divNone1").prev().append('<div class="listitem my-time" >' + data.todolist[i].start_time  + '</div>');
					$("#divNone1").prev().append('<div class="split-line" ></div>');
				}
				
				$("input#check_todo_history").click( function() {
					onTodoCheck(1,this.value);
				} );
				
			}			
			
		}		
	});
	
}


