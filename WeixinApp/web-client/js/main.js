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
			console.log(data);
			
			$("#divTodoList").css('display','block');
			if ( data.todolist.length == 0 ) {
				$("#divNone").css('display','block');
				$("#divNone_split").css('display','block');
			} else {
				$("#divNone").css('display','none');
				$("#divNone_split").css('display','none');
				
				for (let i = 0;i < data.todolist.length;i++ ){
									
					$("#divNone").before('<div style="display;" >');
	
					$("#divNone").before('<input type="checkbox" id="check_todo" style="display:inline-block;height:40px;margin:0px 0px 0px 10px;" value="' + data.todolist[i].id + '" />');
					$("#divNone").before('<div class="listitem" style="display:inline-block;width:75%;white-space:nowrap; overflow:hidden; text-overflow:ellipsis; " >' + data.todolist[i].value  + '</div>');
					$("#divNone").before('<div class="listitem" style="float:right;display:inline-block;" >' + data.todolist[i].start_time  + '</div>');
					
					$("#divNone").before('</div>');
					$("#divNone").before('<div class="split-line" />');					
				}
				
				$("input#check_todo").click( onTodoCheck );
				
			}
			
			// 获取历史todo
			getMyHistoryTodoList();
		}		
	});
	
}

function  onTodoCheck() {
	console.log(this.value);
}


function getMyHistoryTodoList() {
	var history_count = $("#divTodoListHistory").find("input").length;
		
	$.get("https://telemed-healthcare.cn/todolist/main?type=history&open_id="+uid+"&start_index="+history_count,function(data,status){
		if ( "success" == status && 0 == data.error ) {
			console.log("get history todolist success");
			console.log(data);
			
			$("#divTodoListHistory").css('display','block');
			if ( data.todolist.length == 0 ) {
				if ( 0 == history_count ) {
					$("#divNone1").css('display','block');
					$("#divNone1_split").css('display','block');
				}
			} else {
				$("#divNone1").css('display','none');
				$("#divNone1_split").css('display','none');
				
				for (let i = 0;i < data.todolist.length;i++ ){
									
					$("#divNone1").before('<div style="display;" >');
	
					$("#divNone1").before('<input type="checkbox" id="check_todo_history" checked="true" style="display:inline-block;height:40px;margin:0px 0px 0px 10px;" value="' + data.todolist[i].id + '" />');
					$("#divNone1").before('<div class="listitem" style="display:inline-block;width:75%;white-space:nowrap; overflow:hidden; text-overflow:ellipsis; " >' + data.todolist[i].value  + '</div>');
					$("#divNone1").before('<div class="listitem" style="float:right;display:inline-block;" >' + data.todolist[i].start_time  + '</div>');
					
					$("#divNone1").before('</div>');
					$("#divNone1").before('<div class="split-line" />');					
				}
				
				$("input#check_todo_history").click( onHistoryTodoCheck );
				
			}			
			
		}		
	});
	
}

function  onHistoryTodoCheck() {
	console.log(this.value);
}
