var uid = null;
var start_index = 0;
var bEdit = true;

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
			$("body").css('display','block');
			
			$("#imgAvatar").attr("src",data.userinfo.avatarUrl);
			$("#divNickname").text(data.userinfo.nick_name);
			getMyTodoList();
		}		
	});
	
	$("#divMoreHistory").click(function(){
		getMyHistoryTodoList();
	});
	
	$("#divAdd").click(function(){
		window.location.href="additem.html?uid="+uid;
	});
	
	$("#divEdit").click( function() {
		console.log('click edit');
		
		if ( bEdit ) {
			$("div#divDel").css('display','block');
			$("#divEdit").text('完成')
		}
		else {
			$("div#divDel").css('display','none');
			$("#divEdit").text('编辑')
		}		
		bEdit = !bEdit;
	})
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
				
				CalculateElapsedTime(data.todolist);
				for (let i = 0;i < data.todolist.length;i++ ){
					
					/*
					$("#divNone").before('<div id="divTodoItem' + data.todolist[i].id + '"  value_index="' + data.todolist[i].id + '"  ></div>');					
					$("#divNone").prev().append('<input type="checkbox" id="check_todo" class="listitem my-check" value="' + data.todolist[i].id + '" />');
					$("#divNone").prev().append('<div class="listitem my-content" >' + data.todolist[i].value  + '</div>');
					$("#divNone").prev().append('<div class="listitem my-time" >' + data.todolist[i].start_time  + '</div>');
					$("#divNone").prev().append('<div class="split-line" ></div>');
					*/
					
					$("#divNone").before('<div id="divTodoItem' + data.todolist[i].id + '"  value_index="' + data.todolist[i].id + '" start="' + data.todolist[i].start_time_txt + '"  ></div>');
					
					var item = $("#divNone").prev();
					item.append('<input type="checkbox" id="check_todo" class="listitem my-check" value="' + data.todolist[i].id + '" />');
					
					var divContent = $('<div class="listitem" style="width:90%;style="line-height:21px;" ></div>');
					divContent.append('<div class="my-content" >' + data.todolist[i].value  + '</div>');
					
					var divContentBottom = $('<div></div>');
					divContentBottom.append('<div id="divTime" class="listitem" style="font-size:10px;" >' + data.todolist[i].start_time_txt  + '</div>');
					divContentBottom.append('<div class="listitem my-right " style="font-size:10px;" >&nbsp;</div>');
					
					divContent.append(divContentBottom);
					
					item.append(divContent);
					
					item.append('<div id="divDel" class="listitem my-right my-del hand" style="line-height:42px;width:60px;text-align:center;display:none" onclick="onDel(' + data.todolist[i].id + ');" >删除</div>')
					
					item.append('<div class="split-line" ></div>');
					
					
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
				
				item.find("input").click(function(){
					onTodoCheck(1,id);
				})
				
				item.find("#divTime").text('0分');
				item.attr('end','0分');
				
				$("#divHistoryTopLine").after(item);
				
				if ( $("#divTodoList").find("input").length == 0  ) {
					$("#divNone").css('display','block');
				}
			} else {
				$("#divNone").css('display','none');
				
				item.find("input").click(function(){
					onTodoCheck(0,id);
				})
				
				item.find("#divTime").text(item.attr('start'));
				
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
				
				CalculateElapsedTime(data.todolist);
				for (let i = 0;i < data.todolist.length;i++ ){
					
					
					$("#divNone1").before('<div id="divTodoItem' + data.todolist[i].id + '"  value_index="' + data.todolist[i].id + '" start="' + data.todolist[i].start_time_txt + '" end="' + data.todolist[i].end_time_txt + '"   ></div>');
					
					var item = $("#divNone1").prev();
					item.append('<input type="checkbox" id="check_todo_history" checked="true" class="listitem my-check" value="' + data.todolist[i].id + '" />');
					
					var divContent = $('<div class="listitem" style="width:90%;style="line-height:21px;" ></div>');
					divContent.append('<div class="my-content" >' + data.todolist[i].value  + '</div>');
					
					var divContentBottom = $('<div></div>');
					divContentBottom.append('<div id="divTime" class="listitem" style="font-size:10px;" >' + data.todolist[i].end_time_txt  + '</div>');
					divContentBottom.append('<div class="listitem my-right " style="font-size:10px;" >&nbsp;</div>');
					
					divContent.append(divContentBottom);
					
					item.append(divContent);
					
					item.append('<div id="divDel" class="listitem my-right my-del hand" style="line-height:42px;width:60px;text-align:center;display:none" onclick="onDel(' + data.todolist[i].id + ');" >删除</div>')				
					
					item.append('<div class="split-line" ></div>');
					
				}
				
				$("input#check_todo_history").click( function() {
					onTodoCheck(1,this.value);
				} );
				
			}			
			
		}		
	});
	
}



function onDel( id ) {
	//console.log(id );
	
	var r=confirm("确认删除该清单吗？");
	if (r==false)
	{
		return;
	}	
	
	$.get("https://telemed-healthcare.cn/todolist/main?type=delete&id="+id,function(data,status){
		if ( "success" == status && 0 == data.error ) {
			console.log("delete success!");
			
			var item = $("#divTodoItem"+id );
			item.remove();
			
			if ( $("#divTodoListHistory").find("input").length == 0  ) {
				$("#divNone1").css('display','block');
			}
			
			if ( $("#divTodoList").find("input").length == 0  ) {
				$("#divNone").css('display','block');
			}
		}
	});
}


function CalculateElapsedTime( items ) {
	var now = Date.now()
	for (var i in items ) {
	  var start_time = items[i].start_time || 0;
	  if ( start_time > 0 && start_time <= now ) {
		items[i].start_time_txt = this.FormatTime(now-start_time);
	  } else {
		items[i].start_time_txt = '0分'
	  }

	  var end_time = items[i].end_time || 0;
	  if (end_time && end_time > 0 && end_time <= now) {
		items[i].end_time_txt = this.FormatTime(now - end_time);
	  } else {
		items[i].end_time_txt = '0分'
	  }

	}
}

function FormatTime(elapsed) {
	if (elapsed < 0 ) {
	  return ''
	}

	elapsed = parseInt(elapsed / 1000);

	// 小于一天
	if ( elapsed < 3600 * 24 ) {
	  var hour = parseInt( elapsed / 3600 );
	  var minute = parseInt( ( elapsed % 3600 ) / 60 );
	  if ( hour > 0 ) {
		return hour + '小时' + minute + '分'
	  }      
	  return minute + '分'
	}

	var day = parseInt(elapsed / (3600 * 24))
	return day + '天'
}




function onAdditemLoad() {
	uid = getUrlParam('uid');
	//console.log("additem, uid="+uid);
	
	$("#divReturn").click(function(){
		history.back();
	})
}


